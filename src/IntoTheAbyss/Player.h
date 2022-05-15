#pragma once
#include "Vec.h"
#include <memory>
#include <vector>
#include"AreaCollider.h"

using namespace std;

// �v���C���[�̘r�̑O���錾
class PlayerHand;

#include"PlayerAnimation.h"
#include"AfterImg.h"
class LightManager;
#include"StagingInterFace.h"

#include"CharacterInterFace.h"
#include"Tutorial.h"

#include"RunOutOfStaminaEffect.h"

// �v���C���[�N���X
class Player :public CharacterInterFace
{

public:
	/*-- �����o�ϐ� --*/
	int rapidFireTimerLeft;			// �A�˃^�C�}�[����
	int rapidFireTimerRight;		// �A�˃^�C�}�[�E��

	// ���̃L�����̐F
	Color charaColor;

	//int playerGraph;

	//�����ɂ��X�g���b�`
	//const Vec2<float> MAX_STRETCH = { 16.0f,37.0f };	//�ő�X�g���b�`��
	const Vec2<float> MAX_STRETCH = { 10.0f,31.0f };	//�ő�X�g���b�`��
	Vec2<float>stretch_LU = { 0,0 };	//����
	Vec2<float>stretch_RB = { 0,0 };	//�E��
	Vec2<float>fromStretch_LU = { 0,0 };	//�C�[�W���O�p�X�^�[�g�l
	Vec2<float>fromStretch_RB = { 0,0 };	//�C�[�W���O�p�X�^�[�g�l
	const int STRETCH_RETURN_TIME = 17;	//�X�g���b�`���O�ɂȂ�܂ł̃t���[����
	int stretchTimer = STRETCH_RETURN_TIME;
	const float STRETCH_DIV_RATE = 2.0f;	//�X�g���b�`���キ����Ƃ��̊���
	const int FIRST_SHOT_RECOIL_PARTICLE_TIMER = 10.0f;

	//�v���C���[�̌���
	//enum DRAW_DIR { FRONT, BACK, DIR_NUM, DEFAULT = FRONT }playerDir = DEFAULT;
	//�A�j���[�V��������
	PlayerAnimation anim;
	static const int TIRED_DRAW_TIME = 90;
	int tiredTimer = TIRED_DRAW_TIME;

	//�c��
	AfterImg afImg;

	int shotSE;

	Vec2<float>scale;

	const int controllerIdx;
	int bulletGraph;

	int inputInvalidTimerByCrash;	// �����蔻�薳�����^�C�}�[
	const int INPUT_INVALID_TIMER = 30;

	Vec2<float> swingVec;	// �U��񂵃x�N�g�� �ړ������̋t�����ɏo��B

	bool isPrevLeftBottom;
	bool isInputRightStick;
	bool isPrevInputRightStick;
	Vec2<float> prevInputRightStick;

	//�_�b�V�����c�����o�����߂̕ϐ�
	int dashAftImgTimer;

	//���O�̒ʏ�ړ��ʁi�I�[�g�p�C���b�g�p�j
	Vec2<float>autoPilotMove;

public:

	/*-- �萔 --*/

	float RECOIL_AMOUNT = 15.0f;			// �e���������ۂ̔���
	float FIRST_RECOIL_AMOUNT = 15.0;		// �e���������ۂ̔���
	float MAX_RECOIL_AMOUNT = 15.0f;		// �e���������ۂ̔����̍ő�l
	static Vec2<float>GetGeneratePos();

	// �U��񂵂̃N�[���^�C��
	int swingCoolTime;
	const int SWING_COOLTIME = 60;

	// �e�Ɋւ���萔
	const int BULLET_SHOT_COUNT = 5;
	const float BULLET_SHOT_ANGLE = 0.1f;

	// �v���C���[�̕���
	enum PLAYER_DIR_X {
		PLAYER_LEFT,
		PLAYER_RIGHT
	}playerDirX;
	enum PLAYER_DIR_Y {
		PLAYER_FRONT,
		PLAYER_BACK
	}playerDirY;

	float sizeVel;

	//�`���[�g���A���A�C�R��
	Tutorial tutorial;

	//�p�C���b�g�̈ړ����x
	Vec2<float>pilotVel;
	Vec2<float>pilotAccel;
	int playerPilotGraph[2];	//FRONT,BACK

	// �W���X�g�U��񂵃L�����Z���_�b�V���p�̓����蔻������B
	Vec2<float> justCancelDashStartPos;
	Vec2<float> justCancelDashEndPos;


public:

	/*-- �����o�֐� --*/

	// �R���X�g���N�^
	Player(const PLAYABLE_CHARACTER_NAME& CharacterName, const WHICH_TEAM& Team);
	~Player();

private:
	// ����������
	void OnInit()override;

	// �X�V����
	void OnUpdate(const vector<vector<int>>& MapData)override;

	//�X�E�B���O�����Ăяo�����X�V����
	void OnUpdateNoRelatedSwing()override;

	// �`�揈��
	void OnDraw()override;
	void OnDrawUI()override;

	//�}�b�v�`�b�v�ƃq�b�g�����Ƃ�
	void OnHitMapChip(const HIT_DIR& Dir)override;

	void OnBreak()override { isHold = false; }
	void OnBreakFinish()override { anim.ChangeAnim(DEFAULT_FRONT); }
	void OnSwinged()override
	{
		anim.ChangeAnim(SWINGED);
	}
	void OnSwingedFinish()override
	{
		if (!GetNowBreak() && !inputInvalidTimerByCrash)
		{
			anim.ChangeAnim(DEFAULT_FRONT);
		}
	}
	void OnCrash()override
	{
		// ���͎�t�������^�C�}�[���Z�b�e�B���O����B
		inputInvalidTimerByCrash = INPUT_INVALID_TIMER;
		anim.ChangeAnim(SWINGED);
		UsersInput::Instance()->ShakeController(controllerIdx, 0.8f, 13);
	}
	void OnPartnerCrash()override
	{
		UsersInput::Instance()->ShakeController(controllerIdx, 0.8f, 13);
	}
	void OnPilotLeave()override	//�p�C���b�g�����{���痣�ꂽ�u��
	{
		pilotVel = { 0,0 };
		anim.ChangeAnim(NON_PILOT);
		tutorial.SetPilotLeave(true);
	}
	void OnPilotControl()override;		//�p�C���b�g�𓮂�������
	void OnPilotReturn()override	//�p�C���b�g�����{�ɖ߂����u��
	{
		if (anim.GetNowAnim() != TIRED)anim.ChangeAnim(DEFAULT_FRONT);
		tutorial.SetPilotLeave(false);
	}

	bool drawCursorFlag;

private:
	/*-- �N���X���Ŏg�p����֐� --*/

	// ���͏���
	void Input(const vector<vector<int>>& MapData);

	// �ړ�����
	void Move();

	//�e�̔���
	void Shot(const Vec2<float>& GeneratePos, const float& ForwardAngle);

	// �ǂƂ̉����߂��Ɋւ���X�V����
	void PushBackWall();

	bool stopInputFlag;//���͂��֎~����t���O
	bool stopMoveFlag;

	//�X�g���b�`�̒l���v�Z
	void CalculateStretch(const Vec2<float>& Move);
	//�X�g���b�`�l�X�V
	void UpdateStretch();
	//�摜�T�C�Y����v���C���[�T�C�Y�擾
	Vec2<float> GetPlayerGraphSize();

	// �ړ��ʂł̓����蔻��
	void CheckHitMapChipVel(const Vec2<float>& checkPos, const vector<vector<int>>& mapData);

public:
	void OnKnockOut()override { anim.ChangeAnim(KNOCK_OUT); }
};