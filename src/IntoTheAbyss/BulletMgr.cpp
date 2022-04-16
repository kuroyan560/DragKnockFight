#include "BulletMgr.h"
#include "Bullet.h"
#include "MapChipCollider.h"
//#include "BulletParticleMgr.h"
#include"ParticleMgr.h"

void BulletMgr::Setting()
{

	/*-- �Z�b�e�B���O���� --*/

	for (int index = 0; index < BULLET_COUNT; ++index) {

		bullets[index] = make_shared<Bullet>();

	}

}

void BulletMgr::Generate(const int& graphHandle, const Vec2<float>& generatePos, const float& forwardAngle, const bool& isShotRight)
{

	/*-- �������� --*/

	int generateBullet = GENERATE_COUNT;

	// �������鐔�����[�v
	for (int generateCount = 0; generateCount < generateBullet; ++generateCount) {

		// ��������B
		for (int index = 0; index < BULLET_COUNT; ++index) {

			// �����ς݂������珈�����΂��B
			if (bullets[index]->GetIsActive()) continue;

			// �����������s���B

			// ���˂��鐔���̔������炵���p�x�����߂�B��^�ɔ�΂����߁B
			float slipAngle = SLIP_ONE_BULLET * (GENERATE_COUNT / 2.0f);

			// �����̊p�x�����߂�B
			float generateAngle = forwardAngle;

			// ��������p�x�����炷�B
			generateAngle += -slipAngle + SLIP_ONE_BULLET * generateCount;

			// ��������p�x���x�N�g���Ɏ����B
			Vec2<float> generateForwardVec = { cosf(generateAngle), sinf(generateAngle) };

			// ��������B
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

	/*-- �X�V���� --*/

	for (int index = 0; index < BULLET_COUNT; ++index) {

		// �������������珈�����΂��B
		if (!bullets[index]->GetIsActive()) continue;

		bullets[index]->Update();

	}

}

void BulletMgr::Draw()
{

	/*-- �`�揈�� --*/

	for (int index = 0; index < BULLET_COUNT; ++index) {

		// �������������珈�����΂��B
		if (!bullets[index]->GetIsActive()) continue;

		bullets[index]->Draw();

	}

}

void BulletMgr::CheckHit(const vector<vector<int>>& mapData)
{

	/*===== �}�b�v�`�b�v�Ƃ̓����蔻�� =====*/

	for (int index = 0; index < BULLET_COUNT; ++index) {

		// �e����������Ă��Ȃ������珈�����΂��B
		if (!bullets[index]->isActive) continue;

		// ���݂̃}�b�v�`�b�v�ԍ������߂�B
		int mapX = (bullets[index]->pos.x + MAP_CHIP_HALF_SIZE) / MAP_CHIP_SIZE;
		int mapY = (bullets[index]->pos.y + MAP_CHIP_HALF_SIZE) / MAP_CHIP_SIZE;

		// �z�񂪃T�C�Y�O��������e�����ł�����B
		if (mapX <= 0 || mapX >= mapData[0].size()) {

			// �e�p�[�e�B�N���𐶐�����B
			//BulletParticleMgr::Instance()->Generate(bullets[index]->pos, bullets[index]->forwardVec);
			ParticleMgr::Instance()->Generate(bullets[index]->pos, bullets[index]->forwardVec, BULLET);

			bullets[index]->Init();
			continue;
		}

		if (mapY <= 0 || mapY >= mapData.size()) {

			// �e�p�[�e�B�N���𐶐�����B
			//BulletParticleMgr::Instance()->Generate(bullets[index]->pos, bullets[index]->forwardVec);
			ParticleMgr::Instance()->Generate(bullets[index]->pos, bullets[index]->forwardVec, BULLET);

			bullets[index]->Init();
			continue;
		}

		// ���̒e�̃}�b�v�`�b�v���u���b�N(1~9)��������e�����ł�����B
		if (mapData[mapY][mapX] >= 1 && mapData[mapY][mapX] <= 9) {

			// �e�p�[�e�B�N���𐶐�����B
			//BulletParticleMgr::Instance()->Generate(bullets[index]->pos, bullets[index]->forwardVec);
			ParticleMgr::Instance()->Generate(bullets[index]->pos, bullets[index]->forwardVec, BULLET);

			bullets[index]->Init();

		}

	}
}
