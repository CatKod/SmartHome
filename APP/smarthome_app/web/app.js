/**
 * Smart Room Monitoring and Security System - MQTT Dashboard Client logic
 */

let mqttClient = null;
let isConnected = false;

// Application state variables
let state = {
  // ESP32 LWT Status
  gatewayStatus: 'offline', // online / offline
  
  // Environment variables
  temperature: 0.0,
  humidity: 0,
  lightPct: 0,
  lightState: 'DARK',
  motion: 0,
  sound: 0,
  heatAlarm: 0,
  rain: 0,
  deviceOverheat: 'NO',
  
  // States from topics
  systemMode: 'HOME',
  lockState: 'ENGAGED',
  windowState: 'CLOSED',
  hallLightState: 'OFF',
  roomLightState: 'OFF',
  alarmState: 'OFF'
};

// DOM Elements
const elements = {
  // Connection status & buttons
  mqttStatusBadge: document.getElementById('mqtt-status-badge'),
  mqttStatusText: document.getElementById('mqtt-status-text'),
  toggleSettingsBtn: document.getElementById('toggle-settings-btn'),
  connectBtn: document.getElementById('connect-btn'),
  settingsPanel: document.getElementById('settings-panel'),
  clearDebugBtn: document.getElementById('clear-debug-btn'),
  debugLogs: document.getElementById('debug-logs'),
  
  // Config inputs
  hostInput: document.getElementById('mqtt-host'),
  portInput: document.getElementById('mqtt-port'),
  userInput: document.getElementById('mqtt-user'),
  passInput: document.getElementById('mqtt-pass'),
  sslCheckbox: document.getElementById('mqtt-ssl'),
  saveConnectBtn: document.getElementById('save-connect-btn'),
  
  // Command banner
  cmdFeedbackBanner: document.getElementById('cmd-feedback-banner'),
  cmdFeedbackText: document.getElementById('cmd-feedback-text'),
  closeBannerBtn: document.getElementById('close-banner-btn'),
  
  // System Overview
  gatewayStatusDot: document.getElementById('gateway-status-dot'),
  gatewayStatusText: document.getElementById('gateway-status-text'),
  systemModeDisplay: document.getElementById('system-mode-display'),
  doorLockCard: document.getElementById('door-lock-card'),
  doorLockIcon: document.getElementById('door-lock-icon'),
  doorLockText: document.getElementById('door-lock-text'),
  
  // Sensors
  tempVal: document.getElementById('temp-val'),
  humiVal: document.getElementById('humi-val'),
  lightPctText: document.getElementById('light-pct'),
  lightStateText: document.getElementById('light-state'),
  weatherCard: document.getElementById('weather-card'),
  weatherIcon: document.getElementById('weather-icon'),
  weatherStatusText: document.getElementById('weather-status-text'),
  deviceHeatCard: document.getElementById('device-heat-card'),
  deviceHeatIcon: document.getElementById('device-heat-icon'),
  deviceHeatStatus: document.getElementById('device-heat-status'),
  motionAlertBox: document.getElementById('motion-alert-box'),
  motionStatusText: document.getElementById('motion-status-text'),
  soundAlertBox: document.getElementById('sound-alert-box'),
  soundStatusText: document.getElementById('sound-status-text'),
  
  // Controls
  lightbulbStatusIcon: document.getElementById('lightbulb-status-icon'),
  roomLightStatusText: document.getElementById('room-light-status'),
  lightToggleBtn: document.getElementById('light-toggle-btn'),
  windowStatusIcon: document.getElementById('window-status-icon'),
  windowStatusText: document.getElementById('window-status'),
  windowOpenBtn: document.getElementById('window-open-btn'),
  windowCloseBtn: document.getElementById('window-close-btn'),
  securityStatusIcon: document.getElementById('security-status-icon'),
  securityStatusText: document.getElementById('security-status'),
  securityToggleBtn: document.getElementById('security-toggle-btn'),
  alarmStatusIcon: document.getElementById('alarm-status-icon'),
  alarmStatusText: document.getElementById('alarm-status'),
  alarmSilenceBtn: document.getElementById('alarm-silence-btn'),
  hallLightStatusText: document.getElementById('hall-light-status'),
  
  // Logs
  eventLogsTerminal: document.getElementById('event-logs-terminal'),
  clearEventsBtn: document.getElementById('clear-events-btn')
};

// Initialize Application UI
document.addEventListener('DOMContentLoaded', () => {
  setupEventListeners();
  addDebugLog('Hệ thống khởi chạy. Hãy bấm "Kết nối" để bắt đầu.');
});

// Event handlers registry
function setupEventListeners() {
  // Toggle settings visibility
  elements.toggleSettingsBtn.addEventListener('click', () => {
    elements.settingsPanel.classList.toggle('hidden');
  });
  
  // Action buttons connect
  elements.connectBtn.addEventListener('click', toggleConnection);
  elements.saveConnectBtn.addEventListener('click', saveAndConnect);
  
  // Actuator command buttons
  elements.lightToggleBtn.addEventListener('click', handleLightToggle);
  elements.windowOpenBtn.addEventListener('click', () => sendCommand('WINDOW_OPEN'));
  elements.windowCloseBtn.addEventListener('click', () => sendCommand('WINDOW_CLOSE'));
  elements.securityToggleBtn.addEventListener('click', handleSecurityToggle);
  elements.alarmSilenceBtn.addEventListener('click', () => sendCommand('ALARM_OFF'));
  
  // Log consoles
  elements.clearDebugBtn.addEventListener('click', () => {
    elements.debugLogs.textContent = '';
  });
  elements.clearEventsBtn.addEventListener('click', () => {
    elements.eventLogsTerminal.innerHTML = '<div class="terminal-line text-dim">&gt; Nhật ký trống. Đang chờ sự kiện tiếp theo...</div>';
  });
  elements.closeBannerBtn.addEventListener('click', hideCmdBanner);
}

// Log function for connection setup
function addDebugLog(msg) {
  const time = new Date().toLocaleTimeString();
  elements.debugLogs.textContent += `[${time}] ${msg}\n`;
  elements.debugLogs.parentElement.scrollTop = elements.debugLogs.parentElement.scrollHeight;
}

// Log system events into terminal log
function addEventLog(msg) {
  const isWarning = msg.includes('WARNING') || msg.includes('DENIED') || msg.includes('ALERT') || msg.includes('REJECT') || msg.includes('INTRUSION');
  const lineClass = isWarning ? 'text-red-console' : 'text-green-console';
  
  // Remove placeholders if exist
  if (elements.eventLogsTerminal.textContent.includes('Nhật ký trống')) {
    elements.eventLogsTerminal.innerHTML = '';
  }
  
  const line = document.createElement('div');
  line.className = 'terminal-line';
  line.innerHTML = `<span class="text-dim">&gt;</span> <span class="${lineClass}">${msg}</span>`;
  
  elements.eventLogsTerminal.appendChild(line);
  elements.eventLogsTerminal.scrollTop = elements.eventLogsTerminal.scrollHeight;
}

// Toggle Connection State
function toggleConnection() {
  if (isConnected) {
    disconnectBroker();
  } else {
    connectBroker();
  }
}

// Save inputs & Connect
function saveAndConnect() {
  elements.settingsPanel.classList.add('hidden');
  if (isConnected) {
    disconnectBroker();
    // Connect again with delay
    setTimeout(connectBroker, 500);
  } else {
    connectBroker();
  }
}

// Connect to MQTT Broker
function connectBroker() {
  const host = elements.hostInput.value.trim();
  const port = parseInt(elements.portInput.value.trim());
  const user = elements.userInput.value.trim();
  const pass = elements.passInput.value;
  const useSsl = elements.sslCheckbox.checked;
  
  if (!host || isNaN(port)) {
    addDebugLog('LỖI: Vui lòng nhập đúng Host và Cổng.');
    return;
  }
  
  updateMqttStatusBadge('connecting');
  addDebugLog(`Đang kết nối tới ${host}:${port} qua WebSockets...`);
  
  const clientId = `smarthome_web_${Math.floor(Math.random() * 10000)}`;
  mqttClient = new Paho.MQTT.Client(host, port, clientId);
  
  mqttClient.onConnectionLost = onConnectionLost;
  mqttClient.onMessageArrived = onMessageArrived;
  
  const connOptions = {
    onSuccess: onConnectSuccess,
    onFailure: onConnectFailure,
    useSSL: useSsl,
    keepAliveInterval: 20,
    cleanSession: true
  };
  
  if (user) connOptions.userName = user;
  if (pass) connOptions.password = pass;
  
  try {
    mqttClient.connect(connOptions);
  } catch (err) {
    addDebugLog(`LỖI KẾT NỐI: ${err}`);
    updateMqttStatusBadge('disconnected');
  }
}

// Disconnect from Broker
function disconnectBroker() {
  if (mqttClient) {
    addDebugLog('Đang ngắt kết nối...');
    mqttClient.disconnect();
  }
  onConnectionLost({ errorCode: 0, errorMessage: 'Người dùng ngắt kết nối.' });
}

// Connect success callback
function onConnectSuccess() {
  isConnected = true;
  updateMqttStatusBadge('connected');
  addDebugLog('Đã kết nối thành công tới MQTT Broker!');
  
  // Subscribe to status & states
  subscribeTopic('smarthome/status');
  subscribeTopic('smarthome/state/env');
  subscribeTopic('smarthome/state/mode');
  subscribeTopic('smarthome/state/lock');
  subscribeTopic('smarthome/state/window');
  subscribeTopic('smarthome/state/hall_light');
  subscribeTopic('smarthome/state/room_light');
  subscribeTopic('smarthome/state/alarm');
  
  // Subscribe to logs and commands
  subscribeTopic('smarthome/event');
  subscribeTopic('smarthome/cmd/result');
}

// Connect failure callback
function onConnectFailure(err) {
  isConnected = false;
  updateMqttStatusBadge('disconnected');
  addDebugLog(`KẾT NỐI THẤT BẠI: ${err.errorMessage} (Mã: ${err.errorCode})`);
}

// Connection lost callback
function onConnectionLost(responseObject) {
  isConnected = false;
  updateMqttStatusBadge('disconnected');
  
  state.gatewayStatus = 'offline';
  updateGatewayOnlineUI();
  updateVirtualLeds();
  
  if (responseObject.errorCode !== 0) {
    addDebugLog(`Mất kết nối đột ngột: ${responseObject.errorMessage}`);
  } else {
    addDebugLog('Đã ngắt kết nối chủ động.');
  }
}

// Subscribe Topic Helper
function subscribeTopic(topic) {
  if (mqttClient && isConnected) {
    mqttClient.subscribe(topic, { qos: 1 });
    addDebugLog(`Đã subscribe topic: ${topic}`);
  }
}

// Send command publish helper
function sendCommand(payload) {
  if (!mqttClient || !isConnected) {
    addDebugLog(`Không thể gửi lệnh "${payload}": Chưa kết nối MQTT.`);
    showCmdBanner(`Chưa kết nối MQTT!`, 'error');
    return;
  }
  
  try {
    const message = new Paho.MQTT.Message(payload);
    message.destinationName = 'smarthome/cmd';
    message.qos = 1;
    mqttClient.send(message);
    addDebugLog(`Đã gửi lệnh điều khiển: ${payload}`);
  } catch (err) {
    addDebugLog(`Lỗi khi gửi lệnh: ${err}`);
  }
}

// Message Arrived Callback
function onMessageArrived(message) {
  const topic = message.destinationName;
  const payload = message.payloadString.trim();
  
  processPayload(topic, payload);
}

// Process incoming payloads
function processPayload(topic, payload) {
  switch (topic) {
    case 'smarthome/status':
      state.gatewayStatus = payload;
      updateGatewayOnlineUI();
      break;
    case 'smarthome/state/env':
      parseEnvJson(payload);
      break;
    case 'smarthome/state/mode':
      state.systemMode = payload;
      updateSystemModeUI();
      break;
    case 'smarthome/state/lock':
      state.lockState = payload;
      updateDoorLockUI();
      break;
    case 'smarthome/state/window':
      state.windowState = payload;
      updateWindowUI();
      break;
    case 'smarthome/state/hall_light':
      state.hallLightState = payload;
      elements.hallLightStatusText.textContent = payload.toUpperCase();
      elements.hallLightStatusText.className = `info-val ${payload === 'ON' ? 'text-green' : 'text-muted'}`;
      break;
    case 'smarthome/state/room_light':
      state.roomLightState = payload;
      updateRoomLightUI();
      break;
    case 'smarthome/state/alarm':
      state.alarmState = payload;
      updateAlarmSirenUI();
      break;
    case 'smarthome/event':
      addEventLog(payload);
      break;
    case 'smarthome/cmd/result':
      processCmdResult(payload);
      break;
  }
  
  // Recalculate and update the virtual shift register panel LEDs
  updateVirtualLeds();
}

// Robust JSON parse for environments
function parseEnvJson(rawJson) {
  try {
    // Sanitize unquoted YES/NO in formatted C JSON strings from STM32
    // e.g. "device_overheat":YES -> "device_overheat":"YES"
    const sanitized = rawJson
      .replace(/:YES/g, ':"YES"')
      .replace(/:NO/g, ':"NO"')
      .replace(/:yes/g, ':"YES"')
      .replace(/:no/g, ':"NO"');
      
    const data = JSON.parse(sanitized);
    
    state.temperature = data.temp !== undefined ? parseFloat(data.temp) : 0.0;
    state.humidity = data.humi !== undefined ? parseInt(data.humi) : 0;
    state.lightPct = data.light_pct !== undefined ? parseInt(data.light_pct) : 0;
    state.lightState = data.light_state || 'DARK';
    state.motion = data.motion !== undefined ? parseInt(data.motion) : 0;
    state.sound = data.sound !== undefined ? parseInt(data.sound) : 0;
    state.heatAlarm = data.heat_alarm !== undefined ? parseInt(data.heat_alarm) : 0;
    state.rain = data.rain !== undefined ? parseInt(data.rain) : 0;
    state.deviceOverheat = data.device_overheat || 'NO';
    
    updateSensorsUI();
  } catch (err) {
    addDebugLog(`Lỗi giải mã JSON môi trường: ${err} (Raw: ${rawJson})`);
  }
}

// Parse Command Result JSON and trigger feedback banner
function processCmdResult(rawJson) {
  try {
    const res = JSON.parse(rawJson);
    const cmd = res.cmd || 'UNKNOWN';
    const status = res.result || 'UNKNOWN';
    const reason = res.reason || '';
    
    if (status === 'OK') {
      showCmdBanner(`Lệnh "${cmd}" đã được thực hiện thành công!`, 'success');
    } else if (status === 'TIMEOUT') {
      showCmdBanner(`Lệnh "${cmd}" bị quá thời gian phản hồi (Timeout).`, 'warning');
    } else if (status === 'REJECT') {
      showCmdBanner(`Lệnh "${cmd}" bị từ chối! Lý do: ${reason.toUpperCase()}`, 'error');
    }
  } catch (err) {
    addDebugLog(`Lỗi giải mã JSON kết quả lệnh: ${err} (Raw: ${rawJson})`);
  }
}

// Display Command result banner in top of app
function showCmdBanner(text, type) {
  elements.cmdFeedbackBanner.classList.remove('hidden', 'warning', 'error');
  elements.cmdFeedbackText.textContent = text;
  
  const icon = elements.cmdFeedbackBanner.querySelector('.id-icon');
  icon.className = 'fa-solid id-icon';
  
  if (type === 'success') {
    icon.classList.add('fa-check-circle', 'text-green');
  } else if (type === 'warning') {
    elements.cmdFeedbackBanner.classList.add('warning');
    icon.classList.add('fa-circle-exclamation', 'text-yellow');
  } else if (type === 'error') {
    elements.cmdFeedbackBanner.classList.add('error');
    icon.classList.add('fa-triangle-exclamation', 'text-red');
  }
  
  // Auto dismiss after 6 seconds
  clearTimeout(elements.cmdFeedbackBanner.timeoutId);
  elements.cmdFeedbackBanner.timeoutId = setTimeout(hideCmdBanner, 6000);
}

function hideCmdBanner() {
  elements.cmdFeedbackBanner.classList.add('hidden');
}

// Dynamic UI updates functions
function updateMqttStatusBadge(status) {
  elements.mqttStatusBadge.className = `status-badge ${status}`;
  
  if (status === 'connected') {
    elements.mqttStatusText.textContent = 'ĐÃ KẾT NỐI BROKER';
    elements.connectBtn.textContent = 'Ngắt kết nối';
    elements.connectBtn.className = 'btn danger-btn';
  } else if (status === 'disconnected') {
    elements.mqttStatusText.textContent = 'MẤT KẾT NỐI BROKER';
    elements.connectBtn.textContent = 'Kết nối';
    elements.connectBtn.className = 'btn primary-btn';
  } else if (status === 'connecting') {
    elements.mqttStatusText.textContent = 'ĐANG KẾT NỐI...';
    elements.connectBtn.textContent = 'Kết nối';
    elements.connectBtn.className = 'btn primary-btn';
  }
}

function updateGatewayOnlineUI() {
  const isOnline = state.gatewayStatus === 'online';
  
  elements.gatewayStatusDot.className = `indicator-dot ${isOnline ? 'online' : 'offline'}`;
  elements.gatewayStatusText.textContent = `ESP32 Gateway: ${state.gatewayStatus.toUpperCase()}`;
}

function updateSystemModeUI() {
  const mode = state.systemMode;
  elements.systemModeDisplay.className = 'mode-text';
  
  // Set text and class according to mode
  switch (mode) {
    case 'HOME':
      elements.systemModeDisplay.textContent = 'CHẾ ĐỘ HOME';
      elements.systemModeDisplay.classList.add('text-green');
      elements.securityToggleBtn.textContent = 'KÍCH HOẠT AN NINH';
      elements.securityToggleBtn.className = 'btn success-btn w-100';
      break;
    case 'EXIT_DELAY':
      elements.systemModeDisplay.textContent = 'CHỜ RA KHỎI PHÒNG (EXIT DELAY)';
      elements.systemModeDisplay.classList.add('text-yellow');
      elements.securityToggleBtn.textContent = 'TẮT CHỜ RA KHỎI PHÒNG';
      elements.securityToggleBtn.className = 'btn danger-btn w-100';
      break;
    case 'SECURITY':
      elements.systemModeDisplay.textContent = 'AN NINH (SECURITY MODE)';
      elements.systemModeDisplay.classList.add('text-cyan');
      elements.securityToggleBtn.textContent = 'TẮT CHẾ ĐỘ AN NINH';
      elements.securityToggleBtn.className = 'btn danger-btn w-100';
      break;
    case 'SUSPICIOUS':
      elements.systemModeDisplay.textContent = 'PHÁT HIỆN NGHI VẤN (SUSPICIOUS)';
      elements.systemModeDisplay.classList.add('text-orange');
      elements.securityToggleBtn.textContent = 'TẮT AN NINH';
      elements.securityToggleBtn.className = 'btn danger-btn w-100';
      break;
    case 'ALARM':
      elements.systemModeDisplay.textContent = 'CẢNH BÁO XÂM NHẬP (ALARM!)';
      elements.systemModeDisplay.classList.add('text-red');
      elements.securityToggleBtn.textContent = 'TẮT BÁO ĐỘNG';
      elements.securityToggleBtn.className = 'btn danger-btn w-100';
      break;
  }
}

function updateDoorLockUI() {
  const isLocked = state.lockState === 'ENGAGED';
  
  if (isLocked) {
    elements.doorLockCard.className = 'lock-indicator-card locked';
    elements.doorLockIcon.className = 'fa-solid fa-lock';
    elements.doorLockText.textContent = 'ĐÃ KHÓA';
  } else {
    elements.doorLockCard.className = 'lock-indicator-card unlocked';
    elements.doorLockIcon.className = 'fa-solid fa-lock-open';
    elements.doorLockText.textContent = 'MỞ KHÓA';
  }
}

function updateSensorsUI() {
  // Temperature & Humidity
  elements.tempVal.textContent = state.temperature.toFixed(1);
  elements.humiVal.textContent = state.humidity;
  
  // Light intensity
  elements.lightPctText.textContent = state.lightPct;
  elements.lightStateText.textContent = state.lightState;
  
  const lightIcon = elements.lightPctText.parentElement.nextElementSibling.querySelector('i');
  if (state.lightPct > 70) {
    lightIcon.className = 'fa-solid fa-sun text-yellow';
    lightIcon.style.opacity = '1.0';
  } else if (state.lightPct > 30) {
    lightIcon.className = 'fa-solid fa-cloud-sun text-yellow';
    lightIcon.style.opacity = '0.7';
  } else {
    lightIcon.className = 'fa-solid fa-moon text-blue';
    lightIcon.style.opacity = '0.5';
  }
  
  // Weather / Rain Status
  const isRaining = state.rain === 1;
  if (isRaining) {
    elements.weatherCard.classList.add('rainy');
    elements.weatherIcon.className = 'fa-solid fa-cloud-showers-water weather-icon text-red';
    elements.weatherStatusText.textContent = 'ĐANG CÓ MƯA!';
  } else {
    elements.weatherCard.classList.remove('rainy');
    elements.weatherIcon.className = 'fa-solid fa-sun weather-icon text-orange';
    elements.weatherStatusText.textContent = 'Không mưa';
  }
  
  // Security PIR & Sound alerts
  const isMotionActive = state.motion === 1;
  elements.motionAlertBox.className = `alert-box ${isMotionActive ? 'active' : ''}`;
  elements.motionStatusText.textContent = isMotionActive ? 'PHÁT HIỆN ĐỘNG!' : 'Bình thường';
  
  const isSoundActive = state.sound === 1;
  elements.soundAlertBox.className = `alert-box ${isSoundActive ? 'active' : ''}`;
  elements.soundStatusText.textContent = isSoundActive ? 'CÓ TIẾNG ĐỘNG!' : 'Yên tĩnh';
  
  // Device Temperature
  const isOverheated = state.deviceOverheat === 'YES';
  if (isOverheated) {
    elements.deviceHeatCard.classList.add('overheated');
    elements.deviceHeatIcon.className = 'fa-solid fa-triangle-exclamation weather-icon text-red';
    elements.deviceHeatStatus.textContent = 'QUÁ NHIỆT!';
  } else {
    elements.deviceHeatCard.classList.remove('overheated');
    elements.deviceHeatIcon.className = 'fa-solid fa-microchip weather-icon text-teal';
    elements.deviceHeatStatus.textContent = 'ỔN ĐỊNH';
  }
}

function updateRoomLightUI() {
  const isLightOn = state.roomLightState === 'ON';
  
  elements.roomLightStatusText.textContent = isLightOn ? 'Đang bật' : 'Đang tắt';
  elements.roomLightStatusText.className = `control-subtitle ${isLightOn ? 'text-yellow' : ''}`;
  elements.lightToggleBtn.textContent = isLightOn ? 'TẮT ĐÈN' : 'BẬT ĐÈN';
  elements.lightToggleBtn.className = `btn w-100 ${isLightOn ? 'danger-btn' : 'primary-btn'}`;
  
  if (isLightOn) {
    elements.lightbulbStatusIcon.classList.add('active');
  } else {
    elements.lightbulbStatusIcon.classList.remove('active');
  }
}

function updateWindowUI() {
  const winState = state.windowState;
  
  if (winState === 'OPEN') {
    elements.windowStatusText.textContent = 'Trạng thái: MỞ';
    elements.windowStatusIcon.className = 'fa-solid fa-window-restore control-icon active';
  } else if (winState === 'CLOSED') {
    elements.windowStatusText.textContent = 'Trạng thái: ĐÓNG';
    elements.windowStatusIcon.className = 'fa-solid fa-window-maximize control-icon';
  } else if (winState === 'MOVING') {
    elements.windowStatusText.textContent = 'Trạng thái: ĐANG CHẠY...';
    elements.windowStatusIcon.className = 'fa-solid fa-rotate control-icon cyan-active fa-spin';
  }
}

function updateAlarmSirenUI() {
  const isAlarmActive = state.alarmState === 'ON' || state.systemMode === 'ALARM';
  
  elements.alarmStatusText.textContent = isAlarmActive ? 'ĐANG BÁO ĐỘNG!' : 'Tắt';
  elements.alarmStatusText.className = `control-subtitle ${isAlarmActive ? 'text-red' : ''}`;
  elements.alarmSilenceBtn.disabled = !isAlarmActive;
  
  if (isAlarmActive) {
    elements.alarmStatusIcon.className = 'fa-solid fa-bell-ring control-icon text-red fa-bounce';
  } else {
    elements.alarmStatusIcon.className = 'fa-solid fa-bell-slash control-icon';
  }
}

// Recalculate 74HC595 shift register status LEDs (8-bits)
function updateVirtualLeds() {
  const isOnline = state.gatewayStatus === 'online';
  
  // LED 1: System Online (Q0)
  setLedActive(0, isOnline);
  
  // LED 2: Home Mode (Q1) - active when mode is HOME and system is online
  setLedActive(1, isOnline && state.systemMode === 'HOME');
  
  // LED 3: Security Mode (Q2) - active in SECURITY or SUSPICIOUS
  setLedActive(2, isOnline && (state.systemMode === 'SECURITY' || state.systemMode === 'SUSPICIOUS'));
  
  // LED 4: Door Lock Status (Q3) - active when lock is ENGAGED
  setLedActive(3, isOnline && state.lockState === 'ENGAGED');
  
  // LED 5: Motion (Q4) - active when motion is detected
  setLedActive(4, isOnline && state.motion !== 0);
  
  // LED 6: Sound (Q5) - active when sound is detected
  setLedActive(5, isOnline && state.sound !== 0);
  
  // LED 7: Rain (Q6) - active when rain is detected
  setLedActive(6, isOnline && state.rain !== 0);
  
  // LED 8: Alarm (Q7) - active when mode is ALARM
  setLedActive(7, isOnline && state.systemMode === 'ALARM');
}

function setLedActive(bit, isActive) {
  const led = document.getElementById(`led-${bit}`);
  if (led) {
    const bulb = led.querySelector('.led-bulb');
    if (isActive) {
      bulb.classList.add('active');
    } else {
      bulb.classList.remove('active');
    }
  }
}

// Actuators click triggers
function handleLightToggle() {
  const nextCmd = state.roomLightState === 'ON' ? 'LIGHT_OFF' : 'LIGHT_ON';
  sendCommand(nextCmd);
}

function handleSecurityToggle() {
  // Toggle between SECURITY and HOME modes
  const isSecurity = state.systemMode === 'SECURITY' || state.systemMode === 'ALARM' || state.systemMode === 'SUSPICIOUS';
  const nextCmd = isSecurity ? 'SECURITY_OFF' : 'SECURITY_ON';
  sendCommand(nextCmd);
}
