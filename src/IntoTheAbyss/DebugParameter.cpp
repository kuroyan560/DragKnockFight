#include "DebugParameter.h"

DebugParameter::DebugParameter()
{
	//�v���Z�b�g1
	playerData.push_back(PlayerDebugParameterData(0.5f, 15.0f, 7.0f, 20.0f, 30.0f, 21));
	//�v���Z�b�g2
	playerData.push_back(PlayerDebugParameterData(1.0f, 10.0f, 1.0f, 10.0f, 0.0f, 1));

	//�f�t�H���g
	nowData = std::make_shared<PlayerDebugParameterData>(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0);
	loadPresetFlag = true;

	data = std::make_shared<RoundParameterData>();

	data->roundData.maxTimer = 60.0f;
	data->roundData.sizeMaxTimer = 60.0f;
	data->readyData.maxTimer = 60.0f;
	data->readyData.sizeMaxTimer = 60.0f;
	data->fightData.maxTimer = 30.0f;
	data->fightData.sizeMaxTimer = 30.0f;
	data->numberData.maskMaxTimer = 60.0f;
	data->nextNumberData.maskMaxTimer = 60.0f;
}

void DebugParameter::Update()
{
	//�v���Z�b�g�I���̐���-----------------------
	if (playerData.size() < 0)
	{
		selectNum = 0;
	}
	else if (playerData.size() <= selectNum)
	{
		selectNum = playerData.size() - 1;
	}
	//�v���Z�b�g�I���̐���-----------------------


	//�v���Z�b�g�ǂݍ���
	if (loadPresetFlag)
	{
		nowData->ADD_GRAVITY = playerData[selectNum].ADD_GRAVITY;
		nowData->FIRST_RECOIL_AMOUNT = playerData[selectNum].FIRST_RECOIL_AMOUNT;
		nowData->MAX_GRAVITY = playerData[selectNum].MAX_GRAVITY;
		nowData->MAX_RECOIL_AMOUNT = playerData[selectNum].MAX_RECOIL_AMOUNT;
		nowData->RAPID_FIRE_TIMER = playerData[selectNum].RAPID_FIRE_TIMER;
		nowData->RECOIL_AMOUNT = playerData[selectNum].RECOIL_AMOUNT;
	}
}

void DebugParameter::DrawImGui()
{
	/*ImGui::Begin("Parameter");
	ImGui::InputFloat("ADD_GRAVITY", &nowData->ADD_GRAVITY);
	ImGui::InputFloat("MAX_GRAVITY", &nowData->MAX_GRAVITY);
	ImGui::InputFloat("FIRST_RECOIL_AMOUNT", &nowData->FIRST_RECOIL_AMOUNT);
	ImGui::InputFloat("RECOIL_AMOUNT", &nowData->RECOIL_AMOUNT);
	ImGui::InputFloat("MAX_RECOIL_AMOUNT", &nowData->MAX_RECOIL_AMOUNT);
	ImGui::InputInt("RAPID_FIRE_TIMER", &nowData->RAPID_FIRE_TIMER);
	ImGui::InputInt("SelectPreset", &selectNum);
	loadPresetFlag = ImGui::Button("LoadPreset");
	ImGui::End();*/


	ImGui::Begin("Parameter");
	ImGui::Text("RoundTextParam");
	ImGui::InputFloat("MovePosTimer_Round", &data->roundData.maxTimer);
	ImGui::InputFloat("ShrinkSizeTimer_Round", &data->roundData.sizeMaxTimer);
	ImGui::Text("ReadyTextParam");
	ImGui::InputFloat("MovePosTimer_Ready", &data->readyData.maxTimer);
	ImGui::InputFloat("ShrinkSizeTimer_Ready", &data->readyData.sizeMaxTimer);
	ImGui::Text("FightTextParam");
	ImGui::InputFloat("MovePosTimer_Fight", &data->fightData.maxTimer);
	ImGui::InputFloat("ShrinkSizeTimer_Fight", &data->fightData.sizeMaxTimer);
	ImGui::Text("NowNumberTextParam");
	ImGui::InputFloat("ChangeNumberTimer_NowNumber", &data->numberData.maskMaxTimer);
	ImGui::Text("NextNumberTextParam");
	ImGui::InputFloat("ChangeNumberTimer_NextNumber", &data->nextNumberData.maskMaxTimer);
	ImGui::End();
}