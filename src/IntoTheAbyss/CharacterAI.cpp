#include "CharacterAI.h"
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
	moveToGoal = std::make_shared<FollowPath>(betweenPoints);
	//��p�w�̐���--------------------------


	//�헪�w�̐���--------------------------

	//�헪�w�̐���--------------------------
}

void CharacterAI::Init()
{
	//restoreStamina->Init();
		//���u��
	restoreStamina = std::make_unique<RestoreStamina>
		(moveToGoal, betweenPoints, wayPoints);
	restoreStamina->dStartPos = *pos;
	initFlag = true;
}

void CharacterAI::Update()
{
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
