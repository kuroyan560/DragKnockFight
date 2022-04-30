#include "BehavioralLayer.h"

MovingBetweenTwoPoints::MovingBetweenTwoPoints(const std::shared_ptr<OperateMove> &OPERATION)
{
	operateMove = OPERATION;
}

void MovingBetweenTwoPoints::Init(const Vec2<float> &START_POS, const Vec2<float> &END_POS)
{
	timer = 0;
	timeOver = 120;
	//���������ԂŊ����ăX�s�[�h���o��
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
	//�ړ��ɐ���
	if (operateMove->CurrentProgress() == AiResult::OPERATE_SUCCESS)
	{
		bool debug = false;
	}
}

AiResult MovingBetweenTwoPoints::CurrentProgress()
{
	//���ǂ蒅�����琬��
	if (BulletCollision::Instance()->CheckSphereAndSphere(startColision, endColision))
	{
		return AiResult::OPERATE_SUCCESS;
	}
	//���ԓ��ɂ��ǂ蒅���Ȃ������玸�s
	else if (timeOver <= timer)
	{
		return AiResult::OPERATE_FAIL;
	}
	//���s��
	else
	{
		return AiResult::OPERATE_INPROCESS;
	}
}
