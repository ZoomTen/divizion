#pragma once


#include "src/engine/engine.h"
#include <vector>


struct FurnaceGUISysDefChip {
  DivSystem sys;
  float vol, pan, panFR;
  String flags;
  FurnaceGUISysDefChip(DivSystem s, float v, float p, const char* f, float pf=0.0):
    sys(s),
    vol(v),
    pan(p),
    panFR(pf),
    flags(f) {}
};

struct FurnaceGUISysDef {
  String name;
  String extra;
  String definition;
  std::vector<FurnaceGUISysDefChip> orig;
  std::vector<FurnaceGUISysDef> subDefs;
  void bake();
  FurnaceGUISysDef(const char* n, std::initializer_list<FurnaceGUISysDefChip> def, const char* e=NULL);
  FurnaceGUISysDef(const char* n, const char* def, DivEngine* e);
};


struct FurnaceGUISysCategory {
  const char* name;
  const char* description;
  std::vector<FurnaceGUISysDef> systems;
  FurnaceGUISysCategory(const char* n, const char* d):
    name(n),
    description(d) {}
  FurnaceGUISysCategory():
    name(NULL),
    description(NULL) {}
};
