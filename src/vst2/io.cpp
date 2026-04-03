#include "src/divizion.hpp"
#include "src/engine/engine.h"
#include "src/engine/safeWriter.h"
#include <cstdio>
#include <cstring>
#include "vst.hpp"

int32_t setupPresetCopyToHost(Vst::AEffect *effect, void **dest, bool isAPreset)
{
  Divizion *di = (Divizion *)effect->object;
  if (!di) return 0;

  SafeWriter *w = di->e->saveFur(false, true);
  *dest = w->getFinalBuf();
  return w->size();
}

int32_t setupPresetLoadFromHost(Vst::AEffect *effect, void *source, size_t size, bool isAPreset)
{
  Divizion *di = (Divizion *)effect->object;
  if (!di) return 0;

  return (int32_t)di->e->load((unsigned char *)source, size);
}