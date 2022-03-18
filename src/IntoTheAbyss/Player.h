#pragma once
#include "Vec.h"
#include "TimeStopTestBlock.h"
#include <memory>
#include <vector>

using namespace std;

// �v���C���[�̘r�̑O���錾
class PlayerHand;

// �v���C���[�N���X
class Player {

public:

	/*-- �����o�ϐ� --*/

	Vec2<float> centerPos;					// �v���C���[�̒��S���W
	Vec2<float> prevFrameCenterPos;		// �O�t���[���̃v���C���[�̒��S���W
	Vec2<float> vel;						// �ړ���
	float gravity;					// �d��
	bool onGround;					// �v���C���[���ڒn���Ă��邩�̃t���O
	bool firstShot;					// �ŏ��̈ꔭ�������ꂽ���ǂ��� true�Ō����ꂽ����
	bool isWallRight;				// �E�̕ǂɂ������Ă��邩
	bool isWallLeft;				// ���̕ǂɂ������Ă��邩
	int rapidFireTimerLeft;			// �A�˃^�C�}�[����
	int rapidFireTimerRight;		// �A�˃^�C�}�[�E��
	int gravityInvalidTimer;		// �d�͖������^�C�}�[

	// �ǃY���t���O
	bool isSlippingWall[4];			// �ǃY���p�[�e�B�N�����o���t���O

	// �v���C���[�̘r
	unique_ptr<PlayerHand> rHand;	// �E��
	unique_ptr<PlayerHand> lHand;	// ����
	bool isPrevFrameShotBeacon;

	int playerGraph;


public:

	/*-- �萔 --*/

	const float ADD_GRAVITY = 0.5f;				// �v���C���[�ɂ�����d��
	const float MAX_GRAVITY = 15.0f;			// �v���C���[�ɂ�����d�͂̍ő��
	const float RECOIL_AMOUNT = 5.0f;			// �e���������ۂ̔���
	const float FIRST_RECOIL_AMOUNT = 20.0;		// �e���������ۂ̔���
	const float MAX_RECOIL_AMOUNT = 30.0f;		// �e���������ۂ̔����̍ő�l
	//const Vec2<float> PLAYER_SIZE = { 20,20 };			// �v���C���[�̃T�C�Y
	const Vec2<float> PLAYER_SIZE = { 56 / 3.0f,144 / 3.0f };			// �v���C���[�̃T�C�Y
	static Vec2<float>GetGeneratePos();
	const int RAPID_FIRE_TIMER = 21;			// �A�˃^�C�}�[
	const int GRAVITY_INVALID_TIMER = 20;		// �d�͖������^�C�}�[

	// �R���[�e�I�Ȃ�̂��߂̃p�����[�^�[
	const float STOP_DEADLINE_Y = 5.0f;		// Y���̈ړ��ʂ��R���ȉ���������ړ��ʂ�0�ɂ���B
	const float STOP_DEADLINE_X = 3.0f;		// X���̈ړ��ʂ��R���ȉ���������ړ��ʂ�0�ɂ���B

	// �v���C���[�̕���
	enum PLAYER_DIR {

		PLAYER_LEFT,
		PLAYER_RIGHT,
		PLAYER_TOP,
		PLAYER_BOTTOM,

	};


public:

	/*-- �����o�֐� --*/

	// �R���X�g���N�^
	Player();
	~Player();

	// ����������
	void Init(const Vec2<float>& INIT_POS);

	// �X�V����
	void Update(const vector<vector<int>> mapData);

	// �`�揈��
	void Draw();

	// �}�b�v�`�b�v�Ƃ̓����蔻��
	void CheckHit(const vector<vector<int>> mapData, TimeStopTestBlock& testBlock);


	// �������Ƃ̃}�b�v�`�b�v�Ƃ̓����蔻��֐�
	void HitMapChipTop();
	void HitMapChipLeft();
	void HitMapChipRight();
	void HitMapChipBottom();


private:

	/*-- �N���X���Ŏg�p����֐� --*/

	// ���͏���
	void Input(const vector<vector<int>> mapData);

	// �ړ�����
	void Move();

	// �d�͂Ɋւ���X�V����
	void UpdateGravity();

	// �ǂƂ̉����߂��Ɋւ���X�V����
	void PushBackWall();

};