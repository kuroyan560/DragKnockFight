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
	stackFlag = false;
	operateFollow.Init(route[0].pos, route[1].pos);

	initFlag = false;
}

void FollowPath::Update()
{
	//スタック判定が出たらスタック用の処理を動かし、それ以外はパスを辿る
	if (!stackFlag)
	{
		operateFollow.Update();
	}
	else
	{
		dontStack.Update();
	}

	AiResult result = operateFollow.CurrentProgress();

	//成功したら別の二点間の移動を行う
	if (result == AiResult::OPERATE_SUCCESS)
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

	//ボスが何かしらの方法で動きが制限されていない
	bool canMoveFlag = CharacterManager::Instance()->Right()->GetNowSwing();

	//動きが止まっていることを確認したらスタックしない場所を探す
	if (result == AiResult::OPERATE_FAIL && routeHandle < route.size() && initFlag)
	{
		dontStack.Init(CharacterManager::Instance()->Right()->pos);
		stackFlag = true;
	}
	//成功でも失敗でもスタック状態から移動した判定
	if (dontStack.CurrentProgress() != AiResult::OPERATE_INPROCESS)
	{
		stackFlag = false;
	}

	initFlag = true;
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
		int goalXPoint = CharacterAIData::Instance()->wayPoints[0].size() - 3;

		//陣地側のY座標ランダムで取る
		int goalYPoint = KuroFunc::GetRand(0, CharacterAIData::Instance()->wayPoints.size() - 1);
		while (CharacterAIData::Instance()->wayPoints[goalYPoint][goalXPoint].isWall)
		{
			goalYPoint = KuroFunc::GetRand(0, CharacterAIData::Instance()->wayPoints.size() - 1);
		}
		endPoint = CharacterAIData::Instance()->wayPoints[goalYPoint][goalXPoint];
		timer = 1;
	}
	++timer;
}

AiResult MoveToOwnGround::CurrentProgress()
{
	return AiResult();
}