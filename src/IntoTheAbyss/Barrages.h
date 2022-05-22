#pragma once
#include "BarrageBase.h"

// �e���N���X���ƂɃt�@�C�������ƂƂ�ł��Ȃ����ɂȂ肻���Ȃ̂ŁA���̃t�@�C���ɓZ�߂�B
// Ctrl + M + O �ŕ��Ă�������ق����ǂ������ł��I

// �Q�����e���N���X
#pragma region WhirlpoolBarrage

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

#pragma endregion


// 2Way�Q�����e���N���X
#pragma region Whirlpool2WayBarrage

class Whirlpool2WayBarrage : public BarrageBase {

private:

	/*===== �����o�ϐ� =====*/

	bool isEnd;			// �I��������ǂ����BUpdate�֐��ŕԂ��l�B
	int timer;			// �e���̊Ԋu�ȂǂɎg�p����B
	int roundCounter;	// �����������B ���ȏ��������I���ɂ���B
	float angle;		// �e���𔭎˂���p�x�B

	const int SHOOT_DELAY = 2;				// �e�𔭎˂���Ԋu�B
	const int MAX_ROUND = 1;				// ���� 
	const float SHOOT_COUNT_PER_ROUND = 10;	// ����Ō��e�̐��B
	const float ANGLE = DirectX::XM_2PI / SHOOT_COUNT_PER_ROUND;
	const float SPEED = 8.0f;				// �e��

public:

	/*===== �����o�֐� =====*/

	void Start() override;
	void Init() override;
	bool Update(BulletMgrBase& BulletMgr, const Vec2<float>& Pos, const Vec2<float>& TargetPos, const int& GraphHandle)override;

};

#pragma endregion


// �~�`�e���N���X
#pragma region CircularBarrage

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

#pragma endregion


// �G�̕����ɐ������e��
#pragma region TargetShotBarrage

class TargetShotBarrage : public BarrageBase {

private:

	/*===== �����o�ϐ� =====*/

	bool isEnd;			// �I��������ǂ����BUpdate�֐��ŕԂ��l�B
	int timer;			// �e���̊Ԋu�ȂǂɎg�p����B
	int shotCounter;	// ��������

	const int SHOOT_DELAY = 5;				// �e�𔭎˂���Ԋu�B
	const int SHOOT_COUNT = 5;
	const float SPEED = 15.0f;				// �e��

public:

	/*===== �����o�֐� =====*/

	void Start() override;
	void Init() override;
	bool Update(BulletMgrBase& BulletMgr, const Vec2<float>& Pos, const Vec2<float>& TargetPos, const int& GraphHandle)override;

};

#pragma endregion


// �G�̕����ɐ������e�� 3Way
#pragma region TargetShot3WayBarrage

class TargetShot3WayBarrage : public BarrageBase {

private:

	/*===== �����o�ϐ� =====*/

	bool isEnd;			// �I��������ǂ����BUpdate�֐��ŕԂ��l�B
	int timer;			// �e���̊Ԋu�ȂǂɎg�p����B
	int shotCounter;	// ��������

	const int SHOOT_DELAY = 5;				// �e�𔭎˂���Ԋu�B
	const int SHOOT_COUNT = 5;
	const float SPEED = 15.0f;				// �e��
	const float AngleDispersion = 0.5f;		// �p�x�̕��U� ���W�A��

public:

	/*===== �����o�֐� =====*/

	void Start() override;
	void Init() override;
	bool Update(BulletMgrBase& BulletMgr, const Vec2<float>& Pos, const Vec2<float>& TargetPos, const int& GraphHandle)override;

};

#pragma endregion


// �G�̕����ɕ����̒e���U��΂点�Č���
#pragma region ShotGunBarrage

class ShotGunBarrage : public BarrageBase {

private:

	/*===== �����o�ϐ� =====*/

	bool isEnd;			// �I��������ǂ����BUpdate�֐��ŕԂ��l�B

	const int SHOOT_COUNT = 10;				// ����
	const float MAX_SPEED = 15.0f;			// �e���̍ő�l
	const float MIN_SPEED = 5.0f;			// �e���̍ŏ��l
	const float AngleDispersion = 0.5f;		// �p�x�̕��U� ���W�A��

public:

	/*===== �����o�֐� =====*/

	void Start() override;
	void Init() override;
	bool Update(BulletMgrBase& BulletMgr, const Vec2<float>& Pos, const Vec2<float>& TargetPos, const int& GraphHandle)override;

};

#pragma endregion


// �G�̕����ɔg��̒e�����΂��B
#pragma region WaveBarrage

class WaveBarrage : public BarrageBase {

private:

	/*===== �����o�ϐ� =====*/

	bool isEnd;			// �I��������ǂ����BUpdate�֐��ŕԂ��l�B
	int timer;			// �e���̊Ԋu�ȂǂɎg�p����B
	float sinTimer;		// sin�g�Ɏg�p����B

	const float TIMER = 60.0f;				// �����؂�܂ł̍��v����
	const float FRAME_TIMER = 0.2f;			// Sin�g�ɉ��Z����l
	const float SPEED = 8.0f;				// �e��
	const float AngleDispersion = 1.0f;		// �p�x�̕��U� ���W�A��
	const int SHOOT_DELAY = 5;				// �e�����x��

public:

	/*===== �����o�֐� =====*/

	void Start() override;
	void Init() override;
	bool Update(BulletMgrBase& BulletMgr, const Vec2<float>& Pos, const Vec2<float>& TargetPos, const int& GraphHandle)override;

};

#pragma endregion