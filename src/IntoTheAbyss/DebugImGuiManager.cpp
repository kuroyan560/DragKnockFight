#include "DebugImGuiManager.h"

DebugImGuiManager::DebugImGuiManager()
{
	/*handle = 0;
	for (int i = 0; i < initFlag.size(); ++i)
	{
		initFlag[i] = false;
		drawFlags[i] = false;
		item[i] = "";
	}*/
}

int DebugImGuiManager::Add(const std::string &NAME)
{
	item[handle] = NAME;
	drawFlags[handle] = false;
	initFlag[handle] = true;

	int tmp = handle;
	++handle;
	return tmp;
}

const bool &DebugImGuiManager::DrawFlag(const int &HANDLE)
{
	bool getFlag = drawFlags[HANDLE];
	return getFlag;
}

void DebugImGuiManager::DrawImGui()
{
	ImGui::Begin("ImGuiManager");
	for (int i = 0; i < item.size(); ++i)
	{
		if (initFlag[i])
		{
			ImGui::Checkbox(item[i].c_str(), &drawFlags[i]);
		}
	}
	ImGui::End();
}
