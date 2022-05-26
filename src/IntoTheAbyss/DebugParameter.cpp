#include "DebugParameter.h"
#include"SelectStage.h"
#include"CharacterInterFace.h"

DebugParameter::DebugParameter()
{
	aiFlag = false;

	playerData.push_back(PlayerDebugParameterData(15.0f));
	//デフォルト
	nowData = std::make_shared<PlayerDebugParameterData>(15.0f);

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
	playerParamImguiHandle = DebugImGuiManager::Instance()->Add("PlayerParameter");
	swingAngleParamImguiHandle = DebugImGuiManager::Instance()->Add("Paramters");

	bossStageNum = 0;


	bossDebugData.push_back(BossDebugParameterData());
	bossDebugData.push_back(BossDebugParameterData());
	bossDebugData.push_back(BossDebugParameterData());

	//BossDebugParameterData data;
	//bossDebugData.push_back(data);

	swingAngle = 7.0f;
	swingMax = 0.11f;
	useFinishSwingFlag = true;
	generator = 0;
	generatorSpanMax = 60 * 9;
	generatorSpanMin = 60 * 5;
	changeGenerator = false;
	comboResetDist = 300.0f;
	emitRare = 1.0f;
	emitBounce = 2.0f;

	hitPlayer = false;

	canDestroyBounceVel = 2.0f;
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
	}
}

#include"CharacterManager.h"
void DebugParameter::DrawImGui()
{
	if (DebugImGuiManager::Instance()->DrawFlag(playerParamImguiHandle))
	{
		ImGui::Begin("PlayerParameter");
		ImGui::InputFloat("Vel", &playerData[0].playerSpeed);
		ImGui::InputFloat("Damage", &playerData[0].damage);
		ImGui::End();
	}

	if (DebugImGuiManager::Instance()->DrawFlag(bossParamImguiHandle))
	{
		ImGui::Begin("BossParameter");
		ImGui::InputInt("BossParamData", &bossStageNum);

		if (bossStageNum < 0)
		{
			bossStageNum = 0;
		}
		else if (bossDebugData.size() <= bossStageNum)
		{
			bossStageNum = bossDebugData.size() - 1;
		}
		ImGui::InputFloat("Velocity", &bossDebugData[bossStageNum].vel);
		ImGui::InputInt("SWING_COOL_TIME", &bossDebugData[bossStageNum].coolTime);
		ImGui::InputInt("STAMINA_MAX", &bossDebugData[bossStageNum].staminaMax);
		ImGui::InputInt("STAMINA_DASH", &bossDebugData[bossStageNum].staminaDash);
		ImGui::InputInt("STAMINA_SWING", &bossDebugData[bossStageNum].staminaSwing);
		ImGui::InputFloat("SWING_DAMAGE", &bossDebugData[bossStageNum].damage);
		ImGui::InputFloat("BULLET_DAMAGE", &gaugeData->enemyBulletAddGuaugeValue);
		ImGui::InputFloat("STAMINA_HEAL_AMOUNT", &bossDebugData[bossStageNum].staminaHealAmount);
		ImGui::InputFloat("DAMAGE", &bossDebugData[bossStageNum].damage);
		ImGui::Checkbox("enableToDashAfterSwingFlag", &bossDebugData[bossStageNum].enableToDashAfterSwingFlag);
		ImGui::End();
	}
	if (DebugImGuiManager::Instance()->DrawFlag(swingAngleParamImguiHandle))
	{
		ImGui::Begin("Paramters");

		int sec = timer / 60;
		int min = sec / 60;
		sec -= min * 60;
		ImGui::Text("Timer %d : %d", min, sec);
		ImGui::Text("TotalCombo %d", totalCombo);

		ImGui::Separator();

		ImGui::Checkbox("PlayerCheckHit", &hitPlayer);

		ImGui::Separator();

		ImGui::InputFloat("SWING_ANGLE", &swingAngle);
		//ImGui::Checkbox("useFinishSwing", &useFinishSwingFlag);
		ImGui::InputFloat("ComboResetDist", &comboResetDist);
		ImGui::Checkbox("StrongSwing", &CharacterManager::Instance()->Left()->isDebugModeStrongSwing);
		ImGui::SliderInt("ConsecutiveSwingDelay", &CharacterManager::Instance()->Left()->CONSECUTIVE_SWING_TIMER, 1, 30);
		ImGui::SliderInt("AddLineLengthValue", &CharacterManager::Instance()->Left()->ADD_LINE_LENGTH_VEL, 1, 30);
		ImGui::InputFloat("CanDestroyBoundVel", &canDestroyBounceVel);

		ImGui::Separator();
		changeGenerator = ImGui::SliderInt("chipGenerator", &generator, 0, MAP_CHIP_GENERATOR_NUM - 1);
		ImGui::SliderFloat("EmitRare", &emitRare, 0.0f, 3.0f);
		ImGui::SliderFloat("EmitBounce", &emitBounce, 0.0f, 3.0f);

		if (generator == RAND_PATTERN)
		{
			ImGui::InputInt("generatorSpanMin", &generatorSpanMin);
			ImGui::InputInt("generatorSpanMax", &generatorSpanMax);
			assert(generatorSpanMin < generatorSpanMax);
		}

		ImGui::End();
	}


	ImGui::Begin("Ai");
	ImGui::Checkbox("On", &aiFlag);
	ImGui::End();

}

const BossDebugParameterData &DebugParameter::GetBossData()
{
#ifdef DEBUG
	bossStageNum = SelectStage::Instance()->GetStageNum();
#endif // _DEBUG

	return bossDebugData[bossStageNum];
}
