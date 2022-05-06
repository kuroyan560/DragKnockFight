#include "CharacterAI.h"
#include"../Engine/DrawFunc.h"
#include"ScrollMgr.h"
#include"CharacterManager.h"
#include"Stamina.h"

CharacterAI::CharacterAI()
{
	//戦略層の生成--------------------------
	
	//戦略層の生成--------------------------

	startFlag = false;
}

void CharacterAI::Init()
{
	//仮置き
	initFlag = true;
	restoreStamina.Init();
	goToTheField.Init();
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
		float value = restoreStamina.EvaluationFunction();
		restoreStamina.Init();
		staminaInit = true;
	}
	//move->Update(Vec2<float>(15.0f, 0.0f));

	if (initFlag)
	{
		//restoreStamina->route = shortestData;
		//restoreStamina->Update();
		//startPoint = restoreStamina->startPoint;
		//endPoint = restoreStamina->endPoint;
		//startFlag = restoreStamina->startFlag;

		goToTheField.moveToOnwGround.route = shortestData;
		goToTheField.Update();
		startPoint = goToTheField.startPoint;
		endPoint = goToTheField.endPoint;
		startFlag = goToTheField.startFlag;
		CharacterManager::Instance()->Right()->vel = CharacterAIData::Instance()->vel;
	}
}

void CharacterAI::Draw()
{
	if (initFlag)
	{
	}
}
