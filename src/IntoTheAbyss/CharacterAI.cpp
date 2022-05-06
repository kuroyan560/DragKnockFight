#include "CharacterAI.h"
#include"../Engine/DrawFunc.h"
#include"ScrollMgr.h"
#include"CharacterManager.h"
#include"Stamina.h"

CharacterAI::CharacterAI()
{
	//戦略層の生成--------------------------
	strategyArray[0] = std::make_unique<RestoreStamina>();
	strategyArray[1] = std::make_unique<GoToTheField>();
	//戦略層の生成--------------------------

	startFlag = false;
}

void CharacterAI::Init()
{
	//仮置き
	initFlag = true;
	//restoreStamina.Init();
	//goToTheField.Init();

	StrategyOfChoice = STRATEGY_GO_TO_THE_FIELD;
}

void CharacterAI::Update()
{
	//キャラクターAIに必要なデータ集め--------------------------
	CharacterAIData::Instance()->playerData.stamineGauge = 0;
	for (int i = 0; i < CharacterManager::Instance()->Left()->staminaGauge->stamina.size(); ++i)
	{
		if (CharacterManager::Instance()->Left()->staminaGauge->stamina[i].GetIsActivate())
		{
			++CharacterAIData::Instance()->playerData.stamineGauge;
		}
	}

	CharacterAIData::Instance()->bossData.stamineGauge = 0;
	for (int i = 0; i < CharacterManager::Instance()->Right()->staminaGauge->stamina.size(); ++i)
	{
		if (CharacterManager::Instance()->Right()->staminaGauge->stamina[i].GetIsActivate())
		{
			++CharacterAIData::Instance()->bossData.stamineGauge;
		}
	}

	//敵とプレイヤーとの距離
	CharacterAIData::Instance()->distance = CharacterManager::Instance()->Left()->pos.Distance(CharacterManager::Instance()->Right()->pos);
	//キャラクターAIに必要なデータ集め--------------------------


	if (initFlag && !staminaInit)
	{
		//float value = restoreStamina.EvaluationFunction();
		//restoreStamina.Init();
		staminaInit = true;
	}

	if (initFlag)
	{
		strategyArray[StrategyOfChoice]->route = shortestData;
		strategyArray[StrategyOfChoice]->Update();


		//受け渡し-----------------------------------------------------
		startPoint = strategyArray[StrategyOfChoice]->startPoint;
		endPoint = strategyArray[StrategyOfChoice]->endPoint;
		startFlag = strategyArray[StrategyOfChoice]->startFlag;
		CharacterManager::Instance()->Right()->vel = CharacterAIData::Instance()->vel;
	}
}

void CharacterAI::Draw()
{
	if (initFlag)
	{
	}
}
