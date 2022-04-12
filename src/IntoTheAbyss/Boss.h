#include "Vec.h"
#include <vector>
#include "Intersected.h"
#include"AreaCollider.h"
#include"IBossPattern.h"
#include<array>
#include<memory>
#include"Bullet.h"

#include"StagingInterFace.h"
using namespace std;

// �v���C���[�ƈ������荇���{�X�N���X
class Boss {

public:

	/*===== �����o�ϐ� =====*/

	Vec2<float> pos;			// ���W
	Vec2<float> prevPos;		// �O�t���[���̍��W
	Vec2<float> scale;			// �傫��
	Vec2<float> vel;			// ���̃t���[���̈ړ��ʂ̑���
	Vec2<float> moveVel;		// �ړ���
	Vec2<float> swingInertiaVec;// �U��񂵂̊����̈ړ�����
	float swingInertia;			// �U��񂵂̊���
	int stuckWindowTimer;		// �E�B���h�E�ɋ��܂����^�C�}�[
	int afterSwingDelay;		// �U��񂵂̂��ƂɃ{�X�����������Ȃ���Ԃɂ��邽�߂̃^�C�}�[
	INTERSECTED_LINE prevIntersectedLine;

	//�摜
	static const enum DIR { FRONT, BACK, DIR_NUM };
	int graphHandle[DIR_NUM];

	//�N���b�V�����o�⏕
	StagingInterFace crashDevice;


	//�{�X�̃p�^�[������-----------------------
	enum E_BossPattern
	{
		BOSS_PATTERN_NONE = -1,
		BOSS_PATTERN_NORMALMOVE,
		BOSS_PATTERN_ATTACK
	};
	E_BossPattern bossPatternNow, oldBossPattern;
	BossPatternData patternData;
	std::array<std::unique_ptr<IBossPattern>, 2>bossPattern;
	int patternTimer;
	bool atackModeFlag;
	//�{�X�̃p�^�[������-----------------------


public:

	/*===== �萔 =====*/

	const Vec2<float> SCALE = { 80.0f,80.0f };
	const float OFFSET_VEL = 7.0f;
	const float OFFSET_INERTIA = 30.0f;		// �U��񂵂̊���
	const int AFTER_SWING_DELAY = 15;
	const int STRUCK_WINDOW_TIMER = 120.0f;

	std::array<Bullet, 200> bullts;

	//�w�n�Ƃ̔���
	Square areaHitBox;


public:

	/*===== �����o�֐� =====*/

	// �R���X�g���N�^
	Boss();

	// ����������
	void Init();

	// ��������
	void Generate(const Vec2<float> &generatePos);

	// �X�V����
	void Update();

	// �`�揈��
	void Draw();

	// �����蔻��
	void CheckHit(const vector<vector<int>> &mapData, bool &isHitMapChip, const Vec2<float> &playerPos, const Vec2<float> &lineCenterPos);

};