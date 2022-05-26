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
		NUM4_RETURN_DEFPOS,		// �D�e�L�������K��̍��W�ɖ߂��B
		NUM5_RETURN_PLAYER_DEFPOS,

	};

	EFFECT_STATUS status;		// �X�e�[�^�X

	int timer;					// �e�^�C�}�[
	bool isEnd;					// ���o���I��������̃t���O
	Vec2<float> shakeAmount;	// �V�F�C�N�����鏈���S�ʂɎg�p������

	bool isPerfect;				// ���ׂẴu���b�N���󂵂����̃t���O
	Vec2<float> perfectPos;		// [Perfect]�̍��W
	Vec2<float> perfectExp;		// [Perfect]�̊g�k

	// �e�X�e�[�^�X�̎���
	const int NUM1_ZOOMIN_TIMER = 90;
	const int NUM2_ENEMY_SHAKE_TIMER = 120;
	const int NUM3_ENEMY_EXP_TIMER = 240;
	const int NUM4_RETURN_DEFPOS_TIMER = 120;
	const int NUM5_RETURN_PLAYER_DEF_POS = 120;

	// �p�[�t�F�N�g�̉摜�̓����ʁB
	const float PERFECT_MOVE_POS_Y = 500.0f;


public:

	// �R���X�g���N�^
	RoundFinishEffect();

	// ����������
	void Init();

	// �J�n����
	void Start(const bool& IsPerfect);

	// �X�V����
	void Update(const Vec2<float>& LineCenterPos);

	// �`�揈��
	void Draw();

};