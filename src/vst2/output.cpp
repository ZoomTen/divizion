#include "src/const.hpp"
#include "src/engine/engine.h"
#include "src/ta-log.h"
#include "vst.hpp"
#include "../divizion.hpp"
#include <cstdint>

void
processReplacing(Vst::AEffect *effect, float **inputs, float **outputs, int32_t sampleFrames)
{
  Divizion *di = (Divizion *)effect->object;
  if (!di) return;
  if (!di->e) return;
  // wish i can send nextBuf AND process my ticks at the same time but alas.
  // a bug currently remains here: sometimes the vibrato would continue into
  // the next note for a single tick.
  for (int32_t i = 0; i < sampleFrames; i++)
  {
    if (di->sampleCounter >= samplesPerTick)
    {
      // do tick
      di->sampleCounter = 0;
      for (int c = 0; c < sizeof(di->pendingVibratoCounter)/sizeof(di->pendingVibratoCounter[0]);c++)
      {
        di->pendingVibratoCounter[c]--;
        if (di->pendingVibratoCounter[c] < 1)
        {
          DivPattern *p = di->e->song.subsong[0]->pat[c].getPattern(0, true);
          p->data[0][PatternSlot(EffectColumn::ESLOT_VIBRATO_INTENSITY, false)]=0x4;
          p->data[0][PatternSlot(EffectColumn::ESLOT_VIBRATO_INTENSITY, true)]=di->pendingVibratoParam[c];
          di->pendingVibratoCounter[c] = 0;
        }
      }
    }
    di->sampleCounter++;
  }
  di->e->nextBuf(inputs, outputs, effect->numInputs, effect->numOutputs, sampleFrames);
}

void reportError(String what)
{
  logE("%s",what);
}