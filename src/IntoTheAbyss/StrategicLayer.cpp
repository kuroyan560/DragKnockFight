#include "StrategicLayer.h"
#include"DebugKeyManager.h"
#include"StaminaItemMgr.h"
#include"BulletCollision.h"
#include"../IntoTheAbyss/CharacterManager.h"

RestoreStamina::RestoreStamina(const std::shared_ptr<FollowPath> &FOLLOW_PATH, const std::shared_ptr<MovingBetweenTwoPoints> &MOVING_BETWEEN_TOW_POINTS, const std::vector<std::vector<std::shared_ptr<WayPointData>>> &WAYPOINTS) :followPath(FOLLOW_PATH), moveToOhterPlace(MOVING_BETWEEN_TOW_POINTS)
{
	searchStartPoint = std::make_unique<SearchWayPoint>(WAYPOINTS);
	searchGoalPoint = std::make_unique<SearchWayPoint>(WAYPOINTS);
	readyToFollowPathFlag = false;
	readyToGoToGoalFlag = false;
	initRouteFlag = false;

	seachItemFlag = true;
	searchItemIndex = -1;
	prevStartHandle = startPoint.handle;

	searchArea.center = &CharacterManager::Instance()->Right()->pos;
	searchArea.radius = 500.0f;

	initFlag = true;
	getFlag = false;
}

void RestoreStamina::Update()
{
	std::array<StaminaItem, 100>item = StaminaItemMgr::Instance()->GetItemArray();

	if (!getFlag)
	{
		//アイテム探索を開始
		if (seachItemFlag)
		{
			std::vector<float>distance;
			std::vector<int>itemId;
			//探索範囲内にアイテムがあるのか調べる
			for (int i = 0; i < item.size(); ++i)
			{
				//アイテムを一つ以上見つけたら探索準備をする
				//そして距離を測る
				if (BulletCollision::Instance()->CheckSphereAndSphere(*item[i].GetCollisionData(), searchArea))
				{
					distance.push_back(searchArea.center->Distance(*item[i].GetCollisionData()->center));
					itemId.push_back(i);
					seachItemFlag = false;
					initFlag = false;
				}
			}

			//探索範囲内から一番近いアイテムを見る
			float minDistance = 10000.0f;
			for (int i = 0; i < distance.size(); ++i)
			{
				if (distance[i] < minDistance)
				{
					minDistance = distance[i];
					searchItemIndex = itemId[i];
				}
			}

			if (distance.size() != 0)
			{
				//ボスから最も近いウェイポイントをスタート地点とする------------------------
				searchStartPoint->Init(*searchArea.center);
				startPoint = searchStartPoint->Update();
				startPos = startPoint.pos;
				//ボスから最も近いウェイポイントをスタート地点とする------------------------
				//ゴール地点から最も近いウェイポイントをゴール地点とする--------------------------
				searchGoalPoint->Init(*item[searchItemIndex].GetCollisionData()->center);
				endPoint = searchGoalPoint->Update();
				endPos = endPoint.pos;
				//ゴール地点から最も近いウェイポイントをゴール地点とする--------------------------
			}
		}
		else
		{
			//探索範囲内に指定のアイテムがあるかどうか見る
			//無くなったら再検索をかける
			if (!BulletCollision::Instance()->CheckSphereAndSphere(*item[searchItemIndex].GetCollisionData(), searchArea))
			{
				seachItemFlag = true;
			}
		}

		//スタート地点が変わったらその場所に向かうよう初期化する
		if (!initFlag)
		{
			//ボスの現在の座標とスタート地点の移動
			moveToOhterPlace->Init(*searchArea.center, startPos);
			initFlag = true;
		}
		prevStartHandle = startPoint.handle;
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
			moveToOhterPlace->Init(endPos, *item[searchItemIndex].GetCollisionData()->center);
		}

		//探索を開始し、向かう--------------------------
	}

	//時間内に獲得したらまた近くのアイテムを探す。一定時間内にたどり着かなければ失敗
	if (!seachItemFlag && item[searchItemIndex].GetIsAcquired())
	{
		getFlag = true;
	}

	//一定時間内に一定量回復したら成功、出来なければ失敗


}

AiResult RestoreStamina::CurrentProgress()
{
	return AiResult();
}
