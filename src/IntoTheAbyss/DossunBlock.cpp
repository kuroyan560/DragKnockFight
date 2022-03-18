#include "DossunBlock.h"
#include "ScrollMgr.h"
#include "ShakeMgr.h"
#include "Collider.h"
#include "MapChipCollider.h"

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

		changeDirTimer = 0;
		moveDir *= {-1.0f, -1.0f};

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

	}

	// タイマーが一定以上だったら移動処理を行う。
	if (isMove) {

		// 移動する。
		pos += moveDir * Vec2<float>(speed, speed);

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

	}

}

void DossunBlock::Draw()
{

	/*===== 描画処理 =====*/

	Vec2<float> scrollShakeZoom = { ScrollMgr::Instance()->scrollAmount.x * ScrollMgr::Instance()->zoom + ShakeMgr::Instance()->shakeAmount.x * ScrollMgr::Instance()->zoom,
	ScrollMgr::Instance()->scrollAmount.y * ScrollMgr::Instance()->zoom + ShakeMgr::Instance()->shakeAmount.y * ScrollMgr::Instance()->zoom };

	Vec2<float> posZoom = { pos.x * ScrollMgr::Instance()->zoom,pos.y * ScrollMgr::Instance()->zoom };

	Vec2<float> sizeZoom = { size.x * ScrollMgr::Instance()->zoom, size.y * ScrollMgr::Instance()->zoom };

	Vec2<float>leftUp = { posZoom.x - sizeZoom.x - scrollShakeZoom.x,
		posZoom.y - sizeZoom.y - scrollShakeZoom.y };
	Vec2<float>rightBottom = { posZoom.x + sizeZoom.x - scrollShakeZoom.x,
		posZoom.y + sizeZoom.y - scrollShakeZoom.y };

	DrawFunc::DrawBox2D(leftUp, rightBottom, Color(100, 100, 100, 255), true);
}

void DossunBlock::CheckHit(Player& player, const vector<vector<int>>& mapData)
{

	// フラグを初期化。
	bool isDossunTop = false;
	bool isDossunRight = false;
	bool isDossunLeft = false;
	bool isDossunBottom = false;

	// マップチップとドッスンブロックの当たり判定

	if (moveDir.y != 0) {
		isDossunTop = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(pos, size, mapData, INTERSECTED_TOP) != INTERSECTED_NONE;
	}
	if (moveDir.x != 0) {
		isDossunRight = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(pos, size, mapData, INTERSECTED_RIGHT) != INTERSECTED_NONE;
	}
	if (moveDir.x != 0) {
		isDossunLeft = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(pos, size, mapData, INTERSECTED_LEFT) != INTERSECTED_NONE;
	}
	if (moveDir.y != 0) {
		isDossunBottom = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(pos, size, mapData, INTERSECTED_BOTTOM) != INTERSECTED_NONE;
	}

	// どこかしらにぶつかっていれば当たった判定にする。
	if (isDossunTop || isDossunRight || isDossunLeft || isDossunBottom) {

		// 方向転換タイマーを加算。
		++changeDirTimer;

	}
	else {

		// 方向転換タイマーを初期化。
		changeDirTimer = 0;

	}

	// ドッスンブロックとプレイヤーの当たり判定

	bool isDossunVel = Collider::Instance()->CheckHitVel(player.centerPos, player.prevFrameCenterPos, player.vel + player.gimmickVel, player.PLAYER_SIZE, pos, size) != INTERSECTED_NONE;
	isDossunTop = Collider::Instance()->CheckHitSize(player.centerPos, player.PLAYER_SIZE, pos, size, INTERSECTED_TOP) != INTERSECTED_NONE;
	isDossunRight = Collider::Instance()->CheckHitSize(player.centerPos, player.PLAYER_SIZE, pos, size, INTERSECTED_RIGHT) != INTERSECTED_NONE;
	isDossunLeft = Collider::Instance()->CheckHitSize(player.centerPos, player.PLAYER_SIZE, pos, size, INTERSECTED_LEFT) != INTERSECTED_NONE;
	isDossunBottom = Collider::Instance()->CheckHitSize(player.centerPos, player.PLAYER_SIZE, pos, size, INTERSECTED_BOTTOM) != INTERSECTED_NONE;

	// どこかしらにぶつかっていれば当たった判定にする。
	if (isDossunVel || isDossunTop || isDossunRight || isDossunLeft || isDossunBottom) {

		// プレイヤーにドッスンブロックの移動量を渡す。
		player.gimmickVel = Vec2<float>(speed, speed) * moveDir;

		// ドッスンの移動量タイマーを更新。
		isHitPlayer = true;

		// プレイヤーの移動量をかき消す。
		player.gravity *= 0.0f;
		player.vel *= {0.5f, 0.5f};

	}
	else {

		// ドッスンの移動量タイマーを初期化。
		isHitPlayer = false;
		//isMoveTimer = 0;

	}

}

