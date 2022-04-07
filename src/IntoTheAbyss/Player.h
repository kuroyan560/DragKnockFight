#pragma once
#include "Vec.h"
#include "DossunBlock.h"
#include "Bubble.h"
#include <memory>
#include <vector>

using namespace std;

// �v���C���[�̘r�̑O���錾
class PlayerHand;

#include"PlayerAnimation.h"
#include"AfterImg.h"
class LightManager;
#include"Muffler.h"

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
	bool isDead;					// ���񂾂��ǂ����t���O
	bool isDouji;					// �����Ɍ��������̃t���O
	int rapidFireTimerLeft;			// �A�˃^�C�}�[����
	int rapidFireTimerRight;		// �A�˃^�C�}�[�E��
	int gravityInvalidTimer;		// �d�͖������^�C�}�[
	int handReturnTimer;			// ���͂��I����Ă���r���f�t�H���g�̈ʒu�ɖ߂�܂ł̃^�C�}�[
	int asSoonAsInputTimer;			// �ړ����͂��s���Ă��琔�t���[���ԗL�������鏈������邽�߂Ƀ^�C�}�[ ��ɃV���{����
	int firstRecoilParticleTimer;	// �ŏ��̃V���b�g�̂Ƃ��̃p�[�e�B�N�����o���^�C�}�[

	// �ǃY���t���O
	bool isSlippingWall[4];			// �ǃY���p�[�e�B�N�����o���t���O

	// �E�B���h�E�ɋ��܂����^�C�}�[
	int stuckWindowTimer;		// �E�B���h�E�ɋ��܂����^�C�}�[
	const int STRUCK_WINDOW_TIMER = 120.0f;// �E�B���h�E�ɋ��܂����^�C�}�[

	// �v���C���[�̘r
	unique_ptr<PlayerHand> lHand;	// ����
	unique_ptr<PlayerHand> rHand;	// �E��
	bool isPrevFrameShotBeacon;

	//int playerGraph;

	//�����ɂ��X�g���b�`
	//const Vec2<float> MAX_STRETCH = { 16.0f,37.0f };	//�ő�X�g���b�`��
	const Vec2<float> MAX_STRETCH = { 10.0f,31.0f };	//�ő�X�g���b�`��
	Vec2<float>stretch_LU = { 0,0 };	//����
	Vec2<float>stretch_RB = { 0,0 };	//�E��
	Vec2<float>fromStretch_LU = { 0,0 };	//�C�[�W���O�p�X�^�[�g�l
	Vec2<float>fromStretch_RB = { 0,0 };	//�C�[�W���O�p�X�^�[�g�l
	const int STRETCH_RETURN_TIME = 17;	//�X�g���b�`���O�ɂȂ�܂ł̃t���[����
	int stretchTimer = STRETCH_RETURN_TIME;
	const float STRETCH_DIV_RATE = 2.0f;	//�X�g���b�`���キ����Ƃ��̊���
	const int FIRST_SHOT_RECOIL_PARTICLE_TIMER = 10.0f;

	//�v���C���[�̌���
	//enum DRAW_DIR { FRONT, BACK, DIR_NUM, DEFAULT = FRONT }playerDir = DEFAULT;
	//�A�j���[�V��������
	PlayerAnimation anim;

	//�c��
	AfterImg afImg;

	//�e���|�[�g�����Ƃ�����
	const int TELE_FLASH_TIME = 120;
	const int DOUJI_FLASH_TIME = 60;
	int flashTotalTime;
	int flashTimer;

	//�����V���b�g������Ƃ鋖�e�t���[��
	const int DOUJI_ALLOWANCE_FRAME = 22;
	int isLeftFirstShotTimer;
	int isRightFirstShotTimer;

	bool isZeroGravity;
	int changeGravityTimer;
	const int CHANGE_GRAVITY_TIMER = 300;

	//Muffler muffler;

public:

	/*-- �萔 --*/

	//const float ADD_GRAVITY = 0.5f;				// �v���C���[�ɂ�����d��
	float ADD_GRAVITY = 0.1f;				// �v���C���[�ɂ�����d��
	float MAX_GRAVITY = 15.0f;			// �v���C���[�ɂ�����d�͂̍ő��
	float RECOIL_AMOUNT = 7.0f;			// �e���������ۂ̔���
	float FIRST_RECOIL_AMOUNT = 10.0;		// �e���������ۂ̔���
	//const float FIRST_RECOIL_AMOUNT = 15.0;		// �e���������ۂ̔���
	//const float RECOIL_AMOUNT = FIRST_RECOIL_AMOUNT;			// �e���������ۂ̔���
	float MAX_RECOIL_AMOUNT = 30.0f;		// �e���������ۂ̔����̍ő�l
	const float EXT_RATE = 0.6f;	//Player's expand rate used in Draw().
	const Vec2<float> PLAYER_HIT_SIZE = { (80 * EXT_RATE) / 2.0f,(80 * EXT_RATE) / 2.0f };			// �v���C���[�̃T�C�Y
	static Vec2<float>GetGeneratePos();
	int RAPID_FIRE_TIMER = 21;			// �A�˃^�C�}�[
	const int GRAVITY_INVALID_TIMER = 20;		// �d�͖������^�C�}�[

	// �R���[�e�I�Ȃ�̂��߂̃p�����[�^�[
	const float STOP_DEADLINE_Y = 5.0f;		// Y���̈ړ��ʂ��R���ȉ���������ړ��ʂ�0�ɂ���B
	const float STOP_DEADLINE_X = 3.0f;		// X���̈ړ��ʂ��R���ȉ���������ړ��ʂ�0�ɂ���B
	const float VEL_MUL_AMOUNT = 0.95f;	//���C�W��

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
	void Draw(LightManager& LigManager);

	// �}�b�v�`�b�v�Ƃ̓����蔻��
	void CheckHit(const vector<vector<int>> mapData, vector<Bubble>& bubble, vector<DossunBlock>& dossun, const Vec2<float>& bossPos, bool& isHitMapChip, const Vec2<float>& lineCenterPos);

	// �������Ƃ̃}�b�v�`�b�v�Ƃ̓����蔻��֐�
	void HitMapChipTop();
	void HitMapChipLeft();
	void HitMapChipRight();
	void HitMapChipBottom();

	//�v���C���[�̓��͂��֎~����
	void StopDoorLeftRight();
	void StopDoorUpDown();
	bool drawCursorFlag;

	//�X�N���[���Ȃǂɂ��l���������S���W
	Vec2<float> GetCenterDrawPos();

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


	bool doorMoveLeftRightFlag;
	bool doorMoveUpDownFlag;
	bool doorMoveDownFlag;
	//�X�g���b�`�̒l���v�Z
	void CalculateStretch(const Vec2<float>& Move);
	//�X�g���b�`�l�X�V
	void UpdateStretch();
	//�摜�T�C�Y����v���C���[�T�C�Y�擾
	Vec2<float> GetPlayerGraphSize();

	// �ړ��ʂł̓����蔻��
	void CheckHitMapChipVel(const Vec2<float>& checkPos, const vector<vector<int>>& mapData, const Vec2<float>& bossPos, bool& isHitMapChip);
	void CheckHitSize(const Vec2<float>& checkPos, const vector<vector<int>>& mapData, const Vec2<float>& bossPos, bool& isHitMapChip);

};