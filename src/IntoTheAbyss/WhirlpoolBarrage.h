#pragma once
#include "BarrageBase.h"

// �~�`�e���N���X
class WhirlpoolBarrage : public BarrageBase {

private:

	/*===== �����o�ϐ� =====*/

	bool isEnd;			// �I��������ǂ����BUpdate�֐��ŕԂ��l�B
	int timer;			// �e���̊Ԋu�ȂǂɎg�p����B
	int roundCounter;	// �����������B ���ȏ��������I���ɂ���B
	float angle;		// �e���𔭎˂���p�x�B

	const int SHOOT_DELAY = 2;				// �e�𔭎˂���Ԋu�B
	const int MAX_ROUND = 3;
	const float SHOOT_COUNT_PER_ROUND = 10;	// ����Ō��e�̐��B
	const float ANGLE = DirectX::XM_2PI / SHOOT_COUNT_PER_ROUND;
	const float SPEED = 8.0f;				// �e��

public:

	/*===== �����o�֐� =====*/

	void Start() override;
	void Init() override;
	bool Update(BulletMgrBase& BulletMgr, const Vec2<float>& Pos, const int& GraphHandle)override;

};