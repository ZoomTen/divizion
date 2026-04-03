#include "file_dialog.hpp"
#include "src/ta-log.h"

#ifdef WIN32
  #include <windows.h>
#endif

std::string getFileName(bool forSaving)
{
#ifdef WIN32
  OPENFILENAMEW ofn;
  wchar_t szFile[MAX_PATH] = { 0 };
  ZeroMemory(&ofn, sizeof(ofn));
  ofn.lStructSize = sizeof(ofn);
  ofn.hwndOwner = NULL;
  ofn.lpstrFile = szFile;
  ofn.nMaxFile = sizeof(szFile) / sizeof(wchar_t);
  ofn.lpstrFilter = L"all files\0*.*\0";
  ofn.nFilterIndex = 1;
  if (forSaving)
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;
  else ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
  if (forSaving) {
    if (GetSaveFileNameW(&ofn)) {
      int size_needed =
        WideCharToMultiByte(CP_UTF8, 0, ofn.lpstrFile, -1, NULL, 0, NULL, NULL);
      std::string utf8Str(size_needed - 1, 0);
      WideCharToMultiByte(CP_UTF8, 0, ofn.lpstrFile, -1, &utf8Str[0],
                          size_needed, NULL, NULL);
      return utf8Str;
    }
  } else {
    if (GetOpenFileNameW(&ofn)) {
      int size_needed =
        WideCharToMultiByte(CP_UTF8, 0, ofn.lpstrFile, -1, NULL, 0, NULL, NULL);
      std::string utf8Str(size_needed - 1, 0);
      WideCharToMultiByte(CP_UTF8, 0, ofn.lpstrFile, -1, &utf8Str[0],
                          size_needed, NULL, NULL);
      return utf8Str;
    }
  }
#else
  logE("file dialog not yet supported");
#endif

  // TODO: use NFD or something to get file dialogs on Loonix

  return "";
}