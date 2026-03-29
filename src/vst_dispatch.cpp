#include "src/engine/engine.h"
#include "src/gui.hpp"
#include "src/ta-log.h"
#include "src/vst.hpp"
#include "divizion_state.hpp"
#include <cstdint>
#include <cstring>

static int32_t canDo(const char *name);

extern int32_t processEvents(Vst::AEffect *effect, Vst::VstEvents *e);
extern int32_t setupPresetCopyToHost(Vst::AEffect *effect, void **dest, bool isAPreset);
extern int32_t setupPresetLoadFromHost(Vst::AEffect *effect, void *source, size_t size, bool isAPreset);

intptr_t
dispatcher(Vst::AEffect *effect, Vst::VstOpcodeToPlugin opcode, int32_t index, intptr_t value, void *ptr, float opt)
{
  DivizionInstance *di = (DivizionInstance *)effect->object;
  intptr_t result = 0;
  switch (opcode)
  {
  case Vst::effCanDo:
    result = canDo((const char*)ptr);
    break;
  case Vst::effGetChunk: // copy plugin data from plugin to host
    result = setupPresetCopyToHost(effect, (void **)ptr, index==1);
    break;
  case Vst::effSetChunk: // copy plugin data from host to plugin
    result = setupPresetLoadFromHost(effect, ptr, (size_t)value, index==1);
    break;
  case Vst::effProcessEvents:
    result = processEvents(effect, (Vst::VstEvents *)ptr);
    break;
  case Vst::effEditOpen:
    if (di)
    {
      di->g = new Gui(effect);
      if (di->g) di->g->open(ptr);
    }
    break;
  case Vst::effEditGetRect:
    if (di && di->g)
    {
      di->g->getRect((Vst::ERect **)ptr);
    }
    break;
  case Vst::effEditIdle:
    if (di && di->g)
    {
      di->g->idle();
    }
    break;
  case Vst::effEditClose:
    if (di && di->g)
    {
      delete di->g;
    }
  case Vst::effGetVstVersion:
    result = (uint32_t)Vst::kVstVersion;
    break;
  case Vst::effGetNumMidiInputChannels:
    result = 16;
    break;
  case Vst::effGetNumMidiOutputChannels:
    result = 0;
    break;
  case Vst::effSetSampleRate:
    // engine.got / engine.want <- has the audio sample rate
    // need to get that...
    break;
  }
  return result;
}

#define yes_i_can 1
#define i_dunno 0
#define no_i_cannot -1
int32_t canDo(const char *name)
{
  logV("requesting capability: %s", name);
  if (strcmp(name, "receiveVstEvents")) return yes_i_can;
  if (strcmp(name, "receiveVstMidiEvent")) return yes_i_can;
  return no_i_cannot;
}