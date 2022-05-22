#include "CircularBarrage.h"

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
