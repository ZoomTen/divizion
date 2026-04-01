#pragma once

#include "../interface_actions.hpp"
#include <string>
#include <vector>

using std::vector;
using std::string;

struct Mock: DivizionActions
{
	vector<string> infoA;
	vector<string> infoB;
	vector<string> infoC;
	int info{100};

	Mock();
	vector<string> getInstrumentList(void) override;
	void drawInstrumentInfo(int, int) override;
	void actAdd(int type) override;
	void actDuplicate(int type, int index) override;
	void actLoad(int type) override;
	void actSave(int type, int index) override;
	void actMoveUp(int type, int index) override;
	void actMoveDown(int type, int index) override;
	void actDelete(int type, int index) override;
};