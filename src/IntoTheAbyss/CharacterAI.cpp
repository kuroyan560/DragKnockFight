#include "CharacterAI.h"
#include"../Engine/DrawFunc.h"
#include"ScrollMgr.h"

CharacterAI::CharacterAI()
{
	pos = std::make_shared<Vec2<float>>();

	//‘€ì‘w‚Ì¶¬--------------------------
	move = std::make_shared<OperateMove>(pos);
	//‘€ì‘w‚Ì¶¬--------------------------

	//U‚é•‘‚¢‘w‚Ì¶¬--------------------------
	betweenPoints = std::make_shared<MovingBetweenTwoPoints>(move);
	//U‚é•‘‚¢‘w‚Ì¶¬--------------------------

	//íp‘w‚Ì¶¬--------------------------
	moveToGoal = std::make_shared<FollowPath>(betweenPoints);
	//íp‘w‚Ì¶¬--------------------------


	//í—ª‘w‚Ì¶¬--------------------------

	//í—ª‘w‚Ì¶¬--------------------------
}

void CharacterAI::Init()
{
	//restoreStamina->Init();
		//‰¼’u‚«
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
