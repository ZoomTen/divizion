#include <cstdint>
#include "src/engine/engine.h"
#include "src/ta-log.h"
#include "src/vst.hpp"

extern DivEngine engine;

int32_t processEvents(Vst::VstEvents *e)
{
  for (int32_t i = 0; i < e->numEvents; i++)
  {
    Vst::VstEvent *event = e->events[i];
    Vst::VstEventTypes etype = event->type;
    
    // nothing but midi events
    if (etype != Vst::kVstMidiType)
    {
      logV("not a midi event: %s", (int)etype);
      continue;
    }
    else
    {
      logV("got midi event");
    }

    Vst::VstMidiEvent *mevent = (Vst::VstMidiEvent *)event;
    char *data = (char*)&mevent->midiData;

    uint8_t status = data[0] & 0xff;
    uint8_t note = data[1] & 0xff;
    uint8_t velocity = data[2] & 0xff;
    logV("status %02x note %02x vel %02x", status, note, velocity);

    switch(status & 0xf0)
    {
    case 0x90:
      if (note > 0)
      {
        logV("sending note on");
        engine.autoNoteOn(status & 0xf, 0, note-12,velocity);
      }
      else
      {
        logV("sending note off");
        engine.autoNoteOff(status & 0xf, note-12,velocity);
      }
      break;
    case 0x80:
      logV("sending note off");
      engine.autoNoteOff(status & 0xf, note-12,velocity);
      break;
    }
  }
  return 1;
}
