#pragma once
#include "BarrageBase.h"

// �e���N���X���ƂɃt�@�C�������ƂƂ�ł��Ȃ����ɂȂ肻���Ȃ̂ŁA���̃t�@�C���ɓZ�߂�B

// �Q�����e���N���X
class WhirlpoolBarrage : public BarrageBase {

private:

	/*===== �����o�ϐ� =====*/

	bool isEnd;			// �I��������ǂ����BUpdate�֐��ŕԂ��l�B
	int timer;			// �e���̊Ԋu�ȂǂɎg�p����B
	int roundCounter;	// �����������B ���ȏ��������I���ɂ���B
	float angle;		// �e���𔭎˂���p�x�B

	const int SHOOT_DELAY = 2;				// �e�𔭎˂���Ԋu�B
	const int MAX_ROUND = 1;
	const float SHOOT_COUNT_PER_ROUND = 10;	// ����Ō��e�̐��B
	const float ANGLE = DirectX::XM_2PI / SHOOT_COUNT_PER_ROUND;
	const float SPEED = 8.0f;				// �e��

public:

	/*===== �����o�֐� =====*/

	void Start() override;
	void Init() override;
	bool Update(BulletMgrBase& BulletMgr, const Vec2<float>& Pos, const Vec2<float>& TargetPos, const int& GraphHandle)override;

};


// �~�`�e���N���X
class CircularBarrage : public BarrageBase {

private:

	/*===== �����o�ϐ� =====*/

	bool isEnd;			// �I��������ǂ����BUpdate�֐��ŕԂ��l�B

	const float SHOOT_COUNT_PER_ROUND = 10;	// ����Ō��e�̐��B
	const float ANGLE = DirectX::XM_2PI / SHOOT_COUNT_PER_ROUND;
	const float SPEED = 8.0f;				// �e��

public:

	/*===== �����o�֐� =====*/

	void Start() override;
	void Init() override;
	bool Update(BulletMgrBase& BulletMgr, const Vec2<float>& Pos, const Vec2<float>& TargetPos, const int& GraphHandle)override;

};

// �G�̕����ɐ������e��
class TargetShotBarrage : public BarrageBase {

private:

	/*===== �����o�ϐ� =====*/

	bool isEnd;			// �I��������ǂ����BUpdate�֐��ŕԂ��l�B
	int timer;			// �e���̊Ԋu�ȂǂɎg�p����B
	int shotCounter;	// ��������

	const int SHOOT_DELAY = 5;				// �e�𔭎˂���Ԋu�B
	const int SHOOT_COUNT = 3;
	const float SPEED = 10.0f;				// �e��

public:

	/*===== �����o�֐� =====*/

	void Start() override;
	void Init() override;
	bool Update(BulletMgrBase& BulletMgr, const Vec2<float>& Pos, const Vec2<float>& TargetPos, const int& GraphHandle)override;

};

// �G�̕����ɕ����̒e���U��΂点�Č���
class ShotGunBarrage : public BarrageBase {

private:

	/*===== �����o�ϐ� =====*/

	bool isEnd;			// �I��������ǂ����BUpdate�֐��ŕԂ��l�B

	const int SHOOT_COUNT = 10;				// ����
	const float MAX_SPEED = 15.0f;			// �e���̍ő�l
	const float AngleDispersion = 0.5f;		// �p�x�̕��U� ���W�A��
	const float MIN_SPEED = 5.0f;			// �e���̍ŏ��l

public:

	/*===== �����o�֐� =====*/

	void Start() override;
	void Init() override;
	bool Update(BulletMgrBase& BulletMgr, const Vec2<float>& Pos, const Vec2<float>& TargetPos, const int& GraphHandle)override;

};