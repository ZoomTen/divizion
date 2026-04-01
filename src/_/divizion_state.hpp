#include "src/engine/engine.h"
#include "src/gui.hpp"

struct DivizionInstance
{
  char state[100]{0};
  Gui *g{nullptr};
  DivEngine *engine{};

  DivizionInstance(DivEngine *);
};