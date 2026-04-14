#include "../divizion.hpp"
#include "src/const.hpp"
#include "src/engine/engine.h"
#include "src/engine/song.h"
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
    
#if 0
    if (!logger)
    {
      logger = fopen("C:\\Divizion.log", "w");
      initLog(logger);
    }
#else
    initLog(stderr);
#endif
    
    DivEngine *engine = newEngine(); // requires log to be init first

    // loadFile(engine, "Z:\\home\\.subhome\\user.zumi\\projects\\divizion2\\cerulean.fur");
    Divizion *di = new Divizion(engine);

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
  
  e->curSubSong->hz = tickRate;
  e->setConf("lowLatency", 1);
  e->setAudio(DIV_AUDIO_DUMMY);
  e->setView(DIV_STATUS_NOTHING);

  if (!e->preInit()) logE("engine stage -1 init failed");
  else logI("preinit OK");

  if (!e->init()) logE("engine stage 0 init failed");
  else logI("init OK");

  e->createNew("id0=4", "Game Boy", false);

  // run "song" to make effects work
  e->song.subsong[0]->patLen=1;
  e->song.subsong[0]->speeds.val[0]=1;
  e->song.subsong[0]->virtualTempoN = 32767;
  e->song.subsong[0]->virtualTempoD = 1;
  DivPattern *p = e->song.subsong[0]->pat[0].getPattern(0, true);
  e->song.subsong[0]->pat[0].effectCols = EffectColumn::ESLOT_MAX;
  // vibrato shape = square
  p->data[0][PatternSlot(EffectColumn::ESLOT_VIBRATO_SHAPE, false)]=0xe3;
  p->data[0][PatternSlot(EffectColumn::ESLOT_VIBRATO_SHAPE, true)]=0x06;
  e->play();
  return e;
}
