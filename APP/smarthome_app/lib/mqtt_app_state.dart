import 'dart:convert';
import 'dart:math';
import 'package:flutter/material.dart';
import 'package:mqtt_client/mqtt_client.dart';
import 'package:mqtt_client/mqtt_browser_client.dart';

class MqttAppState extends ChangeNotifier {
  // Connection parameters
  String host = 'mqtt.toolhub.app';
  int port = 8084;
  String username = 'vinhhk';
  String password = '20235876';
  bool useSsl = true;

  MqttBrowserClient? _client;

  // Connection states
  bool isConnecting = false;
  bool isConnected = false;
  String logMessages = '';

  // ESP32 System Status
  String systemStatus = 'offline'; // online / offline

  // System State Data (from env JSON)
  double temperature = 0.0;
  int humidity = 0;
  int lightPct = 0;
  String lightState = 'DARK';
  int motion = 0;
  int sound = 0;
  int heatAlarm = 0;
  int rain = 0;
  String deviceOverheat = 'NO';

  // State values (from topics)
  String systemMode = 'HOME';
  String lockState = 'ENGAGED';
  String windowState = 'CLOSED';
  String hallLightState = 'OFF';
  String roomLightState = 'OFF';
  String alarmState = 'OFF';

  // Event Logs and Command feedback
  List<String> eventLogs = [];
  Map<String, dynamic>? lastCmdResult;

  void appendLog(String msg) {
    final timeStr = DateTime.now().toLocal().toString().split(' ')[1].substring(0, 8);
    logMessages += '[$timeStr] $msg\n';
    notifyListeners();
  }

  void clearLogs() {
    logMessages = '';
    notifyListeners();
  }

  void updateConnectionSettings({
    required String newHost,
    required int newPort,
    required String newUsername,
    required String newPassword,
    required bool newUseSsl,
  }) {
    host = newHost;
    port = newPort;
    username = newUsername;
    password = newPassword;
    useSsl = newUseSsl;
    appendLog('Cập nhật cấu hình: $host:$port ($username)');
    notifyListeners();
  }

  Future<bool> connect() async {
    if (isConnected || isConnecting) return true;

    isConnecting = true;
    notifyListeners();
    appendLog('Đang kết nối tới $host:$port qua WebSocket...');

    final clientId = 'smarthome_web_${Random().nextInt(10000)}';
    
    // Initialize MqttBrowserClient
    final scheme = useSsl ? 'wss://' : 'ws://';
    _client = MqttBrowserClient.withPort('$scheme$host', clientId, port);
    _client!.logging(on: false);
    _client!.keepAlivePeriod = 20;

    // Set connection message
    final connMessage = MqttConnectMessage()
        .withClientIdentifier(clientId)
        .authenticateAs(username, password)
        .startClean();
    _client!.connectionMessage = connMessage;

    _client!.onConnected = _onConnected;
    _client!.onDisconnected = _onDisconnected;
    _client!.onSubscribed = _onSubscribed;
    _client!.pongCallback = _pong;

    try {
      await _client!.connect();
      return true;
    } catch (e) {
      appendLog('LỖI KẾT NỐI: $e');
      disconnect();
      return false;
    }
  }

  void disconnect() {
    if (_client != null) {
      appendLog('Đang ngắt kết nối...');
      _client!.disconnect();
    }
    _onDisconnected();
  }

  void _onConnected() {
    isConnected = true;
    isConnecting = false;
    appendLog('Đã kết nối thành công tới MQTT Broker!');
    notifyListeners();

    // Subscribe to status
    _subscribeToTopic('smarthome/status');
    
    // Subscribe to state topics
    _subscribeToTopic('smarthome/state/env');
    _subscribeToTopic('smarthome/state/mode');
    _subscribeToTopic('smarthome/state/lock');
    _subscribeToTopic('smarthome/state/window');
    _subscribeToTopic('smarthome/state/hall_light');
    _subscribeToTopic('smarthome/state/room_light');
    _subscribeToTopic('smarthome/state/alarm');
    
    // Subscribe to event logs and command result topics
    _subscribeToTopic('smarthome/event');
    _subscribeToTopic('smarthome/cmd/result');

    // Setup listener
    _client!.updates!.listen(_onMessageReceived);
  }

  void _onDisconnected() {
    isConnected = false;
    isConnecting = false;
    _client = null;
    systemStatus = 'offline';
    appendLog('Đã ngắt kết nối khỏi Broker.');
    notifyListeners();
  }

  void _onSubscribed(String topic) {
    appendLog('Đã đăng ký (subscribe) thành công topic: $topic');
  }

  void _pong() {
    // Connection is alive (Keep Alive ping response)
  }

  void _subscribeToTopic(String topic) {
    if (_client != null && isConnected) {
      _client!.subscribe(topic, MqttQos.atLeastOnce);
    }
  }

  void _onMessageReceived(List<MqttReceivedMessage<MqttMessage>> c) {
    final recMess = c[0].payload as MqttPublishMessage;
    final topic = c[0].topic;
    final payload = MqttPublishPayload.bytesToStringAsString(recMess.payload.message);

    _handlePayload(topic, payload);
  }

  void _handlePayload(String topic, String payload) {
    final trimmedPayload = payload.trim();
    
    switch (topic) {
      case 'smarthome/status':
        systemStatus = trimmedPayload;
        break;
      case 'smarthome/state/env':
        _parseEnvJson(trimmedPayload);
        break;
      case 'smarthome/state/mode':
        systemMode = trimmedPayload;
        break;
      case 'smarthome/state/lock':
        lockState = trimmedPayload;
        break;
      case 'smarthome/state/window':
        windowState = trimmedPayload;
        break;
      case 'smarthome/state/hall_light':
        hallLightState = trimmedPayload;
        break;
      case 'smarthome/state/room_light':
        roomLightState = trimmedPayload;
        break;
      case 'smarthome/state/alarm':
        alarmState = trimmedPayload;
        break;
      case 'smarthome/event':
        eventLogs.insert(0, trimmedPayload);
        if (eventLogs.length > 50) {
          eventLogs.removeLast();
        }
        break;
      case 'smarthome/cmd/result':
        try {
          lastCmdResult = jsonDecode(trimmedPayload);
        } catch (e) {
          appendLog('Lỗi giải mã JSON cmd/result: $e (Raw: $trimmedPayload)');
        }
        break;
    }
    notifyListeners();
  }

  void _parseEnvJson(String rawJson) {
    try {
      // Fix potential unquoted YES/NO in C-string formatted JSON from ESP32
      // e.g. "device_overheat":YES -> "device_overheat":"YES"
      String sanitized = rawJson
          .replaceAll(':YES', ':"YES"')
          .replaceAll(':NO', ':"NO"')
          .replaceAll(':yes', ':"YES"')
          .replaceAll(':no', ':"NO"');

      final Map<String, dynamic> data = jsonDecode(sanitized);

      temperature = (data['temp'] as num?)?.toDouble() ?? 0.0;
      humidity = (data['humi'] as num?)?.toInt() ?? 0;
      lightPct = (data['light_pct'] as num?)?.toInt() ?? 0;
      lightState = data['light_state']?.toString() ?? 'DARK';
      motion = (data['motion'] as num?)?.toInt() ?? 0;
      sound = (data['sound'] as num?)?.toInt() ?? 0;
      heatAlarm = (data['heat_alarm'] as num?)?.toInt() ?? 0;
      rain = (data['rain'] as num?)?.toInt() ?? 0;
      deviceOverheat = data['device_overheat']?.toString() ?? 'NO';
    } catch (e) {
      appendLog('Lỗi phân tích cú pháp JSON môi trường: $e (Raw: $rawJson)');
    }
  }

  void sendCommand(String command) {
    if (_client == null || !isConnected) {
      appendLog('Không thể gửi lệnh "$command": Chưa kết nối MQTT.');
      return;
    }

    final builder = MqttClientPayloadBuilder();
    builder.addString(command);

    appendLog('Gửi lệnh điều khiển: $command');
    _client!.publishMessage(
      'smarthome/cmd',
      MqttQos.atLeastOnce,
      builder.payload!,
    );
  }
}
