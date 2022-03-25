#include "PlayerHand.h"
#include "ScrollMgr.h"
#include "ShakeMgr.h"
#include "SightCollisionStorage.h"
#include "ViewPort.h"
#include "MapChipCollider.h"

#include"KuroEngine.h"
#include"TexHandleMgr.h"

PlayerHand::PlayerHand()
{

	/*-- コンストラクタ --*/

	// 座標を初期化
	handPos = {};

	// 移動量を初期化
	vel = {};

	// 昇順の座標を初期化
	sightPos = {};

	// プレイヤーの中心からの距離を初期化
	armDistance = 0;

	// 入力された角度を初期化
	inputAngle = 0;

	// 最初の一発は反動が強いフラグを立てる
	isFirstShot = true;

	// ビーコンのクールタイムを初期化
	pikeCooltime = 0;

	// 手の画像をロード
	//playerHandGraph = LoadGraph("Resource/PlayerHand.png");
	//playerHandGraph = TexHandleMgr::LoadGraph("resource/IntoTheAbyss/PlayerHand.png");

	isNoInputTimer = false;
}

void PlayerHand::Init(const float& armDistance)
{

	/*-- コンストラクタ --*/

	// 座標を初期化
	handPos = {};

	// 移動量を初期化
	vel = {};

	// 昇順の座標を初期化
	sightPos = {};

	// プレイヤーの中心からの距離を初期化
	this->armDistance = armDistance;

	// 入力された角度を初期化
	inputAngle = 0;

	// ビーコンのクールタイムを初期化
	pikeCooltime = 0;

	isNoInputTimer = false;

}

void PlayerHand::Update(const Vec2<float>& playerCenterPos)
{

	/*-- 更新処理 --*/

	// プレイヤーの中心座標を元に、腕の位置をセット。
	static const float OFFSET = 4.0f;
	Vec2<float> honraiHandPos = { playerCenterPos.x + armDistance, playerCenterPos.y + OFFSET };

	// 本来あるべきプレイヤーの手の座標に近付ける。
	handPos.x += (honraiHandPos.x - handPos.x) / 1.0f;
	handPos.y += (honraiHandPos.y - handPos.y) / 1.0f;

	// 移動させる。
	handPos += vel;

	// 移動量を減らす。
	vel.x -= vel.x / 4.0f;
	vel.y -= vel.y / 4.0f;

	// 更にそこから角度の方向に移動させる。
	//handPos += {ARM_RANGE_OF_MOTION* cosf(inputAngle), ARM_RANGE_OF_MOTION* sinf(inputAngle)};

	// No Input Dattara
	if (isNoInputTimer) {

		drawPos += (handPos - drawPos) / Vec2<float>(10.0f, 10.0f);

	}
	else {

		drawPos = handPos;

	}

	// 各短槍の更新処理を行う。
	if (teleportPike.isActive) teleportPike.Update();
	if (timeStopPike.isActive) timeStopPike.Update();

	// ビーコンのクールタイムの更新を行う。
	if (pikeCooltime > 0) --pikeCooltime;

}

#include"DrawFunc.h"
#include"DrawFunc_Shadow.h"
void PlayerHand::Draw(LightManager& LigManager, const Vec2<float>& ExtRate, const int& GraphHandle, const float& InitAngle, const Vec2<float>& RotaCenterUV, const bool& DRAW_CURSOR)
{
	/*-- 描画処理 --*/
	Vec2<float> scrollShakeZoom = ScrollMgr::Instance()->scrollAmount + ShakeMgr::Instance()->shakeAmount;
	scrollShakeZoom.x *= ScrollMgr::Instance()->zoom;
	scrollShakeZoom.y *= ScrollMgr::Instance()->zoom;

	afterImg.Draw(ScrollMgr::Instance()->zoom, scrollShakeZoom);


	//DrawFunc::DrawRotaGraph2D(center, ExtRate * ScrollMgr::Instance()->zoom, inputAngle - InitAngle, TexHandleMgr::GetTexBuffer(GraphHandle), RotaCenterUV);
	DrawFunc_Shadow::DrawRotaGraph2D(LigManager, GetCenterDrawPos(), ExtRate * ScrollMgr::Instance()->zoom, inputAngle - InitAngle,
		TexHandleMgr::GetTexBuffer(GraphHandle), nullptr, nullptr, 0.0f, RotaCenterUV);

	// ビーコンを描画
	if (teleportPike.isActive) teleportPike.Draw();
	if (timeStopPike.isActive) timeStopPike.Draw();

	Vec2<float>leftUp = { sightPos.x * ScrollMgr::Instance()->zoom - SIGHT_SIZE * ScrollMgr::Instance()->zoom - scrollShakeZoom.x,
		sightPos.y * ScrollMgr::Instance()->zoom - SIGHT_SIZE * ScrollMgr::Instance()->zoom - scrollShakeZoom.y };
	Vec2<float>rightBottom = { sightPos.x * ScrollMgr::Instance()->zoom + SIGHT_SIZE * ScrollMgr::Instance()->zoom - scrollShakeZoom.x,
		sightPos.y * ScrollMgr::Instance()->zoom + SIGHT_SIZE * ScrollMgr::Instance()->zoom - scrollShakeZoom.y };

	//照準を描画
	if (DRAW_CURSOR)
	{
		DrawFunc::DrawBox2D(leftUp, rightBottom, Color(179, 255, 239, 255), true);
	}

}

void PlayerHand::Shot(const Vec2<float>& forwardVec, const bool& isFirstShot)
{

	/*===== 弾を打った時の処理 =====*/

	// 最初の一発目だったら
	if (isFirstShot) {

		vel.x = forwardVec.x * FIRST_SHOT_VEL;
		vel.y = forwardVec.y * FIRST_SHOT_VEL;

	}
	// そうじゃなかったら
	else {

		vel.x = forwardVec.x * SHOT_VEL;
		vel.y = forwardVec.y * SHOT_VEL;

	}

}

void PlayerHand::CheckHit(const vector<vector<int>>& mapData)
{

	/*===== 当たり判定関数 =====*/

	// 照準を描画する最短距離を求める。
	CheckShortestPoint(mapData);

}

bool PlayerHand::IsIntersected(const Vec2<float>& posA1, const Vec2<float>& posA2, const Vec2<float>& posB1, const Vec2<float>& posB2)
{
	/*--線分の外積を計算して交差判定を行う--*/
	//第一回 線分Aから見たBの交差判定
	Vec2<float> buffA = Vec2<float>(posA2.x - posA1.x, posA2.y - posA1.y);
	buffA.Normalize();
	Vec2<float> buffB = Vec2<float>(posB1.x - posA1.x, posB1.y - posA1.y);
	buffB.Normalize();
	Vec2<float> buffC = Vec2<float>(posA2.x - posA1.x, posA2.y - posA1.y);
	buffC.Normalize();
	Vec2<float> buffD = Vec2<float>(posB2.x - posA1.x, posB2.y - posA1.y);
	buffD.Normalize();
	float buffE = buffA.Cross(buffB);
	float buffF = buffC.Cross(buffD);
	float result1 = buffE * buffF;
	bool zero1 = false;
	if (buffE * buffF == 0) zero1 = true;
	//第二回 線分Bから見たAの交差判定
	buffA = Vec2<float>(posB2.x - posB1.x, posB2.y - posB1.y);
	buffA.Normalize();
	buffB = Vec2<float>(posA1.x - posB1.x, posA1.y - posB1.y);
	buffB.Normalize();
	buffC = Vec2<float>(posB2.x - posB1.x, posB2.y - posB1.y);
	buffC.Normalize();
	buffD = Vec2<float>(posA2.x - posB1.x, posA2.y - posB1.y);
	buffD.Normalize();
	buffE = buffA.Cross(buffB);
	buffF = buffC.Cross(buffD);
	float result2 = buffE * buffF;
	bool zero2 = false;
	if (buffE * buffF == 0) zero2 = true;

	//線分が交差している時は、線分から見て交差判定したい線分の端点2つが両側にある時。
	//外積で左右判定をすると、交差している時は値の結果が+と-になる。
	//つまり両方の外積を掛けて結果が-になった場合のみ交差している。
	//線分AからみてBを判定、線分BからみてAを判定と二通り判定を行う。
	//この2つの判定結果を掛けた時に-、若しくは0の時2つの線分は交差している。
	if ((result1 <= 0 && result2 <= 0) ||
		(result1 <= 0 && zero2) ||
		(zero1 && result2 <= 0)) {
		return true;
	}
	return false;
}

Vec2<float> PlayerHand::CalIntersectPoint(Vec2<float> posA1, Vec2<float> posA2, Vec2<float> posB1, Vec2<float> posB2)
{
	//交点を求める この式は資料そのまま
	Vec2<float> buff = Vec2<float>(posB2.x - posB1.x, posB2.y - posB1.y);
	double d1 = abs(buff.Cross(Vec2<float>(posA1.x - posB1.x, posA1.y - posB1.y)));
	double d2 = abs(buff.Cross(Vec2<float>(posA2.x - posB1.x, posA2.y - posB1.y)));
	double t = d1 / (d1 + d2);

	return Vec2<float>(posA1.x + (posA2.x - posA1.x) * t, posA1.y + (posA2.y - posA1.y) * t);
}

void PlayerHand::EmitAfterImg(const Vec2<float>& TeleAmount, const int& GraphHandle, const Vec2<float>& GraphSize, const Vec2<bool>& Miror)
{
	afterImg.EmitArray(drawPos, drawPos + TeleAmount, GraphHandle, GraphSize, Miror);
}

Vec2<float> PlayerHand::GetCenterDrawPos()
{
	Vec2<float> scrollShakeZoom = ScrollMgr::Instance()->scrollAmount + ShakeMgr::Instance()->shakeAmount;
	scrollShakeZoom.x *= ScrollMgr::Instance()->zoom;
	scrollShakeZoom.y *= ScrollMgr::Instance()->zoom;

	return drawPos * ScrollMgr::Instance()->zoom - scrollShakeZoom;
}

void PlayerHand::CheckShortestPoint(const vector<vector<int>>& mapData)
{

	/*===== 照準を描画する最短距離を求める =====*/

	// 最短距離を保存するようの配列。
	vector<pair<Vec2<float>, float>> shortestPoints;

	// 使用する線分を作成する。
	Vec2<float> handSegmentStart = handPos;
	Vec2<float> handSegmentEnd = Vec2<float>(handPos.x + cosf(inputAngle) * 1000.0f, handPos.y + sinf(inputAngle) * 1000.0f);

	// 保存されているデータの配列数を取得する。
	const int STORAGE_COUNT = SightCollisionStorage::Instance()->data.size();
	for (int index = 0; index < STORAGE_COUNT; ++index) {

		// このインデックスのブロックのサイズを取得。
		const Vec2<float> BLOCK_SIZE = SightCollisionStorage::Instance()->data[index].scale;
		// このインデックスのブロックの座標を取得。
		const Vec2<float>* BLOCK_POS = SightCollisionStorage::Instance()->data[index].pos;

		//// ビューポート外にあったら処理を行わない。
		//if (ViewPort::Instance()->pointPos[ViewPort::LEFT_UP].x - MAP_CHIP_HALF_SIZE > BLOCK_POS->x) continue;
		//if (ViewPort::Instance()->pointPos[ViewPort::RIGHT_UP].x + MAP_CHIP_HALF_SIZE < BLOCK_POS->x) continue;
		//if (ViewPort::Instance()->pointPos[ViewPort::RIGHT_UP].y - MAP_CHIP_HALF_SIZE > BLOCK_POS->y) continue;
		//if (ViewPort::Instance()->pointPos[ViewPort::RIGHT_DOWN].y + MAP_CHIP_HALF_SIZE < BLOCK_POS->y) continue;

		// 四辺分交点を求める。

		// 交点保存用
		vector<Vec2<float>> intersectedPos;

		// 上方向
		if (IsIntersected(handSegmentStart, handSegmentEnd, Vec2<float>(BLOCK_POS->x - BLOCK_SIZE.x, BLOCK_POS->y - BLOCK_SIZE.y), Vec2<float>(BLOCK_POS->x + BLOCK_SIZE.x, BLOCK_POS->y - BLOCK_SIZE.y))) {

			// 交点を求めて保存する。
			intersectedPos.push_back(CalIntersectPoint(handSegmentStart, handSegmentEnd, Vec2<float>(BLOCK_POS->x - BLOCK_SIZE.x, BLOCK_POS->y - BLOCK_SIZE.y), Vec2<float>(BLOCK_POS->x + BLOCK_SIZE.x, BLOCK_POS->y - BLOCK_SIZE.y)));

		}
		// 右方向
		if (IsIntersected(handSegmentStart, handSegmentEnd, Vec2<float>(BLOCK_POS->x + BLOCK_SIZE.x, BLOCK_POS->y - BLOCK_SIZE.y), Vec2<float>(BLOCK_POS->x + BLOCK_SIZE.x, BLOCK_POS->y + BLOCK_SIZE.y))) {

			// 交点を求めて保存する。
			intersectedPos.push_back(CalIntersectPoint(handSegmentStart, handSegmentEnd, Vec2<float>(BLOCK_POS->x + BLOCK_SIZE.x, BLOCK_POS->y - BLOCK_SIZE.y), Vec2<float>(BLOCK_POS->x + BLOCK_SIZE.x, BLOCK_POS->y + BLOCK_SIZE.y)));

		}
		// 下方向
		if (IsIntersected(handSegmentStart, handSegmentEnd, Vec2<float>(BLOCK_POS->x - BLOCK_SIZE.x, BLOCK_POS->y + BLOCK_SIZE.y), Vec2<float>(BLOCK_POS->x + BLOCK_SIZE.x, BLOCK_POS->y + BLOCK_SIZE.y))) {

			// 交点を求めて保存する。
			intersectedPos.push_back(CalIntersectPoint(handSegmentStart, handSegmentEnd, Vec2<float>(BLOCK_POS->x - BLOCK_SIZE.x, BLOCK_POS->y + BLOCK_SIZE.y), Vec2<float>(BLOCK_POS->x + BLOCK_SIZE.x, BLOCK_POS->y + BLOCK_SIZE.y)));

		}
		// 左方向
		if (IsIntersected(handSegmentStart, handSegmentEnd, Vec2<float>(BLOCK_POS->x - BLOCK_SIZE.x, BLOCK_POS->y - BLOCK_SIZE.y), Vec2<float>(BLOCK_POS->x - BLOCK_SIZE.x, BLOCK_POS->y + BLOCK_SIZE.y))) {

			// 交点を求めて保存する。
			intersectedPos.push_back(CalIntersectPoint(handSegmentStart, handSegmentEnd, Vec2<float>(BLOCK_POS->x - BLOCK_SIZE.x, BLOCK_POS->y - BLOCK_SIZE.y), Vec2<float>(BLOCK_POS->x - BLOCK_SIZE.x, BLOCK_POS->y + BLOCK_SIZE.y)));

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

	// 次にマップチップとの最短距離を求める。
	const int MAP_Y = mapData.size();
	for (int height = 0; height < MAP_Y; ++height) {

		const int MAP_X = mapData[height].size();
		for (int width = 0; width < MAP_X; ++width) {

			// このマップチップが1~9以外だったら判定を飛ばす。
			if (mapData[height][width] < 1 || mapData[height][width] > 9) continue;

			// このインデックスのブロックの座標を取得。
			const Vec2<float> BLOCK_POS = Vec2<float>(width * MAP_CHIP_SIZE, height * MAP_CHIP_SIZE);

			// ビューポート外にあったら処理を行わない。
			//if (ViewPort::Instance()->pointPos[ViewPort::LEFT_UP].x - MAP_CHIP_HALF_SIZE > BLOCK_POS.x) continue;
			//if (ViewPort::Instance()->pointPos[ViewPort::RIGHT_UP].x + MAP_CHIP_HALF_SIZE < BLOCK_POS.x) continue;
			//if (ViewPort::Instance()->pointPos[ViewPort::RIGHT_UP].y - MAP_CHIP_HALF_SIZE > BLOCK_POS.y) continue;
			//if (ViewPort::Instance()->pointPos[ViewPort::RIGHT_DOWN].y + MAP_CHIP_HALF_SIZE < BLOCK_POS.y) continue;

			// 四辺分交点を求める。

			// 交点保存用
			vector<Vec2<float>> intersectedPos;

			// 上方向
			if (IsIntersected(handSegmentStart, handSegmentEnd, Vec2<float>(BLOCK_POS.x - MAP_CHIP_HALF_SIZE, BLOCK_POS.y - MAP_CHIP_HALF_SIZE), Vec2<float>(BLOCK_POS.x + MAP_CHIP_HALF_SIZE, BLOCK_POS.y - MAP_CHIP_HALF_SIZE))) {

				// 交点を求めて保存する。
				intersectedPos.push_back(CalIntersectPoint(handSegmentStart, handSegmentEnd, Vec2<float>(BLOCK_POS.x - MAP_CHIP_HALF_SIZE, BLOCK_POS.y - MAP_CHIP_HALF_SIZE), Vec2<float>(BLOCK_POS.x + MAP_CHIP_HALF_SIZE, BLOCK_POS.y - MAP_CHIP_HALF_SIZE)));

			}
			// 右方向
			if (IsIntersected(handSegmentStart, handSegmentEnd, Vec2<float>(BLOCK_POS.x + MAP_CHIP_HALF_SIZE, BLOCK_POS.y - MAP_CHIP_HALF_SIZE), Vec2<float>(BLOCK_POS.x + MAP_CHIP_HALF_SIZE, BLOCK_POS.y + MAP_CHIP_HALF_SIZE))) {

				// 交点を求めて保存する。
				intersectedPos.push_back(CalIntersectPoint(handSegmentStart, handSegmentEnd, Vec2<float>(BLOCK_POS.x + MAP_CHIP_HALF_SIZE, BLOCK_POS.y - MAP_CHIP_HALF_SIZE), Vec2<float>(BLOCK_POS.x + MAP_CHIP_HALF_SIZE, BLOCK_POS.y + MAP_CHIP_HALF_SIZE)));

			}
			// 下方向
			if (IsIntersected(handSegmentStart, handSegmentEnd, Vec2<float>(BLOCK_POS.x - MAP_CHIP_HALF_SIZE, BLOCK_POS.y + MAP_CHIP_HALF_SIZE), Vec2<float>(BLOCK_POS.x + MAP_CHIP_HALF_SIZE, BLOCK_POS.y + MAP_CHIP_HALF_SIZE))) {

				// 交点を求めて保存する。
				intersectedPos.push_back(CalIntersectPoint(handSegmentStart, handSegmentEnd, Vec2<float>(BLOCK_POS.x - MAP_CHIP_HALF_SIZE, BLOCK_POS.y + MAP_CHIP_HALF_SIZE), Vec2<float>(BLOCK_POS.x + MAP_CHIP_HALF_SIZE, BLOCK_POS.y + MAP_CHIP_HALF_SIZE)));

			}
			// 左方向
			if (IsIntersected(handSegmentStart, handSegmentEnd, Vec2<float>(BLOCK_POS.x - MAP_CHIP_HALF_SIZE, BLOCK_POS.y - MAP_CHIP_HALF_SIZE), Vec2<float>(BLOCK_POS.x - MAP_CHIP_HALF_SIZE, BLOCK_POS.y + MAP_CHIP_HALF_SIZE))) {

				// 交点を求めて保存する。
				intersectedPos.push_back(CalIntersectPoint(handSegmentStart, handSegmentEnd, Vec2<float>(BLOCK_POS.x - MAP_CHIP_HALF_SIZE, BLOCK_POS.y - MAP_CHIP_HALF_SIZE), Vec2<float>(BLOCK_POS.x - MAP_CHIP_HALF_SIZE, BLOCK_POS.y + MAP_CHIP_HALF_SIZE)));

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

	// 最後にウィンドウとの当たり判定を行う。

	const float WIN_WIDTH = 1280.0f;
	const float WIN_HEIGHT = 740.0f;

	const Vec2<float> WINDOW_LEFTUP = Vec2<float>(ScrollMgr::Instance()->scrollAmount.x, ScrollMgr::Instance()->scrollAmount.y);
	const Vec2<float> WINDOW_RIGHTUP = Vec2<float>(ScrollMgr::Instance()->scrollAmount.x + WIN_WIDTH, ScrollMgr::Instance()->scrollAmount.y);
	const Vec2<float> WINDOW_LEFTDOWN = Vec2<float>(ScrollMgr::Instance()->scrollAmount.x, ScrollMgr::Instance()->scrollAmount.y + WIN_HEIGHT);
	const Vec2<float> WINDOW_RIGHTDOWN = Vec2<float>(ScrollMgr::Instance()->scrollAmount.x + WIN_WIDTH, ScrollMgr::Instance()->scrollAmount.y + WIN_HEIGHT);

	// 上方向
	if (IsIntersected(handSegmentStart, handSegmentEnd, WINDOW_LEFTUP, WINDOW_RIGHTUP)) {
		pair<Vec2<float>, float> buff;
		buff.first = CalIntersectPoint(handSegmentStart, handSegmentEnd, WINDOW_LEFTUP, WINDOW_RIGHTUP);
		buff.second = Vec2<float>(buff.first - handSegmentStart).Length();
		shortestPoints.push_back(buff);
	}
	// 右方向
	if (IsIntersected(handSegmentStart, handSegmentEnd, WINDOW_RIGHTUP, WINDOW_RIGHTDOWN)) {
		pair<Vec2<float>, float> buff;
		buff.first = CalIntersectPoint(handSegmentStart, handSegmentEnd, WINDOW_RIGHTUP, WINDOW_RIGHTDOWN);
		buff.second = Vec2<float>(buff.first - handSegmentStart).Length();
		shortestPoints.push_back(buff);
	}
	// 下方向
	if (IsIntersected(handSegmentStart, handSegmentEnd, WINDOW_LEFTDOWN, WINDOW_RIGHTDOWN)) {
		pair<Vec2<float>, float> buff;
		buff.first = CalIntersectPoint(handSegmentStart, handSegmentEnd, WINDOW_LEFTDOWN, WINDOW_RIGHTDOWN);
		buff.second = Vec2<float>(buff.first - handSegmentStart).Length();
		shortestPoints.push_back(buff);
	}
	// 左方向
	if (IsIntersected(handSegmentStart, handSegmentEnd, WINDOW_LEFTUP, WINDOW_LEFTDOWN)) {
		pair<Vec2<float>, float> buff;
		buff.first = CalIntersectPoint(handSegmentStart, handSegmentEnd, WINDOW_LEFTUP, WINDOW_LEFTDOWN);
		buff.second = Vec2<float>(buff.first - handSegmentStart).Length();
		shortestPoints.push_back(buff);
	}


	/*-- ここまでの過程で様々な最短を求めることができた。 --*/

	// 最短の座標を保存する用変数。
	float shortestLength = 100000.0f;

	// 全ての最短の中から最も短いものを求める。
	const int SHORTEST_COUNT = shortestPoints.size();

	// サイズが0だったら照準をどっかに飛ばしてリターン。
	if (SHORTEST_COUNT <= 0) {

		sightPos = { -100,-100 };
		return;

	}

	for (int index = 0; index < SHORTEST_COUNT; ++index) {

		// 保存されているデータより大きかったら処理を飛ばす。
		if (shortestPoints[index].second >= shortestLength) continue;

		// データを保存する。
		shortestLength = shortestPoints[index].second;
		sightPos = shortestPoints[index].first;

	}

	// 座標をアラインメントする。
	//sightPos.x = RoundUp(sightPos.x, SIGHT_SIZE * 2.0f);
	//sightPos.y = RoundUp(sightPos.y, SIGHT_SIZE * 2.0f);

}
