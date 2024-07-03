#include "include/secugen_windows/secugen_windows_plugin.h"

#include <flutter/method_channel.h>
#include <flutter/plugin_registrar_windows.h>
#include <flutter/standard_method_codec.h>

#include <map>
#include <memory>
#include <sstream>

namespace secugen_windows {

// static
void SecugenWindowsPlugin::RegisterWithRegistrar(
    flutter::PluginRegistrarWindows *registrar) {
  auto channel =
      std::make_unique<flutter::MethodChannel<flutter::EncodableValue>>(
          registrar->messenger(), "secugen_windows",
          &flutter::StandardMethodCodec::GetInstance());

  auto plugin = std::make_unique<SecugenWindowsPlugin>();

  channel->SetMethodCallHandler(
      [plugin_pointer = plugin.get()](const auto &call, auto result) {
        plugin_pointer->HandleMethodCall(call, std::move(result));
      });

  registrar->AddPlugin(std::move(plugin));
}

SecugenWindowsPlugin::SecugenWindowsPlugin() : m_hFpm(NULL), m_initialized(false) {
  // Create a hidden window for handling auto-on messages
  WNDCLASS wc = {0};
  wc.lpfnWndProc = WndProc;
  wc.hInstance = GetModuleHandle(NULL);
  wc.lpszClassName = L"SecuGenAutoOnWindow";
  RegisterClass(&wc);

  m_hWnd = CreateWindow(L"SecuGenAutoOnWindow", L"SecuGenAutoOnWindow", 0, 0, 0, 0, 0, HWND_MESSAGE, NULL, GetModuleHandle(NULL), this);
}

SecugenWindowsPlugin::~SecugenWindowsPlugin() {
  if (m_initialized) {
    Close();
    Terminate();
  }
  if (m_hWnd) {
    DestroyWindow(m_hWnd);
  }
}

void SecugenWindowsPlugin::HandleMethodCall(
    const flutter::MethodCall<flutter::EncodableValue> &method_call,
    std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result) {
  if (method_call.method_name().compare("initialize") == 0) {
    bool success = Initialize();
    result->Success(flutter::EncodableValue(success));
  } else if (method_call.method_name().compare("open") == 0) {
    const auto* arguments = std::get_if<flutter::EncodableMap>(method_call.arguments());
    int deviceId = std::get<int>(arguments->at(flutter::EncodableValue("deviceId")));
    bool success = Open(deviceId);
    result->Success(flutter::EncodableValue(success));
  } else if (method_call.method_name().compare("close") == 0) {
    bool success = Close();
    result->Success(flutter::EncodableValue(success));
  } else if (method_call.method_name().compare("terminate") == 0) {
    bool success = Terminate();
    result->Success(flutter::EncodableValue(success));
  } else if (method_call.method_name().compare("captureFingerprint") == 0) {
    std::vector<uint8_t> fingerprintData = CaptureFingerprint();
    result->Success(flutter::EncodableValue(fingerprintData));
  } else if (method_call.method_name().compare("getImageQuality") == 0) {
    int quality = GetImageQuality();
    result->Success(flutter::EncodableValue(quality));
  } else if (method_call.method_name().compare("setBrightness") == 0) {
    const auto* arguments = std::get_if<flutter::EncodableMap>(method_call.arguments());
    int brightness = std::get<int>(arguments->at(flutter::EncodableValue("brightness")));
    bool success = SetBrightness(brightness);
    result->Success(flutter::EncodableValue(success));
  } else if (method_call.method_name().compare("createTemplate") == 0) {
    const auto* arguments = std::get_if<flutter::EncodableMap>(method_call.arguments());
    std::vector<uint8_t> image = std::get<std::vector<uint8_t>>(arguments->at(flutter::EncodableValue("image")));
    std::vector<uint8_t> templateData = CreateTemplate(image);
    result->Success(flutter::EncodableValue(templateData));
  } else if (method_call.method_name().compare("matchTemplate") == 0) {
    const auto* arguments = std::get_if<flutter::EncodableMap>(method_call.arguments());
    std::vector<uint8_t> template1 = std::get<std::vector<uint8_t>>(arguments->at(flutter::EncodableValue("template1")));
    std::vector<uint8_t> template2 = std::get<std::vector<uint8_t>>(arguments->at(flutter::EncodableValue("template2")));
    bool match = MatchTemplate(template1, template2);
    result->Success(flutter::EncodableValue(match));
  } else if (method_call.method_name().compare("enableAutoOn") == 0) {
    const auto* arguments = std::get_if<flutter::EncodableMap>(method_call.arguments());
    bool enable = std::get<bool>(arguments->at(flutter::EncodableValue("enable")));
    EnableAutoOn(enable);
    result->Success(nullptr);
  } else {
    result->NotImplemented();
  }
}

bool SecugenWindowsPlugin::Initialize() {
  if (m_initialized) return true;

  DWORD err = SGFPM_Create(&m_hFpm);
  if (err != SGFPM_OK) return false;

  err = SGFPM_Init(m_hFpm);
  if (err != SGFPM_OK) {
    SGFPM_Terminate(m_hFpm);
    return false;
  }

  m_initialized = true;
  return true;
}

bool SecugenWindowsPlugin::Open(int deviceId) {
  if (!m_initialized) return false;

  DWORD err = SGFPM_OpenDevice(m_hFpm, deviceId);
  return (err == SGFPM_OK);
}

bool SecugenWindowsPlugin::Close() {
  if (!m_initialized) return false;

  DWORD err = SGFPM_CloseDevice(m_hFpm);
  return (err == SGFPM_OK);
}

bool SecugenWindowsPlugin::Terminate() {
  if (!m_initialized) return false;

  DWORD err = SGFPM_Terminate(m_hFpm);
  m_initialized = false;
  return (err == SGFPM_OK);
}

std::vector<uint8_t> SecugenWindowsPlugin::CaptureFingerprint() {
  if (!m_initialized) return std::vector<uint8_t>();

  DWORD width, height;
  SGFPM_GetDeviceInfo(m_hFpm, &width, &height, NULL, NULL, NULL, NULL);

  std::vector<uint8_t> imageBuffer(width * height);
  DWORD err = SGFPM_GetImage(m_hFpm, imageBuffer.data());
  if (err != SGFPM_OK) return std::vector<uint8_t>();

  return imageBuffer;
}

int SecugenWindowsPlugin::GetImageQuality() {
  if (!m_initialized) return -1;

  DWORD quality;
  DWORD err = SGFPM_GetImageQuality(m_hFpm, 0, 0, NULL, &quality);
  if (err != SGFPM_OK) return -1;

  return static_cast<int>(quality);
}

bool SecugenWindowsPlugin::SetBrightness(int brightness) {
  if (!m_initialized) return false;

  DWORD err = SGFPM_SetBrightness(m_hFpm, brightness);
  return (err == SGFPM_OK);
}

std::vector<uint8_t> SecugenWindowsPlugin::CreateTemplate(const std::vector<uint8_t>& image) {
  if (!m_initialized) return std::vector<uint8_t>();

  DWORD templateSize;
  SGFPM_GetMaxTemplateSize(m_hFpm, &templateSize);

  std::vector<uint8_t> templateBuffer(templateSize);
  DWORD err = SGFPM_CreateTemplate(m_hFpm, NULL, image.data(), templateBuffer.data());
  if (err != SGFPM_OK) return std::vector<uint8_t>();

  return templateBuffer;
}

bool SecugenWindowsPlugin::MatchTemplate(const std::vector<uint8_t>& template1, const std::vector<uint8_t>& template2) {
  if (!m_initialized) return false;

  BOOL matched;
  DWORD err = SGFPM_MatchTemplate(m_hFpm, const_cast<BYTE*>(template1.data()), const_cast<BYTE*>(template2.data()), SGFPM_DEFAULT_MATCH_SCORE, &matched);
  if (err != SGFPM_OK) return false;

  return matched == TRUE;
}

void SecugenWindowsPlugin::EnableAutoOn(bool enable) {
  if (!m_initialized) return;

  if (enable) {
    SGFPM_EnableAutoOnEvent(m_hFpm, TRUE, m_hWnd, WM_USER_SGAUTO_ON_EVENT);
  } else {
    SGFPM_EnableAutoOnEvent(m_hFpm, FALSE, NULL, 0);
  }
}

LRESULT CALLBACK SecugenWindowsPlugin::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
  if (message == WM_USER_SGAUTO_ON_EVENT) {
    SecugenWindowsPlugin* plugin = reinterpret_cast<SecugenWindowsPlugin*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
    if (plugin) {
      plugin->HandleAutoOnEvent(wParam, lParam);
    }
    return 0;
  }

  return DefWindowProc(hWnd, message, wParam, lParam);
}

void SecugenWindowsPlugin::HandleAutoOnEvent(WPARAM wParam, LPARAM lParam) {
  // Capture the fingerprint and send it to Flutter
  std::vector<uint8_t> fingerprintData = CaptureFingerprint();
  if (!fingerprintData.empty()) {
    m_channel->InvokeMethod("onFingerprintCaptured", std::make_unique<flutter::EncodableValue>(fingerprintData));
  }
}

}  // namespace secugen_windows

void SecugenWindowsPluginRegisterWithRegistrar(
    FlutterDesktopPluginRegistrarRef registrar) {
  secugen_windows::SecugenWindowsPlugin::RegisterWithRegistrar(
      flutter::PluginRegistrarManager::GetInstance()
          ->GetRegistrar<flutter::PluginRegistrarWindows>(registrar));
}