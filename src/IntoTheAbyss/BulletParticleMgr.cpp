#include "BulletParticleMgr.h"
#include "BulletParticle.h"

#include"KuroFunc.h"

void BulletParticleMgr::Setting()
{

	/*===== �Z�b�e�B���O���� =====*/

	for (int index = 0; index < MAX_PARTICLE; ++index) {

		particle[index] = make_shared<BulletParticle>();

	}

}

void BulletParticleMgr::Init()
{

	/*===== ���������� =====*/

	for (int index = 0; index < MAX_PARTICLE; ++index) {

		particle[index]->Init();

	}

}

void BulletParticleMgr::Generate(const Vec2<float>& generatePos, const Vec2<float>& forwardVec)
{

	/*===== �������� =====*/

	// �������鐔���������񂷁B
	for (int generateCount = 0; generateCount < GENERATE_PARTICLE; ++generateCount) {

		for (int index = 0; index < MAX_PARTICLE; ++index) {

			// �����ς݂������珈�����΂��B
			if (particle[index]->isAlive) continue;

			// ��������ʒu�������_���ɂ��炷�B
			Vec2<float> pos = generatePos;
			//pos.x += GetRand(6) - 3;
			//pos.y += GetRand(6) - 3;
			pos.x += KuroFunc::GetRand(6) - 3;
			pos.y += KuroFunc::GetRand(6) - 3;

			// ��������B
			particle[index]->Generate(pos, forwardVec);

			break;

		}

	}

}

void BulletParticleMgr::Update()
{

	/*===== �X�V���� =====*/

	for (int index = 0; index < MAX_PARTICLE; ++index) {

		// �������������珈�����΂��B
		if (!particle[index]->isAlive) continue;

		// ��������B
		particle[index]->Update();

	}

}

void BulletParticleMgr::Draw()
{

	/*===== �`�揈�� =====*/

	for (int index = 0; index < MAX_PARTICLE; ++index) {

		// �������������珈�����΂��B
		if (!particle[index]->isAlive) continue;

		// ��������B
		particle[index]->Draw();

	}

}
