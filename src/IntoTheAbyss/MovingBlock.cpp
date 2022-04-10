#include "MovingBlock.h"
#include "ScrollMgr.h"
#include "ShakeMgr.h"
#include "MapChipCollider.h"

MovingBlock::MovingBlock(const Vec2<float>& generatePos)
{

	/*===== コンストラクタ =====*/

	pos = generatePos;
	prevFramePos = generatePos;
	vel = {};
	gravity = 0;
	onGround = false;
	isCloseToThePlayer = true;

}

void MovingBlock::Init()
{
	pos = {};
	prevFramePos = {};
	vel = {};
	gravity = 0;
	onGround = false;
	isCloseToThePlayer = true;
}

void MovingBlock::Update(const Vec2<float>& playerPos)
{

	/*===== 更新処理 =====*/

	// プレイヤーとの距離を求める。
	float playerDistance = pos.Distance(playerPos);

	// 距離が一定以上離れていたら処理を行わない。
	if (playerDistance >= DO_PROCESS_DISTANCE) {

		isCloseToThePlayer = false;
		return;

	}
	else {

		isCloseToThePlayer = true;

	}


	/*-- 重力の更新処理 --*/

	// 接地していたら
	if (onGround) {

		// 重力を初期化する。				onGroundが毎フレーム変わるみたいになったら、ここで初期化している量を1とかにしよう。
		gravity = 0;

		// Y軸の移動量を無効化する。
		vel.y = 0;

	}
	// 接地していなかったら
	else {

		// 重力を追加する。
		gravity += ADD_GRAVITY;

	}

	// 重力が限界を超えないようにする。
	if (gravity >= MAX_GRAVITY) gravity = MAX_GRAVITY;


	/*-- 移動量の更新処理 --*/

	// 移動量を0に近付ける。
	vel.x -= vel.x / 10.0f;
	vel.y -= vel.y / 10.0f;

	// 移動量が一定以下だったら0にする。
	if (fabs(vel.x) <= 0.1f) vel.x = 0;
	if (fabs(vel.y) <= 0.1f) vel.y = 0;


	/*-- 移動処理 --*/

	// このフレームの座標を保存。
	prevFramePos = pos;

	// 移動させる。
	pos += vel;

	// 重力をつける。
	pos.y += gravity;

}

#include"TexHandleMgr.h"
#include"DrawFunc.h"
void MovingBlock::Draw(const int& graphHandle)
{

	/*===== 描画処理 =====*/

	// ブロックを描画する。
	//DrawExtendGraph(pos.x * ScrollMgr::Instance()->zoom - SCALE * ScrollMgr::Instance()->zoom - scrollShakeZoom.x,
	//	pos.y * ScrollMgr::Instance()->zoom - SCALE * ScrollMgr::Instance()->zoom - scrollShakeZoom.y,
	//	pos.x * ScrollMgr::Instance()->zoom + SCALE * ScrollMgr::Instance()->zoom - scrollShakeZoom.x,
	//	pos.y * ScrollMgr::Instance()->zoom + SCALE * ScrollMgr::Instance()->zoom - scrollShakeZoom.y,
	//	graphHandle, FALSE);
	Vec2<float>leftUp = { pos.x - SCALE,pos.y - SCALE };
	leftUp = ScrollMgr::Instance()->Affect(leftUp);
	Vec2<float>rightBottom = { pos.x + SCALE,pos.y + SCALE };
	rightBottom = ScrollMgr::Instance()->Affect(rightBottom);

	DrawFunc::DrawExtendGraph2D(leftUp, rightBottom, TexHandleMgr::GetTexBuffer(graphHandle));

}

void MovingBlock::HitGround()
{

	onGround = true;
	vel.y = 0;
	gravity = 0;

}

void MovingBlock::HitBullet(const Vec2<float>& bulletForwardVec)
{

	vel = { bulletForwardVec.x * ADD_VEL, bulletForwardVec.y * ADD_VEL };

	// 移動量が限界を超えないようにする。
	if (vel.x != 0) {

		int sigh = vel.x / vel.x;
		if (fabs(vel.x) >= MAX_VEL) {

			vel.x = MAX_VEL * sigh;

		}

	}
	if (vel.y != 0) {

		int sigh = vel.y / vel.y;
		if (fabs(vel.y) >= MAX_VEL) {

			vel.y = MAX_VEL * sigh;

		}

	}

}

void MovingBlock::CheckHitMapChip(const vector<vector<int>>& mapData)
{

	/*===== マップチップとの当たり判定 =====*/

	INTERSECTED_LINE intersectedLine{};

	// 当たり判定を行う。
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheVel(pos, prevFramePos, vel, Vec2<float>(SCALE, SCALE), onGround, mapData);

	// スケールによる当たり判定を行って重ならないようにする。
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(pos, Vec2<float>(SCALE, SCALE), mapData, INTERSECTED_BOTTOM);
	if (intersectedLine == INTERSECTED_TOP) HitGround();
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(pos, Vec2<float>(SCALE, SCALE), mapData, INTERSECTED_TOP);
	if (intersectedLine == INTERSECTED_TOP) HitGround();
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(pos, Vec2<float>(SCALE, SCALE), mapData, INTERSECTED_LEFT);
	if (intersectedLine == INTERSECTED_TOP) HitGround();
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(pos, Vec2<float>(SCALE, SCALE), mapData, INTERSECTED_RIGHT);
	if (intersectedLine == INTERSECTED_TOP) HitGround();

	// 更に重なり防止(角対策)の処理を行う。 上の2つの判定で重なりはなくなっているはずなので、以下の処理は角以外では呼ばれないはず…

	const int MAP_CHIP_X = mapData[0].size();
	const int MAP_CHIP_Y = mapData.size();
	for (int mapY = 0; mapY < MAP_CHIP_Y; ++mapY) {

		for (int mapX = 0; mapX < MAP_CHIP_X; ++mapX) {

			// そのマップチップが1~9以外だったら処理を飛ばす。
			if (!(mapData[mapY][mapX] >= 1 && mapData[mapY][mapX] <= 9)) continue;

			// マップチップの座標を求める。
			const float centerX = mapX * MAP_CHIP_SIZE;
			const float centerY = mapY * MAP_CHIP_SIZE;

			// X軸、Y軸での距離を求める。
			float distanceX = centerX - pos.x;
			float distanceY = centerY - pos.y;

			// 距離の絶対値
			float distanceXABS = fabs(distanceX);
			float distanceYABS = fabs(distanceY);

			// 移動ブロックとプレイヤーの大きさの合計。
			Vec2<float> playerMovingBlockSize = { MAP_CHIP_HALF_SIZE + SCALE,MAP_CHIP_HALF_SIZE + SCALE };

			// XYの距離が動的ブロックとプレイヤーのサイズ以上だったら処理を飛ばす。
			if (distanceXABS >= playerMovingBlockSize.x ||
				distanceYABS >= playerMovingBlockSize.y) continue;

			// XYの遠い方を検索して遠い方のみ押し戻し処理を行う。
			bool isPushBackX = distanceXABS >= distanceYABS;

			// Xを押し戻す場合。
			if (isPushBackX) {

				// プレイヤーより右に居た場合
				if (distanceX <= 0) {

					// 押し戻す距離を求める。
					float pushBackDistance = playerMovingBlockSize.x + distanceX;

					// 押し戻す。
					pos.x += pushBackDistance;

				}
				else {

					// 押し戻す距離を求める。
					float pushBackDistance = playerMovingBlockSize.x - distanceX;

					// 押し戻す。
					pos.x -= pushBackDistance;

				}

			}
			// Yを押し戻す場合。
			else {

				if (distanceY >= 0) {

					// 押し戻す距離を求める。
					float pushBackDistance = playerMovingBlockSize.y - distanceY;

					// 押し戻す。
					pos.y -= pushBackDistance;

					// この場合は地面にあたった状態。
					//HitMapChipBottom();
					onGround = true;

				}
				else {

					// 押し戻す距離を求める。
					float pushBackDistance = playerMovingBlockSize.y + distanceY;

					// 押し戻す。
					pos.y += pushBackDistance;

				}

			}

		}

	}

}
