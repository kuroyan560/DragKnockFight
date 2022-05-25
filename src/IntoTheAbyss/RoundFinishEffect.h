#pragma once
#include "Vec.h"
#include "CharacterManager.h"
#include "Camera.h"
#include "Singleton.h"

#include "RoundFinishParticle.h"

class RoundFinishEffect : public Singleton<RoundFinishEffect> {

public:

	/*===== �����o�ϐ� =====*/

	enum class EFFECT_STATUS {

		NUM1_ZOOMIN,			// �@��l�̐^�񒆂ɃY�[���C������B
		NUM2_ENEMY_SHAKE,		// �A�G���V�F�C�N������B
		NUM3_ENEMY_EXP,			// �B�G�𔚔�������B
		NUM4_EXP_END,			// �C�������I��点��B
		NUM5_RETURN_DEFPOS,		// �D�e�L�������K��̍��W�ɖ߂��B

	};

	EFFECT_STATUS status;		// �X�e�[�^�X

	int timer;					// �e�^�C�}�[
	bool isEnd;					// ���o���I��������̃t���O
	Vec2<float> shakeAmount;	// �V�F�C�N�����鏈���S�ʂɎg�p������

	// �e�X�e�[�^�X�̎���
	const int NUM1_ZOOMIN_TIMER = 90;
	const int NUM2_ENEMY_SHAKE_TIMER = 150;
	const int NUM3_ENEMY_EXP_TIMER = 240;



public:

	// �R���X�g���N�^
	RoundFinishEffect();

	// ����������
	void Init();

	// �J�n����
	void Start();

	// �X�V����
	void Update(const Vec2<float>& LineCenterPos);

	// �`�揈��
	void Draw();

};