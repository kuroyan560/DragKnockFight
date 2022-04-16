#include "BulletMgr.h"
#include "Bullet.h"
#include "MapChipCollider.h"
//#include "BulletParticleMgr.h"
#include"ParticleMgr.h"

void BulletMgr::Setting()
{

	/*-- セッティング処理 --*/

	for (int index = 0; index < BULLET_COUNT; ++index) {

		bullets[index] = make_shared<Bullet>();

	}

}

void BulletMgr::Generate(const int& graphHandle, const Vec2<float>& generatePos, const float& forwardAngle, const bool& isShotRight)
{

	/*-- 生成処理 --*/

	int generateBullet = GENERATE_COUNT;

	// 生成する数分ループ
	for (int generateCount = 0; generateCount < generateBullet; ++generateCount) {

		// 生成する。
		for (int index = 0; index < BULLET_COUNT; ++index) {

			// 生成済みだったら処理を飛ばす。
			if (bullets[index]->GetIsActive()) continue;

			// 生成処理を行う。

			// 発射する数分の半分ずらした角度を求める。扇型に飛ばすため。
			float slipAngle = SLIP_ONE_BULLET * (GENERATE_COUNT / 2.0f);

			// 引数の角度を求める。
			float generateAngle = forwardAngle;

			// 生成する角度をずらす。
			generateAngle += -slipAngle + SLIP_ONE_BULLET * generateCount;

			// 生成する角度をベクトルに治す。
			Vec2<float> generateForwardVec = { cosf(generateAngle), sinf(generateAngle) };

			// 生成する。
			bullets[index]->Generate(graphHandle, generatePos, generateForwardVec, isShotRight == true ? Bullet::R_HAND : Bullet::L_HAND);

			break;

		}



	}

}

void BulletMgr::Init()
{
	for (auto& b : bullets)b->Init();
}

void BulletMgr::Update()
{

	/*-- 更新処理 --*/

	for (int index = 0; index < BULLET_COUNT; ++index) {

		// 未生成だったら処理を飛ばす。
		if (!bullets[index]->GetIsActive()) continue;

		bullets[index]->Update();

	}

}

void BulletMgr::Draw()
{

	/*-- 描画処理 --*/

	for (int index = 0; index < BULLET_COUNT; ++index) {

		// 未生成だったら処理を飛ばす。
		if (!bullets[index]->GetIsActive()) continue;

		bullets[index]->Draw();

	}

}

void BulletMgr::CheckHit(const vector<vector<int>>& mapData)
{

	/*===== マップチップとの当たり判定 =====*/

	for (int index = 0; index < BULLET_COUNT; ++index) {

		// 弾が生成されていなかったら処理を飛ばす。
		if (!bullets[index]->isActive) continue;

		// 現在のマップチップ番号を求める。
		int mapX = (bullets[index]->pos.x + MAP_CHIP_HALF_SIZE) / MAP_CHIP_SIZE;
		int mapY = (bullets[index]->pos.y + MAP_CHIP_HALF_SIZE) / MAP_CHIP_SIZE;

		// 配列がサイズ外だったら弾を消滅させる。
		if (mapX <= 0 || mapX >= mapData[0].size()) {

			// 弾パーティクルを生成する。
			//BulletParticleMgr::Instance()->Generate(bullets[index]->pos, bullets[index]->forwardVec);
			ParticleMgr::Instance()->Generate(bullets[index]->pos, bullets[index]->forwardVec, BULLET);

			bullets[index]->Init();
			continue;
		}

		if (mapY <= 0 || mapY >= mapData.size()) {

			// 弾パーティクルを生成する。
			//BulletParticleMgr::Instance()->Generate(bullets[index]->pos, bullets[index]->forwardVec);
			ParticleMgr::Instance()->Generate(bullets[index]->pos, bullets[index]->forwardVec, BULLET);

			bullets[index]->Init();
			continue;
		}

		// その弾のマップチップがブロック(1~9)だったら弾を消滅させる。
		if (mapData[mapY][mapX] >= 1 && mapData[mapY][mapX] <= 9) {

			// 弾パーティクルを生成する。
			//BulletParticleMgr::Instance()->Generate(bullets[index]->pos, bullets[index]->forwardVec);
			ParticleMgr::Instance()->Generate(bullets[index]->pos, bullets[index]->forwardVec, BULLET);

			bullets[index]->Init();

		}

	}
}
