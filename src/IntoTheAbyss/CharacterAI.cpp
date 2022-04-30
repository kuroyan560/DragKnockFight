#include "CharacterAI.h"
#include"BehavioralLayer.h"
#include"../Engine/DrawFunc.h"
#include"ScrollMgr.h"

void CharacterAI::Init()
{
	move = std::make_unique<OperateMove>(pos);
	betweenPoints = std::make_unique<MovingBetweenTwoPoints>(move);
	moveToGoal = std::make_unique<FollowPath>(betweenPoints);

	moveToGoal->Init(shortestData);
	initFlag = true;
}

void CharacterAI::Update()
{
	if (initFlag)
	{
		moveToGoal->Update();
	}
}

void CharacterAI::Draw()
{
	if (initFlag)
	{
		Vec2<float>drawPos = *pos;
		DrawFunc::DrawCircle2D(ScrollMgr::Instance()->Affect(drawPos), 10.0f, Color(100, 100, 100, 255));
	}
}
