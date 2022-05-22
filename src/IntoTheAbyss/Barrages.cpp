#include "Barrages.h"

#pragma region WhirlpoolBarrage

void WhirlpoolBarrage::Start()
{

	/*===== ���������� =====*/

	isEnd = false;
	timer = 0;
	angle = 0;
	roundCounter = 0;

}

void WhirlpoolBarrage::Init()
{

	isEnd = true;
	timer = 0;
	angle = 0;
	roundCounter = 0;

}

bool WhirlpoolBarrage::Update(BulletMgrBase& BulletMgr, const Vec2<float>& Pos, const int& GraphHandle)
{

	/*===== �X�V���� =====*/

	if (isEnd) return true;

	// �^�C�}�[���X�V���āA�e�����������X�V����B
	++timer;
	if (SHOOT_DELAY < timer) {

		timer = 0;

		// �e�𐶐��B
		BulletMgr.Generate(GraphHandle, Pos, angle, SPEED);

		// �p�x���X�V�B
		angle += ANGLE;

		// �p�x������𒴂�����������������s���B
		if (DirectX::XM_2PI <= angle) {

			angle = 0;
			++roundCounter;

		}

	}

	// ���ȏ�������珈�����I����B
	if (MAX_ROUND <= roundCounter) {

		isEnd = true;

	}

	return isEnd;
}

#pragma endregion


#pragma region CircularBarrage

void CircularBarrage::Start()
{

	/*===== �J�n =====*/

	isEnd = false;

}

void CircularBarrage::Init()
{

	/*===== �����I�� =====*/

	isEnd = true;

}

bool CircularBarrage::Update(BulletMgrBase& BulletMgr, const Vec2<float>& Pos, const int& GraphHandle)
{

	/*===== �X�V���� =====*/

	if (isEnd) return true;

	float shootAngle = 0;

	// ����ɒe�𐶐�����B
	for (int index = 0; index < SHOOT_COUNT_PER_ROUND; ++index) {

		// �e�𐶐����Ċp�x���X�V����B
		BulletMgr.Generate(GraphHandle, Pos, shootAngle, SPEED);

		shootAngle += ANGLE;

	}

	// ��񌂂�����I���B
	isEnd = true;

	return isEnd;
}

#pragma endregion