#include "BehavioralLayer.h"
#include"CharacterManager.h"

MovingBetweenTwoPoints::MovingBetweenTwoPoints()
{
	initFlag = false;
}

void MovingBetweenTwoPoints::Init(const Vec2<float> &START_POS, const Vec2<float> &END_POS)
{
	timer = 0;
	timeOver = 120;
	//���������ԂŊ����ăX�s�[�h���o��
	float time = static_cast<float>(timeOver);
	endPos = END_POS;
	Vec2<float>normal((END_POS - START_POS));
	normal.Normalize();
	vel = normal * Vec2<float>(14.0f, 14.0f);

	startColision.center = &CharacterManager::Instance()->Right()->pos;
	startColision.radius = 5.0f;
	endColision.center = &endPos;
	endColision.radius = 20.0f;
	initFlag = true;
}

void MovingBetweenTwoPoints::Update()
{
	++timer;
	operateMove.Update(vel);
	//�ړ��ɐ���
	if (operateMove.CurrentProgress() == AiResult::OPERATE_SUCCESS)
	{
		bool debug = false;
	}
}

AiResult MovingBetweenTwoPoints::CurrentProgress()
{
	if (initFlag)
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
}

SearchWayPoint::SearchWayPoint()
{
}

void SearchWayPoint::Init(const Vec2<float> &START_POS)
{
	startPos = START_POS;
}

const WayPointData &SearchWayPoint::Update()
{
	wayPoints = CharacterAIData::Instance()->wayPoints;
	float minDistance = 1000000.0f;
	Vec2<int>handle;
	//�ł��߂��E�F�C�|�C���g�̒T��
	for (int y = 0; y < wayPoints.size(); ++y)
	{
		for (int x = 0; x < wayPoints[y].size(); ++x)
		{
			if (wayPoints[y][x]->handle != Vec2<int>(-1, -1))
			{
				float distance = startPos.Distance(wayPoints[y][x]->pos);
				if (distance < minDistance)
				{
					minDistance = distance;
					handle = { x,y };
				}
			}
		}
	}

	return *wayPoints[handle.y][handle.x];
}

AiResult SearchWayPoint::CurrentProgress()
{
	return AiResult();
}
