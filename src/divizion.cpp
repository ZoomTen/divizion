#include "divizion_state.hpp"
#include "src/engine/engine.h"
#include "src/ta-log.h"
#include "vst.hpp"
#include <cstdio>
#include <cstdlib>

extern void processReplacing(Vst::AEffect *effect, float **inputs, float **outputs, int32_t sampleFrames);
extern void setParameter(Vst::AEffect *effect, int32_t index, float parameter);
extern float getParameter(Vst::AEffect *effect, int32_t index);
extern intptr_t dispatcher(Vst::AEffect *effect, Vst::VstOpcodeToPlugin opcode, int32_t index, intptr_t value, void *ptr, float opt);

FILE *logger = nullptr;

static DivEngine *newEngine(void);

// i can't C yet i must C++
extern "C"
{
  Vst::AEffect *VSTPluginMain(Vst::AudioMasterCallbackFunc masterCb)
  {
    Vst::AEffect *e = (Vst::AEffect*)(calloc(1, sizeof(Vst::AEffect)));

    // if (!logger)
    // {
    //   logger = fopen("C:\\Divizion.log", "w");
    //   initLog(logger);
    // }
    initLog(stdout);
    
    DivEngine *engine = newEngine(); // requires log to be init first
    DivizionInstance *di = new DivizionInstance(engine);

    e->magic = Vst::kEffectMagic;
    e->dispatcher = dispatcher;
    e->numPrograms = 0;
    e->numParams = 0;
    e->setParameter = setParameter;
    e->getParameter = getParameter;
    e->numInputs = 0;
    e->numOutputs = 2;
    e->flags = Vst::effFlagsIsSynth | Vst::effFlagsCanReplacing | Vst::effFlagsHasEditor | Vst::effFlagsProgramChunks;
    e->object = di;
    e->uniqueID = *(int32_t*)"DIVZ";
    e->version = 1;
    e->processReplacing = processReplacing;
    return e;
  }
}

DivEngine *newEngine(void)
{
  DivEngine *e = new DivEngine();
  if (!e->prePreInit())
  {
    logE("engine stage -2 init failed");
  }
  else
  {
    logI("prepreinit OK");
  }
  e->setAudio(DIV_AUDIO_DUMMY);
  e->setView(DIV_STATUS_NOTHING);
  if (!e->preInit())
  {
    logE("engine stage -1 init failed");
  }
  else
  {
    logI("preinit OK");
  }
  if (!e->init())
  {
    logE("engine stage 0 init failed");
  }
  else
  {
    logI("init OK");
  }
  e->createNew("id0=4", "Game Boy", false);
  return e;
}