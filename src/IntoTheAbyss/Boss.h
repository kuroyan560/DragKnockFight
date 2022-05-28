#include "Vec.h"
#include <vector>
#include "Intersected.h"
#include"AreaCollider.h"
#include<array>
#include<memory>

#include"StagingInterFace.h"
using namespace std;

#include"CharacterInterFace.h"

#include"NavigationAI.h"
#include"CharacterAI.h"
#include"PlayerAnimation.h"
#include"BossHandMgr.h"

#include"BulletMgrBase.h"
#include"Barrages.h"

#include"StarParticleMgr.h"


// �v���C���[�ƈ������荇���{�X�N���X
class Boss : public CharacterInterFace
{
public:
	/*===== �����o�ϐ� =====*/
	Vec2<float> moveVel;		// �ړ���
	Vec2<float> swingInertiaVec;// �U��񂵂̊����̈ړ�����
	float swingInertia;			// �U��񂵂̊���
	int afterSwingDelay;		// �U��񂵂̂��ƂɃ{�X�����������Ȃ���Ԃɂ��邽�߂̃^�C�}�[
	INTERSECTED_LINE prevIntersectedLine;

	//�摜
	static const enum AnimHandle
	{
		//FRONT,
		//BACK,
		//DAMAGE,
		//SWING,
		DEFAULT,
		EXPLOSION_OPEN,
		EXPLOSION_CLOSE,
		ANIMAHANDLE_MAX
	};
	int afterImgageTimer;
	std::shared_ptr<PlayerAnimation> anim;

	//�{�X�̃p�^�[������-----------------------
	NavigationAI navigationAi;
	CharacterAI characterAi;
	bool initNaviAiFlag;
	//�{�X�̃p�^�[������-----------------------

	enum CrashMode
	{
		NONE_LEVEL = -1,
		FIRST_LEVEL,
		SECOND_LEVEL,
		THIRD_LEVEL
	};
	CrashMode bossCrashModel;
	int flashTimer;
	std::array<int, 3>flashMaxTimer;//�t���b�V���̊Ԋu
	std::array<int, 3>crashMaxNum;	//�i�K���Ƃłǂꂭ�炢�u���b�N���󂵂���؂�ւ�邩

	float bossGraphRadian;


	//�{�X�̃p�����[�^�[�ύX
	int bossImGuiHandle;
	int prevStaminaMax;

	float bossCount;
	float countDown;
	Vec2<float>shakeDir;
	Sprite bossGraph;
	bool initShakeFalg;
	Vec2<float>bossScale;

	Vec2<float>pointPos;
	float angle;

	int appearBossGraphHandle;


	int crashNumber;

	StarParticleMgr starParticleMgr;

public:
	/*===== �萔 =====*/

	//const Vec2<float> SCALE = { 80.0f,80.0f };
	const float OFFSET_VEL = 2.0f;
	const float OFFSET_INERTIA = 30.0f;		// �U��񂵂̊���
	const int AFTER_SWING_DELAY = 15;
	const int STRUCK_WINDOW_TIMER = 120.0f;

	const float SWING_DISTANCE_DEADLINE = 200.0f;	//�v���C���[��U��񂷋����̃f�b�g���C��

public:
	/*===== �����o�֐� =====*/

	// �R���X�g���N�^
	Boss();

private:
	// ����������
	void OnInit()override;

	// �X�V����
	void OnUpdate(const std::vector<std::vector<int>> &MapData)override;

	//�X�E�B���O�����Ăяo�����X�V����
	void OnUpdateNoRelatedSwing()override {}

	// �`�揈��
	void OnDraw(const bool &isRoundStartEffect)override;
	void OnDrawUI()override {}

	void OnHitMapChip(const HIT_DIR &Dir)override {}

	void OnBreak()override {}
	void OnBreakFinish()override {}
	void OnSwinged()override {}
	void OnSwingedFinish()override {}
	void OnCrash()override {}
	void OnPartnerCrash()override {}
	virtual void OnPilotLeave()override {}	//�p�C���b�g�����{���痣�ꂽ�u��
	virtual void OnPilotControl()override {}		//�p�C���b�g�𓮂�������
	virtual void OnPilotReturn()override {}	//�p�C���b�g�����{�ɖ߂����u��

	void OnStaminaHeal(const int &HealAmount)override
	{
		CharacterAIData::Instance()->addTimer = HealAmount;
	};

	void Shot(const Vec2<float> &generatePos, const float &forwardAngle, const float &speed);

public:
	void OnKnockOut()override {};
};