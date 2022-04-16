#include "BossPatternNormalMove.h"
#include "Camera.h"
#include"../KuroEngine.h"
#include"../IntoTheAbyss/DebugParameter.h"
#include"SelectStage.h"
#include"StageMgr.h"
#include"Collider.h"

BossPatternNormalMove::BossPatternNormalMove()
{
}

void BossPatternNormalMove::Init()
{
	pullTimer = 0;
}

void BossPatternNormalMove::Update(BossPatternData *DATA)
{
	if (Camera::Instance()->Active())
	{
		*DATA->moveVel = { 0,0 };
		return;
	}

	bool allDidntHitFlg = true;
	//近距離レイと壁との判定
	for (int i = 0; i < DATA->nearLimmitLine.size(); ++i)
	{
		DATA->nearLimmitLine[i].hitFlag = CheckMapChipWallAndRay(DATA->nearLimmitLine[i].startPos, DATA->nearLimmitLine[i].endPos);
		
		if (DATA->nearLimmitLine[i].hitFlag)
		{
			allDidntHitFlg = false;
		}
	}


	//遠距離レイと壁との判定
	for (int i = 0; i < DATA->farLimmitLine.size(); ++i)
	{
		//近距離の判定が全部なくなるまで有効にし続ける
		if (CheckMapChipWallAndRay(DATA->farLimmitLine[i].startPos, DATA->farLimmitLine[i].endPos))
		{
			DATA->farLimmitLine[i].hitFlag = true;
		}
		if (allDidntHitFlg)
		{
			DATA->farLimmitLine[i].hitFlag = false;
		}
	}







	int PULL_SPAN_MIN = DebugParameter::Instance()->bossDebugData.PULL_SPAN_MIN;
	int PULL_SPAN_MAX = DebugParameter::Instance()->bossDebugData.PULL_SPAN_MAX;
	int PULL_SPAN = KuroFunc::GetRand(PULL_SPAN_MIN, PULL_SPAN_MAX);

	Vec2<float>ACCEL = { 0.0f,0.0f };	//加速度
	float PULL_POWER_MIN = DebugParameter::Instance()->bossDebugData.PULL_POWER_MIN;
	float PULL_POWER_MAX = DebugParameter::Instance()->bossDebugData.PULL_POWER_MAX;

	if (pullTimer < PULL_SPAN)
	{
		++pullTimer;
		if (PULL_SPAN <= pullTimer)
		{
			PULL_SPAN = KuroFunc::GetRand(PULL_SPAN_MIN, PULL_SPAN_MAX);
			pullTimer = 0;

			//どの方向に進んでいいのか
			//auto rad = Angle::ConvertToRadian(KuroFunc::GetRand(-70, 70));
			float rad = GetDir(DATA->nearLimmitLine, DATA->farLimmitLine);
			auto power = KuroFunc::GetRand(PULL_POWER_MIN, PULL_POWER_MAX);
			ACCEL.x = cos(rad) * power * DebugParameter::Instance()->bossDebugData.PULL_ADD_X_POWER;
			ACCEL.y = sin(rad) * power;
		}
	}
	DATA->moveVel->x = KuroMath::Lerp(DATA->moveVel->x, DebugParameter::Instance()->bossDebugData.moveX, 0.1f);
	DATA->moveVel->y = KuroMath::Lerp(DATA->moveVel->y, 0.0f, 0.1f);
	*DATA->moveVel += ACCEL;

	ACCEL = KuroMath::Lerp(ACCEL, { 0.0f,0.0f }, 0.8f);

	if (UsersInput::Instance()->Input(DIK_0)) {
		//	vel.x = OFFSET_VEL * 5.0f;
		bool debug = false;
	}
}

bool BossPatternNormalMove::CheckMapChipWallAndRay(const Vec2<float> &START_POS, const Vec2<float> &END_POS)
{
	//どうやって使うか
	Vec2<float>handSegmentStart(START_POS), handSegmentEnd(END_POS);//線分
	Vec2<float>handSegmentDir(END_POS - START_POS);					//線分の方向
	Vec2<float>handPos(START_POS);									//線分の始点
	Vec2<float>sightPos;						//求められた交点の中の最短距離
	RoomMapChipArray mapData = StageMgr::Instance()->GetMapChipData(SelectStage::Instance()->GetStageNum(), SelectStage::Instance()->GetRoomNum());					//マップ
	//どうやって使うか


	// 最短距離を保存するようの配列。
	std::vector<std::pair<Vec2<float>, float>> shortestPoints;

	// 照準のレイの方向によって当たり判定を無効化する為のフラグをセットする。
	bool isTop = handSegmentDir.y < 0;
	bool isLeft = handSegmentDir.x < 0;

	// 次にマップチップとの最短距離を求める。
	const int MAP_Y = mapData.size();
	for (int height = 0; height < MAP_Y; ++height) {

		const int MAP_X = mapData[height].size();
		for (int width = 0; width < MAP_X; ++width) {

			// このマップチップが1~9以外だったら判定を飛ばす。
			if (mapData[height][width] < 1 || mapData[height][width] > 9) continue;

			// このインデックスのブロックの座標を取得。
			const Vec2<float> BLOCK_POS = Vec2<float>(width * MAP_CHIP_SIZE, height * MAP_CHIP_SIZE);

			Vec2<int> windowSize = WinApp::Instance()->GetWinCenter();

			// 一定範囲以外だったら処理を飛ばす。
			bool checkInsideTop = BLOCK_POS.y < handPos.y - windowSize.y;
			bool checkInsideBottom = handPos.y + windowSize.y > BLOCK_POS.y;
			bool checkInsideLeft = BLOCK_POS.x < handPos.x + windowSize.x;
			bool checkInsideRight = handPos.x + windowSize.x > BLOCK_POS.x;
			if (checkInsideTop && checkInsideBottom && checkInsideLeft && checkInsideRight) {
				//player.onGround = false;
				continue;
			}

			// そのブロックが内包されているブロックだったら処理を飛ばす。
			Vec2<int> mapChipIndex = { width, height };
			if (StageMgr::Instance()->IsItWallIn(SelectStage::Instance()->GetStageNum(), SelectStage::Instance()->GetRoomNum(), mapChipIndex)) {

				continue;

			}

			// レイの方向とブロックの位置関係で処理を飛ばす。ウィンドウを4分割するやつ
			float offsetHandPos = MAP_CHIP_SIZE;
			if (isLeft && handPos.x + offsetHandPos < BLOCK_POS.x) continue;
			if (!isLeft && BLOCK_POS.x < handPos.x - offsetHandPos) continue;
			if (isTop && handPos.y + offsetHandPos < BLOCK_POS.y) continue;
			if (!isTop && BLOCK_POS.y < handPos.y - offsetHandPos) continue;


			// 四辺分交点を求める。

			// 交点保存用
			vector<Vec2<float>> intersectedPos;

			// 上方向
			if (Collider::Instance()->IsIntersected(handSegmentStart, handSegmentEnd, Vec2<float>(BLOCK_POS.x - MAP_CHIP_HALF_SIZE, BLOCK_POS.y - MAP_CHIP_HALF_SIZE), Vec2<float>(BLOCK_POS.x + MAP_CHIP_HALF_SIZE, BLOCK_POS.y - MAP_CHIP_HALF_SIZE))) {

				// 交点を求めて保存する。
				intersectedPos.push_back(Collider::Instance()->CalIntersectPoint(handSegmentStart, handSegmentEnd, Vec2<float>(BLOCK_POS.x - MAP_CHIP_HALF_SIZE, BLOCK_POS.y - MAP_CHIP_HALF_SIZE), Vec2<float>(BLOCK_POS.x + MAP_CHIP_HALF_SIZE, BLOCK_POS.y - MAP_CHIP_HALF_SIZE)));

			}
			// 右方向
			if (Collider::Instance()->IsIntersected(handSegmentStart, handSegmentEnd, Vec2<float>(BLOCK_POS.x + MAP_CHIP_HALF_SIZE, BLOCK_POS.y - MAP_CHIP_HALF_SIZE), Vec2<float>(BLOCK_POS.x + MAP_CHIP_HALF_SIZE, BLOCK_POS.y + MAP_CHIP_HALF_SIZE))) {

				// 交点を求めて保存する。
				intersectedPos.push_back(Collider::Instance()->CalIntersectPoint(handSegmentStart, handSegmentEnd, Vec2<float>(BLOCK_POS.x + MAP_CHIP_HALF_SIZE, BLOCK_POS.y - MAP_CHIP_HALF_SIZE), Vec2<float>(BLOCK_POS.x + MAP_CHIP_HALF_SIZE, BLOCK_POS.y + MAP_CHIP_HALF_SIZE)));

			}
			// 下方向
			if (Collider::Instance()->IsIntersected(handSegmentStart, handSegmentEnd, Vec2<float>(BLOCK_POS.x - MAP_CHIP_HALF_SIZE, BLOCK_POS.y + MAP_CHIP_HALF_SIZE), Vec2<float>(BLOCK_POS.x + MAP_CHIP_HALF_SIZE, BLOCK_POS.y + MAP_CHIP_HALF_SIZE))) {

				// 交点を求めて保存する。
				intersectedPos.push_back(Collider::Instance()->CalIntersectPoint(handSegmentStart, handSegmentEnd, Vec2<float>(BLOCK_POS.x - MAP_CHIP_HALF_SIZE, BLOCK_POS.y + MAP_CHIP_HALF_SIZE), Vec2<float>(BLOCK_POS.x + MAP_CHIP_HALF_SIZE, BLOCK_POS.y + MAP_CHIP_HALF_SIZE)));

			}
			// 左方向
			if (Collider::Instance()->IsIntersected(handSegmentStart, handSegmentEnd, Vec2<float>(BLOCK_POS.x - MAP_CHIP_HALF_SIZE, BLOCK_POS.y - MAP_CHIP_HALF_SIZE), Vec2<float>(BLOCK_POS.x - MAP_CHIP_HALF_SIZE, BLOCK_POS.y + MAP_CHIP_HALF_SIZE))) {

				// 交点を求めて保存する。
				intersectedPos.push_back(Collider::Instance()->CalIntersectPoint(handSegmentStart, handSegmentEnd, Vec2<float>(BLOCK_POS.x - MAP_CHIP_HALF_SIZE, BLOCK_POS.y - MAP_CHIP_HALF_SIZE), Vec2<float>(BLOCK_POS.x - MAP_CHIP_HALF_SIZE, BLOCK_POS.y + MAP_CHIP_HALF_SIZE)));

			}

			// 最短距離を求める。
			Vec2<float> shortestPos = {};
			float shoterstLength = 1000000.0f;

			// サイズが0だったら処理を飛ばす。
			const int INTERSECTED_COUNT = intersectedPos.size();
			if (INTERSECTED_COUNT <= 0) continue;

			// 最短距離を求める。
			for (int index = 0; index < INTERSECTED_COUNT; ++index) {

				// 保存されているデータより大きかったら処理を飛ばす。
				float lengthBuff = Vec2<float>(intersectedPos[index] - handPos).Length();
				if (lengthBuff >= shoterstLength) continue;

				// データを保存する。
				shoterstLength = lengthBuff;
				shortestPos = intersectedPos[index];

			}

			// 最短の距離を保存する。
			pair<Vec2<float>, float> buff = { shortestPos, shoterstLength };
			shortestPoints.push_back(buff);
		}
	}


	/*-- ここまでの過程で様々な最短を求めることができた。 --*/

	// 最短の座標を保存する用変数。
	float shortestLength = 100000.0f;

	// 全ての最短の中から最も短いものを求める。
	const int SHORTEST_COUNT = shortestPoints.size();

	// サイズが0だったら照準をどっかに飛ばしてリターン。
	if (SHORTEST_COUNT <= 0) {

		sightPos = { -100,-100 };
		return false;
	}

	for (int index = 0; index < SHORTEST_COUNT; ++index) {

		// 保存されているデータより大きかったら処理を飛ばす。
		if (shortestPoints[index].second >= shortestLength) continue;

		// データを保存する。
		shortestLength = shortestPoints[index].second;
		sightPos = shortestPoints[index].first;
	}


	//最短距離が一つでも算出されたら当たり判定を出す
	return 0 < shortestPoints.size();
}

float BossPatternNormalMove::GetDir(const std::array<BossLimitMoveData, 8> &DATA, const std::array<BossLimitMoveData, 4> &FAR_LINE_DATA)
{
	//どの角度に向かって進んでいいか配列に纏めた物
	std::vector<float>allowToUseThisAngleArray;
	std::vector<float>moveToPlayerAreaAngleArray;

	//遠距離レイを近距離の処理を合わせる為に別の配列に組み込む
	std::array<BossLimitMoveData, 8>tmpFarLineData;
	tmpFarLineData[0] = FAR_LINE_DATA[0];
	tmpFarLineData[2] = FAR_LINE_DATA[1];
	tmpFarLineData[4] = FAR_LINE_DATA[2];
	tmpFarLineData[6] = FAR_LINE_DATA[3];


	//どの方向に向かって進んでいいか
	for (int i = 0; i < DATA.size(); ++i)
	{
		//プレイヤー陣地の方向に進む
		if (!DATA[i].hitFlag && 3 <= i && i <= 5)
		{
			//向いている方向から上下それぞれ45度の範囲内で乱数を取る
			int maxAngle = (i + 1) * 45;
			int minAngle = maxAngle - 90;
			float rad = Angle::ConvertToRadian(KuroFunc::GetRand(minAngle, maxAngle));
			moveToPlayerAreaAngleArray.push_back(rad);
		}
		//プレイヤー陣地の方向以外に進む
		else if (!DATA[i].hitFlag && !tmpFarLineData[i].hitFlag)
		{
			//向いている方向から上下それぞれ45度の範囲内で乱数を取る
			int maxAngle = (i + 1) * 45;
			int minAngle = maxAngle - 90;
			float rad = Angle::ConvertToRadian(KuroFunc::GetRand(minAngle, maxAngle));
			allowToUseThisAngleArray.push_back(rad);
		}
	}

	int getDirNum = -1;
	//プレイヤー陣地に向かわない処理
	if (allowToUseThisAngleArray.size() != 0)
	{
		getDirNum = KuroFunc::GetRand(0, allowToUseThisAngleArray.size() - 1);
		return 	allowToUseThisAngleArray[getDirNum];
	}
	else
	{
		getDirNum = KuroFunc::GetRand(0, moveToPlayerAreaAngleArray.size() - 1);
		return 	moveToPlayerAreaAngleArray[getDirNum];
	}
}
