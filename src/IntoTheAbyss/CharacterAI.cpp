#include "CharacterAI.h"
#include"BehavioralLayer.h"
#include"../Engine/DrawFunc.h"
#include"ScrollMgr.h"

CharacterAI::CharacterAI()
{
	pos = std::make_unique<Vec2<float>>();
	move = std::make_unique<OperateMove>(pos);
	betweenPoints = std::make_unique<MovingBetweenTwoPoints>(move);
	moveToGoal = std::make_unique<FollowPath>(betweenPoints);
}

void CharacterAI::Init()
{
	moveToGoal->Init(shortestData);
	initFlag = true;
}

void CharacterAI::Update()
{
	if (initFlag && moveToGoal->CurrentProgress() == AiResult::OPERATE_INPROCESS)
	{
		moveToGoal->Update();
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
