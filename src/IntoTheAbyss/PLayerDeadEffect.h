#pragma once
#include "Vec.h"
#include "Singleton.h"

// ���S���̃G�t�F�N�g�p�[�e�B�N��
class PlayerDeadParticle{

public:

	/*===== �����o�ϐ� ====*/

	Vec2<float> pos;
	Vec2<float> fowardVec;
	float radius;
	float speed;


public:

	/*===== �萔 =====*/

	const float MAX_SPEED = 10.0f;
	const float DEF_RADIUS = 10.0f;


public:

	/*===== �����o�֐� =====*/

	// �R���X�g���N�^
	PlayerDeadParticle(){};

	// ��������
	void Generate(){};

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

	int blackOutTimer;			// �Ó]����^�C�}�[

	int slowTimer;				// ��̃^�C�}�[4�����킹���A���S�ɉ�肪�~�܂�܂ł̃^�C�}�[

	bool isActive;				// ���S�G�t�F�N�g���L��������Ă��邩


public:

	/*===== �萔 =====*/

	const int PLAYER_TURN_WHITE_TIMER = 60;		// �v���C���[�������Ȃ�܂ł̃^�C�}�[
	const int PLAYER_EXIT_TIMER = 60;			// ���S�ɔ����Ȃ����v���C���[��������܂ł̃^�C�}�[
	const int PARTICLE_STOP_TIMER = 60;			// �p�[�e�B�N�����~�܂�܂ł̃^�C�}�[
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