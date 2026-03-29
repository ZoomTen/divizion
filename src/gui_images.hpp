#pragma once

#include "SDL_render.h"

enum ImageId
{
  ImageId_Test = 0,

  _nImages
};

struct Image
{
  ImageId id;
  unsigned char *data;
  int w;
  int h;
  int channels;
  SDL_Texture *t;
};