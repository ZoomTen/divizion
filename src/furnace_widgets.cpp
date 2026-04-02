#include "furnace_widgets.hpp"

bool isCtrlWheelModifierHeld()
{
  return ImGui::IsKeyDown(ImGuiMod_Ctrl) || ImGui::IsKeyDown(ImGuiMod_Super);
}

#define CW_ADDITION(T)                                                         \
  if (p_min != NULL && p_max != NULL) {                                        \
    if (*((T*)p_min) > *((T*)p_max)) {                                         \
      if (self->wheelY < 0) {                                                  \
        if ((*((T*)p_data) - self->wheelY) > *((T*)p_min)) {                   \
          *((T*)p_data) = *((T*)p_min);                                        \
        } else {                                                               \
          *((T*)p_data) -= self->wheelY;                                       \
        }                                                                      \
      } else {                                                                 \
        if ((*((T*)p_data) - self->wheelY) < *((T*)p_max)) {                   \
          *((T*)p_data) = *((T*)p_max);                                        \
        } else {                                                               \
          *((T*)p_data) -= self->wheelY;                                       \
        }                                                                      \
      }                                                                        \
    } else {                                                                   \
      if (self->wheelY > 0) {                                                  \
        if ((*((T*)p_data) + self->wheelY) > *((T*)p_max)) {                   \
          *((T*)p_data) = *((T*)p_max);                                        \
        } else {                                                               \
          *((T*)p_data) += self->wheelY;                                       \
        }                                                                      \
      } else {                                                                 \
        if ((*((T*)p_data) + self->wheelY) < *((T*)p_min)) {                   \
          *((T*)p_data) = *((T*)p_min);                                        \
        } else {                                                               \
          *((T*)p_data) += self->wheelY;                                       \
        }                                                                      \
      }                                                                        \
    }                                                                          \
  }

bool CWSliderScalar(DivizionActionsImpl* self, const char* label,
                    ImGuiDataType data_type, void* p_data, const void* p_min,
                    const void* p_max, const char* format,
                    ImGuiSliderFlags flags)
{
  flags ^= ImGuiSliderFlags_AlwaysClamp;
  if (ImGui::SliderScalar(label, data_type, p_data, p_min, p_max, format,
                          flags))
    return true;

  if (ImGui::IsItemHovered() && ctrlWheeling) {
    switch (data_type) {
    case ImGuiDataType_U8:
      CW_ADDITION(unsigned char);
      break;
    case ImGuiDataType_S8:
      CW_ADDITION(signed char);
      break;
    case ImGuiDataType_U16:
      CW_ADDITION(unsigned short);
      break;
    case ImGuiDataType_S16:
      CW_ADDITION(short);
      break;
    case ImGuiDataType_U32:
      CW_ADDITION(unsigned int);
      break;
    case ImGuiDataType_S32:
      CW_ADDITION(int);
      break;
    case ImGuiDataType_Float:
      CW_ADDITION(float);
      break;
    case ImGuiDataType_Double:
      CW_ADDITION(double);
      break;
    }
    return true;
  }

  return false;
}

bool CWSliderInt(DivizionActionsImpl* self, const char* label, int* v,
                 int v_min, int v_max, const char* format,
                 ImGuiSliderFlags flags)
{
  return CWSliderScalar(self, label, ImGuiDataType_S32, v, &v_min, &v_max,
                        format, flags);
}