import 'package:flutter/material.dart';
import 'package:provider/provider.dart';
import 'mqtt_app_state.dart';
import 'dashboard_screen.dart';

void main() {
  runApp(
    ChangeNotifierProvider(
      create: (_) => MqttAppState(),
      child: const MainApp(),
    ),
  );
}

class MainApp extends StatelessWidget {
  const MainApp({super.key});

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'Smart Room Dashboard',
      debugShowCheckedModeBanner: false,
      theme: ThemeData(
        brightness: Brightness.dark,
        primarySwatch: Colors.blue,
      ),
      home: const DashboardScreen(),
    );
  }
}
