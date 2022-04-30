#include "BehavioralLayer.h"

MovingBetweenTwoPoints::MovingBetweenTwoPoints(const std::shared_ptr<OperateMove> &OPERATION)
{
	operateMove = OPERATION;
}

void MovingBetweenTwoPoints::Init(const Vec2<float> &START_POS, const Vec2<float> &END_POS)
{
	timer = 0;
	timeOver = 120;
	//距離を時間で割ってスピードを出す
	float time = static_cast<float>(timeOver);
	endPos = END_POS;
	vel = Vec2<float>((END_POS - START_POS) / time);

	startColision.center = operateMove->pos.get();
	startColision.radius = 5.0f;
	endColision.center = &endPos;
	endColision.radius = 5.0f;
}

void MovingBetweenTwoPoints::Update()
{
	++timer;
	operateMove->Update(vel);
	//移動に成功
	if (operateMove->CurrentProgress() == AiResult::OPERATE_SUCCESS)
	{
		bool debug = false;
	}
}

AiResult MovingBetweenTwoPoints::CurrentProgress()
{
	//たどり着いたら成功
	if (BulletCollision::Instance()->CheckSphereAndSphere(startColision, endColision))
	{
		return AiResult::OPERATE_SUCCESS;
	}
	//時間内にたどり着かなかったら失敗
	else if (timeOver <= timer)
	{
		return AiResult::OPERATE_FAIL;
	}
	//実行中
	else
	{
		return AiResult::OPERATE_INPROCESS;
	}
}
