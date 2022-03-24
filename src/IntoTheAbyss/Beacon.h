#pragma once
#include "Vec.h"

// �Z�����ʗp�񋓑�
enum PIKE_ID {

	PIKE_TELEPORT,		// �u�Ԉړ��̒Z��
	PIKE_TIMESTOP,		// ���Ԓ�~�̒Z��

};

// �r�[�R���N���X
class Beacon {

public:

	/*===== �����o�ϐ� =====*/

	Vec2<float> pos;				// ���W
	Vec2<float> prevFramePos;		// �O�t���[���̍��W
	Vec2<float> forwardVec;		// �ړ������x�N�g��
	Vec2<float> gimmickVel;
	Vec2<float> prevFrameScroll;	// �O�t���[���̃E�B���h�E�X�N���[���� �E�B���h�E�Ɏh�������Ƃ��ɃE�B���h�E�ɍ��킹�ē��������߂ɕK�v
	float* stopTargetPos;
	float stopPos;
	float speed;			// �ړ����x
	bool isActive;			// �L��������Ă��邩(���˂��ꂽ��)�t���O
	bool isHitWall;			// �ǂɓ������Ă��邩�̃t���O
	bool isHitWindow;		// �E�B���h�E�Ƀq�b�g�������ǂ����̃t���O

	// �Z�������ʂ��邽�߂�ID
	PIKE_ID id;


public:

	/*===== �萔 =====*/

	const float MAX_SPEED = 20.0f;	// �ړ����x
	const float SCALE = 10.0f;		// �傫��


public:

	/*===== �����o�֐� =====*/

	// �R���X�g���N�^
	Beacon();

	// ����������
	void Init();

	// ��������
	void Generate(const Vec2<float>& generatePos, const Vec2<float>& forwadVec, const PIKE_ID& id);

	// �X�V����
	void Update();

	// �`�揈��
	void Draw();

	// �Ăѓ������B
	void MoveAgain() {
		if (stopTargetPos != nullptr)*stopTargetPos = stopPos;
	}

};