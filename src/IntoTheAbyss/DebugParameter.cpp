#include "DebugParameter.h"

DebugParameter::DebugParameter()
{
	//プリセット1
	playerData.push_back(PlayerDebugParameterData(0.5f, 15.0f, 7.0f, 20.0f, 30.0f, 21));
	//プリセット2
	playerData.push_back(PlayerDebugParameterData(1.0f, 10.0f, 7.0f, 10.0f, 0.0f, 1));

	//デフォルト
	nowData = std::make_shared<PlayerDebugParameterData>(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0);

	gaugeData = std::make_shared<GaugeDebugParameterData>();
	loadPresetFlag = true;

	roundData = std::make_shared<RoundParameterData>();

	roundData->roundData.maxTimer = 60.0f;
	roundData->roundData.sizeMaxTimer = 60.0f;
	roundData->readyData.maxTimer = 60.0f;
	roundData->readyData.sizeMaxTimer = 60.0f;
	roundData->fightData.maxTimer = 30.0f;
	roundData->fightData.sizeMaxTimer = 30.0f;
	roundData->numberData.maskMaxTimer = 60.0f;
	roundData->nextNumberData.maskMaxTimer = 60.0f;

	roundParamImguiHandle = DebugImGuiManager::Instance()->Add("RoundParameter");
	bossParamImguiHandle = DebugImGuiManager::Instance()->Add("BossParameter");
}

void DebugParameter::Update()
{
	//プリセット選択の制限-----------------------
	if (playerData.size() < 0)
	{
		selectNum = 0;
	}
	else if (playerData.size() <= selectNum)
	{
		selectNum = playerData.size() - 1;
	}
	//プリセット選択の制限-----------------------


	//プリセット読み込み
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
	if (DebugImGuiManager::Instance()->DrawFlag(roundParamImguiHandle))
	{
		ImGui::Begin("RoundParameter");
		ImGui::Text("RoundTextParam");
		ImGui::InputFloat("MovePosTimer_Round", &roundData->roundData.maxTimer);
		ImGui::InputFloat("ShrinkSizeTimer_Round", &roundData->roundData.sizeMaxTimer);
		ImGui::Text("ReadyTextParam");
		ImGui::InputFloat("MovePosTimer_Ready", &roundData->readyData.maxTimer);
		ImGui::InputFloat("ShrinkSizeTimer_Ready", &roundData->readyData.sizeMaxTimer);
		ImGui::Text("FightTextParam");
		ImGui::InputFloat("MovePosTimer_Fight", &roundData->fightData.maxTimer);
		ImGui::InputFloat("ShrinkSizeTimer_Fight", &roundData->fightData.sizeMaxTimer);
		ImGui::Text("NowNumberTextParam");
		ImGui::InputFloat("ChangeNumberTimer_NowNumber", &roundData->numberData.maskMaxTimer);
		ImGui::Text("NextNumberTextParam");
		ImGui::InputFloat("ChangeNumberTimer_NextNumber", &roundData->nextNumberData.maskMaxTimer);
		ImGui::End();
	}

	if (DebugImGuiManager::Instance()->DrawFlag(bossParamImguiHandle))
	{
		ImGui::Begin("BossParameter");
		std::string statusString = GetStatus(bossDebugData.bossNowStatus);
		std::string drawStatusStirng = "BOSS_NOW_STATUS:" + statusString;
		ImGui::Text(drawStatusStirng.c_str());
		ImGui::Text("moveVelX:%f", bossDebugData.moveVel.x);
		ImGui::Text("moveVelY:%f", bossDebugData.moveVel.y);
		ImGui::InputFloat("PULL_POWER_MIN", &bossDebugData.PULL_POWER_MIN);
		ImGui::InputFloat("PULL_POWER_MAX", &bossDebugData.PULL_POWER_MAX);
		ImGui::InputFloat("PULL_ADD_X_POWER", &bossDebugData.PULL_ADD_X_POWER);
		ImGui::InputFloat("moveX", &bossDebugData.moveX);
		ImGui::InputInt("PULL_SPAN_MIN", &bossDebugData.PULL_SPAN_MIN);
		ImGui::InputInt("PULL_SPAN_MAX", &bossDebugData.PULL_SPAN_MAX);
		ImGui::Text("DrawDebug");
		ImGui::Checkbox("NearRay", &bossDebugData.drawNearRayFlag);
		ImGui::Checkbox("FarRay", &bossDebugData.drawFarRayFlag);
		ImGui::Checkbox("Boss", &bossDebugData.drawBossFlag);
		ImGui::End();
	}
}