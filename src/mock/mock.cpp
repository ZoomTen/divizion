#include "mock.hpp"
#include "imgui.h"
#include <random>

static string randomStr(void);

Mock::Mock()
{
	for (int i = 0; i < 48; i++)
	{
		this->infoA.push_back(randomStr());
	}
	for (int i = 0; i < 24; i++)
	{
		this->infoB.push_back(randomStr());
	}
	for (int i = 0; i < 12; i++)
	{
		this->infoC.push_back(randomStr());
	}
}

vector<string> Mock::getInstrumentList(void) { return this->infoA; }

string randomStr(void)
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> length_dist(6, 18);
	std::uniform_int_distribution<> char_dist('a', 'z');
	std::string result;

	int length = length_dist(gen);
	result.reserve(length); // Efficiency: pre-allocate memory
	for (int i = 0; i < length; ++i)
	{
		result += static_cast<char>(char_dist(gen));
	}
	return result;
}

void Mock::drawInstrumentInfo(int type, int index)
{
	ImGui::Text("selected type: %d", type);
	ImGui::Text("selected index: %d", index);
	ImGui::Text("(%d)", this->info);
	if (ImGui::Button("set state!"))
	{
		this->info++;
	}
}

void Mock::actAdd(int type) {
  printf("add of type %d\n", type);
}
void Mock::actDuplicate(int type, int index) {
  printf("duplicate of type %d index %d\n", type, index);
}
void Mock::actLoad(int type) {
  printf("load of type %d\n", type);
}
void Mock::actSave(int type, int index) {
  printf("save of type %d index %d\n", type, index);
}
void Mock::actMoveUp(int type, int index) {
  printf("move up of type %d index %d\n", type, index);
}
void Mock::actMoveDown(int type, int index) {
  printf("move down of type %d index %d\n", type, index);
}
void Mock::actDelete(int type, int index) {
  printf("delete of type %d index %d\n", type, index);
}