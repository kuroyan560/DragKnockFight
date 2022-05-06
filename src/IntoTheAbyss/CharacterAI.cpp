#include "CharacterAI.h"
#include"../Engine/DrawFunc.h"
#include"ScrollMgr.h"
#include"CharacterManager.h"
#include"Stamina.h"

CharacterAI::CharacterAI()
{
	startTimer = 0;
	initFlag = false;
}

void CharacterAI::Init()
{
	//戦略層の生成--------------------------
	strategyArray[0] = std::make_unique<RestoreStamina>();
	strategyArray[1] = std::make_unique<GoToTheField>();
	//戦略層の生成--------------------------

	startFlag = false;
	strategyOfChoice = STRATEGY_GO_TO_THE_FIELD;
	strategyArray[strategyOfChoice]->Init();
	initFlag = true;
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

	if (initFlag)
	{
		//意思決定--------------------------
		//戦略が成功又は失敗した際
		if (strategyArray[strategyOfChoice]->CurrentProgress() == AiResult::OPERATE_SUCCESS ||
			strategyArray[strategyOfChoice]->CurrentProgress() == AiResult::OPERATE_FAIL)
		{
			float min = 10.0f;
			int selecting = 0;
			//評価値の最大値から戦略を決定する
			for (int i = 0; i < strategyArray.size(); ++i)
			{
				float strategyEvaluationValue = strategyArray[i]->EvaluationFunction();
				if (strategyEvaluationValue < min)
				{
					selecting = i;
					min = strategyEvaluationValue;
				}
			}
			strategyOfChoice = static_cast<AiStrategy>(selecting);
			strategyArray[strategyOfChoice]->Init();
		}
		//意思決定--------------------------

		strategyArray[strategyOfChoice]->route = shortestData;
		strategyArray[strategyOfChoice]->Update();


		//受け渡し-----------------------------------------------------
		startPoint = strategyArray[strategyOfChoice]->startPoint;
		endPoint = strategyArray[strategyOfChoice]->endPoint;
		startFlag = strategyArray[strategyOfChoice]->startFlag;
		CharacterManager::Instance()->Right()->vel = CharacterAIOrder::Instance()->vel;
	}
}

void CharacterAI::Draw()
{
}
