#include "DebugParameter.h"

DebugParameter::DebugParameter()
{
	//プリセット1
	playerData.push_back(PlayerDebugParameterData(0.5f, 15.0f, 7.0f, 20.0f, 30.0f, 21));

	//デフォルト
	nowData = std::make_shared<PlayerDebugParameterData>(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0);
	loadPresetFlag = true;
}

void DebugParameter::Update()
{
	if (loadPresetFlag)
	{
		nowData->ADD_GRAVITY = playerData[0].ADD_GRAVITY;
		nowData->FIRST_RECOIL_AMOUNT = playerData[0].FIRST_RECOIL_AMOUNT;
		nowData->MAX_GRAVITY = playerData[0].MAX_GRAVITY;
		nowData->MAX_RECOIL_AMOUNT = playerData[0].MAX_RECOIL_AMOUNT;
		nowData->RAPID_FIRE_TIMER = playerData[0].RAPID_FIRE_TIMER;
		nowData->RECOIL_AMOUNT = playerData[0].RECOIL_AMOUNT;
	}
}

void DebugParameter::DrawImGui()
{
	ImGui::Begin("Parameter");
	ImGui::InputFloat("ADD_GRAVITY", &nowData->ADD_GRAVITY);
	ImGui::InputFloat("MAX_GRAVITY", &nowData->MAX_GRAVITY);
	ImGui::InputFloat("FIRST_RECOIL_AMOUNT", &nowData->FIRST_RECOIL_AMOUNT);
	ImGui::InputFloat("RECOIL_AMOUNT", &nowData->RECOIL_AMOUNT);
	ImGui::InputFloat("MAX_RECOIL_AMOUNT", &nowData->MAX_RECOIL_AMOUNT);
	ImGui::InputInt("RAPID_FIRE_TIMER", &nowData->RAPID_FIRE_TIMER);
	loadPresetFlag = ImGui::Button("LoadPreset1");
	ImGui::End();
}