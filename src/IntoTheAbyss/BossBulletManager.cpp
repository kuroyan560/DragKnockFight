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
	/*-- �������� --*/

	for (int index = 0; index < BULLET_COUNT; ++index) {

		// �����ς݂������珈�����΂��B
		if (bullets[index]->GetIsActive()) continue;

		// ��������p�x���x�N�g���Ɏ����B
		Vec2<float> generateForwardVec = { cosf(forwardAngle), sinf(forwardAngle) };

		// ��������B
		bullets[index]->Generate(generatePos, generateForwardVec, false, true ? Bullet::R_HAND : Bullet::L_HAND, speed);

		break;
	}
}

void BossBulletManager::Update()
{
	/*-- �X�V���� --*/

	for (int index = 0; index < BULLET_COUNT; ++index) {

		// �������������珈�����΂��B
		if (!bullets[index]->GetIsActive()) continue;

		bullets[index]->Update();

	}
}

void BossBulletManager::Draw()
{
	/*-- �`�揈�� --*/

	for (int index = 0; index < BULLET_COUNT; ++index) {

		// �������������珈�����΂��B
		if (!bullets[index]->GetIsActive()) continue;

		bullets[index]->Draw();

	}
}
