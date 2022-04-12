#include "BossBulletManager.h"

BossBulletManager::BossBulletManager()
{
	for (int index = 0; index < BULLET_COUNT; ++index)
	{
		bullets[index] = make_shared<Bullet>();
	}
}

void BossBulletManager::Init()
{
	for (int index = 0; index < BULLET_COUNT; ++index)
	{
		bullets[index]->Init();
	}
}

void BossBulletManager::Generate(const Vec2<float> &generatePos, const float &forwardAngle, const float &speed)
{
	/*-- 生成処理 --*/

	for (int index = 0; index < BULLET_COUNT; ++index) {

		// 生成済みだったら処理を飛ばす。
		if (bullets[index]->GetIsActive()) continue;

		// 生成する角度をベクトルに治す。
		Vec2<float> generateForwardVec = { cosf(forwardAngle), sinf(forwardAngle) };

		// 生成する。
		bullets[index]->Generate(generatePos, generateForwardVec, false, true ? Bullet::R_HAND : Bullet::L_HAND, speed);

		break;
	}
}

void BossBulletManager::Update()
{
	/*-- 更新処理 --*/

	for (int index = 0; index < BULLET_COUNT; ++index) {

		// 未生成だったら処理を飛ばす。
		if (!bullets[index]->GetIsActive()) continue;

		bullets[index]->Update();

	}
}

void BossBulletManager::Draw()
{
	/*-- 描画処理 --*/

	for (int index = 0; index < BULLET_COUNT; ++index) {

		// 未生成だったら処理を飛ばす。
		if (!bullets[index]->GetIsActive()) continue;

		bullets[index]->Draw();

	}
}
