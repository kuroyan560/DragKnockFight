#include "StrategicLayer.h"
#include"DebugKeyManager.h"
#include"StaminaItemMgr.h"
#include"BulletCollision.h"
#include"../IntoTheAbyss/CharacterManager.h"

const float RestoreStamina::SEARCH_RADIUS = 500.0f;
const int RestoreStamina::SUCCEED_GAIN_STAMINA_VALUE = 3;

RestoreStamina::RestoreStamina(const std::shared_ptr<FollowPath> &FOLLOW_PATH, const std::shared_ptr<MovingBetweenTwoPoints> &MOVING_BETWEEN_TOW_POINTS, const std::vector<std::vector<std::shared_ptr<WayPointData>>> &WAYPOINTS) :followPath(FOLLOW_PATH)
{
	searchStartPoint = std::make_unique<SearchWayPoint>(WAYPOINTS);
	searchGoalPoint = std::make_unique<SearchWayPoint>(WAYPOINTS);
	initRouteFlag = true;

	seachItemFlag = true;
	searchItemIndex = -1;
	prevItemIndex = -1;
	prevStartHandle = startPoint.handle;

	searchArea.center = &CharacterManager::Instance()->Right()->pos;
	searchArea.radius = SEARCH_RADIUS;

	initFlag = true;
	getFlag = false;

	moveToOnwGround = std::make_unique<MoveToOwnGround>(followPath);
	startFlag = true;
}

void RestoreStamina::Init()
{
	staminaGauge = CharacterAIData::Instance()->bossData.stamineGauge;
	timer = 0;
	timeOver = 60 * 10;
}

void RestoreStamina::Update()
{
	std::array<StaminaItem, 100>item = StaminaItemMgr::Instance()->GetItemArray();


	//アイテム探索を開始
	if (seachItemFlag)
	{
		SearchData result = SearchItem(searchArea);

		if (result.itemIndex != -1 && result.itemIndex != prevItemIndex)
		{
			searchItemIndex = result.itemIndex;
			seachItemFlag = false;
			initFlag = false;
			//ゴール地点から最も近いウェイポイントをゴール地点とする--------------------------
			searchGoalPoint->Init(*item[searchItemIndex].GetCollisionData()->center);
			endPoint = searchGoalPoint->Update();
			endPos = endPoint.pos;
			//ゴール地点から最も近いウェイポイントをゴール地点とする--------------------------

			prevItemIndex = result.itemIndex;
			initRouteFlag = false;
			getFlag = true;
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


	//アイテムが無い場合は基本陣地に向かう
	if (!getFlag)
	{
		moveToOnwGround->route = route;
		moveToOnwGround->Update();
		startPoint = moveToOnwGround->startPoint;
		endPoint = moveToOnwGround->endPoint;
	}
	//アイテムが見つかったらその方向に向かう
	else
	{
		//ボスから最も近いウェイポイントをスタート地点とする------------------------
		if (startPoint.handle != endPoint.handle)
		{
			searchStartPoint->Init(*searchArea.center);
			startPoint = searchStartPoint->Update();
			startPos = startPoint.pos;
		}
		//ボスから最も近いウェイポイントをスタート地点とする------------------------

		//探索を開始し、向かう--------------------------
		if (route.size() != 0 && (!initRouteFlag || prevStartHandle != startPoint.handle))
		{
			followPath->Init(route);
			initRouteFlag = true;
		}
		prevStartHandle = startPoint.handle;

		//ウェイポイントに沿って移動する
		followPath->Update();
		//探索を開始し、向かう--------------------------


		//時間内に獲得したらまた近くのアイテムを探す。一定時間内にたどり着かなければ失敗
		if (!seachItemFlag && item[searchItemIndex].GetIsAcquired())
		{
			seachItemFlag = true;
			getFlag = false;
		}
	}

	//戦略実行からの経過時間
	++timer;
}

AiResult RestoreStamina::CurrentProgress()
{
	//一定時間内に一定量回復したら成功、出来なければ失敗
	int sub = staminaGauge - CharacterAIData::Instance()->bossData.stamineGauge;
	if (SUCCEED_GAIN_STAMINA_VALUE <= sub && timer < timeOver)
	{
		return AiResult::OPERATE_SUCCESS;
	}
	else if (timeOver <= timer)
	{
		return AiResult::OPERATE_FAIL;
	}
	else
	{
		return AiResult::OPERATE_INPROCESS;
	}
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


	return static_cast<float>(evaluationValue / data->EVALUATION_MAX_VALUE);
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
	result.distance = 10000.0f;
	result.itemIndex = -1;
	for (int i = 0; i < distance.size(); ++i)
	{
		if (distance[i] < result.distance)
		{
			result.distance = distance[i];
			result.itemIndex = itemId[i];
		}
	}

	return result;
}

const float AcquireASuperiorityGauge::SUCCEED_GAUGE_VALUE = 0.3f;

AcquireASuperiorityGauge::AcquireASuperiorityGauge()
{
}

void AcquireASuperiorityGauge::Init()
{
	nowGauge = CharacterAIData::Instance()->bossData.gaugeValue;
	timer = 0;
	timeOver = 60 * 10;

	crashEnemyFlag = false;
	dontCrashFlag = false;
}

void AcquireASuperiorityGauge::Update()
{
	//ウェイポイント
	const std::vector<std::vector<std::shared_ptr<WayPointData>>> wayPoints = CharacterAIData::Instance()->wayPoints;

	//自分が有利不利か判断する
	//クラッシュされる位置かされない位置かで判断する


	//自分が有利ならクラッシュしに行く
	if (crashEnemyFlag)
	{

	}

	//自分が不利ならクラッシュされないように立ち回る
	if (dontCrashFlag)
	{

	}


	//戦略実行中
	++timer;
}

AiResult AcquireASuperiorityGauge::CurrentProgress()
{
	//一定時間内に一定量回復したら成功、出来なければ失敗
	int sub = nowGauge - CharacterAIData::Instance()->bossData.gaugeValue;
	if (SUCCEED_GAUGE_VALUE <= sub && timer < timeOver)
	{
		return AiResult::OPERATE_SUCCESS;
	}
	else if (timeOver <= timer)
	{
		return AiResult::OPERATE_FAIL;
	}
	else
	{
		return AiResult::OPERATE_INPROCESS;
	}
}

GoToTheField::GoToTheField(const std::shared_ptr<FollowPath> &OPERATE)
{
	moveToOnwGround = std::make_unique<MoveToOwnGround>(OPERATE);
}

void GoToTheField::Init()
{
	timer = 0;
	timeOver = 60 * 10;
	startFlag = false;
	goToTheFieldFlag = true;
}

void GoToTheField::Update()
{
	//自分が自陣に近づく
	if (goToTheFieldFlag)
	{
		moveToOnwGround->Update();
		startPoint = moveToOnwGround->startPoint;
		endPoint = moveToOnwGround->endPoint;
		startFlag = true;
	}
	//自分が敵陣に近づかない
	else
	{

	}

	//敵を振り回しで移動させる
	if (!CharacterManager::Instance()->Right()->IsHitWall())
	{
	}
	//敵をダッシュで移動させる
	if (CharacterAIData::Instance()->dashFlag)
	{
	}
}

AiResult GoToTheField::CurrentProgress()
{
	if (timer < timeOver)
	{
		return AiResult::OPERATE_SUCCESS;
	}
	else if (timeOver <= timer)
	{
		return AiResult::OPERATE_FAIL;
	}
	else
	{
		return AiResult::OPERATE_INPROCESS;
	}
}
