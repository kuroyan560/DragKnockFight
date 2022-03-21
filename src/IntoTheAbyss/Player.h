#pragma once
#include "Vec.h"
#include "TimeStopTestBlock.h"
#include "Bubble.h"
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
	Vec2<float> gimmickVel;			// �M�~�b�N����^������ړ���(�h�b�X���u���b�N�ɒ���t���������B)
	float gravity;					// �d��
	bool onGround;					// �v���C���[���ڒn���Ă��邩�̃t���O
	bool firstShot;					// �ŏ��̈ꔭ�������ꂽ���ǂ��� true�Ō����ꂽ����
	bool isWallRight;				// �E�̕ǂɂ������Ă��邩
	bool isWallLeft;				// ���̕ǂɂ������Ă��邩
	bool inBubble;					// �V���{���ʂɓ����Ă��邩�ǂ���
	int rapidFireTimerLeft;			// �A�˃^�C�}�[����
	int rapidFireTimerRight;		// �A�˃^�C�}�[�E��
	int gravityInvalidTimer;		// �d�͖������^�C�}�[
	int handReturnTimer;			// ���͂��I����Ă���r���f�t�H���g�̈ʒu�ɖ߂�܂ł̃^�C�}�[
	int asSoonAsInputTimer;			// �ړ����͂��s���Ă��琔�t���[���ԗL�������鏈������邽�߂Ƀ^�C�}�[ ��ɃV���{����

	// �ǃY���t���O
	bool isSlippingWall[4];			// �ǃY���p�[�e�B�N�����o���t���O

	// �v���C���[�̘r
	unique_ptr<PlayerHand> lHand;	// ����
	unique_ptr<PlayerHand> rHand;	// �E��
	bool isPrevFrameShotBeacon;

	int playerGraph;

public:

	/*-- �萔 --*/

	const float ADD_GRAVITY = 0.5f;				// �v���C���[�ɂ�����d��
	const float MAX_GRAVITY = 15.0f;			// �v���C���[�ɂ�����d�͂̍ő��
	const float RECOIL_AMOUNT = 5.0f;			// �e���������ۂ̔���
	const float FIRST_RECOIL_AMOUNT = 20.0;		// �e���������ۂ̔���
	const float MAX_RECOIL_AMOUNT = 30.0f;		// �e���������ۂ̔����̍ő�l
	const float EXT_RATE = 0.6f;	//Player's expand rate used in Draw().
	const Vec2<float> PLAYER_SIZE = { (56 * EXT_RATE) / 2.0f,(144 * EXT_RATE) / 2.0f };			// �v���C���[�̃T�C�Y
	static Vec2<float>GetGeneratePos();
	const int RAPID_FIRE_TIMER = 21;			// �A�˃^�C�}�[
	const int GRAVITY_INVALID_TIMER = 20;		// �d�͖������^�C�}�[

	// �R���[�e�I�Ȃ�̂��߂̃p�����[�^�[
	const float STOP_DEADLINE_Y = 5.0f;		// Y���̈ړ��ʂ��R���ȉ���������ړ��ʂ�0�ɂ���B
	const float STOP_DEADLINE_X = 3.0f;		// X���̈ړ��ʂ��R���ȉ���������ړ��ʂ�0�ɂ���B

	// �肪�����ʒu�ɖ߂�܂ł̃^�C�}�[
	const int DEF_HAND_RETURN_TIMER = 180;

	// ����̏����ʒu
	const float DEF_LEFT_HAND_ANGLE = 2.35619f;
	// �E��̏����ʒu
	const float DEF_RIGHT_HAND_ANGLE = 0.785398f;

	// ���͂���Ă��琔�t���[�����擾���邽�߂̃^�C�}�[�̂Ńt�H���`�l
	const int AS_SOON_AS_INPUT_TIMER = 2;

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
	void CheckHit(const vector<vector<int>> mapData, vector<Bubble>& bubble, TimeStopTestBlock& testBlock);


	// �������Ƃ̃}�b�v�`�b�v�Ƃ̓����蔻��֐�
	void HitMapChipTop();
	void HitMapChipLeft();
	void HitMapChipRight();
	void HitMapChipBottom();

	//�v���C���[�̓��͂��֎~����
	void StopDoorLeftRight();
	void StopDoorUpDown();


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

	bool stopInputFlag;//���͂��֎~����t���O
	bool stopMoveFlag;
	bool drawCursorFlag;

	bool doorMoveLeftRightFlag;
	bool doorMoveUpDownFlag;
};