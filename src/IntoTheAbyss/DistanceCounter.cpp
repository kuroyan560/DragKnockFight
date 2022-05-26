#include "DistanceCounter.h"
#include "CharacterManager.h"

DistanceCounter::DistanceCounter()
{

	/*===== �R���X�g���N�^ =====*/

	distance = 0;
	lineCenterPos = { -1000,-10000 };

}

void DistanceCounter::Init()
{

	/*===== ������ =====*/

	distance = 0;
	lineCenterPos = { -1000,-10000 };

}

void DistanceCounter::Update()
{

	/*===== �X�V���� =====*/

	// ��L�����̍��W�𑫂����l�B
	Vec2<float> addCharaPos = CharacterManager::Instance()->Right()->pos + CharacterManager::Instance()->Left()->pos;

	// ���̒��S�����߂�B
	lineCenterPos = addCharaPos / 2.0f;

	// 2�_�Ԃ̋��������߂�B
	distance = addCharaPos.Length() - DEAD_LINE;

}

void DistanceCounter::Draw()
{

	/*===== �`�揈�� =====*/

}
