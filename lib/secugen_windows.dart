import 'dart:async';
import 'dart:typed_data';

import 'package:flutter/services.dart';

class SecugenWindows {
  static const MethodChannel _channel = MethodChannel('secugen_windows');
  // static const EventChannel _eventChannel = EventChannel('secugen_windows/events');

  static Future<bool> initialize() async {
    return await _channel.invokeMethod('initialize');
  }

  static Future<bool> open(int deviceId) async {
    return await _channel.invokeMethod('open', {'deviceId': deviceId});
  }

  static Future<bool> close() async {
    return await _channel.invokeMethod('close');
  }

  static Future<bool> terminate() async {
    return await _channel.invokeMethod('terminate');
  }

  static Future<Uint8List> captureFingerprint() async {
    return await _channel.invokeMethod('captureFingerprint');
  }

  static Future<int> getImageQuality() async {
    return await _channel.invokeMethod('getImageQuality');
  }

  static Future<bool> setBrightness(int brightness) async {
    return await _channel.invokeMethod('setBrightness', {'brightness': brightness});
  }

  static Future<Uint8List> createTemplate(Uint8List image) async {
    return await _channel.invokeMethod('createTemplate', {'image': image});
  }

  static Future<bool> matchTemplate(Uint8List template1, Uint8List template2) async {
    return await _channel.invokeMethod('matchTemplate', {'template1': template1, 'template2': template2});
  }

  static Future<void> enableAutoOn(bool enable) async {
    await _channel.invokeMethod('enableAutoOn', {'enable': enable});
  }


}