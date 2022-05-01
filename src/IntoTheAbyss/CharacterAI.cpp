#include "CharacterAI.h"
#include"BehavioralLayer.h"
#include"../Engine/DrawFunc.h"
#include"ScrollMgr.h"

CharacterAI::CharacterAI()
{
	pos = std::make_shared<Vec2<float>>();

	//����w�̐���--------------------------
	move = std::make_shared<OperateMove>(pos);
	//����w�̐���--------------------------

	//�U�镑���w�̐���--------------------------
	betweenPoints = std::make_shared<MovingBetweenTwoPoints>(move);
	//�U�镑���w�̐���--------------------------

	//��p�w�̐���--------------------------
	moveToGoal = std::make_unique<FollowPath>(betweenPoints);
	//��p�w�̐���--------------------------


	//�헪�w�̐���--------------------------
	//�헪�w�̐���--------------------------
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
