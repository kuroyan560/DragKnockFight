#include "StrategicLayer.h"
#include"DebugKeyManager.h"

RestoreStamina::RestoreStamina(const std::shared_ptr<FollowPath> &FOLLOW_PATH, const std::shared_ptr<MovingBetweenTwoPoints> &MOVING_BETWEEN_TOW_POINTS, const std::array<std::array<std::shared_ptr<WayPointData>, NavigationAI::WAYPOINT_MAX_Y>, NavigationAI::WAYPOINT_MAX_X> &WAYPOINTS) :followPath(FOLLOW_PATH), moveToOhterPlace(MOVING_BETWEEN_TOW_POINTS)
{
	searchStartPoint = std::make_unique<SearchWayPoint>(WAYPOINTS);
	searchGoalPoint = std::make_unique<SearchWayPoint>(WAYPOINTS);
	initFlag = false;
	readyToFollowPathFlag = false;
	readyToGoToGoalFlag = false;
	dEndPos = { 1300.0f,400.0f };
	initRouteFlag = false;
}

void RestoreStamina::Update()
{
	//探索範囲内にアイテムがあるのか調べる
	if (1)
	{

	}

	if (DebugKeyManager::Instance()->DebugKeyTrigger(DIK_F, "BossStart", "DIK_F"))
	{
		initFlag = true;
	}

	//探索したアイテムから近場のウェイポイントを調べる。見つけたらそのウェイポイントをゴールとする
	if (!initFlag)
	{
		//ボスから最も近いウェイポイントをスタート地点とする
		searchStartPoint->Init(dStartPos);
		startPoint = searchStartPoint->Update();
		startPos = startPoint.pos;

		searchGoalPoint->Init(dEndPos);
		endPoint = searchGoalPoint->Update();
		endPos = endPoint.pos;

		//ボスの現在の座標とスタート地点の移動
		moveToOhterPlace->Init(dStartPos, startPos);
		initFlag = true;
	}
	moveToOhterPlace->Update();

	//探索を開始し、向かう--------------------------
	//近くのウェイポイントに移動する
	if (moveToOhterPlace->CurrentProgress() == AiResult::OPERATE_SUCCESS && !readyToFollowPathFlag)
	{
		readyToFollowPathFlag = true;
		startFlag = true;
	}
	if (route.size() != 0 && !initRouteFlag)
	{
		followPath->Init(route);
		initRouteFlag = true;
	}

	//ウェイポイントに沿って移動する
	if (readyToFollowPathFlag && !readyToGoToGoalFlag && route.size() != 0)
	{
		followPath->Update();
	}
	//ゴールまでたどり着いたら目的地まで向かう
	if (followPath->CurrentProgress() == AiResult::OPERATE_SUCCESS && !readyToGoToGoalFlag)
	{
		readyToGoToGoalFlag = true;
		moveToOhterPlace->Init(endPos, dEndPos);
	}

	//探索を開始し、向かう--------------------------


	//時間内に獲得したらまた近くのアイテムを探す。一定時間内にたどり着かなければ失敗

	//一定時間内に一定量回復したら成功、出来なければ失敗


}

AiResult RestoreStamina::CurrentProgress()
{
	return AiResult();
}
