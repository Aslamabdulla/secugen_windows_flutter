#ifndef FLUTTER_PLUGIN_SECUGEN_WINDOWS_PLUGIN_H_
#define FLUTTER_PLUGIN_SECUGEN_WINDOWS_PLUGIN_H_

#include <flutter/method_channel.h>
#include <flutter/plugin_registrar_windows.h>

#include <memory>
#include <vector>

#include "sgfplib.h"

namespace secugen_windows {

class SecugenWindowsPlugin : public flutter::Plugin {
 public:
  static void RegisterWithRegistrar(flutter::PluginRegistrarWindows *registrar);

  SecugenWindowsPlugin();
  virtual ~SecugenWindowsPlugin();

  // Disallow copy and assign.
  SecugenWindowsPlugin(const SecugenWindowsPlugin&) = delete;
  SecugenWindowsPlugin& operator=(const SecugenWindowsPlugin&) = delete;

 private:
  // Called when a method is called on this plugin's channel from Dart.
  void HandleMethodCall(
      const flutter::MethodCall<flutter::EncodableValue> &method_call,
      std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result);

  bool Initialize();
  bool Open(int deviceId);
  bool Close();
  bool Terminate();
  std::vector<uint8_t> CaptureFingerprint();
  int GetImageQuality();
  bool SetBrightness(int brightness);
  std::vector<uint8_t> CreateTemplate(const std::vector<uint8_t>& image);
  bool MatchTemplate(const std::vector<uint8_t>& template1, const std::vector<uint8_t>& template2);
  void EnableAutoOn(bool enable);

  static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
  void HandleAutoOnEvent(WPARAM wParam, LPARAM lParam);

  HSGFPM m_hFpm;
  bool m_initialized;
  HWND m_hWnd;
  std::unique_ptr<flutter::MethodChannel<flutter::EncodableValue>> m_channel;
};

}  // namespace secugen_windows

#endif  // FLUTTER_PLUGIN_SECUGEN_WINDOWS_PLUGIN_H_