#pragma once
#include "Vec.h"
#include "Singleton.h"

class DistanceCounter : public Singleton<DistanceCounter> {

public:

	/*===== �����o�֐� =====*/

	float distance;		// 2�L�����Ԃ̋���
	Vec2<float> lineCenterPos;	// 2�L�����Ԃ̒��S�_�̍��W

	const float DEAD_LINE = 300.0f;		// ����������I���̋���


public:

	/*===== �����o�֐� =====*/

	// �R���X�g���N�^
	DistanceCounter();

	// ����������
	void Init();

	// �X�V����
	void Update();

	// �`�揈��
	void Draw();

};