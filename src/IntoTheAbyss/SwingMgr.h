#include "Vec.h"
#include "Singleton.h"

// �U��񂵂̏����̍ۂɃv���C���[�ƃ{�X�̋��n���ƂȂ�N���X
class SwingMgr : public Singleton<SwingMgr> {

public:

	/*===== �����o�ϐ� =====*/

	bool isSwingPlayer;				// �v���C���[���U��񂵏�Ԃ��ǂ����̃t���O
	bool isSwingBoss;				// �{�X���U��񂵏�Ԃ��ǂ����̃t���O
	float easingTimer;				// �U��񂵂Ɏg�p����C�[�W���O�^�C�}�[
	float easeAmount;				// �^�C�}�[���狁�߂���C�[�W���O��
	float easeChangeAmountY;		// Y���̕ω���
	float lineLength;				// �R�̒����̑���
	Vec2<float> easingStartVec;		// �U��񂵃C�[�W���O�̊J�n�n�_
	Vec2<float> easingEndVec;		// �U��񂵃C�[�W���O�̏I���n�_
	Vec2<float> easingNowVec;		// �U��񂵃C�[�W���O�̍��̒l
	Vec2<float> playerPos;			// �v���C���[���W
	Vec2<float> bossPos;			// �{�X���W


public:

	/*===== �萔 =====*/

	const float ADD_EASINGTIMER = 0.05f;	// �C�[�W���O�^�C�}�[�ɉ��Z�����


public:

	/*===== �����o�֐� =====*/

	// �R���X�g���N�^
	SwingMgr();

	// ����������
	void Init();

	// �X�V����
	void Update(const Vec2<float>& playerPos, const Vec2<float>& bossPos, const float& lineLength);

	void PlaySE();
};