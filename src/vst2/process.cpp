#include <cstdint>
#include "src/const.hpp"
#include "src/engine/dispatch.h"
#include "vst.hpp"
#include "../divizion.hpp"
#include "src/engine/engine.h"
#include "src/ta-log.h"

static void triggerCC(Divizion* self, unsigned char cc, unsigned char value,
               unsigned char chan);
static unsigned char range127to7(unsigned char v);
static unsigned char range127to15(unsigned char v);

int32_t processEvents(Vst::AEffect* effect, Vst::VstEvents* e)
{
  Divizion* di = (Divizion*)effect->object;
  if (!di) return 0;
  if (!di->e) return 0;

  for (int32_t i = 0; i < e->numEvents; i++) {
    Vst::VstEvent* event = e->events[i];
    Vst::VstEventTypes etype = event->type;

    // nothing but midi events
    if (etype != Vst::kVstMidiType) {
      // logV("not a midi event: %s", (int)etype);
      continue;
    } else {
      // logV("got midi event");
    }

    Vst::VstMidiEvent* mevent = (Vst::VstMidiEvent*)event;
    char* data = (char*)&mevent->midiData;

    uint8_t status = data[0] & 0xff;
    uint8_t note = data[1] & 0xff;
    uint8_t velocity = data[2] & 0xff;
    // logV("status %02x note %02x vel %02x", status, note, velocity);

    uint8_t chan = status & 0xf;
    switch (status & 0xf0) {
    case 0x90:
      if (note > 0) {
        // logV("sending note on");
        di->e->setMidiVolExp(1.0);
        di->pendingVibratoCounter[chan] = di->vibdelay[chan];
        // reset vib
        DivPattern *p = di->e->song.subsong[0]->pat[chan].getPattern(0, true);
        p->data[0][PatternSlot(EffectColumn::ESLOT_VIBRATO_INTENSITY, false)] = 0x4;
        p->data[0][PatternSlot(EffectColumn::ESLOT_VIBRATO_INTENSITY, true)] = 0x0;
        //
        di->e->noteOn(chan, di->instruments[chan], note - 12, velocity);
      } else {
        // logV("sending note off");
        di->e->noteOff(chan);
      }
      break;
    case 0x80:
      // logV("sending note off");
      di->e->noteOff(chan);
      break;
    case 0xE0: // pitch bend
    {
      uint16_t bend14bit = (data[2] << 7) | data[1];
      int bendValue = (int)bend14bit - 8192;
      di->e->dispatchCmd(DivCommand(DIV_CMD_PITCH, chan, bendValue));
      break;
    }
    case 0xC0: // program change
    {
      unsigned char newinst = data[1];
      di->instruments[chan] = newinst;
      break;
    }
    case 0xB0: // CC
    {
      triggerCC(di, data[1], data[2], chan);
      break;
    }
    }
  }
  return 1;
}

void triggerCC(Divizion* self, unsigned char cc, unsigned char value,
               unsigned char chan)
{
  switch (cc) {
  case 76: // vib speed
    self->vibspeed[chan] = value;
    self->pendingVibratoParam[chan] = ((range127to15(value) & 0x0f) << 4) + (self->pendingVibratoParam[chan] & 0x0f);
    break;
  case 77: // vib depth
    self->vibdepth[chan] = value;
    self->pendingVibratoParam[chan] = ((range127to15(value) & 0x0f)) + (self->pendingVibratoParam[chan] & 0xf0);
    break;
  case 78: // vib delay
    self->vibdelay[chan] = value;
    break;
  default:
    break;
  }
}

unsigned char range127to15(unsigned char v){
  return (unsigned char)((double)v / (double)127.0 * (double)15.0);
}