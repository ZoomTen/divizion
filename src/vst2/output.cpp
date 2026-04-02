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

  di->e->nextBuf(inputs, outputs, effect->numInputs, effect->numOutputs, sampleFrames);
}

void reportError(String what)
{
  logE("%s",what);
}