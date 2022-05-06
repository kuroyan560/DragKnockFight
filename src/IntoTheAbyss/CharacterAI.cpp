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
	//�헪�w�̐���--------------------------
	strategyArray[0] = std::make_unique<RestoreStamina>();
	strategyArray[1] = std::make_unique<GoToTheField>();
	//�헪�w�̐���--------------------------

	startFlag = false;
	strategyOfChoice = STRATEGY_GO_TO_THE_FIELD;
	strategyArray[strategyOfChoice]->Init();
	initFlag = true;
}

void CharacterAI::Update()
{
	//�L�����N�^�[AI�ɕK�v�ȃf�[�^�W��--------------------------
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

	//�G�ƃv���C���[�Ƃ̋���
	CharacterAIData::Instance()->distance = CharacterManager::Instance()->Left()->pos.Distance(CharacterManager::Instance()->Right()->pos);
	//�L�����N�^�[AI�ɕK�v�ȃf�[�^�W��--------------------------

	if (initFlag)
	{
		//�ӎv����--------------------------
		//�헪���������͎��s������
		if (strategyArray[strategyOfChoice]->CurrentProgress() == AiResult::OPERATE_SUCCESS ||
			strategyArray[strategyOfChoice]->CurrentProgress() == AiResult::OPERATE_FAIL)
		{
			float min = 10.0f;
			int selecting = 0;
			//�]���l�̍ő�l����헪�����肷��
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
		//�ӎv����--------------------------

		strategyArray[strategyOfChoice]->route = shortestData;
		strategyArray[strategyOfChoice]->Update();


		//�󂯓n��-----------------------------------------------------
		startPoint = strategyArray[strategyOfChoice]->startPoint;
		endPoint = strategyArray[strategyOfChoice]->endPoint;
		startFlag = strategyArray[strategyOfChoice]->startFlag;
		CharacterManager::Instance()->Right()->vel = CharacterAIOrder::Instance()->vel;
	}
}

void CharacterAI::Draw()
{
}
