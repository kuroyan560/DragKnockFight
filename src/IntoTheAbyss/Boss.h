#include "Vec.h"
#include <vector>
#include "Intersected.h"
#include"AreaCollider.h"
#include"IBossPattern.h"
#include<array>
#include<memory>
#include"Bullet.h"
#include"BulletCollision.h"

#include"StagingInterFace.h"
using namespace std;

#include"CharacterInterFace.h"

// �v���C���[�ƈ������荇���{�X�N���X
class Boss : public CharacterInterFace
{
public:
	/*===== �����o�ϐ� =====*/
	Vec2<float> moveVel;		// �ړ���
	Vec2<float> swingInertiaVec;// �U��񂵂̊����̈ړ�����
	float swingInertia;			// �U��񂵂̊���
	int afterSwingDelay;		// �U��񂵂̂��ƂɃ{�X�����������Ȃ���Ԃɂ��邽�߂̃^�C�}�[
	INTERSECTED_LINE prevIntersectedLine;

	//�摜
	static const enum DIR { FRONT, BACK, DIR_NUM };
	int graphHandle[DIR_NUM];

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

	bool initPaticleFlag;
	int moveTimer;
	Vec2<float>initScale;
public:
	/*===== �萔 =====*/

	//const Vec2<float> SCALE = { 80.0f,80.0f };
	const float OFFSET_VEL = 2.0f;
	const float OFFSET_INERTIA = 30.0f;		// �U��񂵂̊���
	const int AFTER_SWING_DELAY = 15;
	const int STRUCK_WINDOW_TIMER = 120.0f;

	std::array<Bullet, 200> bullts;

public:
	/*===== �����o�֐� =====*/

	// �R���X�g���N�^
	Boss();

private:
	// ����������
	void OnInit()override;

	// �X�V����
	void OnUpdate(const std::vector<std::vector<int>>& MapData)override;

	// �`�揈��
	void OnDraw()override;

	void OnCheckHit(const std::vector<std::vector<int>>& MapData, const Vec2<float>& LineCenterPos)override {};
	virtual void OnHitMapChip(const HIT_DIR& Dir)override {};

public:
	bool Appear()override;
};