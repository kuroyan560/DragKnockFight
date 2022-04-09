#include "Boss.h"
#include "ShakeMgr.h"
#include "ScrollMgr.h"
#include "DrawFunc.h"
#include "MapChipCollider.h"
#include "WinApp.h"
#include"UsersInput.h"
#include"SuperiorityGauge.h"

#include"TexHandleMgr.h"
#include"BossPatternNormalMove.h"
#include"BossPatternAttack.h"

Boss::Boss()
{

	pos = {};
	scale = {};
	vel = {};

	graphHandle[FRONT] = TexHandleMgr::LoadGraph("resource/ChainCombat/enemy.png");
	graphHandle[BACK] = TexHandleMgr::LoadGraph("resource/ChainCombat/enemy_back.png");

	areaHitBox.center = &pos;


	bossPattern[0] = std::make_unique<BossPatternNormalMove>();
	bossPattern[1] = std::make_unique<BossPatternAttack>();


	//�p�^�[���ɓn���f�[�^�̏�����
	patternData.bossPos = &vel;
	patternData.stuckWindowTimer = &stuckWindowTimer;

	bossPatternNow = BOSS_PATTERN_NORMALMOVE;
	patternTimer = 0;

}

void Boss::Init()
{

	pos = {};
	scale = {};
	vel = {};

}

void Boss::Generate(const Vec2<float> &generatePos)
{

	/*===== �������� =====*/

	pos = generatePos;
	scale = SCALE;
	vel = { OFFSET_VEL,0 };
	prevIntersectedLine = INTERSECTED_NONE;
	stuckWindowTimer = 0;
	areaHitBox.size = scale;
}

#include"Camera.h"
void Boss::Update()
{

	/*===== �X�V���� =====*/

	// �O�t���[���̍��W��ۑ�
	prevPos = pos;

	++patternTimer;

	if (300 <= patternTimer)
	{
		if (atackModeFlag)
		{
			bossPatternNow = BOSS_PATTERN_ATTACK;
		}
		else
		{
			bossPatternNow = BOSS_PATTERN_NORMALMOVE;
		}
		atackModeFlag = !atackModeFlag;
		patternTimer = 0;
	}


	//�{�X�̋���
	bossPattern[bossPatternNow]->Update(&patternData);
}

void Boss::Draw()
{

	/*===== �`�揈�� =====*/

	//DrawFunc::DrawBox2D(pos - scale - scrollShakeAmount, pos + scale - scrollShakeAmount, Color(230, 38, 113, 255), DXGI_FORMAT_R8G8B8A8_UNORM, true);
	DIR dir = FRONT;
	if (vel.y < 0)dir = BACK;
	DrawFunc::DrawExtendGraph2D(ScrollMgr::Instance()->Affect(pos - scale), ScrollMgr::Instance()->Affect(pos + scale), TexHandleMgr::GetTexBuffer(graphHandle[dir]), AlphaBlendMode_Trans);
}

void Boss::CheckHit(const vector<vector<int>>& mapData, bool& isHitMapChip, const Vec2<float>& playerPos, const Vec2<float>& lineCenterPos)
{

	/*===== �����蔻�� =====*/

	// �{�X���}�b�v�`�b�v�Ɠ������Ă��邩�̃t���O
	bool isHitMapChipBoss = false;
	INTERSECTED_LINE intersectedBuff = {};

	// �}�b�v�`�b�v�ڐ��łǂ��ɓ���������
	bool isHitTop = false;
	bool isHitRight = false;
	bool isHitLeft = false;
	bool isHitBottom = false;

	bool onGround = false;
	// �ړ��ʂ����ɂ����}�b�v�`�b�v�Ƃ̓����蔻����s���B
	Vec2<float> moveDir = pos - prevPos;
	moveDir.Normalize();
	INTERSECTED_LINE intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheVel(pos, prevPos, moveDir * scale, scale, onGround, mapData, false);
	isHitTop = intersectedLine == INTERSECTED_TOP;
	isHitRight = intersectedLine == INTERSECTED_RIGHT;
	isHitLeft = intersectedLine == INTERSECTED_LEFT;
	isHitBottom = intersectedLine == INTERSECTED_BOTTOM;

	// �����蔻���ۑ��B
	isHitMapChipBoss = intersectedLine != INTERSECTED_NONE;

	// �X�P�[�������ɂ����}�b�v�`�b�v�Ƃ̓����蔻����s���B
	float offset = 1.0f;
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(pos, scale, mapData, INTERSECTED_TOP);
	if (!isHitMapChipBoss && intersectedLine != INTERSECTED_NONE)isHitMapChipBoss = true;
	if (!isHitTop && intersectedLine == INTERSECTED_TOP) isHitTop = true;
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(pos, scale, mapData, INTERSECTED_TOP);
	if (!isHitMapChipBoss && intersectedLine != INTERSECTED_NONE)isHitMapChipBoss = true;
	if (!isHitTop && intersectedLine == INTERSECTED_TOP) isHitTop = true;

	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(pos, scale, mapData, INTERSECTED_BOTTOM);
	if (!isHitMapChipBoss && intersectedLine != INTERSECTED_NONE)isHitMapChipBoss = true;
	if (!isHitBottom && intersectedLine == INTERSECTED_BOTTOM) isHitBottom = true;
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(pos, scale, mapData, INTERSECTED_BOTTOM);
	if (!isHitMapChipBoss && intersectedLine != INTERSECTED_NONE)isHitMapChipBoss = true;
	if (!isHitBottom && intersectedLine == INTERSECTED_BOTTOM) isHitBottom = true;

	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(pos, scale, mapData, INTERSECTED_RIGHT);
	if (!isHitMapChipBoss && intersectedLine != INTERSECTED_NONE)isHitMapChipBoss = true;
	if (!isHitRight && intersectedLine == INTERSECTED_RIGHT) isHitRight = true;
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(pos, scale, mapData, INTERSECTED_RIGHT);
	if (!isHitMapChipBoss && intersectedLine != INTERSECTED_NONE)isHitMapChipBoss = true;
	if (!isHitRight && intersectedLine == INTERSECTED_RIGHT) isHitRight = true;

	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(pos, scale, mapData, INTERSECTED_LEFT);
	if (!isHitMapChipBoss && intersectedLine != INTERSECTED_NONE)isHitMapChipBoss = true;
	if (!isHitLeft && intersectedLine == INTERSECTED_LEFT) isHitLeft = true;
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(pos, scale, mapData, INTERSECTED_LEFT);
	if (!isHitMapChipBoss && intersectedLine != INTERSECTED_NONE)isHitMapChipBoss = true;
	if (!isHitLeft && intersectedLine == INTERSECTED_LEFT) isHitLeft = true;


	// �}�b�v�`�b�v�Ɠ������Ă�����
	if (isHitMapChipBoss) {

		// �}�b�v�`�b�v�Ɠ������Ă���ꏊ�ɂ���āA�����������Ă��邩�ǂ����𒲂ׂ�B

		// �v���C���[�Ƃ̊p�x
		float playerAngle = atan2(playerPos.y - pos.y, playerPos.x - pos.x);
		playerAngle = fabs(playerAngle);

		if (isHitTop) {

			// �}�b�v�`�b�v�̏�ɂ������Ă����Ƃ������Ƃ́A�v���C���[���������ɂ����OK�I
			// �������̋�̓I�Ȓl��
			const float MIN_ANGLE = 0.785398f;
			const float MAX_ANGLE = 2.35619f;

			// �p�x�����̒l�̊Ԃ�������
			if (MIN_ANGLE <= playerAngle && playerAngle <= MAX_ANGLE) {

				// �����������Ă���B
				isHitMapChip = true;
				intersectedBuff = INTERSECTED_BOTTOM;

			}

		}

		if (isHitBottom) {

			// �}�b�v�`�b�v�̉��ɂ������Ă����Ƃ������Ƃ́A�v���C���[��������ɂ����OK�I
			// ������̋�̓I�Ȓl��
			const float MIN_ANGLE = 3.92699f;
			const float MAX_ANGLE = 5.49779f;

			// �p�x�����̒l�̊Ԃ�������
			if (MIN_ANGLE <= playerAngle && playerAngle <= MAX_ANGLE) {

				// �����������Ă���B
				isHitMapChip = true;
				intersectedBuff = INTERSECTED_TOP;

			}

		}

		if (isHitRight) {

			// �}�b�v�`�b�v�̉E�ɂ������Ă����Ƃ������Ƃ́A�v���C���[���������ɂ����OK�I
			// �������̋�̓I�Ȓl��
			const float MIN_ANGLE = 2.35619f;
			const float MAX_ANGLE = 3.92699f;

			// �p�x�����̒l�̊Ԃ�������
			if (MIN_ANGLE <= playerAngle && playerAngle <= MAX_ANGLE) {

				// �����������Ă���B
				isHitMapChip = true;
				intersectedBuff = INTERSECTED_RIGHT;

			}

		}

		if (isHitLeft) {

			// �}�b�v�`�b�v�̍��ɂ������Ă����Ƃ������Ƃ́A�v���C���[���E�����ɂ����OK�I
			// �E�����̋�̓I�Ȓl��
			const float MIN_ANGLE = 0.785398f;
			const float MAX_ANGLE = 5.49779f;

			// �p�x�����̒l�̊Ԃ�������
			if (MAX_ANGLE <= playerAngle && playerAngle <= MIN_ANGLE) {

				// �����������Ă���B
				isHitMapChip = true;
				intersectedBuff = INTERSECTED_LEFT;

			}

		}

	}

	// �}�b�v�`�b�v�ɂ������Ă����Ԃŉ�ʊO�ɏo����
	if (isHitMapChip) {

		Vec2<int> windowSize = WinApp::Instance()->GetWinCenter();
		windowSize *= Vec2<int>(2, 2);

		// �{�X�ƃv���C���[�̋���
		float distanceX = fabs(lineCenterPos.x - pos.x);
		float disntaceY = fabs(lineCenterPos.y - pos.y);

		// �E�B���h�E���E
		if (windowSize.x <= pos.x + scale.x - ScrollMgr::Instance()->scrollAmount.x || pos.x - scale.x - ScrollMgr::Instance()->scrollAmount.x <= 0) {

			stuckWindowTimer = STRUCK_WINDOW_TIMER;
			ShakeMgr::Instance()->SetShake(20);
			SuperiorityGauge::Instance()->AddPlayerGauge(10);

		}
		// �E�B���h�E�㉺
		if (windowSize.y <= pos.y + scale.y - ScrollMgr::Instance()->scrollAmount.y || pos.y - scale.y - ScrollMgr::Instance()->scrollAmount.y <= 0) {

			stuckWindowTimer = STRUCK_WINDOW_TIMER;
			ShakeMgr::Instance()->SetShake(20);
			SuperiorityGauge::Instance()->AddPlayerGauge(10);

		}

	}

	prevIntersectedLine = intersectedBuff;

	if (0 < stuckWindowTimer) {

		isHitMapChip = false;

	}

}
