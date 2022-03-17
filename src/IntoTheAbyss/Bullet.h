#pragma once
#include "Vec.h"
#include <vector>

using namespace std;

class MovingBlock;

// �e�N���X
class Bullet {

public:

	/*-- �萔 --*/

	const float MAX_RADIUS = 5.0f;
	const float MAX_SPEED = 30.0f;
	const int DEAD_TIMER = 120;

	enum SHOT_HAND {
		R_HAND,
		L_HAND,
	};


public:

	/*-- �����o�ϐ� --*/

	Vec2<float> pos;			// ���W
	Vec2<float> forwardVec;	// �ړ������x�N�g��
	float alpha;
	float speed;
	int deadTimer;		// ���ʂ܂ł̎���
	bool isActive;		// �������Ă��邩�̃t���O
	bool isFirstFrame;
	SHOT_HAND handID;	// �����ꂽ��


public:

	/*-- �����o�֐� --*/

	// �R���X�g���N�^
	Bullet();

	// ����������
	void Init();

	// ��������
	void Generate(const Vec2<float>& generatePos, const Vec2<float> forwardVec, const bool isFirstShot, const SHOT_HAND& id);

	// �X�V����
	void Update();

	// �`�揈��
	void Draw();

	// �����t���O�̃Q�b�^
	inline const bool& GetIsActive() { return isActive; }


};