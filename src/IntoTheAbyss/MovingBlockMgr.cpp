#include "MovingBlockMgr.h"
#include "MovingBlock.h"
#include "CSVLoader.h"
#include "MapChipCollider.h"
#include "BulletMgr.h"
#include "Bullet.h"

void MovingBlockMgr::Load(const vector<vector<int>>& mapData)
{

	/*===== 動的ブロックをロード =====*/

	const int MOVING_SIZE_Y = mapData.size();		// 縦の要素数
	const int MOVING_SIZE_X = mapData[0].size();	// 横の要素数
	const int MOVING_BLOCK_INDEX = 10;				// 動的ブロックを生成するインデックス番号

	for (int height = 0; height < MOVING_SIZE_Y; ++height) {

		for (int width = 0; width < MOVING_SIZE_X; ++width) {

			// その要素が動的ブロックを生成するインデックス番号だったらだったら。
			if (mapData[height][width] != MOVING_BLOCK_INDEX) continue;

			// 生成する。
			movingBlock.push_back(make_shared<MovingBlock>(Vec2<float>(width * MAP_CHIP_SIZE, height * MAP_CHIP_SIZE)));

		}

	}

}

void MovingBlockMgr::Init()
{
	for (int index = 0; index < movingBlock.size(); ++index) movingBlock[index]->Init();
}

void MovingBlockMgr::Update(const Vec2<float>& playerPos)
{

	/*===== 更新処理 =====*/

	const int BLOCK_SIZE = movingBlock.size();
	for (int index = 0; index < BLOCK_SIZE; ++index) {
		movingBlock[index]->Update(playerPos);
	}

}

void MovingBlockMgr::Draw(const int& graphHandle)
{

	/*===== 描画処理 =====*/

	const int BLOCK_SIZE = movingBlock.size();
	for (int index = 0; index < BLOCK_SIZE; ++index) {
		movingBlock[index]->Draw(graphHandle);
	}

}

void MovingBlockMgr::CheckHit(const vector<vector<int>>& mapData)
{

	/*===== ブロックとブロックの当たり判定 =====*/

	const int MOVING_BLOCK_SIZE = movingBlock.size();

	// 動的ブロックのサイズが0以下だったら処理を行わない。
	if (MOVING_BLOCK_SIZE <= 0) return;

	for (int baseIndex = 0; baseIndex < MOVING_BLOCK_SIZE; ++baseIndex) {

		// 処理を行わないフラグが立っていたら処理を飛ばす。
		if (!movingBlock[baseIndex]->isCloseToThePlayer) continue;

		for (int targetIndex = 0; targetIndex < MOVING_BLOCK_SIZE; ++targetIndex) {

			// インデックスが同じだったら当たり判定を行わない。
			if (baseIndex == targetIndex) continue;

			// 対象のブロックの処理を行わないフラグが立っていたら処理を飛ばす。
			if (!movingBlock[targetIndex]->isCloseToThePlayer) continue;

			// X軸、Y軸での距離を求める。
			float distanceX = MovingBlockMgr::Instance()->movingBlock[baseIndex]->pos.x - MovingBlockMgr::Instance()->movingBlock[targetIndex]->pos.x;
			float distanceY = MovingBlockMgr::Instance()->movingBlock[baseIndex]->pos.y - MovingBlockMgr::Instance()->movingBlock[targetIndex]->pos.y;

			// 距離の絶対値
			float distanceXABS = fabs(distanceX);
			float distanceYABS = fabs(distanceY);

			// 移動ブロックとプレイヤーの大きさの合計。
			Vec2<float> playerMovingBlockSize = { MovingBlockMgr::Instance()->movingBlock[baseIndex]->SCALE + MovingBlockMgr::Instance()->movingBlock[targetIndex]->SCALE,
				MovingBlockMgr::Instance()->movingBlock[baseIndex]->SCALE + MovingBlockMgr::Instance()->movingBlock[targetIndex]->SCALE };

			// XYの距離が動的ブロックとプレイヤーのサイズ以上だったら処理を飛ばす。
			if (distanceXABS >= playerMovingBlockSize.x ||
				distanceYABS >= playerMovingBlockSize.y) continue;

			// XYの遠い方を検索して遠い方のみ押し戻し処理を行う。
			bool isPushBackX = distanceXABS >= distanceYABS;

			// 両方の移動量を求めて、移動量が小さい方を動かす。
			int moveIndex = 0;
			int moveIndexY = 0;
			Vec2<float> velTarget = MovingBlockMgr::Instance()->movingBlock[targetIndex]->vel;
			velTarget.y += MovingBlockMgr::Instance()->movingBlock[targetIndex]->gravity;
			Vec2<float> velBase = MovingBlockMgr::Instance()->movingBlock[baseIndex]->vel;
			velBase.y += MovingBlockMgr::Instance()->movingBlock[baseIndex]->gravity;
			if (velTarget.Length() >= velBase.Length()) {

				moveIndex = baseIndex;
				moveIndexY = targetIndex;

			}
			else {

				moveIndex = targetIndex;
				moveIndexY = baseIndex;

			}

			// Xを押し戻す場合。
			if (isPushBackX) {

				// プレイヤーより右に居た場合
				if (distanceX <= 0) {

					// 押し戻す距離を求める。
					float pushBackDistance = playerMovingBlockSize.x + distanceX;

					// 押し戻す。
					MovingBlockMgr::Instance()->movingBlock[moveIndex]->pos.x += pushBackDistance;

				}
				else {

					// 押し戻す距離を求める。
					float pushBackDistance = playerMovingBlockSize.x - distanceX;

					// 押し戻す。
					MovingBlockMgr::Instance()->movingBlock[moveIndex]->pos.x -= pushBackDistance;

				}

			}
			// Yを押し戻す場合。
			else {

				if (distanceY >= 0) {

					// 押し戻す距離を求める。
					float pushBackDistance = playerMovingBlockSize.y - distanceY;

					// 押し戻す。
					MovingBlockMgr::Instance()->movingBlock[moveIndexY]->pos.y -= pushBackDistance;

					// この場合は地面にあたった状態。
					MovingBlockMgr::Instance()->movingBlock[targetIndex]->onGround = true;
					MovingBlockMgr::Instance()->movingBlock[baseIndex]->onGround = true;
					MovingBlockMgr::Instance()->movingBlock[targetIndex]->gravity = 0;
					MovingBlockMgr::Instance()->movingBlock[baseIndex]->gravity = 0;

				}
				else {

					// 押し戻す距離を求める。
					float pushBackDistance = playerMovingBlockSize.y + distanceY;

					// 押し戻す。
					MovingBlockMgr::Instance()->movingBlock[moveIndexY]->pos.y += pushBackDistance;

					MovingBlockMgr::Instance()->movingBlock[targetIndex]->onGround = true;
					MovingBlockMgr::Instance()->movingBlock[baseIndex]->onGround = true;
					MovingBlockMgr::Instance()->movingBlock[targetIndex]->gravity = 0;
					MovingBlockMgr::Instance()->movingBlock[baseIndex]->gravity = 0;

				}

			}

		}

	}


	/*===== マップチップとの当たり判定 =====*/

	for (int index = 0; index < MOVING_BLOCK_SIZE; ++index) {

		// プレイヤーと離れていたら処理を行わない。
		if (!movingBlock[index]->isCloseToThePlayer) continue;

		// マップチップとの当たり判定
		movingBlock[index]->CheckHitMapChip(mapData);

	}


	/*===== 弾との当たり判定 =====*/

	// ブロックと弾の合計サイズ。
	const float BULLET_BLOCK_SIZE = movingBlock[0]->SCALE + BulletMgr::Instance()->bullets[0]->MAX_RADIUS;

	// ブロックの数分処理を行う。
	for (int blockIndex = 0; blockIndex < MOVING_BLOCK_SIZE; ++blockIndex) {

		// ブロックが処理を行わないフラグが立っていたら処理を飛ばす。
		if (!movingBlock[blockIndex]->isCloseToThePlayer) continue;

		Vec2<float>* blockPos = &(movingBlock[blockIndex]->pos);

		// 弾の数分処理を行う。
		const int BULLET_COUNT = BulletMgr::Instance()->BULLET_COUNT;
		for (int bulletIndex = 0; bulletIndex < BULLET_COUNT; ++bulletIndex) {

			// 弾の生存フラグが立っていなかったら処理を飛ばす。
			if (!BulletMgr::Instance()->bullets[bulletIndex]->isActive) continue;

			Vec2<float>* bulletPos = &(BulletMgr::Instance()->bullets[bulletIndex]->pos);

			// 距離を求める関数が重いので、ここで大雑把に離れていたら処理を飛ばす処理を書く。 重かったらコメントアウトを外す。
			//if (!(blockPos->x + BULLET_BLOCK_SIZE >= bulletPos->x && blockPos->x - BULLET_BLOCK_SIZE <= bulletPos->x &&
			//	blockPos->y + BULLET_BLOCK_SIZE >= bulletPos->y && blockPos->y - BULLET_BLOCK_SIZE <= bulletPos->y)) continue;

			// 弾との距離を求める。
			float bulletDistance = blockPos->Distance(*bulletPos);

			// 距離が一定以上離れていたら処理を飛ばす。
			if (bulletDistance > BULLET_BLOCK_SIZE) continue;

			// ここまで来たら当たっているということ。

			// ブロックを弾の進行方向ベクトルの方に飛ばす。
			movingBlock[blockIndex]->HitBullet(BulletMgr::Instance()->bullets[bulletIndex]->forwardVec);

			// 弾を消滅させる。
			BulletMgr::Instance()->bullets[bulletIndex]->Init();

		}

	}

}
