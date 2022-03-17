#pragma once
#include "Vec.h"

// ���Ԓ�~�̒Z���̐��\���m�F����p�̃v���C���[��ǔ�����u���b�N
class TimeStopTestBlock {

public:

	/*===== �����o�ϐ� =====*/

	Vec2<float> pos;				// ���ݍ��W
	bool isTimeStop;		// ���Ԓ�~�̒Z�����h�����Ă��邩�̃t���O


public:

	/*===== �萔 =====*/

	const float SCALE = 10.0f;


public:

	/*===== �����o�֐� =====*/

	// �R���X�g���N�^
	TimeStopTestBlock();

	// �X�V����
	void Update(const Vec2<float>& playerPos);

	// �`�揈��
	void Draw(const int& blockGraph);
};