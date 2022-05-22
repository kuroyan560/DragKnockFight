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
	//距離を時間で割ってスピードを出す
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

	//プレイヤーがダッシュしたらその後に遅れてダッシュする
	const float USE_DASH_STAMINA_GAUGE = 0.3f;
	//プレイヤーがダッシュした時に遅れてダッシュする
	const bool dashCounterFlag = CharacterAIData::Instance()->dashFlag && USE_DASH_STAMINA_GAUGE <= CharacterAIData::Instance()->bossData.stamineGauge;

	//デバックで振り回し後にダッシュするかを指定したもの
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
	//スタミナの減算は若干遅れて行う
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
		//たどり着いたら成功
		if (BulletCollision::Instance()->CheckSphereAndSphere(startColision, endColision))
		{
			return AiResult::OPERATE_SUCCESS;
		}
		//時間内にたどり着かない、もしくは動きが止まったら失敗
		else if (timeOver <= timer || operateMove.CurrentProgress() == AiResult::OPERATE_FAIL)
		{
			return AiResult::OPERATE_FAIL;
		}
		//実行中
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
	//最も近いウェイポイントの探索
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
	//下方向の探索
	while (true)
	{
		WayPointData check = wayPoints[handle.y + downIndex][handle.x];
		//壁なら探索しない
		if (check.isWall)
		{
			downIndex = 0;
			break;
		}
		//右側の壁の距離が一定以上離れていたら候補として選択する
		else if (200 <= check.wallDistanceRight)
		{
			downResult = check;
			break;
		}
		++downIndex;
	}



	int topIndex = 0;
	WayPointData topResult;
	//上方向の探索
	while (true)
	{
		WayPointData check = wayPoints[handle.y + topIndex][handle.x];
		//壁なら探索しない
		if (check.isWall)
		{
			topIndex = 0;
			break;
		}
		//右側の壁の距離が一定以上離れていたら候補として選択する
		else if (200 <= check.wallDistanceRight)
		{
			topResult = check;
			break;
		}
		++topIndex;
	}

	//上方向のウェイポイントより下方向のウェイポイントが右の壁との距離が遠かったらスタックしない場所として採用する
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