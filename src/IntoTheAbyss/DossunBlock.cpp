#include "DossunBlock.h"
#include "ScrollMgr.h"
#include "ShakeMgr.h"
#include "Collider.h"
#include "MapChipCollider.h"
#include "ParticleMgr.h"

#include"DrawFunc.h"

DossunBlock::DossunBlock()
{

	/*===== コンストラクタ =====*/

	// 各変数を初期化。
	pos = {};
	size = {};
	moveDir = {};
	speed = 0;
	isHitPlayer = false;
	isMove = false;
	isReturn = false;
	isMoveTimer = 0;
	changeDirTimer = 0;
	id = {};
	sightData.pos = &pos;
	sightData.scale = size;

}

void DossunBlock::Init()
{

	/*===== 初期化処理 =====*/

	// 各変数を初期化。
	pos = {};
	size = {};
	moveDir = {};
	speed = 0;
	isHitPlayer = false;
	isMove = false;
	isReturn = false;
	isMoveTimer = 0;
	changeDirTimer = 0;
	id = {};

}

void DossunBlock::Generate(Vec2<float> generatePos, Vec2<float> endPos, const Vec2<float>& size, const E_GIMMICK& dossunID)
{

	/*===== 生成処理 =====*/

	// 生成位置を初期化。
	pos = generatePos;

	// 大きさを初期化
	this->size = size;

	// 移動方向を初期化。
	this->moveDir = endPos - generatePos;
	this->moveDir.Normalize();

	// ドッスンIDを初期化。
	id = dossunID;

	// その他変数を初期化。
	speed = 0;
	isMoveTimer = 0;
	changeDirTimer = 0;
	isHitPlayer = false;
	isMove = false;
	isReturn = false;
	isTimeStopPikeAlive = nullptr;
	sightData = { &pos,size/* * Vec2<float>(2.0f,2.0f)*/ };
	alpha = 1;

}

void DossunBlock::Update()
{

	/*===== 更新処理 =====*/

	// プレイヤーと当たっていたらタイマーを加算する。
	if (isHitPlayer) {

		// タイマーを加算する。
		++isMoveTimer;

	}
	else {

		isMoveTimer = 0;

	}

	// 移動するまでのタイマーが一定以上になったら移動させる。
	if (isMoveTimer >= IS_MOVE_TIMER) {

		isMove = true;

	}

	// 移動方向チェンジタイマーが一定以上だったら方向転換する。
	if (changeDirTimer >= CHANGE_DIR_TIMER) {
		moveDir *= {-1.0f, -1.0f};

		noCheckHitTimer = 10.0f;

		bool prevFrameFlag = isReturn;

		// 戻っているフラグを反転させる。
		isReturn = isReturn ? false : true;

		// 戻った瞬間は移動を停止させる。
		if (prevFrameFlag && !isReturn && isHitPlayer) {

		}
		else if (prevFrameFlag && !isReturn) {

			speed = 0;
			isMove = false;

		}

		if (isMove) {
			changeDirTimer = 0;
		}

	}

	// タイマーが一定以上だったら移動処理を行う。
	if (isMove) {

		if (speed <= 1.0f) speed = 1.0f;

		// IDに応じて移動量を加算する。
		switch (id)
		{
		case GIMMICK_DOSSN_OFF:

			/*-- 電源OFFドッスンの場合 --*/

			break;
		case GIMMICK_DOSSN_ON_LOW:

			/*-- 低出力ドッスンの場合 --*/

			// 移動速度を加算する。
			speed += LOW_POWER_ADD_SPEED;

			// 移動速度が限界を超えないようにする。
			if (speed >= LOW_POWER_MAX_SPEED) {

				speed = LOW_POWER_MAX_SPEED;

			}


			break;
		case GIMMICK_DOSSN_ON_HIGH:

			/*-- 高出力ドッスンの場合 --*/

			break;
		default:
			break;
		}

		if (isTimeStopPikeAlive != nullptr) speed = 0;
		if (changeDirTimer > 0) speed = 0;

		// 移動する。
		pos += moveDir * Vec2<float>(speed, speed);

	}

	if (isTimeStopPikeAlive != nullptr && *isTimeStopPikeAlive == false) isTimeStopPikeAlive = nullptr;

	if (0 < noCheckHitTimer) --noCheckHitTimer;


	// アルファ値に関する処理

	// アルファを毎フレーム0に近づける。
	alpha -= alpha / 10.0f;

	if (isHitPlayer) {

		alpha = (float)isMoveTimer / IS_MOVE_TIMER * 255.0f;

	}
	if (isMove) {

		alpha = 255;

	}


}

#include"D3D12App.h"
void DossunBlock::Draw()
{

	/*===== 描画処理 =====*/

	Vec2<float> scrollShakeZoom = { ScrollMgr::Instance()->scrollAmount.x * ScrollMgr::Instance()->zoom - ShakeMgr::Instance()->shakeAmount.x * ScrollMgr::Instance()->zoom,
	ScrollMgr::Instance()->scrollAmount.y * ScrollMgr::Instance()->zoom - ShakeMgr::Instance()->shakeAmount.y * ScrollMgr::Instance()->zoom };

	Vec2<float> posZoom = { pos.x * ScrollMgr::Instance()->zoom,pos.y * ScrollMgr::Instance()->zoom };

	Vec2<float> sizeZoom = { size.x * ScrollMgr::Instance()->zoom, size.y * ScrollMgr::Instance()->zoom };

	Vec2<float>leftUp = { posZoom.x - sizeZoom.x - scrollShakeZoom.x,
		posZoom.y - sizeZoom.y - scrollShakeZoom.y };
	Vec2<float>rightBottom = { posZoom.x + sizeZoom.x - scrollShakeZoom.x,
		posZoom.y + sizeZoom.y - scrollShakeZoom.y };

	DrawFunc::DrawBox2D(leftUp, rightBottom, Color(100, 100, 100, 255), D3D12App::Instance()->GetBackBuffFormat(), true);

	// 移動し始めるときに光らせる用
	DrawFunc::DrawBox2D(leftUp, rightBottom, Color(255, 255, 255, alpha), D3D12App::Instance()->GetBackBuffFormat(), true, AlphaBlendMode_Trans);
}

void DossunBlock::CheckHit(const vector<vector<int>>& mapData)
{

	// フラグを初期化。
	bool isDossunTop = false;
	bool isDossunRight = false;
	bool isDossunLeft = false;
	bool isDossunBottom = false;

	// マップチップとドッスンブロックの当たり判定

	float offset = 1.0f;

	if (noCheckHitTimer <= 0) {

		if (moveDir.y != 0) {
			isDossunTop = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(pos, size, mapData, INTERSECTED_TOP) != INTERSECTED_NONE;
			if (!isDossunTop) isDossunTop = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(pos + Vec2<float>(-size.x + 10.0f, 0), size, mapData, INTERSECTED_TOP) != INTERSECTED_NONE;
			if (!isDossunTop) isDossunTop = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(pos + Vec2<float>(size.x - 10.0f, 0), size, mapData, INTERSECTED_TOP) != INTERSECTED_NONE;
			pos.y -= offset;
		}
		if (moveDir.x != 0) {
			isDossunRight = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(pos, size, mapData, INTERSECTED_RIGHT) != INTERSECTED_NONE;
			if (!isDossunRight) isDossunRight = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(pos + Vec2<float>(0, -size.y + 10.0f), size, mapData, INTERSECTED_RIGHT) != INTERSECTED_NONE;
			if (!isDossunRight) isDossunRight = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(pos + Vec2<float>(0, size.y - 10.0f), size, mapData, INTERSECTED_RIGHT) != INTERSECTED_NONE;
			pos.x -= offset;
		}
		if (moveDir.x != 0) {
			isDossunLeft = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(pos, size, mapData, INTERSECTED_LEFT) != INTERSECTED_NONE;
			if (!isDossunLeft) isDossunLeft = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(pos + Vec2<float>(0, -size.y + 10.0f), size, mapData, INTERSECTED_LEFT) != INTERSECTED_NONE;
			if (!isDossunLeft) isDossunLeft = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(pos + Vec2<float>(0, size.y - 10.0f), size, mapData, INTERSECTED_LEFT) != INTERSECTED_NONE;
			pos.x += offset;
		}
		if (moveDir.y != 0) {
			isDossunBottom = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(pos, size, mapData, INTERSECTED_BOTTOM) != INTERSECTED_NONE;
			if (!isDossunBottom) isDossunBottom = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(pos + Vec2<float>(-size.x + 10.0f, 0), size, mapData, INTERSECTED_BOTTOM) != INTERSECTED_NONE;
			if (!isDossunBottom) isDossunBottom = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(pos + Vec2<float>(size.x - 10.0f, 0), size, mapData, INTERSECTED_BOTTOM) != INTERSECTED_NONE;
			pos.y += offset;
		}

	}

	// どこかしらにぶつかっていれば当たった判定にする。
	if (isDossunTop || isDossunRight || isDossunLeft || isDossunBottom) {

		// 最初の1F目はシェイクをつける。
		if (changeDirTimer == 1) {

			ShakeMgr::Instance()->SetShake(ShakeMgr::Instance()->DOSSUN_LOW_POWER_SHAKE_AMOUNT);

			const float div = 5.0f;

			// 移動している方向に応じて生成する位置を変える。
			if (0 < fabs(moveDir.x)) {

				// 左右に動いている場合

				const float generateCount = size.y * 2.0f / div;

				for (int index = 0; index < generateCount; ++index) {

					ParticleMgr::Instance()->Generate(pos + Vec2<float>(size.x * moveDir.x, -size.y + (index * div)), moveDir, DOSSUN);

				}

				// 上と下に生成する。
				ParticleMgr::Instance()->Generate(pos + Vec2<float>(size.x * moveDir.x, -size.y), moveDir + Vec2<float>(0, -1.0f), DOSSUN);
				ParticleMgr::Instance()->Generate(pos + Vec2<float>(size.x * moveDir.x, size.y), moveDir + Vec2<float>(0, 1.0f), DOSSUN);
				ParticleMgr::Instance()->Generate(pos + Vec2<float>(size.x * moveDir.x, -size.y), Vec2<float>(0.0f, -1.0f), DOSSUN);
				ParticleMgr::Instance()->Generate(pos + Vec2<float>(size.x * moveDir.x, size.y), Vec2<float>(0.0f, 1.0f), DOSSUN);

			}
			else if (0 < fabs(moveDir.y)) {

				// 上下に動いている場合

				const float generateCount = size.x * 2.0f / div;

				for (int index = 0; index < generateCount; ++index) {

					ParticleMgr::Instance()->Generate(pos + Vec2<float>(-size.x + (index * div), size.y * moveDir.y), moveDir, DOSSUN);

				}

				// 上と下に生成する。
				ParticleMgr::Instance()->Generate(pos + Vec2<float>(-size.x, size.y * moveDir.y), moveDir + Vec2<float>(-1.0f, 0.0f), DOSSUN);
				ParticleMgr::Instance()->Generate(pos + Vec2<float>(size.x, size.y * moveDir.y), moveDir + Vec2<float>(1.0f, 0.0f), DOSSUN);
				ParticleMgr::Instance()->Generate(pos + Vec2<float>(-size.x, size.y * moveDir.y), Vec2<float>(-1.0f, 0.0f), DOSSUN);
				ParticleMgr::Instance()->Generate(pos + Vec2<float>(size.x, size.y * moveDir.y), Vec2<float>(1.0f, 0.0f), DOSSUN);

			}

		}

		// 方向転換タイマーを加算。
		if (isMove) {
			++changeDirTimer;
		}

	}
	else {

		// 方向転換タイマーを初期化。
		changeDirTimer = 0;

	}

}

