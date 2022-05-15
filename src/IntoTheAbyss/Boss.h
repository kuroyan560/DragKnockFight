#include "Vec.h"
#include <vector>
#include "Intersected.h"
#include"AreaCollider.h"
#include"IBossPattern.h"
#include<array>
#include<memory>

#include"StagingInterFace.h"
using namespace std;

#include"CharacterInterFace.h"

#include"NavigationAI.h"
#include"CharacterAI.h"
#include"PlayerAnimation.h"

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
		FRONT,
		BACK,
		DAMAGE,
		ANIMAHANDLE_MAX
	};
	int afterImgageTimer;
	std::shared_ptr<PlayerAnimation> anim;

	//�{�X�̃p�^�[������-----------------------
	NavigationAI navigationAi;
	CharacterAI characterAi;
	//�{�X�̃p�^�[������-----------------------

	//�{�X�̃p�����[�^�[�ύX
	int bossImGuiHandle;

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
	void OnUpdate(const std::vector<std::vector<int>>& MapData)override;

	//�X�E�B���O�����Ăяo�����X�V����
	void OnUpdateNoRelatedSwing()override {}

	// �`�揈��
	void OnDraw()override;
	void OnDrawUI()override {}

	void OnHitMapChip(const HIT_DIR& Dir)override {}

	void OnBreak()override {}
	void OnBreakFinish()override {}
	void OnSwinged()override {}
	void OnSwingedFinish()override {}
	void OnCrash()override {}
	void OnPartnerCrash()override {}
	virtual void OnPilotLeave()override {}	//�p�C���b�g�����{���痣�ꂽ�u��
	virtual void OnPilotControl()override {}		//�p�C���b�g�𓮂�������
	virtual void OnPilotReturn()override {}	//�p�C���b�g�����{�ɖ߂����u��

	void Shot(const Vec2<float>& generatePos, const float& forwardAngle, const float& speed);
public:
	void OnKnockOut()override {};
};