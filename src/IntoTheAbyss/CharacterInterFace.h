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
#include"SwingLineSegmentMgr.h"
#include"TexHandleMgr.h"
#include"CharacterInfo.h"

class StaminaMgr;

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

	int swingTimer;				// �U��񂵂̌o�ߎ���

	//���o�Ȃǂ̓����̊֌W�œ������~�߂�
	bool canMove;
	//���o�Ȃǂ̊֌W�œ����蔻����Ƃ�Ȃ��Ȃ�
	bool hitCheck;

	//�����E��
	WHICH_TEAM team;

	PLAYABLE_CHARACTER_NAME characterName;

	//�X�^���p�^�C�}�[
	int stanTimer;
	//�_���[�W�p�^�C�}�[
	int damageTimer;

	//�p�C���b�g�؂藣��
	bool isPilotDetached = false;	// �p�C���b�g�؂藣�������̃t���O
	Vec2<float>pilotReturnStartPos;	//�p�C���b�g�����{�ɖ߂钼�O�̍��W
	int pilotReturnTimer;	//�p�C���b�g���߂鏈���̎��Ԍv���^�C�}�[
	int pilotReturnTotalTime;	//�p�C���b�g�����{�ɖ߂�܂ł̎���

	// �D���Q�[�W���f�t�H���g�ɖ߂�܂ł̃^�C�}�[
	int gaugeReturnTimer;		// �N���b�V���Ȃǂ����ۂɐ��l���������A0�̎��Ƀf�t�H���g�̃Q�[�W�ʂɖ߂�B
	const int GAUGE_RETURN_TIMER = 60;

	//�o�ꉉ�o
	bool initPaticleFlag;
	int moveTimer;


protected:
	BulletMgrBase bulletMgr;
	bool nowSwing;
	bool isAdvancedEntrySwing;		// �U��񂵐�s����
	int advancedEntrySwingTimer;	// �U��񂵂̐�s���͂��󂯕t���Ă���A�����������܂ł̃^�C�}�[
	const int ADVANCED_ENTRY_SWING_TIMER = 30;
	Vec2<float> nowSwingVec;		// ���݂̊p�x
	Vec2<float> swingTargetVec;		// �ڕW�n�_
	float addSwingAngle;			// �U��񂵂ŉ�]������� ���񂾂񑝂���B
	bool isSwingClockWise;			// ���̐U��񂵂����v��肩�ǂ��� true...���v���...�E��]  false...�����v���...����]
	float addSwingRate;				// �U��񂵑��x�ɉ��Z����� �߂����ɉ�]���x��������������B
	const float ADD_SWING_ANGLE = 0.02f;
	const float MAX_SWING_ANGLE = 0.13f;

	//�U��񂵉����p�����N���X
	SwingLineSegmentMgr CWSwingSegmentMgr;	// ���v���
	SwingLineSegmentMgr CCWSwingSegmentMgr;	// �����v���
	bool isInputSwingRB;					// RB�ŃX�C���O�������ǂ��� false��������LB
	int rbHandle;
	int lbHandle;
	int lineHandle;
	int arrowHandle;
	int reticleHandle;

	//�p�C���b�g�����{���痣�����ő勗��
	const float PILOT_RANGE = 300.0f;
	Vec2<float>pilotPos;	// �p�C���b�g�̍��W
	Vec2<float>pilotSize = { 64.0f,64.0f };	//�p�C���b�g�̃T�C�Y
	int pilotGraph = -1;
	bool pilotDrawMiror = false;

protected:
	static const enum HIT_DIR { LEFT, RIGHT, TOP, BOTTOM, HIT_DIR_NUM };
	std::array<int, HIT_DIR_NUM>mapChipHit;
	bool stackMapChip;

	//�����J�n���ɌĂяo�����
	CharacterInterFace(const Vec2<float>& HonraiSize);


	std::weak_ptr<CharacterInterFace>partner;
	StagingInterFace stagingDevice;
	static const int INIT_SIZE = 5;
	Vec2<float>size;	//�T�C�Y
	Vec2<float>appearExtRate;	//�o�ꉉ�o

	//[�L�������ƂɈႤ�֐�]
	virtual void OnInit() = 0;
	virtual void OnUpdate(const std::vector<std::vector<int>>& MapData) = 0;
	virtual void OnUpdateNoRelatedSwing() = 0;	//�X�E�B���O���ł��ʂ鏈��
	virtual void OnDraw() = 0;
	virtual void OnDrawUI() = 0;
	virtual void OnHitMapChip(const HIT_DIR& Dir) = 0;
	virtual void OnBreak() = 0;
	virtual void OnBreakFinish() = 0;
	virtual void OnSwinged() = 0;
	virtual void OnSwingedFinish() = 0;
	virtual void OnCrash() = 0;	//�h�����h���N���b�V�������u��
	virtual void OnPartnerCrash() = 0;	//�h����h���N���b�V�������u��
	virtual void OnPilotLeave() = 0;	//�p�C���b�g�����{���痣�ꂽ�u��
	virtual void OnPilotControl() = 0;		//�p�C���b�g�𓮂�������
	virtual void OnPilotReturn() = 0;	//�p�C���b�g�����{�ɖ߂����u��

	//[���ʊ֐�]
	//�U���
	void SwingPartner(const Vec2<float>& SwingTargetVec, const bool& IsClockWise);
	//�p�C���b�g�؂藣��
	void SetPilotDetachedFlg(const bool& Flg);

	//�Q�b�^��
	const Vec2<float>& GetPartnerPos()
	{
		return partner.lock()->pos;
	}
	//���`�[�����E�`�[����
	const WHICH_TEAM& GetWhichTeam() { return team; }
	//�p�C���b�g�����{�̊O�ɂ��邩
	bool IsPilotOutSide() { return isPilotDetached || pilotReturnTimer < pilotReturnTotalTime; }

	// �����蔻����ۑ��B
	void SaveHitInfo(bool& isHitTop, bool& isHitBottom, bool& isHitLeft, bool& isHitRight, const INTERSECTED_LINE& intersectedLine);

public:
	//�o�ꉉ�o������������
	void Appear();
	bool CompleteAppear() { return 20 <= moveTimer; }

	// �O�t���[���̍��W��ۑ��B
	inline void SavePrevFramePos() {
		prevPrevPos = prevPos;
		prevPos = pos;
	}

	// �U��񂵂̗\�����������B
	void InitSwingLineSegmetn();

	// �ړ��ʂ��������B
	inline void InitVel() { vel = {}; }

public:
	static const int LINE_LENGTH = 150;
	static const int ADD_LINE_LENGTH_VEL = 100;	// �ړ��ʂɉ����ĐL�т�addLineLength�̍ő��
	float MOVE_SPEED_PLAYER = 15.0f;			// �ړ����x
	float addLineLength;	//�R
	Vec2<float> pos;			// ���W
	Vec2<float>vel;
	Vec2<float> prevPos;		// �O�t���[���̍��W
	Vec2<float> prevPrevPos;		// �O�̃t���[���̑O�t���[���̍��W ���O���������Đ\����Ȃ��c
	bool isHold;				// ����ł��邩�t���O
	std::shared_ptr<StaminaMgr> staminaGauge;	// �X�^�~�i�Q�[�W�N���X
	const int SWING_STAMINA = 2;	// �U��񂵎��̏���X�^�~�i
	const int DASH_STAMINA = 1;		// �_�b�V�����̏���X�^�~�i

	void RegisterCharacterInfo(const std::shared_ptr<CharacterInterFace>Partner, const WHICH_TEAM& Team, const PLAYABLE_CHARACTER_NAME& Name)
	{
		partner = Partner;
		team = Team;
		characterName = Name;
	}
	void Init(const Vec2<float>& GeneratePos, const bool& Appear);	//���E���h�J�n���ɌĂяo�����
	void Update(const std::vector<std::vector<int>>& MapData, const Vec2<float>& LineCenterPos);
	void Draw();
	void DrawUI();
	//�����蔻��
	void CheckHit(const std::vector<std::vector<int>>& MapData, const Vec2<float>& LineCenterPos);
	void CheckHitStuck(const std::vector<std::vector<int>>& MapData);

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

	//�p�C���b�g�̍��W�i�؂藣�����ĂȂ��Ƃ���nullptr��Ԃ��j
	const Vec2<float>* GetPilotPosPtr()
	{
		if (!isPilotDetached)return nullptr;
		return &pilotPos;
	}

	void SetCanMove(const bool& Flg) { canMove = Flg; }
	void SetHitCheck(const bool& Flg) { hitCheck = Flg; }

	void FinishSwing();

	const PLAYABLE_CHARACTER_NAME& GetCharacterName() { return characterName; }
	const Color& GetTeamColor()
	{
		static const Color TEAM_COLOR[TEAM_NUM] =
		{
			Color(47,255,139,255),
			Color(239,1,144,255)
		};
		return TEAM_COLOR[team];
	}

	//�m�b�N�A�E�g���ꂽ��
	virtual void OnKnockOut() = 0;

	/// <summary>
	/// ���v���ɕǂɓ����邩�ǂ���
	/// </summary>
	/// <returns></returns>
	bool ClockwiseHitsTheWall()
	{
		return CWSwingSegmentMgr.IsHitWall();
	}
	/// <summary>
	/// �����v���񂵂��ۂɓ����邩�ǂ���
	/// </summary>
	/// <returns></returns>
	bool CounterClockwiseHitsTheWall()
	{
		return CCWSwingSegmentMgr.IsHitWall();
	}
};

