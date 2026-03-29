#include "src/engine/engine.h"
#include "src/ta-log.h"
#include "vst.hpp"
#include "divizion_state.hpp"
#include <cstdint>

void
processReplacing(Vst::AEffect *effect, float **inputs, float **outputs, int32_t sampleFrames)
{
  DivizionInstance *di = (DivizionInstance *)effect->object;
  if (!di) return;
  if (!di->engine) return;

  di->engine->nextBuf(inputs, outputs, effect->numInputs, effect->numOutputs, sampleFrames);
}

void reportError(String what)
{
  logE("%s",what);
}