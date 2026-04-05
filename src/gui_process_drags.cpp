#include "gui_process_drags.hpp"

#define B30(tt)                                                                \
  (self->macroDragBit30 ? ((((tt) & 0xc0000000) == 0x40000000                  \
                            || ((tt) & 0xc0000000) == 0x80000000)              \
                             ? 0x40000000                                      \
                             : 0)                                              \
                        : 0)

#define MACRO_DRAG(t)                                                          \
  if (self->macroDragSettingBit30) {                                           \
    if (self->macroDragLastX != x || self->macroDragLastY != y) {              \
      self->macroDragLastX = x;                                                \
      self->macroDragLastY = y;                                                \
      if (self->macroDragInitialValueSet) {                                    \
        if (!self->macroDragInitialValue) {                                    \
          if (t[x] & 0x80000000) {                                             \
            t[x] &= ~0x40000000;                                               \
          } else {                                                             \
            t[x] |= 0x40000000;                                                \
          }                                                                    \
        } else {                                                               \
          if (t[x] & 0x80000000) {                                             \
            t[x] |= 0x40000000;                                                \
          } else {                                                             \
            t[x] &= ~0x40000000;                                               \
          }                                                                    \
        }                                                                      \
      } else {                                                                 \
        self->macroDragInitialValue =                                          \
          (((t[x]) & 0xc0000000) == 0x40000000                                 \
           || ((t[x]) & 0xc0000000) == 0x80000000);                            \
        self->macroDragInitialValueSet = true;                                 \
        t[x] ^= 0x40000000;                                                    \
      }                                                                        \
    }                                                                          \
  } else if (self->macroDragBitMode) {                                         \
    if (self->macroDragLastX != x || self->macroDragLastY != y) {              \
      self->macroDragLastX = x;                                                \
      self->macroDragLastY = y;                                                \
      if (self->macroDragInitialValueSet) {                                    \
        if (self->macroDragInitialValue) {                                     \
          t[x] = (((t[x]) & ((1 << self->macroDragMax) - 1)) & (~(1 << y)));   \
        } else {                                                               \
          t[x] = (((t[x]) & ((1 << self->macroDragMax) - 1)) | (1 << y));      \
        }                                                                      \
      } else {                                                                 \
        self->macroDragInitialValue =                                          \
          (((t[x]) & ((1 << self->macroDragMax) - 1)) & (1 << y));             \
        self->macroDragInitialValueSet = true;                                 \
        t[x] = (((t[x]) & ((1 << self->macroDragMax) - 1)) ^ (1 << y));        \
      }                                                                        \
      t[x] &= (1 << self->macroDragMax) - 1;                                   \
    }                                                                          \
  } else {                                                                     \
    if (self->macroDragLineMode) {                                             \
      if (!self->macroDragInitialValueSet) {                                   \
        self->macroDragLineInitial = ImVec2(x, y);                             \
        self->macroDragLineInitialV = ImVec2(dragX, dragY);                    \
        self->macroDragInitialValueSet = true;                                 \
        self->macroDragMouseMoved = false;                                     \
      } else if (!self->macroDragMouseMoved) {                                 \
        if ((pow(dragX - self->macroDragLineInitialV.x, 2.0)                   \
             + pow(dragY - self->macroDragLineInitialV.y, 2.0))                \
            >= 16.0f) {                                                        \
          self->macroDragMouseMoved = true;                                    \
        }                                                                      \
      }                                                                        \
      if (self->macroDragMouseMoved) {                                         \
        if ((int)round(x - self->macroDragLineInitial.x) == 0) {               \
          t[x] = B30(t[x]) ^ (int)(self->macroDragLineInitial.y);              \
        } else {                                                               \
          if ((int)round(x - self->macroDragLineInitial.x) < 0) {              \
            for (int i = 0; i <= (int)round(self->macroDragLineInitial.x - x); \
                 i++) {                                                        \
              int index = (int)round(x + i);                                   \
              if (index < 0) continue;                                         \
              t[index] =                                                       \
                B30(t[index])                                                  \
                ^ (int)(y                                                      \
                        + (self->macroDragLineInitial.y - y)                   \
                            * ((float)i                                        \
                               / (float)(self->macroDragLineInitial.x - x)));  \
            }                                                                  \
          } else {                                                             \
            for (int i = 0; i <= (int)round(x - self->macroDragLineInitial.x); \
                 i++) {                                                        \
              int index = (int)round(i + self->macroDragLineInitial.x);        \
              if (index < 0) continue;                                         \
              t[index] =                                                       \
                B30(t[index])                                                  \
                ^ (int)(self->macroDragLineInitial.y                           \
                        + (y - self->macroDragLineInitial.y)                   \
                            * ((float)i                                        \
                               / (x - self->macroDragLineInitial.x)));         \
            }                                                                  \
          }                                                                    \
        }                                                                      \
      }                                                                        \
    } else {                                                                   \
      t[x] = B30(t[x]) ^ (y);                                                  \
    }                                                                          \
  }


void processDrags(DivizionActionsImpl* self, int dragX, int dragY)
{
  if (self->macroDragActive) {
    if (self->macroDragLen > 0) {
      int x = ((dragX - self->macroDragStart.x) * self->macroDragLen
               / MAX(1, self->macroDragAreaSize.x));
      if (x < 0) x = 0;
      if (x >= self->macroDragLen) x = self->macroDragLen - 1;
      x += self->macroDragScroll;
      int y;
      if (self->macroDragBitMode) {
        y = (int)(self->macroDragMax
                  - ((dragY - self->macroDragStart.y)
                     * (double(self->macroDragMax - self->macroDragMin)
                        / (double)MAX(1, self->macroDragAreaSize.y))));
      } else {
        y = round(self->macroDragMax
                  - ((dragY - self->macroDragStart.y)
                     * (double(self->macroDragMax - self->macroDragMin)
                        / (double)MAX(1, self->macroDragAreaSize.y))));
      }
      if (y > self->macroDragMax) y = self->macroDragMax;
      if (y < self->macroDragMin) y = self->macroDragMin;
      if (self->macroDragChar) {
        MACRO_DRAG(self->macroDragCTarget);
      } else {
        MACRO_DRAG(self->macroDragTarget);
      }
    }
  }
  if (self->macroLoopDragActive) {
    if (self->macroLoopDragLen > 0) {
      int x = (dragX - self->macroLoopDragStart.x) * self->macroLoopDragLen
              / MAX(1, self->macroLoopDragAreaSize.x);
      if (x < 0) x = 0;
      if (x >= self->macroLoopDragLen) {
        x = -1;
      } else {
        x += self->macroDragScroll;
      }
      *self->macroLoopDragTarget = x;
    }
  }
  if (self->waveDragActive) {
    if (self->waveDragLen > 0) {
      int x = (dragX - self->waveDragStart.x) * self->waveDragLen
              / MAX(1, self->waveDragAreaSize.x);
      if (x < 0) x = 0;
      if (x >= self->waveDragLen) x = self->waveDragLen - 1;
      int y = (self->waveDragMax + 1)
              - ((dragY - self->waveDragStart.y)
                 * (double((self->waveDragMax + 1) - self->waveDragMin)
                    / (double)MAX(1, self->waveDragAreaSize.y)));
      if (y > self->waveDragMax) y = self->waveDragMax;
      if (y < self->waveDragMin) y = self->waveDragMin;
      self->waveDragTarget[x] = y;
    }
  }
#if 0
  if (sampleDragActive) {
    int x= samplePos + floor(double(dragX - sampleDragStart.x) * sampleZoom);
    int x1=
      samplePos + floor(double(dragX - sampleDragStart.x + 1) * sampleZoom);
    if (x < 0) x= 0;
    if (sampleDragMode) {
      if (x >= (int)sampleDragLen) x= sampleDragLen - 1;
    } else {
      if (x > (int)sampleDragLen) x= sampleDragLen;
    }
    if (x1 < 0) x1= 0;
    if (x1 >= (int)sampleDragLen) x1= sampleDragLen - 1;
    double y= 0.5 - double(dragY - sampleDragStart.y) / sampleDragAreaSize.y;
    if (sampleDragMode) { // draw
      if (sampleDragTarget) {
        if (sampleDrag16) {
          int val= y * 65536;
          if (val < -32768) val= -32768;
          if (val > 32767) val= 32767;
          for (int i= x; i <= x1; i++)
            ((short*)sampleDragTarget)[i]= val;
        } else {
          int val= y * 256;
          if (val < -128) val= -128;
          if (val > 127) val= 127;
          for (int i= x; i <= x1; i++)
            ((signed char*)sampleDragTarget)[i]= val;
        }
        updateSampleTex= true;
      }
    } else { // select
      if (sampleSelStart < 0) {
        sampleSelStart= x;
      }
      sampleSelEnd= x;
    }
  }
  if (orderScrollLocked) {
    if (fabs(orderScrollRealOrigin.x - dragX) > 2.0f * dpiScale
        || fabs(orderScrollRealOrigin.y - dragY) > 2.0f * dpiScale)
      orderScrollTolerance= false;
    orderScroll= (orderScrollSlideOrigin - dragX) / (40.0 * dpiScale);
    if (orderScroll < 0.0f) orderScroll= 0.0f;
    if (orderScroll > (float)e->curSubSong->ordersLen - 1)
      orderScroll= e->curSubSong->ordersLen - 1;
  }
#endif
}