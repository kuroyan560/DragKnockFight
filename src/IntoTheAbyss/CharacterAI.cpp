#include "CharacterAI.h"
#include"../Engine/DrawFunc.h"
#include"ScrollMgr.h"
#include"CharacterManager.h"
#include"Stamina.h"

CharacterAI::CharacterAI()
{
	//�헪�w�̐���--------------------------
	strategyArray[0] = std::make_unique<RestoreStamina>();
	strategyArray[1] = std::make_unique<GoToTheField>();
	//�헪�w�̐���--------------------------

	startFlag = false;
}

void CharacterAI::Init()
{
	//���u��
	initFlag = true;
	//restoreStamina.Init();
	//goToTheField.Init();

	StrategyOfChoice = STRATEGY_GO_TO_THE_FIELD;
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


		//�󂯓n��-----------------------------------------------------
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
