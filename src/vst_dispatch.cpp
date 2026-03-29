#include "src/engine/engine.h"
#include "src/gui.hpp"
#include "src/vst.hpp"
#include <cstdint>
#include <cstring>

extern Gui *gui;
extern DivEngine engine;

extern int32_t processEvents(Vst::VstEvents *e);
static int32_t canDo(const char *name);

intptr_t
dispatcher(Vst::AEffect *effect, Vst::VstOpcodeToPlugin opcode, int32_t index, intptr_t value, void *ptr, float opt)
{
  intptr_t result = 0;
  switch (opcode)
  {
  case Vst::effCanDo:
    result = canDo((const char*)ptr);
    break;
  case Vst::effProcessEvents:
    result = processEvents((Vst::VstEvents *)ptr);
    break;
  case Vst::effEditOpen:
    if (gui != nullptr)
      gui->open(ptr);
    break;
  case Vst::effEditGetRect:
    if (gui != nullptr)
      gui->getRect((Vst::ERect **)ptr);
    break;
  case Vst::effEditIdle:
    if (gui != nullptr)
      gui->idle();
    break;
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
  if (strcmp(name, "receiveVstEvents")) return yes_i_can;
  if (strcmp(name, "receiveVstMidiEvent")) return yes_i_can;
  return no_i_cannot;
}