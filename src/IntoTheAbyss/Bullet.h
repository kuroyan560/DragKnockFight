#pragma once
#include "Vec.h"
#include <vector>
#include"BulletCollision.h"

using namespace std;

class MovingBlock;

// �e�N���X
class Bullet {

public:

	/*-- �萔 --*/

	//const float MAX_RADIUS = 5.0f;
	const float MAX_RADIUS = 20.0f;
	const float MAX_SPEED = 30.0f;
	const int DEAD_TIMER = 120;

public:

	/*-- �����o�ϐ� --*/

	Vec2<float> pos;			// ���W
	Vec2<float> prevPos;
	Vec2<float> forwardVec;	// �ړ������x�N�g��
	float alpha;
	float speed;
	int deadTimer;		// ���ʂ܂ł̎���
	bool isActive;		// �������Ă��邩�̃t���O
	bool isFirstFrame;

	std::shared_ptr<SphereCollision> bulletHitBox;

	int graph;
public:

	/*-- �����o�֐� --*/

	// �R���X�g���N�^
	Bullet();

	// ����������
	void Init();

	// ��������
	void Generate(const int& graphHandle, const Vec2<float>& generatePos, const Vec2<float> forwardVec, const float& speed);

	// �X�V����
	void Update();

	// �`�揈��
	void Draw();

	// �����t���O�̃Q�b�^
	inline const bool& GetIsActive() { return isActive; }


};