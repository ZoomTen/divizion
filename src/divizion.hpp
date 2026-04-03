#pragma once

/* This represents a single instance of Divizion, however the plugin shape is */

#include "src/engine/engine.h"

struct Divizion {
  DivEngine* e;
  uint8_t instruments[16]{0};

  Divizion(DivEngine *e);
};