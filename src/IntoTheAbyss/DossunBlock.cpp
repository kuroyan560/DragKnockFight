#include "DossunBlock.h"
#include "ScrollMgr.h"
#include "ShakeMgr.h"

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
	isMoveTimer = 0;
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
	isMoveTimer = 0;
	id = {};

}

void DossunBlock::Generate(Vec2<float> generatePos, Vec2<float> endPos, const Vec2<float>& size, const DossunID& dossunID)
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
	isHitPlayer = false;

}

void DossunBlock::Update()
{

	/*===== 更新処理 =====*/

	// プレイヤーと当たっていたらタイマーを加算する。
	if (isHitPlayer && isMoveTimer < IS_MOVE_TIMER) {

		// タイマーを加算する。
		++isMoveTimer;

	}

	// タイマーが一定以上だったら移動処理を行う。
	if (isMoveTimer >= IS_MOVE_TIMER) {

		// 移動する。
		pos = moveDir * Vec2<float>(speed, speed);

		// IDに応じて移動量を加算する。
		switch (id)
		{
		case DOSSUN_OFF_POWER:

			/*-- 電源OFFドッスンの場合 --*/

			break;
		case DOSSUN_LOW_POWER:

			/*-- 低出力ドッスンの場合 --*/

			// 移動速度を加算する。
			speed += LOW_POWER_ADD_SPEED;

			// 移動速度が限界を超えないようにする。
			if (speed >= LOW_POWER_MAX_SPEED) {

				speed = LOW_POWER_MAX_SPEED;

			}


			break;
		case DOSSUN_HIGH_POWER:

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

