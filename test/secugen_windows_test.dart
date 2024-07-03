import 'package:flutter_test/flutter_test.dart';
import 'package:secugen_windows/secugen_windows.dart';
import 'package:secugen_windows/secugen_windows_platform_interface.dart';
import 'package:secugen_windows/secugen_windows_method_channel.dart';
import 'package:plugin_platform_interface/plugin_platform_interface.dart';

class MockSecugenWindowsPlatform
    with MockPlatformInterfaceMixin
    implements SecugenWindowsPlatform {

  @override
  Future<String?> getPlatformVersion() => Future.value('42');
}

void main() {
  final SecugenWindowsPlatform initialPlatform = SecugenWindowsPlatform.instance;

  test('$MethodChannelSecugenWindows is the default instance', () {
    expect(initialPlatform, isInstanceOf<MethodChannelSecugenWindows>());
  });

  test('getPlatformVersion', () async {
    SecugenWindows secugenWindowsPlugin = SecugenWindows();
    MockSecugenWindowsPlatform fakePlatform = MockSecugenWindowsPlatform();
    SecugenWindowsPlatform.instance = fakePlatform;

    expect(await secugenWindowsPlugin.getPlatformVersion(), '42');
  });
}
