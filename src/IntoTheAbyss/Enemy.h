#pragma once
#include "Vec.h"

// �K���G�N���X
class Enemy {

private:

	/*-- �����o�ϐ� --*/

	Vec2<float> pos;
	bool isHit;
	//Vec2<float> forwardVec;


private:

	/*-- �萔 --*/

	//const float SPEED = 5.0f;
	const Vec2<float> SIZE = { 100.0f,100.0f };


public:

	/*-- �����o�֐� --*/

	// �R���X�g���N�^
	Enemy();

	// ����������
	void Init();

	// �X�V����
	void Update(const Vec2<float>& playerPos);

	// �`�揈��
	void Draw();

	// �e�Ƃ̓����蔻��
	void CheckHitBullet();


};