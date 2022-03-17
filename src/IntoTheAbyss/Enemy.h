#pragma once
#include "Vec.h"

// �K���G�N���X
class Enemy {

private:

	/*-- �����o�ϐ� --*/

	Vec2<float> pos;
	Vec2<float> forwardVec;


private:

	/*-- �萔 --*/

	const float SPEED = 5.0f;
	const Vec2<float> SIZE = { 20.0f,20.0f };


public:

	/*-- �����o�֐� --*/

	// �R���X�g���N�^
	Enemy();

	// ����������
	void Init();

	// �X�V����
	void Update();

	// �`�揈��
	void Draw();


};