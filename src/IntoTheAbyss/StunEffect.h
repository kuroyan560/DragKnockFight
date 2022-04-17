#include "Vec.h"
#include "Singleton.h"
#include"CharacterInterFace.h"

// �X�^�������ۂ̉��o�N���X
class StunEffect : public Singleton<StunEffect> {

private:

	/*===== �����o�ϐ� =====*/

	const Vec2<float>* focusPos;		// �t�H�[�J�X����Ώۂ̍��W
	int focusTime;				// �t�H�[�J�X���鎞��

	// �ȉ�BREAK�摜�p�̃����o�ϐ�
	Vec2<float> breakUIPos;		// [BREAK]�摜��`�悷����W
	Vec2<float> iconPos;		// �A�C�R���̍��W
	Vec2<float> breakUIPosDef;	// [BREAK]�摜��`�悷����W�̃f�t�H���g�̒l
	float breakUIRadian;		// [BREAK]�摜�̃��W�A��
	float breakUIExtRate;		// [BREAK]�摜�̊g�嗦
	int breakUITimer;			// [BREAK]�摜�̊e�����Ɏg�p����^�C�}�[
	int enemyBreakHandle;		// �G��[BREAK]�̉摜�n���h��
	int playerBreakHandle;		// �v���C���[��[BRWAK]�̉摜�n���h��
	int useHandle;				// ���ۂɕ`�悷��摜�n���h��
	WHICH_TEAM breakTeam;

	enum BREAK_UI_STATUS {
		STATUS_GENERATE,		// �������Ē����Ɍ������V�[��
		STATUS_STOP,			// �����ɂ��Ď~�܂��Ă���V�[��
		STATUS_ICON,			// ���S����A�C�R���Ɍ������Ă����V�[��
		STATUS_STUN_END
	};
	BREAK_UI_STATUS status;


public:

	bool isActive;				// ���̉��o���L��������Ă��邩�̃t���O


private:

	/*===== �萔 =====*/

	static const int BREAK_UI_GENERATE_TIMER = 50;
	static const int BREAK_UI_STOP_TIMER = 60;
	static const int BREAK_UI_ICON_TIMER = 30;
	const int FOCUS_TIME = 120;
	const float FOCUS_ZOOM = 1.7f;
	const float DEF_RADIAN = 3600.0f;
	const float EXT_RATE = 2.0f;				// �摜�̑傫��
	const float ADD_EXT_RATE = 0.5f;			// �A�C�R���Ɍ������ۂɂ�����Ƃ����g�傷���


public:
	//���o�ɂ����鎞��
	static const int GetTotalTime()
	{
		return BREAK_UI_GENERATE_TIMER + BREAK_UI_STOP_TIMER + BREAK_UI_ICON_TIMER;
	}

	/*===== �����o�֐� =====*/

	// �R���X�g���N�^
	StunEffect();

	// ����������
	void Init();

	// ��������
	void Activate(const Vec2<float>& focusPos, const Vec2<float>& iconPos, const WHICH_TEAM& Team);

	// �X�V����
	void Update();

	// �`�揈��
	void Draw();

};