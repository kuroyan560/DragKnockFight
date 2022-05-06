#include "TacticalLayer.h"
#include"CharacterManager.h"
#include"StageMgr.h"

FollowPath::FollowPath()
{
	goalFlag = false;
}

void FollowPath::Init(const std::vector<WayPointData> &ROUTE)
{
	route = ROUTE;
	routeHandle = 0;
	goalFlag = false;
	operateFollow.Init(route[0].pos, route[1].pos);
}

void FollowPath::Update()
{
	//初手ウェイポイントと離れていたら近づく
	//二点間の移動
	operateFollow.Update();

	//成功したら別の二点間の移動を行う
	if (operateFollow.CurrentProgress() == AiResult::OPERATE_SUCCESS)
	{
		++routeHandle;
		//最短ルートのゴールまで辿り着いた
		if (route.size() <= routeHandle || route.size() <= routeHandle + 1)
		{
			goalFlag = true;
		}
		//たどり着くまで二点間の移動の続ける
		else
		{
			operateFollow.Init(route[routeHandle].pos, route[routeHandle + 1].pos);
		}
	}
}

AiResult FollowPath::CurrentProgress()
{
	if (goalFlag)
	{
		return AiResult::OPERATE_SUCCESS;
	}
	else
	{
		return AiResult::OPERATE_INPROCESS;
	}
}



MoveToOwnGround::MoveToOwnGround()
{
	timer = 0;
	timeOver = 60 * 3;
}

void MoveToOwnGround::Init()
{

}

void MoveToOwnGround::Update()
{
	//探索開始
	if (route.size() != 0 && (startPoint.handle != prevStartHandle || endPoint.handle != prevEndHandle))
	{
		followPath.Init(route);
		prevStartHandle = startPoint.handle;
		prevEndHandle = endPoint.handle;
	}
	followPath.Update();


	//スタート始点の探索
	searchPoint.Init(CharacterManager::Instance()->Right()->pos);
	startPoint = searchPoint.Update();

	//一定時間ごとに何処をゴールにするか切り替える
	if (timeOver <= timer || timer == 0)
	{
		//ゴール地点の探索
		Vec2<float>goalPos(CharacterManager::Instance()->Right()->pos);
		//X軸は自陣、Y軸はボスの座標に合わせた座標
		goalPos.x = (StageMgr::Instance()->GetMapChipData(0, 0)[0].size() * 50.0f) - 150.0f;
		searchPoint.Init(goalPos);
		endPoint = searchPoint.Update();
		timer = 1;
	}
	++timer;
}

AiResult MoveToOwnGround::CurrentProgress()
{
	return AiResult();
}
