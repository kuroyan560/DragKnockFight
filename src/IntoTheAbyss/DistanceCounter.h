#pragma once
#include "Vec.h"
#include "Singleton.h"
#include <array>

class DistanceCounter : public Singleton<DistanceCounter> {

public:

	/*===== �����o�֐� =====*/

	float distance;		// 2�L�����Ԃ̋���
	Vec2<float> lineCenterPos;	// 2�L�����Ԃ̒��S�_�̍��W
	Vec2<float> shakeAmount;	// �V�F�C�N��
	const float MAX_SHAKE_DISTANCE = 1000.0f;	// �V�F�C�N���n�߂�J�n����
	const float MAX_SHAKE_AMOUNT = 15.0f;		// �V�F�C�N�̍ő��

	const float DEAD_LINE = 300.0f;		// ����������I���̋���

	std::array<int, 12> fontGraph;

	bool isExpSmall;	// ���������邩�ǂ����t���O
	float exp;			// ���̑傫���̊���


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