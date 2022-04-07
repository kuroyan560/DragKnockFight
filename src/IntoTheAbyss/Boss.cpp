#include "Boss.h"
#include "ShakeMgr.h"
#include "ScrollMgr.h"
#include "DrawFunc.h"
#include "MapChipCollider.h"
#include "WinApp.h"
#include"UsersInput.h"

#include"TexHandleMgr.h"

Boss::Boss()
{

	pos = {};
	scale = {};
	vel = {};

	graphHandle[FRONT] = TexHandleMgr::LoadGraph("resource/ChainCombat/enemy.png");
	graphHandle[BACK] = TexHandleMgr::LoadGraph("resource/ChainCombat/enemy_back.png");
}

void Boss::Init()
{

	pos = {};
	scale = {};
	vel = {};

}

void Boss::Generate(const Vec2<float>& generatePos)
{

	/*===== �������� =====*/

	pos = generatePos;
	scale = SCALE;
	vel = { OFFSET_VEL,0 };
	prevIntersectedLine = INTERSECTED_NONE;
	stuckWindowTimer = 0;

}

void Boss::Update()
{

	/*===== �X�V���� =====*/

	// �O�t���[���̍��W��ۑ�
	prevPos = pos;

	vel.x += (OFFSET_VEL - vel.x) / 10.0f;

	if (UsersInput::Instance()->Input(DIK_0)) {

		vel.x = OFFSET_VEL * 5.0f;

	}

	if (0 < stuckWindowTimer) --stuckWindowTimer;

}

void Boss::Draw()
{

	/*===== �`�揈�� =====*/

	Vec2<float> scrollShakeAmount = ScrollMgr::Instance()->scrollAmount + ShakeMgr::Instance()->shakeAmount;

	//DrawFunc::DrawBox2D(pos - scale - scrollShakeAmount, pos + scale - scrollShakeAmount, Color(230, 38, 113, 255), DXGI_FORMAT_R8G8B8A8_UNORM, true);

	DIR dir = FRONT;
	if (vel.y < 0)dir = BACK;
	DrawFunc::DrawExtendGraph2D(pos - scale - scrollShakeAmount, pos + scale - scrollShakeAmount, TexHandleMgr::GetTexBuffer(graphHandle[dir]), AlphaBlendMode_Trans);
}

void Boss::CheckHit(const vector<vector<int>>& mapData, bool& isHitMapChip, const Vec2<float>& playerPos)
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

		// �{�X�ƃv���C���[�̋���
		float distanceX = fabs(playerPos.x - pos.x);
		float disntaceY = fabs(playerPos.y - pos.y);

		// �E�B���h�E���E
		if (windowSize.x <= distanceX) {

			stuckWindowTimer = STRUCK_WINDOW_TIMER;
			ShakeMgr::Instance()->SetShake(20);

		}
		// �E�B���h�E�㉺
		if (windowSize.y <= disntaceY) {

			stuckWindowTimer = STRUCK_WINDOW_TIMER;
			ShakeMgr::Instance()->SetShake(20);

		}

	}

	prevIntersectedLine = intersectedBuff;

	if (0 < stuckWindowTimer) {

		isHitMapChip = false;

	}

}
