#pragma once

#include "interface_actions.hpp"
#include "src/engine/engine.h"

struct DivizionActionsImpl : DivizionActions
{
	DivEngine *e{nullptr};
  DivizionActionsImpl(DivEngine *e);

  void drawInstrumentInfo(ActiveItemType type, int index) override;
	void actAdd(ActiveItemType type) override;
	void actDuplicate(ActiveItemType type, int index) override;
	void actLoad(ActiveItemType type) override;
	void actSave(ActiveItemType type, int index) override;
	void actMoveUp(ActiveItemType type, int index) override;
	void actMoveDown(ActiveItemType type, int index) override;
	void actDelete(ActiveItemType type, int index) override;
	vector<DivInstrument*> getInstrumentList() override;
	vector<DivWavetable*> getWavetables() override;
	vector<DivSample*> getSamples() override;
};
