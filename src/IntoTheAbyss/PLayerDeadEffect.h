#pragma once
#include "Vec.h"
#include "Singleton.h"
#include <array>

// ���S���̃G�t�F�N�g�p�[�e�B�N��
class PlayerDeadParticle {

public:

	/*===== �����o�ϐ� ====*/

	Vec2<float> pos;
	Vec2<float> fowardVec;
	float angle;
	float radius;
	float length;
	bool isActive;


public:

	/*===== �萔 =====*/

	const float MAX_SPEED = 100.0f;
	const float DEF_RADIUS = 15.0f;


public:

	/*===== �����o�֐� =====*/

	// �R���X�g���N�^
	PlayerDeadParticle();

	// ����������
	void Init();

	// ��������
	void Generate(const Vec2<float>& generatePos, const Vec2<float>& forwardVec);

	// �X�V����
	void Update();

	// �`�揈��
	void Draw();

};

// �v���C���[���S�G�t�F�N�g�N���X
class PlayerDeadEffect : public Singleton<PlayerDeadEffect> {

public:

	/*===== �����o�ϐ� =====*/

	Vec2<float> playerPos;		// �v���C���[�̍��W

	int playerTurnWhiteTimer;	// �v���C���[�𔒂�����܂ł̃^�C�}�[
	float playerWhitePar;		// �v���C���[�𔒂�����p�[�Z���e�[�W �^�C�}�[�����Ɍv�Z����B

	int playerExitDelayTimer;	// �v���C���[�������Ȃ��Ă��������܂ł̃^�C�}�[
	bool isExitPlayer;			// �v���C���[�̕`���؂邩

	int particleStopTimer;		// �p�[�e�B�N�����~�܂�܂ł̃^�C�}�[
	float particleSpeedPar;		// �p�[�e�B�N���̈ړ����x�̊���

	int blackOutDelayTimer;		// �Ó]����܂ł̒x���̃^�C�}�[

	int blackOutTimer;			// �Ó]����^�C�}�[
	bool isInitStage;			// �X�e�[�W������������t���O

	int slowTimer;				// ��̃^�C�}�[4�����킹���A���S�ɉ�肪�~�܂�܂ł̃^�C�}�[

	bool isActive;				// ���S�G�t�F�N�g���L��������Ă��邩

	static const int PARTICLE_COUNT = 4;
	std::array<PlayerDeadParticle, PARTICLE_COUNT> deadParticle;


public:

	/*===== �萔 =====*/

	const int PLAYER_TURN_WHITE_TIMER = 60;		// �v���C���[�������Ȃ�܂ł̃^�C�}�[
	const int PLAYER_EXIT_TIMER = 30;			// ���S�ɔ����Ȃ����v���C���[��������܂ł̃^�C�}�[
	const int PARTICLE_STOP_TIMER = 10;			// �p�[�e�B�N�����~�܂�܂ł̃^�C�}�[
	const int BLACK_OUT_DELAY_TIMER = 30;		// �Ó]����܂ł̒x���̃^�C�}�[
	const int BLACK_OUT_TIMER = 60;				// �Ó]����
	const int SLOW_TIMER = PLAYER_TURN_WHITE_TIMER + PLAYER_EXIT_TIMER + PARTICLE_STOP_TIMER;	// ���肪���S�Ɏ~�܂�܂ł̃^�C�}�[


public:

	/*===== �����o�֐� =====*/

	// �R���X�g���N�^
	PlayerDeadEffect();

	// ����������
	void Init();

	// �L��������
	void Activate(Vec2<float>& playerPos);

	// �X�V����
	void Update();

	// �`�揈��
	void Draw();

};