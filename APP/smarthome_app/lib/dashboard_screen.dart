import 'package:flutter/material.dart';
import 'package:google_fonts/google_fonts.dart';
import 'package:provider/provider.dart';
import 'mqtt_app_state.dart';

class DashboardScreen extends StatefulWidget {
  const DashboardScreen({super.key});

  @override
  State<DashboardScreen> createState() => _DashboardScreenState();
}

class _DashboardScreenState extends State<DashboardScreen> {
  final _hostController = TextEditingController();
  final _portController = TextEditingController();
  final _usernameController = TextEditingController();
  final _passwordController = TextEditingController();
  bool _useSsl = true;
  bool _isSettingsExpanded = false;

  @override
  void initState() {
    super.initState();
    // Load initial values from the provider
    final state = Provider.of<MqttAppState>(context, listen: false);
    _hostController.text = state.host;
    _portController.text = state.port.toString();
    _usernameController.text = state.username;
    _passwordController.text = state.password;
    _useSsl = state.useSsl;
  }

  @override
  void dispose() {
    _hostController.dispose();
    _portController.dispose();
    _usernameController.dispose();
    _passwordController.dispose();
    super.dispose();
  }

  void _saveAndConnect(MqttAppState state) {
    final port = int.tryParse(_portController.text) ?? 8084;
    state.updateConnectionSettings(
      newHost: _hostController.text.trim(),
      newPort: port,
      newUsername: _usernameController.text.trim(),
      newPassword: _passwordController.text,
      newUseSsl: _useSsl,
    );
    state.connect();
    setState(() {
      _isSettingsExpanded = false;
    });
  }

  @override
  Widget build(BuildContext context) {
    final state = Provider.of<MqttAppState>(context);
    final isDesktop = MediaQuery.of(context).size.width > 900;

    return Scaffold(
      backgroundColor: const Color(0xFF0F172A), // Deep Slate background
      body: Container(
        decoration: const BoxDecoration(
          gradient: LinearGradient(
            begin: Alignment.topLeft,
            end: Alignment.bottomRight,
            colors: [
              Color(0xFF0F172A),
              Color(0xFF1E293B),
              Color(0xFF0F172A),
            ],
          ),
        ),
        child: SafeArea(
          child: Column(
            children: [
              _buildHeader(state),
              if (state.lastCmdResult != null) _buildCmdResultBanner(state),
              Expanded(
                child: SingleChildScrollView(
                  padding: const EdgeInsets.all(16.0),
                  child: Column(
                    crossAxisAlignment: CrossAxisAlignment.stretch,
                    children: [
                      _buildSettingsPanel(state),
                      const SizedBox(height: 16),
                      if (isDesktop)
                        Row(
                          crossAxisAlignment: CrossAxisAlignment.start,
                          children: [
                            Expanded(
                              flex: 2,
                              child: Column(
                                children: [
                                  _buildGatewayStatusSection(state),
                                  const SizedBox(height: 16),
                                  _buildSensorsGrid(state, gridCount: 2),
                                ],
                              ),
                            ),
                            const SizedBox(width: 16),
                            Expanded(
                              flex: 1,
                              child: Column(
                                children: [
                                  _buildControlPanel(state),
                                  const SizedBox(height: 16),
                                  _buildTerminalLogs(state),
                                ],
                              ),
                            ),
                          ],
                        )
                      else
                        Column(
                          children: [
                            _buildGatewayStatusSection(state),
                            const SizedBox(height: 16),
                            _buildSensorsGrid(state, gridCount: 1),
                            const SizedBox(height: 16),
                            _buildControlPanel(state),
                            const SizedBox(height: 16),
                            _buildTerminalLogs(state),
                          ],
                        ),
                    ],
                  ),
                ),
              ),
            ],
          ),
        ),
      ),
    );
  }

  Widget _buildHeader(MqttAppState state) {
    Color statusColor = Colors.red;
    String statusText = 'MẤT KẾT NỐI';
    Widget statusIcon = const Icon(Icons.cloud_off, color: Colors.red);

    if (state.isConnected) {
      statusColor = Colors.green;
      statusText = 'ĐÃ KẾT NỐI';
      statusIcon = const Icon(Icons.cloud_done, color: Colors.green);
    } else if (state.isConnecting) {
      statusColor = Colors.orange;
      statusText = 'ĐANG KẾT NỐI';
      statusIcon = const SizedBox(
        width: 16,
        height: 16,
        child: CircularProgressIndicator(
          strokeWidth: 2,
          valueColor: AlwaysStoppedAnimation<Color>(Colors.orange),
        ),
      );
    }

    return Container(
      padding: const EdgeInsets.symmetric(horizontal: 20, vertical: 16),
      decoration: BoxDecoration(
        color: const Color(0x1F1E293B),
        border: const Border(
          bottom: BorderSide(color: Color(0x1F94A3B8), width: 1.0),
        ),
      ),
      child: Row(
        mainAxisAlignment: MainAxisAlignment.spaceBetween,
        children: [
          Row(
            children: [
              const Icon(Icons.home_work_outlined, color: Colors.blueAccent, size: 28),
              const SizedBox(width: 12),
              Text(
                'SMART ROOM DASHBOARD',
                style: GoogleFonts.outfit(
                  color: Colors.white,
                  fontWeight: FontWeight.bold,
                  fontSize: 20,
                  letterSpacing: 1.2,
                ),
              ),
            ],
          ),
          Row(
            children: [
              Container(
                padding: const EdgeInsets.symmetric(horizontal: 12, vertical: 6),
                decoration: BoxDecoration(
                  color: statusColor.withOpacity(0.1),
                  borderRadius: BorderRadius.circular(20),
                  border: Border.all(color: statusColor.withOpacity(0.3)),
                ),
                child: Row(
                  children: [
                    statusIcon,
                    const SizedBox(width: 8),
                    Text(
                      statusText,
                      style: GoogleFonts.outfit(
                        color: statusColor,
                        fontWeight: FontWeight.w600,
                        fontSize: 12,
                      ),
                    ),
                  ],
                ),
              ),
              const SizedBox(width: 12),
              IconButton(
                icon: Icon(
                  _isSettingsExpanded ? Icons.close : Icons.settings,
                  color: Colors.white70,
                ),
                onPressed: () {
                  setState(() {
                    _isSettingsExpanded = !_isSettingsExpanded;
                  });
                },
                tooltip: 'Cài đặt kết nối',
              ),
              if (!state.isConnected)
                ElevatedButton(
                  onPressed: state.isConnecting ? null : () => state.connect(),
                  style: ElevatedButton.styleFrom(
                    backgroundColor: Colors.blueAccent,
                    foregroundColor: Colors.white,
                    shape: RoundedRectangleBorder(
                      borderRadius: BorderRadius.circular(8),
                    ),
                  ),
                  child: const Text('Kết nối'),
                )
              else
                ElevatedButton(
                  onPressed: () => state.disconnect(),
                  style: ElevatedButton.styleFrom(
                    backgroundColor: Colors.redAccent,
                    foregroundColor: Colors.white,
                    shape: RoundedRectangleBorder(
                      borderRadius: BorderRadius.circular(8),
                    ),
                  ),
                  child: const Text('Ngắt'),
                ),
            ],
          ),
        ],
      ),
    );
  }

  Widget _buildCmdResultBanner(MqttAppState state) {
    final result = state.lastCmdResult!;
    final cmd = result['cmd'] ?? 'UNKNOWN';
    final status = result['result'] ?? 'UNKNOWN';
    final reason = result['reason'] ?? '';

    final isOk = status == 'OK';
    final isTimeout = status == 'TIMEOUT';
    final isReject = status == 'REJECT';

    Color bannerColor = Colors.grey;
    String text = 'Lệnh: $cmd - Kết quả: $status';

    if (isOk) {
      bannerColor = Colors.green;
      text = 'Lệnh "$cmd" đã thực hiện thành công!';
    } else if (isTimeout) {
      bannerColor = Colors.orange;
      text = 'Lệnh "$cmd" bị quá thời gian phản hồi (Timeout).';
    } else if (isReject) {
      bannerColor = Colors.red;
      text = 'Lệnh "$cmd" bị từ chối! Lý do: ${reason.toString().toUpperCase()}';
    }

    return Container(
      width: double.infinity,
      padding: const EdgeInsets.symmetric(horizontal: 20, vertical: 10),
      color: bannerColor.withOpacity(0.2),
      child: Row(
        children: [
          Icon(
            isOk ? Icons.check_circle : Icons.warning,
            color: bannerColor,
            size: 20,
          ),
          const SizedBox(width: 12),
          Expanded(
            child: Text(
              text,
              style: GoogleFonts.outfit(
                color: Colors.white,
                fontWeight: FontWeight.w500,
                fontSize: 14,
              ),
            ),
          ),
          IconButton(
            icon: const Icon(Icons.close, color: Colors.white54, size: 16),
            onPressed: () {
              setState(() {
                state.lastCmdResult = null;
              });
            },
          )
        ],
      ),
    );
  }

  Widget _buildSettingsPanel(MqttAppState state) {
    if (!_isSettingsExpanded) return const SizedBox.shrink();

    return Container(
      padding: const EdgeInsets.all(16),
      decoration: BoxDecoration(
        color: const Color(0x1F1E293B),
        borderRadius: BorderRadius.circular(16),
        border: Border.all(color: const Color(0x1F94A3B8)),
      ),
      child: Column(
        crossAxisAlignment: CrossAxisAlignment.stretch,
        children: [
          Row(
            mainAxisAlignment: MainAxisAlignment.spaceBetween,
            children: [
              Text(
                'CẤU HÌNH KẾT NỐI MQTT (WEBSOCKET)',
                style: GoogleFonts.outfit(
                  color: Colors.white70,
                  fontWeight: FontWeight.bold,
                  fontSize: 14,
                ),
              ),
              TextButton(
                onPressed: () {
                  state.clearLogs();
                },
                child: const Text('Xóa nhật ký debug'),
              )
            ],
          ),
          const SizedBox(height: 12),
          Row(
            children: [
              Expanded(
                flex: 3,
                child: _buildTextField(
                  controller: _hostController,
                  labelText: 'MQTT Host',
                  hintText: 'mqtt.toolhub.app',
                ),
              ),
              const SizedBox(width: 12),
              Expanded(
                flex: 1,
                child: _buildTextField(
                  controller: _portController,
                  labelText: 'Cổng (WebSocket)',
                  hintText: '8084',
                  keyboardType: TextInputType.number,
                ),
              ),
            ],
          ),
          const SizedBox(height: 12),
          Row(
            children: [
              Expanded(
                child: _buildTextField(
                  controller: _usernameController,
                  labelText: 'Tài khoản',
                  hintText: 'vinhhk',
                ),
              ),
              const SizedBox(width: 12),
              Expanded(
                child: _buildTextField(
                  controller: _passwordController,
                  labelText: 'Mật khẩu',
                  hintText: '20235876',
                  obscureText: true,
                ),
              ),
            ],
          ),
          const SizedBox(height: 12),
          Row(
            mainAxisAlignment: MainAxisAlignment.spaceBetween,
            children: [
              Row(
                children: [
                  Checkbox(
                    value: _useSsl,
                    onChanged: (val) {
                      setState(() {
                        _useSsl = val ?? true;
                        if (_useSsl && _portController.text == '8083') {
                          _portController.text = '8084';
                        } else if (!_useSsl && _portController.text == '8084') {
                          _portController.text = '8083';
                        }
                      });
                    },
                    activeColor: Colors.blueAccent,
                  ),
                  Text(
                    'Sử dụng SSL/TLS (WSS)',
                    style: GoogleFonts.outfit(color: Colors.white.withOpacity(0.9), fontSize: 13),
                  ),
                ],
              ),
              ElevatedButton.icon(
                onPressed: () => _saveAndConnect(state),
                icon: const Icon(Icons.save),
                label: const Text('Lưu & Kết nối'),
                style: ElevatedButton.styleFrom(
                  backgroundColor: Colors.blueAccent,
                  foregroundColor: Colors.white,
                  padding: const EdgeInsets.symmetric(horizontal: 20, vertical: 12),
                ),
              ),
            ],
          ),
          const SizedBox(height: 12),
          Container(
            height: 100,
            padding: const EdgeInsets.all(8),
            decoration: BoxDecoration(
              color: Colors.black.withOpacity(0.3),
              borderRadius: BorderRadius.circular(8),
              border: Border.all(color: Colors.white12),
            ),
            child: SingleChildScrollView(
              reverse: true,
              child: Text(
                state.logMessages.isEmpty ? 'Chưa có nhật ký.' : state.logMessages,
                style: const TextStyle(
                  color: Colors.lightGreenAccent,
                  fontFamily: 'monospace',
                  fontSize: 11,
                ),
              ),
            ),
          )
        ],
      ),
    );
  }

  Widget _buildTextField({
    required TextEditingController controller,
    required String labelText,
    required String hintText,
    bool obscureText = false,
    TextInputType keyboardType = TextInputType.text,
  }) {
    return TextField(
      controller: controller,
      obscureText: obscureText,
      keyboardType: keyboardType,
      style: const TextStyle(color: Colors.white),
      decoration: InputDecoration(
        labelText: labelText,
        labelStyle: const TextStyle(color: Colors.white54),
        hintText: hintText,
        hintStyle: const TextStyle(color: Colors.white24),
        filled: true,
        fillColor: Colors.white.withOpacity(0.05),
        border: OutlineInputBorder(
          borderRadius: BorderRadius.circular(8),
          borderSide: const BorderSide(color: Colors.white12),
        ),
        enabledBorder: OutlineInputBorder(
          borderRadius: BorderRadius.circular(8),
          borderSide: const BorderSide(color: Colors.white12),
        ),
        focusedBorder: OutlineInputBorder(
          borderRadius: BorderRadius.circular(8),
          borderSide: const BorderSide(color: Colors.blueAccent, width: 1.5),
        ),
      ),
    );
  }

  Widget _buildGatewayStatusSection(MqttAppState state) {
    // Determine Gateway Online status
    final isOnline = state.systemStatus == 'online';
    final mode = state.systemMode;

    // Badges details based on mode
    Color modeColor = Colors.green;
    String modeText = 'BÌNH THƯỜNG (HOME)';
    IconData modeIcon = Icons.home;
    bool isAlarm = false;

    switch (mode) {
      case 'HOME':
        modeColor = Colors.green;
        modeText = 'CHẾ ĐỘ HOME';
        modeIcon = Icons.home;
        break;
      case 'EXIT_DELAY':
        modeColor = Colors.amber;
        modeText = 'CHỜ RA KHỎI PHÒNG (EXIT DELAY)';
        modeIcon = Icons.run_circle_outlined;
        break;
      case 'SECURITY':
        modeColor = Colors.cyan;
        modeText = 'AN NINH (SECURITY MODE)';
        modeIcon = Icons.shield_outlined;
        break;
      case 'SUSPICIOUS':
        modeColor = Colors.orange;
        modeText = 'PHÁT HIỆN NGHI VẤN (SUSPICIOUS)';
        modeIcon = Icons.remove_red_eye;
        break;
      case 'ALARM':
        modeColor = Colors.red;
        modeText = 'CẢNH BÁO BÁO ĐỘNG (ALARM!)';
        modeIcon = Icons.warning_amber_rounded;
        isAlarm = true;
        break;
    }

    return Container(
      padding: const EdgeInsets.all(16),
      decoration: BoxDecoration(
        color: const Color(0x1F1E293B),
        borderRadius: BorderRadius.circular(16),
        border: Border.all(
          color: isAlarm ? Colors.red.withOpacity(0.4) : const Color(0x1F94A3B8),
          width: isAlarm ? 1.5 : 1.0,
        ),
        boxShadow: isAlarm
            ? [
                BoxShadow(
                  color: Colors.red.withOpacity(0.15),
                  blurRadius: 10,
                  spreadRadius: 2,
                )
              ]
            : null,
      ),
      child: Row(
        children: [
          Expanded(
            child: Column(
              crossAxisAlignment: CrossAxisAlignment.start,
              children: [
                Row(
                  children: [
                    Container(
                      width: 10,
                      height: 10,
                      decoration: BoxDecoration(
                        color: isOnline ? Colors.green : Colors.red,
                        shape: BoxShape.circle,
                        boxShadow: [
                          BoxShadow(
                            color: (isOnline ? Colors.green : Colors.red).withOpacity(0.5),
                            blurRadius: 6,
                            spreadRadius: 2,
                          )
                        ],
                      ),
                    ),
                    const SizedBox(width: 8),
                    Text(
                      'ESP32 Gateway: ${state.systemStatus.toUpperCase()}',
                      style: GoogleFonts.outfit(
                        color: Colors.white,
                        fontWeight: FontWeight.bold,
                        fontSize: 14,
                      ),
                    ),
                  ],
                ),
                const SizedBox(height: 12),
                Row(
                  children: [
                    Icon(modeIcon, color: modeColor, size: 24),
                    const SizedBox(width: 8),
                    Expanded(
                      child: Text(
                        modeText,
                        style: GoogleFonts.outfit(
                          color: modeColor,
                          fontWeight: FontWeight.bold,
                          fontSize: 16,
                        ),
                      ),
                    ),
                  ],
                ),
              ],
            ),
          ),
          const VerticalDivider(color: Colors.white24, width: 20, thickness: 1),
          // Door Lock state
          _buildLockIndicator(state.lockState),
        ],
      ),
    );
  }

  Widget _buildLockIndicator(String lockState) {
    final isLocked = lockState == 'ENGAGED';
    final lockColor = isLocked ? Colors.redAccent : Colors.greenAccent;
    final lockIcon = isLocked ? Icons.lock : Icons.lock_open;
    final lockText = isLocked ? 'ĐÃ KHÓA' : 'MỞ KHÓA';

    return Container(
      padding: const EdgeInsets.symmetric(horizontal: 16, vertical: 8),
      decoration: BoxDecoration(
        color: lockColor.withOpacity(0.1),
        borderRadius: BorderRadius.circular(12),
        border: Border.all(color: lockColor.withOpacity(0.3)),
      ),
      child: Column(
        mainAxisSize: MainAxisSize.min,
        children: [
          Icon(lockIcon, color: lockColor, size: 24),
          const SizedBox(height: 4),
          Text(
            lockText,
            style: GoogleFonts.outfit(
              color: lockColor,
              fontWeight: FontWeight.bold,
              fontSize: 12,
            ),
          ),
          Text(
            'Khóa cửa',
            style: GoogleFonts.outfit(
              color: Colors.white38,
              fontSize: 9,
            ),
          ),
        ],
      ),
    );
  }

  Widget _buildSensorsGrid(MqttAppState state, {required int gridCount}) {
    return GridView.count(
      crossAxisCount: gridCount,
      shrinkWrap: true,
      physics: const NeverScrollableScrollPhysics(),
      crossAxisSpacing: 16,
      mainAxisSpacing: 16,
      childAspectRatio: 1.5,
      children: [
        _buildSensorCard(
          title: 'Nhiệt độ & Độ ẩm',
          icon: Icons.thermostat,
          iconColor: Colors.orangeAccent,
          child: Column(
            mainAxisAlignment: MainAxisAlignment.center,
            children: [
              Row(
                mainAxisAlignment: MainAxisAlignment.spaceEvenly,
                children: [
                  Column(
                    children: [
                      Text(
                        '${state.temperature.toStringAsFixed(1)}°C',
                        style: GoogleFonts.outfit(
                          color: Colors.white,
                          fontSize: 28,
                          fontWeight: FontWeight.bold,
                        ),
                      ),
                      Text(
                        'Nhiệt độ',
                        style: GoogleFonts.outfit(color: Colors.white54, fontSize: 12),
                      ),
                    ],
                  ),
                  const SizedBox(
                    height: 40,
                    child: VerticalDivider(color: Colors.white24, thickness: 1),
                  ),
                  Column(
                    children: [
                      Text(
                        '${state.humidity}%',
                        style: GoogleFonts.outfit(
                          color: Colors.blueAccent,
                          fontSize: 28,
                          fontWeight: FontWeight.bold,
                        ),
                      ),
                      Text(
                        'Độ ẩm',
                        style: GoogleFonts.outfit(color: Colors.white54, fontSize: 12),
                      ),
                    ],
                  ),
                ],
              ),
            ],
          ),
        ),
        _buildSensorCard(
          title: 'Ánh sáng phòng',
          icon: Icons.wb_sunny_outlined,
          iconColor: Colors.yellowAccent,
          child: Column(
            mainAxisAlignment: MainAxisAlignment.center,
            children: [
              Text(
                '${state.lightPct}%',
                style: GoogleFonts.outfit(
                  color: Colors.white,
                  fontSize: 28,
                  fontWeight: FontWeight.bold,
                ),
              ),
              const SizedBox(height: 4),
              Container(
                padding: const EdgeInsets.symmetric(horizontal: 8, vertical: 2),
                decoration: BoxDecoration(
                  color: Colors.white12,
                  borderRadius: BorderRadius.circular(10),
                ),
                child: Text(
                  state.lightState,
                  style: GoogleFonts.outfit(
                    color: Colors.yellowAccent,
                    fontSize: 12,
                    fontWeight: FontWeight.bold,
                  ),
                ),
              ),
            ],
          ),
        ),
        _buildSensorCard(
          title: 'Thời tiết (Cảm biến Mưa)',
          icon: Icons.beach_access,
          iconColor: Colors.lightBlueAccent,
          child: Column(
            mainAxisAlignment: MainAxisAlignment.center,
            children: [
              Icon(
                state.rain == 1 ? Icons.cloud_sharp : Icons.wb_sunny,
                color: state.rain == 1 ? Colors.blueAccent : Colors.amber,
                size: 32,
              ),
              const SizedBox(height: 8),
              Text(
                state.rain == 1 ? 'ĐANG CÓ MƯA!' : 'Không có mưa',
                style: GoogleFonts.outfit(
                  color: state.rain == 1 ? Colors.redAccent : Colors.greenAccent,
                  fontSize: 16,
                  fontWeight: FontWeight.bold,
                ),
              ),
            ],
          ),
        ),
        _buildSensorCard(
          title: 'Bảo mật (Cảm biến chuyển động)',
          icon: Icons.directions_walk,
          iconColor: Colors.purpleAccent,
          child: Column(
            mainAxisAlignment: MainAxisAlignment.center,
            children: [
              Row(
                mainAxisAlignment: MainAxisAlignment.spaceEvenly,
                children: [
                  Column(
                    children: [
                      Icon(
                        state.motion == 1 ? Icons.warning : Icons.check_circle_outline,
                        color: state.motion == 1 ? Colors.redAccent : Colors.greenAccent,
                        size: 24,
                      ),
                      const SizedBox(height: 4),
                      Text(
                        state.motion == 1 ? 'CÓ ĐỘNG!' : 'Bình thường',
                        style: GoogleFonts.outfit(
                          color: state.motion == 1 ? Colors.redAccent : Colors.greenAccent,
                          fontSize: 13,
                          fontWeight: FontWeight.bold,
                        ),
                      ),
                      Text(
                        'Chuyển động (PIR)',
                        style: GoogleFonts.outfit(color: Colors.white30, fontSize: 9),
                      )
                    ],
                  ),
                  const SizedBox(
                    height: 40,
                    child: VerticalDivider(color: Colors.white24, thickness: 1),
                  ),
                  Column(
                    children: [
                      Icon(
                        state.sound == 1 ? Icons.volume_up : Icons.volume_off,
                        color: state.sound == 1 ? Colors.amber : Colors.greenAccent,
                        size: 24,
                      ),
                      const SizedBox(height: 4),
                      Text(
                        state.sound == 1 ? 'CÓ TIẾNG!' : 'Yên tĩnh',
                        style: GoogleFonts.outfit(
                          color: state.sound == 1 ? Colors.amber : Colors.greenAccent,
                          fontSize: 13,
                          fontWeight: FontWeight.bold,
                        ),
                      ),
                      Text(
                        'Cảm biến Âm thanh',
                        style: GoogleFonts.outfit(color: Colors.white30, fontSize: 9),
                      )
                    ],
                  ),
                ],
              ),
            ],
          ),
        ),
        _buildSensorCard(
          title: 'Nhiệt độ cục bộ thiết bị',
          icon: Icons.developer_board,
          iconColor: Colors.tealAccent,
          child: Column(
            mainAxisAlignment: MainAxisAlignment.center,
            children: [
              Text(
                state.deviceOverheat == 'YES' ? 'QUÁ NHIỆT!' : 'ỔN ĐỊNH',
                style: GoogleFonts.outfit(
                  color: state.deviceOverheat == 'YES' ? Colors.redAccent : Colors.tealAccent,
                  fontSize: 18,
                  fontWeight: FontWeight.bold,
                ),
              ),
              const SizedBox(height: 4),
              Text(
                'Cảm biến Thermistor ở phần cứng',
                style: GoogleFonts.outfit(color: Colors.white38, fontSize: 11),
              ),
            ],
          ),
        ),
      ],
    );
  }

  Widget _buildSensorCard({
    required String title,
    required IconData icon,
    required Color iconColor,
    required Widget child,
  }) {
    return Container(
      padding: const EdgeInsets.all(12),
      decoration: BoxDecoration(
        color: const Color(0x1F1E293B),
        borderRadius: BorderRadius.circular(16),
        border: Border.all(color: const Color(0x1F94A3B8)),
      ),
      child: Column(
        crossAxisAlignment: CrossAxisAlignment.stretch,
        children: [
          Row(
            children: [
              Icon(icon, color: iconColor, size: 16),
              const SizedBox(width: 6),
              Text(
                title.toUpperCase(),
                style: GoogleFonts.outfit(
                  color: Colors.white54,
                  fontSize: 10,
                  fontWeight: FontWeight.bold,
                  letterSpacing: 1.0,
                ),
              ),
            ],
          ),
          const SizedBox(height: 8),
          Expanded(child: child),
        ],
      ),
    );
  }

  Widget _buildControlPanel(MqttAppState state) {
    final isRoomLightOn = state.roomLightState == 'ON';
    final isHallLightOn = state.hallLightState == 'ON';
    final isAlarmOn = state.alarmState == 'ON' || state.systemMode == 'ALARM';

    // Window text description
    String windowText = 'ĐÓNG';
    IconData windowIcon = Icons.window;
    if (state.windowState == 'OPEN') {
      windowText = 'MỞ';
      windowIcon = Icons.window_outlined;
    } else if (state.windowState == 'MOVING') {
      windowText = 'ĐANG CHẠY...';
      windowIcon = Icons.sync;
    }

    return Container(
      padding: const EdgeInsets.all(16),
      decoration: BoxDecoration(
        color: const Color(0x1F1E293B),
        borderRadius: BorderRadius.circular(16),
        border: Border.all(color: const Color(0x1F94A3B8)),
      ),
      child: Column(
        crossAxisAlignment: CrossAxisAlignment.stretch,
        children: [
          Text(
            'BẢNG ĐIỀU KHIỂN THIẾT BỊ',
            style: GoogleFonts.outfit(
              color: Colors.white70,
              fontWeight: FontWeight.bold,
              fontSize: 14,
              letterSpacing: 1.0,
            ),
          ),
          const SizedBox(height: 16),
          // Room light control
          _buildControlRow(
            icon: Icons.lightbulb,
            iconColor: isRoomLightOn ? Colors.yellowAccent : Colors.white38,
            title: 'Đèn trong phòng',
            subtitle: isRoomLightOn ? 'Đang bật' : 'Đang tắt',
            action: ElevatedButton(
              onPressed: () {
                state.sendCommand(isRoomLightOn ? 'LIGHT_OFF' : 'LIGHT_ON');
              },
              style: ElevatedButton.styleFrom(
                backgroundColor: isRoomLightOn ? Colors.orangeAccent : Colors.blueAccent,
                foregroundColor: Colors.white,
              ),
              child: Text(isRoomLightOn ? 'Tắt đèn' : 'Bật đèn'),
            ),
          ),
          const Divider(color: Colors.white12, height: 20),
          // Window control
          _buildControlRow(
            icon: windowIcon,
            iconColor: Colors.cyanAccent,
            title: 'Cửa sổ phòng',
            subtitle: 'Trạng thái: $windowText',
            action: Row(
              mainAxisSize: MainAxisSize.min,
              children: [
                IconButton(
                  icon: const Icon(Icons.keyboard_arrow_up, color: Colors.greenAccent),
                  onPressed: () => state.sendCommand('WINDOW_OPEN'),
                  tooltip: 'Mở cửa sổ',
                ),
                IconButton(
                  icon: const Icon(Icons.keyboard_arrow_down, color: Colors.redAccent),
                  onPressed: () => state.sendCommand('WINDOW_CLOSE'),
                  tooltip: 'Đóng cửa sổ',
                ),
              ],
            ),
          ),
          const Divider(color: Colors.white12, height: 20),
          // Security control
          _buildControlRow(
            icon: Icons.shield,
            iconColor: state.systemMode == 'SECURITY' ? Colors.cyanAccent : Colors.white38,
            title: 'Hệ thống An ninh',
            subtitle: state.systemMode == 'SECURITY' ? 'Đang kích hoạt' : 'Chưa bật',
            action: ElevatedButton(
              onPressed: () {
                if (state.systemMode == 'SECURITY' || state.systemMode == 'ALARM') {
                  state.sendCommand('SECURITY_OFF');
                } else {
                  state.sendCommand('SECURITY_ON');
                }
              },
              style: ElevatedButton.styleFrom(
                backgroundColor: state.systemMode == 'SECURITY' ? Colors.redAccent : Colors.cyan,
                foregroundColor: Colors.white,
              ),
              child: Text(state.systemMode == 'SECURITY' ? 'Tắt An Ninh' : 'Bật An Ninh'),
            ),
          ),
          const Divider(color: Colors.white12, height: 20),
          // Alarm controls (Buzzer)
          _buildControlRow(
            icon: Icons.notifications_active,
            iconColor: isAlarmOn ? Colors.redAccent : Colors.white38,
            title: 'Còi báo động',
            subtitle: isAlarmOn ? 'ĐANG BÁO ĐỘNG!' : 'Bình thường',
            action: ElevatedButton(
              onPressed: isAlarmOn ? () => state.sendCommand('ALARM_OFF') : null,
              style: ElevatedButton.styleFrom(
                backgroundColor: Colors.red,
                foregroundColor: Colors.white,
                disabledBackgroundColor: Colors.white12,
                disabledForegroundColor: Colors.white30,
              ),
              child: const Text('Tắt còi'),
            ),
          ),
          const Divider(color: Colors.white12, height: 20),
          // Informational displays
          Row(
            mainAxisAlignment: MainAxisAlignment.spaceBetween,
            children: [
              Row(
                children: [
                  const Icon(Icons.lightbulb_outline, color: Colors.amber, size: 16),
                  const SizedBox(width: 8),
                  Text(
                    'Đèn hành lang (Tự động):',
                    style: GoogleFonts.outfit(color: Colors.white70, fontSize: 12),
                  ),
                ],
              ),
              Text(
                isHallLightOn ? 'ON (BẬT)' : 'OFF (TẮT)',
                style: GoogleFonts.outfit(
                  color: isHallLightOn ? Colors.greenAccent : Colors.white38,
                  fontWeight: FontWeight.bold,
                  fontSize: 12,
                ),
              ),
            ],
          ),
        ],
      ),
    );
  }

  Widget _buildControlRow({
    required IconData icon,
    required Color iconColor,
    required String title,
    required String subtitle,
    required Widget action,
  }) {
    return Row(
      children: [
        Icon(icon, color: iconColor, size: 24),
        const SizedBox(width: 12),
        Expanded(
          child: Column(
            crossAxisAlignment: CrossAxisAlignment.start,
            children: [
              Text(
                title,
                style: GoogleFonts.outfit(
                  color: Colors.white,
                  fontWeight: FontWeight.w600,
                  fontSize: 14,
                ),
              ),
              Text(
                subtitle,
                style: GoogleFonts.outfit(
                  color: Colors.white38,
                  fontSize: 12,
                ),
              ),
            ],
          ),
        ),
        action,
      ],
    );
  }

  Widget _buildTerminalLogs(MqttAppState state) {
    return Container(
      padding: const EdgeInsets.all(16),
      decoration: BoxDecoration(
        color: const Color(0x1F1E293B),
        borderRadius: BorderRadius.circular(16),
        border: Border.all(color: const Color(0x1F94A3B8)),
      ),
      child: Column(
        crossAxisAlignment: CrossAxisAlignment.stretch,
        children: [
          Row(
            mainAxisAlignment: MainAxisAlignment.spaceBetween,
            children: [
              Row(
                children: [
                  const Icon(Icons.terminal, color: Colors.greenAccent, size: 18),
                  const SizedBox(width: 8),
                  Text(
                    'NHẬT KÝ SỰ KIỆN HỆ THỐNG',
                    style: GoogleFonts.outfit(
                      color: Colors.white70,
                      fontWeight: FontWeight.bold,
                      fontSize: 13,
                      letterSpacing: 1.0,
                    ),
                  ),
                ],
              ),
              IconButton(
                icon: const Icon(Icons.delete_sweep, color: Colors.white54, size: 18),
                onPressed: () {
                  setState(() {
                    state.eventLogs.clear();
                  });
                },
                tooltip: 'Xóa nhật ký sự kiện',
              )
            ],
          ),
          const SizedBox(height: 12),
          Container(
            height: 150,
            padding: const EdgeInsets.all(8),
            decoration: BoxDecoration(
              color: Colors.black.withOpacity(0.4),
              borderRadius: BorderRadius.circular(8),
              border: Border.all(color: Colors.white12),
            ),
            child: state.eventLogs.isEmpty
                ? Center(
                    child: Text(
                      'Chưa nhận được sự kiện nào...',
                      style: GoogleFonts.outfit(color: Colors.white24, fontSize: 12),
                    ),
                  )
                : ListView.builder(
                    itemCount: state.eventLogs.length,
                    itemBuilder: (context, index) {
                      final log = state.eventLogs[index];
                      // Highlight warning & alarm logs
                      bool isWarn = log.contains('WARNING') || log.contains('DENIED') || log.contains('ALERT') || log.contains('REJECT') || log.contains('INTRUSION');
                      Color logColor = isWarn ? Colors.redAccent : Colors.greenAccent;

                      return Padding(
                        padding: const EdgeInsets.symmetric(vertical: 2.0),
                        child: Row(
                          crossAxisAlignment: CrossAxisAlignment.start,
                          children: [
                            const Text(
                              '> ',
                              style: TextStyle(color: Colors.white30, fontFamily: 'monospace', fontSize: 12),
                            ),
                            Expanded(
                              child: Text(
                                log,
                                style: TextStyle(
                                  color: logColor,
                                  fontFamily: 'monospace',
                                  fontSize: 12,
                                ),
                              ),
                            ),
                          ],
                        ),
                      );
                    },
                  ),
          ),
        ],
      ),
    );
  }
}
