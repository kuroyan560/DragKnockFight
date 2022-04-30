#include "TacticalLayer.h"

FollowPath::FollowPath(const std::shared_ptr<MovingBetweenTwoPoints> &OPERATE)
{
	operateFollow = OPERATE;
}

void FollowPath::Init(const std::vector<WayPointData> &ROUTE)
{
	route = ROUTE;
	routeHandle = 0;
	operateFollow->Init(route[0].pos, route[1].pos);
}

void FollowPath::Update()
{
	//初手ウェイポイントと離れていたら近づく


	//二点間の移動
	operateFollow->Update();

	//成功したら別の二点を移動する
	if (operateFollow->CurrentProgress() == AiResult::OPERATE_SUCCESS)
	{
		++routeHandle;
		//最短ルートのゴールまで辿り着いた
		if (route.size() <= routeHandle || route.size() <= routeHandle + 1)
		{
			bool debug(false);
		}
		//たどり着くまで二点間の移動の続ける
		else
		{
			operateFollow->Init(route[routeHandle].pos, route[routeHandle + 1].pos);
		}
	}
}

AiResult FollowPath::CurrentProgress()
{
	return AiResult();
}
