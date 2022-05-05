#include "CharacterAI.h"
#include"../Engine/DrawFunc.h"
#include"ScrollMgr.h"
#include"CharacterManager.h"
#include"Stamina.h"

CharacterAI::CharacterAI()
{
	pos = std::make_shared<Vec2<float>>();

	//ìwÌ¶¬--------------------------
	move = std::make_shared<OperateMove>(pos);
	//ìwÌ¶¬--------------------------

	//Ué¢wÌ¶¬--------------------------
	betweenPoints = std::make_shared<MovingBetweenTwoPoints>(move);
	//Ué¢wÌ¶¬--------------------------

	//ípwÌ¶¬--------------------------
	moveToGoal = std::make_shared<FollowPath>(betweenPoints);
	//ípwÌ¶¬--------------------------


	//íªwÌ¶¬--------------------------

	//íªwÌ¶¬--------------------------
}

void CharacterAI::Init()
{
	//restoreStamina->Init();
	//¼u«
	restoreStamina = std::make_unique<RestoreStamina>
		(moveToGoal, betweenPoints, wayPoints);
	initFlag = true;
}

void CharacterAI::Update()
{
	//LN^[AIÉKvÈf[^Wß--------------------------
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


	//GÆvC[ÆÌ£
	CharacterAIData::Instance()->distance = CharacterManager::Instance()->Left()->pos.Distance(CharacterManager::Instance()->Right()->pos);
	//LN^[AIÉKvÈf[^Wß--------------------------


	if (initFlag && !staminaInit)
	{
		float value = restoreStamina->EvaluationFunction();
		restoreStamina->Init();
		staminaInit = true;
	}
	//move->Update(Vec2<float>(15.0f, 0.0f));

	if (initFlag)
	{
		restoreStamina->route = shortestData;
		restoreStamina->Update();
	}
}

void CharacterAI::Draw()
{
	if (initFlag)
	{
		Vec2<float>drawPos = *pos;
		DrawFunc::DrawCircle2D(ScrollMgr::Instance()->Affect(drawPos), 10.0f, Color(128, 0, 128, 255));
	}
}
