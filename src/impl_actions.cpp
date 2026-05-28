#include "impl_actions.hpp"
#include "imgui.h"
#include "imgui_internal.h"
#include "src/engine/instrument.h"
#include "src/engine/sample.h"
#include "src/engine/wavetable.h"
#include "src/interface_actions.hpp"
#include <cstdio>

static int loadFile(DivEngine* e, String path);
static int saveFile(DivEngine* e, String path);

DivizionActionsImpl::DivizionActionsImpl(DivEngine* e)
{
  this->e = e;
  this->guiErrorMessage = "";
}

vector<DivInstrument*> DivizionActionsImpl::getInstrumentList()
{
  return this->e->song.ins;
}

vector<DivWavetable*> DivizionActionsImpl::getWavetables()
{
  return this->e->song.wave;
}

vector<DivSample*> DivizionActionsImpl::getSamples()
{
  return this->e->song.sample;
}

void DivizionActionsImpl::panic()
{
  this->e->syncReset();
}

void DivizionActionsImpl::drawChanInfo()
{
  // TODO: able to map MIDI channels to chip channels
  if (ImGui::BeginTable("SystemList", 2)) {
    // ImGui::TableSetupColumn("c1", ImGuiTableColumnFlags_WidthFixed);
    ImGui::TableSetupColumn("c2", ImGuiTableColumnFlags_WidthFixed);
    ImGui::TableSetupColumn("c3", ImGuiTableColumnFlags_WidthFixed);
    ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
    // ImGui::TableNextColumn();
    ImGui::TableNextColumn();
    ImGui::Text("MIDI");
    ImGui::TableNextColumn();
    ImGui::Text("Chip");
    for (int i = 0; i < 16; i++) {
      ImGui::PushID(i);
      ImGui::TableNextRow();
      // ImGui::TableNextColumn();
      // ImGui::Button(ICON_FA_ARROWS);
      ImGui::TableNextColumn();
      ImGui::Text("  %d  ", i + 1);
      ImGui::TableNextColumn();
      ImGui::Text("%s #%d", e->getSystemName(e->sysOfChan[i]),
                  e->dispatchChanOfChan[i]);
      ImGui::PopID();
    }
    ImGui::EndTable();
  }
}

void DivizionActionsImpl::loadPrjFile(std::string name)
{
  loadFile(this->e, name);
}

void DivizionActionsImpl::savePrjFile(std::string name)
{
  saveFile(this->e, name);
}

int loadFile(DivEngine* e, String path)
{
  if (!path.empty()) {
    logI("loading module...");
    FILE* f = fopen(path.c_str(), "rb");
    if (f == NULL) {
      perror("error");
      return 1;
    }
    if (fseek(f, 0, SEEK_END) < 0) {
      perror("size error");
      fmt::sprintf(_("on seek: %s"), strerror(errno));
      fclose(f);
      return 1;
    }
    ssize_t len = ftell(f);
    if (len == (SIZE_MAX >> 1)) {
      perror("could not get file length");
      fmt::sprintf(_("on pre tell: %s"), strerror(errno));
      fclose(f);
      return 1;
    }
    if (len < 1) {
      if (len == 0) {
        logE("that file is empty!");
      } else {
        perror("tell error");
        fmt::sprintf(_("on tell: %s"), strerror(errno));
      }
      fclose(f);
      return 1;
    }
    if (fseek(f, 0, SEEK_SET) < 0) {
      perror("size error");
      fmt::sprintf(_("on get size: %s"), strerror(errno));
      fclose(f);
      return 1;
    }
    unsigned char* file = new unsigned char[len];
    if (fread(file, 1, (size_t)len, f) != (size_t)len) {
      perror("read error");
      fmt::sprintf(_("on read: %s"), strerror(errno));
      fclose(f);
      delete[] file;
      return 1;
    }
    fclose(f);
    if (!e->load(file, (size_t)len, path.c_str())) {
      e->getLastError();
      logE("could not open file!");
      return 1;
    }
  }
  return 0;
}

int saveFile(DivEngine* e, String path)
{
  SafeWriter* w;
  String lastError;
  logD("saving file...");
  w = e->saveFur(false, true);
  if (w == NULL) {
    lastError = e->getLastError();
    logE("couldn't save! %s", lastError);
    return 3;
  }
  logV("opening file for writing...");
  FILE* outFile = fopen(path.c_str(), "wb");
  if (outFile == NULL) {
    lastError = strerror(errno);
    logE("couldn't save! %s", lastError);
    w->finish();
    return 1;
  }
  if (fwrite(w->getFinalBuf(), 1, w->size(), outFile) != w->size()) {
    logE("did not write entirely: %s!", strerror(errno));
    lastError = strerror(errno);
    fclose(outFile);
    w->finish();
    return 1;
  }
  fclose(outFile);
  w->finish();
  logD("save complete.");
  return 0;
}