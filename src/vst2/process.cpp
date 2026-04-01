#include <cstdint>
#include "src/engine/engine.h"
#include "src/ta-log.h"
#include "src/vst.hpp"
#include "divizion_state.hpp"

int32_t processEvents(Vst::AEffect *effect, Vst::VstEvents *e)
{
  DivizionInstance *di = (DivizionInstance *)effect->object;
  if (!di) return 0;
  if (!di->engine) return 0;

  for (int32_t i = 0; i < e->numEvents; i++)
  {
    Vst::VstEvent *event = e->events[i];
    Vst::VstEventTypes etype = event->type;
    
    // nothing but midi events
    if (etype != Vst::kVstMidiType)
    {
      // logV("not a midi event: %s", (int)etype);
      continue;
    }
    else
    {
      // logV("got midi event");
    }

    Vst::VstMidiEvent *mevent = (Vst::VstMidiEvent *)event;
    char *data = (char*)&mevent->midiData;

    uint8_t status = data[0] & 0xff;
    uint8_t note = data[1] & 0xff;
    uint8_t velocity = data[2] & 0xff;
    // logV("status %02x note %02x vel %02x", status, note, velocity);

    switch(status & 0xf0)
    {
    case 0x90:
      if (note > 0)
      {
        // logV("sending note on");
        di->engine->noteOn(status & 0xf, 0, note-12,velocity);
      }
      else
      {
        // logV("sending note off");
        di->engine->noteOff(status & 0xf);
      }
      break;
    case 0x80:
      // logV("sending note off");
      di->engine->noteOff(status & 0xf);
      break;
    }
  }
  return 1;
}
