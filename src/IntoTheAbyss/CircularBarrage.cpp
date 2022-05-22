#include "CircularBarrage.h"
#include "BulletMgrBase.h"

void CircularBarrage::Start()
{

	/*===== ���������� =====*/

	isEnd = false;
	timer = 0;
	angle = 0;
	roundCounter = 0;

}

void CircularBarrage::Init()
{

	isEnd = true;
	timer = 0;
	angle = 0;
	roundCounter = 0;

}

bool CircularBarrage::Update(BulletMgrBase& BulletMgr, const Vec2<float>& Pos, const int& GraphHandle)
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
