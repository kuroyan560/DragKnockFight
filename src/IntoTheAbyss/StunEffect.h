#include "Vec.h"
#include "Singleton.h"

// �X�^�������ۂ̉��o�N���X
class StunEffect : public Singleton<StunEffect> {

private:

	/*===== �����o�ϐ� =====*/

	Vec2<float>* focusPos;	// �t�H�[�J�X����Ώۂ̍��W
	int focusTime;			// �t�H�[�J�X���鎞��

public:

	bool isActive;			// ���̉��o���L��������Ă��邩�̃t���O


private:

	/*===== �萔 =====*/

	const int FOCUS_TIME = 120;
	const float FOCUS_ZOOM = 1.7f;


public:

	/*===== �����o�֐� =====*/

	// �R���X�g���N�^
	StunEffect();

	// ����������
	void Init();

	// ��������
	void Activate(Vec2<float>& focusPos);

	// �X�V����
	void Update();

};