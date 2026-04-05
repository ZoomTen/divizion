#include "imgui.h"
#include "text_input_widget.hpp"

#ifdef WIN32
  #include <minwindef.h>
  #include <windef.h>
  #include <windows.h>
  #include <winnt.h>
  #include "res.h"

extern "C" IMAGE_DOS_HEADER __ImageBase;
  #define HINST_THISCOMPONENT ((HINSTANCE) & __ImageBase)

struct InputDialogParams {
  const char* title;
  const char* initial;
  char* result;
  size_t resultSize;
};

static INT_PTR CALLBACK InputDialogProc(HWND dlg, UINT msg, WPARAM wParam,
                                        LPARAM lParam);

static bool WindowsInputBox(const char* title, char* buf, size_t bufSize,
                            HWND parent);
#endif

bool InputText(const char* label, std::string* str)
{
#ifdef WIN32
  std::string compositeLabel = *str + "##" + std::string(label);
  bool r = ImGui::Button(compositeLabel.c_str());
  if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort)) {
    ImGui::SetTooltip("click here to change");
  }
  if (r) {
    char buf[256];
    strncpy(buf, str->c_str(), sizeof(buf));
    buf[sizeof(buf) - 1] = '\0';
    HWND parent = (HWND)ImGui::GetMainViewport()->PlatformHandleRaw;
    if (WindowsInputBox(label, buf, sizeof(buf), parent)) *str = buf;
  }
  return r;
#else
  std::string compositeLabel = "##" + std::string(label);
  return ImGui::InputText(compositeLabel.c_str(), str);
#endif
}

#ifdef WIN32
bool WindowsInputBox(const char* title, char* buf, size_t bufSize, HWND parent)
{
  InputDialogParams p{ title, buf, buf, bufSize };
  INT_PTR i = DialogBoxParamA(HINST_THISCOMPONENT, MAKEINTRESOURCEA(IDD_INPUT),
                              parent, InputDialogProc, (LPARAM)&p);
  return i == IDOK;
}

INT_PTR CALLBACK InputDialogProc(HWND dlg, UINT msg, WPARAM wParam,
                                 LPARAM lParam)
{
  switch (msg) {
  case WM_INITDIALOG: {
    auto* p = reinterpret_cast<InputDialogParams*>(lParam);
    SetWindowTextA(dlg, p->title);
    SetWindowLongPtrA(dlg, GWLP_USERDATA, (LONG_PTR)p);
    SetDlgItemTextA(dlg, IDC_INPUT, p->initial);
    SendDlgItemMessageA(dlg, IDC_INPUT, EM_SETSEL, 0, -1);
    return TRUE;
  }
  case WM_COMMAND:
    switch (LOWORD(wParam)) {
    case IDOK: {
      auto* p = reinterpret_cast<InputDialogParams*>(
        GetWindowLongPtrA(dlg, GWLP_USERDATA));
      GetDlgItemTextA(dlg, IDC_INPUT, p->result, (int)p->resultSize);
      EndDialog(dlg, IDOK);
      return TRUE;
    }
    case IDCANCEL:
      EndDialog(dlg, IDCANCEL);
      return TRUE;
    }
    break;
  }
  return FALSE;
}
#endif