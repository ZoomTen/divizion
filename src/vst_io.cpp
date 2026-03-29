#include "src/ta-log.h"
#include <cstdio>
#include <cstring>
#include "divizion_state.hpp"

int32_t setupPresetCopyToHost(Vst::AEffect *effect, void **dest, bool isAPreset)
{
  DivizionInstance *di = (DivizionInstance *)effect->object;
  if (!di) return 0;

  logV("setupPresetCopyToHost dest:%p, preset?%d", (void*)dest,isAPreset);
  memset(di->state, 0, sizeof(di->state));
  sprintf(di->state, isAPreset?"preset!":"bank!");

  *dest = di->state;
  logV("setup done");
  return sizeof(di->state);
}

int32_t setupPresetLoadFromHost(Vst::AEffect *effect, void *source, size_t size, bool isAPreset)
{
  DivizionInstance *di = (DivizionInstance *)effect->object;
  if (!di) return 0;
  
  logV("setupPresetLoadFromHost source:%p, size:%d, preset?%d", source,size,isAPreset);
  memcpy(di->state, source, size>sizeof(di->state)?sizeof(di->state):size);
  logV("load done, state is: %s", di->state);
  return 1;
}