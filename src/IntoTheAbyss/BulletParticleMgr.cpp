#include "BulletParticleMgr.h"
#include "BulletParticle.h"

#include"KuroFunc.h"

void BulletParticleMgr::Setting()
{

	/*===== セッティング処理 =====*/

	for (int index = 0; index < MAX_PARTICLE; ++index) {

		particle[index] = make_shared<BulletParticle>();

	}

}

void BulletParticleMgr::Init()
{

	/*===== 初期化処理 =====*/

	for (int index = 0; index < MAX_PARTICLE; ++index) {

		particle[index]->Init();

	}

}

void BulletParticleMgr::Generate(const Vec2<float>& generatePos, const Vec2<float>& forwardVec)
{

	/*===== 生成処理 =====*/

	// 生成する数分処理を回す。
	for (int generateCount = 0; generateCount < GENERATE_PARTICLE; ++generateCount) {

		for (int index = 0; index < MAX_PARTICLE; ++index) {

			// 生成済みだったら処理を飛ばす。
			if (particle[index]->isAlive) continue;

			// 生成する位置をランダムにずらす。
			Vec2<float> pos = generatePos;
			//pos.x += GetRand(6) - 3;
			//pos.y += GetRand(6) - 3;
			pos.x += KuroFunc::GetRand(6) - 3;
			pos.y += KuroFunc::GetRand(6) - 3;

			// 生成する。
			particle[index]->Generate(pos, forwardVec);

			break;

		}

	}

}

void BulletParticleMgr::Update()
{

	/*===== 更新処理 =====*/

	for (int index = 0; index < MAX_PARTICLE; ++index) {

		// 未生成だったら処理を飛ばす。
		if (!particle[index]->isAlive) continue;

		// 生成する。
		particle[index]->Update();

	}

}

void BulletParticleMgr::Draw()
{

	/*===== 描画処理 =====*/

	for (int index = 0; index < MAX_PARTICLE; ++index) {

		// 未生成だったら処理を飛ばす。
		if (!particle[index]->isAlive) continue;

		// 生成する。
		particle[index]->Draw();

	}

}
