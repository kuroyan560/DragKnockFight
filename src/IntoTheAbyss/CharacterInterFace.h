#pragma once
#include"Vec.h"
#include<memory>
#include<vector>
#include"StagingInterFace.h"
#include"AreaCollider.h"
#include"../IntoTheAbyss/BulletCollision.h"
#include<array>
#include "Intersected.h"
#include"BulletMgrBase.h"

static const enum PLAYABLE_CHARACTER_NAME { PLAYABLE_LUNA, PLAYABLE_LACY, PLAYABLE_BOSS_0, PLAYABLE_CHARACTER_NUM, PLAYER_CHARACTER_NUM = PLAYABLE_LACY + 1 };
static const enum WHICH_TEAM { LEFT_TEAM, RIGHT_TEAM, TEAM_NUM };

class CharacterInterFace
{
private:
	void SwingUpdate();
	void Crash(const Vec2<float>& MyVec);
	void CrashUpdate();

	//��ʋ��݃N���b�V������
	const int STACK_WINDOW_TIMER = 120.0f;// �E�B���h�E�ɋ��܂����^�C�}�[
	int stackWindowTimer;		// �E�B���h�E�ɋ��܂����^�C�}�[

	//�w�n�Ƃ̔���
	Square areaHitBox;

	//�e�Ƃ̓����蔻��
	SphereCollision bulletHitSphere;

	const int AFTER_SWING_DELAY = 15;
	Vec2<float> swingInertiaVec;// �U��񂵂̊����̈ړ�����
	float swingInertia;			// �U��񂵂̊���
	int afterSwingDelay;		// �U��񂵂̂��ƂɃ{�X�����������Ȃ���Ԃɂ��邽�߂̃^�C�}�[

	//���o�Ȃǂ̓����̊֌W�œ������~�߂�
	bool canMove;
	//���o�Ȃǂ̊֌W�œ����蔻����Ƃ�Ȃ��Ȃ�
	bool hitCheck;

	//�����E��
	WHICH_TEAM team;

	//�X�^���p�^�C�}�[
	int stanTimer;
	//�_���[�W�p�^�C�}�[
	int damageTimer;

protected:
	BulletMgrBase bulletMgr;
	bool nowSwing;
	Vec2<float> nowSwingVec;		// ���݂̊p�x
	Vec2<float> swingTargetVec;		// �ڕW�n�_
	float addSwingAngle;			// �U��񂵂ŉ�]������� ���񂾂񑝂���B
	bool isSwingClockWise;			// ���̐U��񂵂����v��肩�ǂ��� true...���v���...�E��]  false...�����v���...����]
	const float ADD_SWING_ANGLE = 0.002f;
	const float MAX_SWING_ANGLE = 0.07f;


protected:
	static const enum HIT_DIR { LEFT, RIGHT, TOP, BOTTOM, HIT_DIR_NUM };
	std::array<int, HIT_DIR_NUM>mapChipHit;
	bool stackMapChip;

	//�����J�n���ɌĂяo�����
	CharacterInterFace(const Vec2<float>& HonraiSize) : size(HonraiSize)
	{
		areaHitBox.center = &pos;
		areaHitBox.size = size;
		bulletHitSphere.center = &pos;
		bulletHitSphere.radius = size.x;
	}

	std::weak_ptr<CharacterInterFace>partner;
	StagingInterFace stagingDevice;
	Vec2<float>size;	//�T�C�Y

	//[�L�������ƂɈႤ�֐�]
	virtual void OnInit() = 0;
	virtual void OnUpdate(const std::vector<std::vector<int>>& MapData) = 0;
	virtual void OnUpdateNoRelatedSwing() = 0;	//�X�E�B���O���ł��ʂ鏈��
	virtual void OnDraw() = 0;
	virtual void OnDrawUI() = 0;
	virtual void OnHitMapChip(const HIT_DIR& Dir) = 0;

	//[���ʊ֐�]
	//�U���
	void SwingPartner(const Vec2<float>& SwingTargetVec);
	//�Q�b�^��
	const Vec2<float>& GetPartnerPos()
	{
		return partner.lock()->pos;
	}
	//�U��񂵂ɂ�銵��
	Vec2<float>GetSwingInertia() { return swingInertiaVec * swingInertia; }
	//�U��񂵒���̍d������
	bool GetSwingRigor() { return 0 < afterSwingDelay; }
	//���`�[�����E�`�[����
	const WHICH_TEAM& GetWhichTeam() { return team; }

	// �����蔻����ۑ��B
	void SaveHitInfo(bool& isHitTop, bool& isHitBottom, bool& isHitLeft, bool& isHitRight, const INTERSECTED_LINE& intersectedLine);

public:
	//�o�ꉉ�o������������
	virtual bool Appear() = 0;

public:
	static const int LINE_LENGTH = 150;
	static const int ADD_LINE_LENGTH_VEL = 100;	// �ړ��ʂɉ����ĐL�т�addLineLength�̍ő��
	float MOVE_SPEED_PLAYER = 15.0f;			// �ړ����x
	float addLineLength;	//�R
	Vec2<float> pos;			// ���W
	Vec2<float>vel;
	Vec2<float> prevPos;		// �O�t���[���̍��W
	bool isHold;				// ����ł��邩�t���O
	bool isGripPowerEmpty;		// ���̓^�C�}�[���g���؂��Ă���񕜂���܂ł𔻒f���邽�߂̃t���O
	int gripPowerTimer;			// ���̓^�C�}�[
	const int MAX_GRIP_POWER_TIMER = 180;

	void RegisterCharacterInfo(const std::shared_ptr<CharacterInterFace>Partner, const WHICH_TEAM& Team)
	{
		partner = Partner;
		team = Team;
	}
	void Init(const Vec2<float>& GeneratePos);	//���E���h�J�n���ɌĂяo�����
	void Update(const std::vector<std::vector<int>>& MapData, const Vec2<float>& LineCenterPos);
	void Draw();
	void DrawUI();
	//�����蔻��
	void CheckHit(const std::vector<std::vector<int>>& MapData, const Vec2<float>& LineCenterPos);

	//�X�^��
	void Break();
	//�_���[�W
	void Damage();

	const Square& GetAreaHitBox() { return areaHitBox; }
	const SphereCollision& GetBulletHitSphere() { return bulletHitSphere; }

	const int& GetStackWinTimer() { return stackWindowTimer; }
	const bool& GetNowSwing() { return nowSwing; }
	const bool& GetNowStuckWin() { return 0 < stackWindowTimer; }
	const bool& GetCanMove() { return canMove && !stanTimer; }
	const bool& GetStackFlag() { return stackMapChip; }
	const bool& GetNowBreak() { return stanTimer; }
	BulletMgrBase& GetBulletMgr() { return bulletMgr; }

	void SetCanMove(const bool& Flg) { canMove = Flg; }
	void SetHitCheck(const bool& Flg) { hitCheck = Flg; }

	inline void FinishSwing() { nowSwing = false; }
};

