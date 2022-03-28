#pragma once
#include "Vec.h"

enum ENEMY_ID {

	ENEMY_BOSS,
	ENEMY_SMALL,

};

// �K���G�N���X
class Enemy {

public:

	/*-- �����o�ϐ� --*/

	Vec2<float> pos;
	Vec2<float> size;
	bool isHit;
	ENEMY_ID id;
	bool isActive;
	int hitPoint;
	//Vec2<float> forwardVec;


private:

	/*-- �萔 --*/

	//const float SPEED = 5.0f;
	const Vec2<float> SIZE_BIG = { 100.0f,100.0f };
	const Vec2<float> SIZE_SMALL = { 30.0f,30.0f };
	const float KNOCK_BACK = 30.0f;
	const int HITPOINT_SMALL = 5;


public:

	/*-- �����o�֐� --*/

	// �R���X�g���N�^
	Enemy();

	// ����������
	void Init();

	// ��������
	void Generate(const ENEMY_ID& id);

	// �X�V����
	void Update(const Vec2<float>& playerPos);

	// �`�揈��
	void Draw();

	// �e�Ƃ̓����蔻��
	void CheckHitBullet();


};