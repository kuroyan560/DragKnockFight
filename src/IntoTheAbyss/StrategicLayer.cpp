#include "StrategicLayer.h"
#include"DebugKeyManager.h"
#include"StaminaItemMgr.h"
#include"BulletCollision.h"
#include"../IntoTheAbyss/CharacterManager.h"

const float RestoreStamina::SEARCH_RADIUS = 500.0f;

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
	searchArea.radius = SEARCH_RADIUS;

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
			SearchData result = SearchItem(searchArea);

			if (result.itemIndex != -1)
			{
				seachItemFlag = false;
				initFlag = false;
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

float RestoreStamina::EvaluationFunction()
{
	CharacterAIData *data = CharacterAIData::Instance();
	//評価値
	int evaluationValue = 0;
	//マップチップ番号
	Vec2<int>bossHandle;
	bossHandle.x = static_cast<int>(CharacterManager::Instance()->Right()->pos.x / 50.0f);
	bossHandle.y = static_cast<int>(CharacterManager::Instance()->Right()->pos.y / 50.0f);
	Vec2<int>playerHandle;
	playerHandle.x = static_cast<int>(CharacterManager::Instance()->Left()->pos.x / 50.0f);
	playerHandle.y = static_cast<int>(CharacterManager::Instance()->Left()->pos.y / 50.0f);



	//優勢ゲージ n6~7
	if (0.6f <= data->bossData.gaugeValue && data->bossData.gaugeValue <= 0.7f)
	{
		evaluationValue += 2;
	}
	//スタミナが自分＜敵
	if (data->playerData.stamineGauge < data->bossData.stamineGauge)
	{
		evaluationValue += 2;
	}

	const float nearDistance = 200.0f;
	//自分と壁との距離が近い
	if (!data->wayPoints[bossHandle.y][bossHandle.x]->isWall)
	{
		bool nearTopFlag = data->wayPoints[bossHandle.y][bossHandle.x]->wallDistanceTop < nearDistance;
		bool nearBottomFlag = data->wayPoints[bossHandle.y][bossHandle.x]->wallDistanceBottom < nearDistance;
		bool nearLeftFlag = data->wayPoints[bossHandle.y][bossHandle.x]->wallDistanceLeft < nearDistance;
		bool nearRightFlag = data->wayPoints[bossHandle.y][bossHandle.x]->wallDistanceRight < nearDistance;

		if (nearTopFlag || nearBottomFlag || nearLeftFlag || nearRightFlag)
		{
			evaluationValue += 2;
		}
	}
	//敵と壁との距離が近くない
	if (!data->wayPoints[playerHandle.y][playerHandle.x]->isWall)
	{
		bool farTopFlag = nearDistance < data->wayPoints[playerHandle.y][playerHandle.x]->wallDistanceTop;
		bool farBottomFlag = nearDistance < data->wayPoints[playerHandle.y][playerHandle.x]->wallDistanceBottom;
		bool farLeftFlag = nearDistance < data->wayPoints[playerHandle.y][playerHandle.x]->wallDistanceLeft;
		bool farRightFlag = nearDistance < data->wayPoints[playerHandle.y][playerHandle.x]->wallDistanceRight;

		if (farTopFlag || farBottomFlag || farLeftFlag || farRightFlag)
		{
			evaluationValue += 1;
		}
	}

	//敵より自分の方がアイテムが近い
	//自分側
	SearchData bossResult = SearchItem(searchArea);
	//敵側
	SphereCollision hitBox;
	hitBox.center = &CharacterManager::Instance()->Left()->pos;
	hitBox.radius = SEARCH_RADIUS;
	SearchData playerResult = SearchItem(hitBox);
	if (playerResult.distance < bossResult.distance)
	{
		evaluationValue += 2;
	}

	//敵の振り回し入力があった
	if (data->swingFlag)
	{
		evaluationValue += 2;
	}


	//自陣へのウェイポイントが少ない


	return static_cast<float>(evaluationValue / 10);
}

RestoreStamina::SearchData RestoreStamina::SearchItem(const SphereCollision &DATA)
{
	std::array<StaminaItem, 100>item = StaminaItemMgr::Instance()->GetItemArray();

	std::vector<float>distance;
	std::vector<int>itemId;
	//探索範囲内にアイテムがあるのか調べる
	for (int i = 0; i < item.size(); ++i)
	{
		//アイテムを一つ以上見つけたら探索準備をする
		//そして距離を測る
		if (BulletCollision::Instance()->CheckSphereAndSphere(*item[i].GetCollisionData(), DATA))
		{
			distance.push_back(DATA.center->Distance(*item[i].GetCollisionData()->center));
			itemId.push_back(i);
		}
	}

	//探索範囲内から一番近いアイテムを見る
	SearchData result;
	result.distance = -1.0f;
	result.itemIndex = -1;
	float minDistance = 10000.0f;
	for (int i = 0; i < distance.size(); ++i)
	{
		if (distance[i] < minDistance)
		{
			result.distance = minDistance;
			result.itemIndex = itemId[i];
		}
	}

	return result;
}
