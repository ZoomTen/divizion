#include "src/engine/engine.h"
#include "src/ta-log.h"
#include "vst.hpp"
#include <cstdint>

extern DivEngine engine;

void
processReplacing(Vst::AEffect *effect, float **inputs, float **outputs, int32_t sampleFrames)
{
  engine.nextBuf(inputs, outputs, effect->numInputs, effect->numOutputs, sampleFrames);
}

void reportError(String what) {
  logE("%s",what);
}