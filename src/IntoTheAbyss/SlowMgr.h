#pragma once
#include "Singleton.h"

// �X���[���[�V�������o�y�уq�b�g�X�g�b�v�̍ۂ̃X���[�ȂǂɎg�p����l���Ǘ�����N���X
class SlowMgr : public Singleton<SlowMgr> {

public:

	/*===== �����o�ϐ� =====*/

	float playerDeadSlow;	// �v���C���[�̃X���[���[�V����
	float prevFramePlayerDeadSlow;	// �O�t���[��


public:

	/*===== �萔 =====*/


public:

	/*===== �����o�ϐ� =====*/

	// �R���X�g���N�^
	SlowMgr() {

		playerDeadSlow = 1.0f;

	}

	// �X�V����
	void Update() {

		prevFramePlayerDeadSlow = playerDeadSlow;

	}

};