#include "src/engine/instrument.h"
#include "src/engine/sample.h"
#include "src/engine/wavetable.h"
#include "src/gui.hpp"
#include <cstdint>

void renderListItem(Gui *self, size_t i, DivInstrument *item)
{

	if (ImGui::Selectable(item->name.c_str(), i == self->instSelected))
	{
		self->selectedType = self->currentlyViewingType;
		self->selectedIndex = i;
		self->instSelected = i;
	}
}

void renderListItem(Gui *self, size_t i, DivWavetable *item)
{

	// max of 256 wavetables
	char strId[4] = {0};
	snprintf(strId, sizeof(strId), "%d", (int) i);

	if (ImGui::Selectable(strId, i == self->instSelected))
	{
		self->selectedType = self->currentlyViewingType;
		self->selectedIndex = i;
		self->instSelected = i;
	}
}

void renderListItem(Gui *self, size_t i, DivSample *item)
{

	if (ImGui::Selectable(item->name.c_str(), i == self->instSelected))
	{
		self->selectedType = self->currentlyViewingType;
		self->selectedIndex = i;
		self->instSelected = i;
	}
}