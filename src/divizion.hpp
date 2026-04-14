#pragma once

/* This represents a single instance of Divizion, however the plugin shape is */

#include "src/engine/engine.h"

struct Divizion {
  DivEngine* e;
  uint8_t instruments[16]{0};
  int sampleCounter{0};

  int vibspeed[16]{-1};
  int vibdepth[16]{-1};
  int vibdelay[16]{-1};
  int pendingVibratoCounter[16]{-1};
  unsigned char pendingVibratoParam[16]{0};

  Divizion(DivEngine *e);
};