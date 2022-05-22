#include "BehavioralLayer.h"
#include"CharacterManager.h"
#include"Stamina.h"
#include"DebugParameter.h"

MovingBetweenTwoPoints::MovingBetweenTwoPoints()
{
	initFlag = false;
	dashTimer = 0;
}

void MovingBetweenTwoPoints::Init(const Vec2<float> &START_POS, const Vec2<float> &END_POS, bool STACK_FLAG)
{
	timer = 0;
	timeOver = 120;
	//���������ԂŊ����ăX�s�[�h���o��
	float time = static_cast<float>(timeOver);
	endPos = END_POS;
	Vec2<float>normal((END_POS - START_POS));
	normal.Normalize();

	float localVel = DebugParameter::Instance()->GetBossData().vel;
	vel = normal * Vec2<float>(localVel, localVel);
	if (STACK_FLAG)
	{
		vel.x = 0.0f;
	}

	startColision.center = &CharacterManager::Instance()->Right()->pos;
	startColision.radius = 5.0f;
	endColision.center = &endPos;
	endColision.radius = 20.0f;
	initFlag = true;

}

void MovingBetweenTwoPoints::Update()
{
	++timer;

	//�v���C���[���_�b�V�������炻�̌�ɒx��ă_�b�V������
	const float USE_DASH_STAMINA_GAUGE = 0.3f;
	//�v���C���[���_�b�V���������ɒx��ă_�b�V������
	const bool dashCounterFlag = CharacterAIData::Instance()->dashFlag && USE_DASH_STAMINA_GAUGE <= CharacterAIData::Instance()->bossData.stamineGauge;

	//�f�o�b�N�ŐU��񂵌�Ƀ_�b�V�����邩���w�肵������
	if (DebugParameter::Instance()->GetBossData().enableToDashAfterSwingFlag)
	{
		if (dashCounterFlag || CharacterAIData::Instance()->releaseSwingFlag)
		{
			dashFlag = true;
		}
	}
	else
	{
		if (dashCounterFlag)
		{
			dashFlag = true;
		}
	}

	if (dashFlag)
	{
		++dashTimer;
	}
	//�X�^�~�i�̌��Z�͎኱�x��čs��
	if (15 <= dashTimer)
	{
		operateDash.Init(vel / 2.0f);
		CharacterAIOrder::Instance()->dashFlag = true;
		CharacterAIData::Instance()->finishDashFlag = true;
		CharacterManager::Instance()->Right()->staminaGauge->ConsumesStamina(DebugParameter::Instance()->GetBossData().staminaDash);
		dashTimer = 0;
		dashFlag = false;
	}

	operateMove.Update(vel + operateDash.Update());
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
		//���ԓ��ɂ��ǂ蒅���Ȃ��A�������͓������~�܂����玸�s
		else if (timeOver <= timer || operateMove.CurrentProgress() == AiResult::OPERATE_FAIL)
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
			if (wayPoints[y][x].handle != Vec2<int>(-1, -1))
			{
				float distance = startPos.Distance(wayPoints[y][x].pos);
				if (distance < minDistance)
				{
					minDistance = distance;
					handle = { x,y };
				}
			}
		}
	}
	return wayPoints[handle.y][handle.x];
}

AiResult SearchWayPoint::CurrentProgress()
{
	return AiResult();
}

SearchingToNotGetStuck::SearchingToNotGetStuck()
{
}

const WayPointData &SearchingToNotGetStuck::Update(const Vec2<float> START_POS)
{
	search.Init(START_POS);
	WayPointData startWayPoint = search.Update();
	wayPoints = CharacterAIData::Instance()->wayPoints;

	Vec2<int>handle = startWayPoint.handle;

	int downIndex = 0;
	WayPointData downResult;
	//�������̒T��
	while (true)
	{
		WayPointData check = wayPoints[handle.y + downIndex][handle.x];
		//�ǂȂ�T�����Ȃ�
		if (check.isWall)
		{
			downIndex = 0;
			break;
		}
		//�E���̕ǂ̋��������ȏ㗣��Ă�������Ƃ��đI������
		else if (200 <= check.wallDistanceRight)
		{
			downResult = check;
			break;
		}
		++downIndex;
	}



	int topIndex = 0;
	WayPointData topResult;
	//������̒T��
	while (true)
	{
		WayPointData check = wayPoints[handle.y + topIndex][handle.x];
		//�ǂȂ�T�����Ȃ�
		if (check.isWall)
		{
			topIndex = 0;
			break;
		}
		//�E���̕ǂ̋��������ȏ㗣��Ă�������Ƃ��đI������
		else if (200 <= check.wallDistanceRight)
		{
			topResult = check;
			break;
		}
		++topIndex;
	}

	//������̃E�F�C�|�C���g��艺�����̃E�F�C�|�C���g���E�̕ǂƂ̋���������������X�^�b�N���Ȃ��ꏊ�Ƃ��č̗p����
	if (topIndex < downIndex)
	{
		return topResult;
	}
	else
	{
		return downResult;
	}
}

AiResult SearchingToNotGetStuck::CurrentProgress()
{
	return AiResult();
}


MoveToNotStack::MoveToNotStack()
{
}

void MoveToNotStack::Init(const Vec2<float> &START_POS)
{
	WayPointData endPoint = searchToNotStack.Update(START_POS);
	operateFollow.Init(START_POS, endPoint.pos, true);
}

void MoveToNotStack::Update()
{
	operateFollow.Update();
}

AiResult MoveToNotStack::CurrentProgress()
{
	if (operateFollow.CurrentProgress() == AiResult::OPERATE_SUCCESS)
	{
		return AiResult::OPERATE_SUCCESS;
	}
	else
	{
		return AiResult::OPERATE_INPROCESS;
	}
}