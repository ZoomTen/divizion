#include "../divizion.hpp"
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
static int loadFile(DivEngine* e, String path);

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
  
  if (!e->prePreInit()) logE("engine stage -2 init failed");
  else logI("prepreinit OK");

  e->setAudio(DIV_AUDIO_DUMMY);
  e->setView(DIV_STATUS_NOTHING);

  if (!e->preInit()) logE("engine stage -1 init failed");
  else logI("preinit OK");

  if (!e->init()) logE("engine stage 0 init failed");
  else logI("init OK");

  e->createNew("id0=4", "Game Boy", false);
  return e;
}

int loadFile(DivEngine* e, String path)
{
  if (!path.empty()) {
    logI("loading module...");
    FILE* f= fopen(path.c_str(), "rb");
    if (f == NULL) {
      perror("error");
      return 1;
    }
    if (fseek(f, 0, SEEK_END) < 0) {
      perror("size error");
      fmt::sprintf(_("on seek: %s"), strerror(errno));
      fclose(f);
      return 1;
    }
    ssize_t len= ftell(f);
    if (len == (SIZE_MAX >> 1)) {
      perror("could not get file length");
      fmt::sprintf(_("on pre tell: %s"), strerror(errno));
      fclose(f);
      return 1;
    }
    if (len < 1) {
      if (len == 0) {
        logE("that file is empty!");
      } else {
        perror("tell error");
        fmt::sprintf(_("on tell: %s"), strerror(errno));
      }
      fclose(f);
      return 1;
    }
    if (fseek(f, 0, SEEK_SET) < 0) {
      perror("size error");
      fmt::sprintf(_("on get size: %s"), strerror(errno));
      fclose(f);
      return 1;
    }
    unsigned char* file= new unsigned char[len];
    if (fread(file, 1, (size_t)len, f) != (size_t)len) {
      perror("read error");
      fmt::sprintf(_("on read: %s"), strerror(errno));
      fclose(f);
      delete[] file;
      return 1;
    }
    fclose(f);
    if (!e->load(file, (size_t)len, path.c_str())) {
      e->getLastError();
      logE("could not open file!");
      return 1;
    }
  }
  return 0;
}