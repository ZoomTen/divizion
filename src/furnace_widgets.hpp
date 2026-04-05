#pragma once
#include "imgui.h"
#include "src/impl_actions.hpp"

bool isCtrlWheelModifierHeld();

bool CWSliderScalar(DivizionActionsImpl* self, const char* label,
                    ImGuiDataType data_type, void* p_data, const void* p_min,
                    const void* p_max, const char* format = NULL,
                    ImGuiSliderFlags flags = 0);

bool CWSliderInt(DivizionActionsImpl* self, const char* label, int* v,
                 int v_min, int v_max, const char* format = NULL,
                 ImGuiSliderFlags flags = 0);
#define ctrlWheeling (isCtrlWheelModifierHeld() && self->wheelY != 0)