#include "Boss.h"
#include "ShakeMgr.h"
#include "ScrollMgr.h"
#include "DrawFunc.h"
#include "MapChipCollider.h"
#include "WinApp.h"
#include "UsersInput.h"
#include "SuperiorityGauge.h"
#include "SwingMgr.h"
#include "SuperiorityGauge.h"
#include "SlowMgr.h"
#include "StunEffect.h"

#include"TexHandleMgr.h"
#include"CrashMgr.h"

Boss::Boss()
{

	pos = {};
	scale = {};
	vel = {};
	moveVel = {};

	graphHandle[FRONT] = TexHandleMgr::LoadGraph("resource/ChainCombat/enemy.png");
	graphHandle[BACK] = TexHandleMgr::LoadGraph("resource/ChainCombat/enemy_back.png");

	areaHitBox.center = &pos;
	Init();
}

void Boss::Init()
{

	pos = {};
	scale = {};
	vel = {};
	moveVel = {};
	crashDevice.Init();

}

void Boss::Generate(const Vec2<float>& generatePos)
{

	/*===== �������� =====*/

	pos = generatePos;
	scale = SCALE;
	vel = { OFFSET_VEL,0 };
	moveVel = { OFFSET_VEL,0 };
	swingInertia = 0;
	swingInertiaVec = {};
	afterSwingDelay = 0;
	prevIntersectedLine = INTERSECTED_NONE;
	stuckWindowTimer = 0;
	areaHitBox.size = scale;
}

#include"Camera.h"
void Boss::Update()
{
	/*===== �X�V���� =====*/
	crashDevice.Update();

	// �O�t���[���̍��W��ۑ�
	prevPos = pos;

	if (Camera::Instance()->Active())
	{
		moveVel = { 0,0 };
		return;
	}

	// �������X�V�B
	if (0 < swingInertia) {
		swingInertia -= swingInertia / 5.0f;
	}

	// �U��񂵒���̍d���̃^�C�}�[���X�V�B
	if (0 < afterSwingDelay) --afterSwingDelay;




	// [�U��񂵒����U��񂳂ꒆ��������] �X�V�������s��Ȃ��B�@�@�Վ��̎����ł��B
	bool isSwingNow = SwingMgr::Instance()->isSwingBoss || SwingMgr::Instance()->isSwingPlayer;

	// [�d����] [�X�^�����o��] �͓������Ȃ�
	if (0 < afterSwingDelay || StunEffect::Instance()->isActive) {
		// �������Ȃ��B
	}
	else if (isSwingNow) {

		// �U��񂵒���������B

		pos = SwingMgr::Instance()->playerPos + SwingMgr::Instance()->easingNowVec * SwingMgr::Instance()->lineLength;

		// ���傤�ǏI������u�Ԃ̏ꍇ�A�����֌W�̕ϐ����X�V����B
		if (1.0f <= SwingMgr::Instance()->easingTimer) {

			swingInertiaVec = (pos - prevPos).GetNormal();
			swingInertia = (prevPos - pos).Length();
			afterSwingDelay = AFTER_SWING_DELAY;

		}

	}
	else {

		static const int PULL_SPAN_MIN = 30;
		static const int PULL_SPAN_MAX = 70;
		static int PULL_SPAN = KuroFunc::GetRand(PULL_SPAN_MIN, PULL_SPAN_MAX);
		static int PULL_TIMER = 0;
		static Vec2<float>ACCEL = { 0.0f,0.0f };	//�����x
		static const float PULL_POWER_MIN = 15.0f;
		static const float PULL_POWER_MAX = 25.0f;

		if (PULL_TIMER < PULL_SPAN)
		{
			PULL_TIMER++;
			if (PULL_SPAN <= PULL_TIMER)
			{
				PULL_SPAN = KuroFunc::GetRand(PULL_SPAN_MIN, PULL_SPAN_MAX);
				PULL_TIMER = 0;

				auto rad = Angle::ConvertToRadian(KuroFunc::GetRand(-70, 70));
				auto power = KuroFunc::GetRand(PULL_POWER_MIN, PULL_POWER_MAX);
				ACCEL.x = cos(rad) * power * 1.6f;
				ACCEL.y = sin(rad) * power;
			}
		}
		moveVel.x = KuroMath::Lerp(moveVel.x, OFFSET_VEL, 0.1f);
		moveVel.y = KuroMath::Lerp(moveVel.y, 0.0f, 0.1f);
		moveVel += ACCEL;

		ACCEL = KuroMath::Lerp(ACCEL, { 0.0f,0.0f }, 0.8f);

		if (UsersInput::Instance()->Input(DIK_0)) {
			vel.x = OFFSET_VEL * 5.0f;
		}

	}

	if (0 < stuckWindowTimer) --stuckWindowTimer;


	// �ړ��ʂ̑��ʂ����߂�B

	if (0 < afterSwingDelay) {

		// �U��񂵒���̍d�����c���Ă���ꍇ�́A�����݂̂��ړ��ʂƂ���B
		vel = swingInertiaVec * swingInertia;

	}
	else {

		vel = moveVel + swingInertiaVec * swingInertia;

	}

}

#include"DrawFunc_FillTex.h"
#include"D3D12App.h"
void Boss::Draw()
{

	/*===== �`�揈�� =====*/

	//DrawFunc::DrawBox2D(pos - scale - scrollShakeAmount, pos + scale - scrollShakeAmount, Color(230, 38, 113, 255), DXGI_FORMAT_R8G8B8A8_UNORM, true);
	DIR dir = FRONT;
	if (vel.y < 0)dir = BACK;

	auto drawPos = pos + crashDevice.GetShake();
	static auto WHITE_TEX = D3D12App::Instance()->GenerateTextureBuffer(Color());
	DrawFunc_FillTex::DrawExtendGraph2D(ScrollMgr::Instance()->Affect(drawPos - scale), ScrollMgr::Instance()->Affect(drawPos + scale),
		TexHandleMgr::GetTexBuffer(graphHandle[dir]), WHITE_TEX, crashDevice.GetFlashAlpha());
}

void Boss::CheckHit(const vector<vector<int>>& mapData, bool& isHitMapChip, const Vec2<float>& playerPos, const Vec2<float>& lineCenterPos)
{

	/*===== �����蔻�� =====*/

	// �{�X���}�b�v�`�b�v�Ɠ������Ă��邩�̃t���O
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

	// �X�P�[�������ɂ����}�b�v�`�b�v�Ƃ̓����蔻����s���B
	float offset = 1.0f;
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(pos, scale, mapData, INTERSECTED_TOP);
	if (!isHitTop && intersectedLine == INTERSECTED_TOP) isHitTop = true;
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(pos, scale, mapData, INTERSECTED_TOP);
	if (!isHitTop && intersectedLine == INTERSECTED_TOP) isHitTop = true;

	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(pos, scale, mapData, INTERSECTED_BOTTOM);
	if (!isHitBottom && intersectedLine == INTERSECTED_BOTTOM) isHitBottom = true;
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(pos, scale, mapData, INTERSECTED_BOTTOM);
	if (!isHitBottom && intersectedLine == INTERSECTED_BOTTOM) isHitBottom = true;

	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(pos, scale, mapData, INTERSECTED_RIGHT);
	if (!isHitRight && intersectedLine == INTERSECTED_RIGHT) isHitRight = true;
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(pos, scale, mapData, INTERSECTED_RIGHT);
	if (!isHitRight && intersectedLine == INTERSECTED_RIGHT) isHitRight = true;

	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(pos, scale, mapData, INTERSECTED_LEFT);
	if (!isHitLeft && intersectedLine == INTERSECTED_LEFT) isHitLeft = true;
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(pos, scale, mapData, INTERSECTED_LEFT);
	if (!isHitLeft && intersectedLine == INTERSECTED_LEFT) isHitLeft = true;


	// �}�b�v�`�b�v�Ɠ������Ă�����
	if (isHitTop || isHitRight || isHitLeft || isHitBottom) {

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

		// �U��񂳂�Ă����Ԃ�������A�V�F�C�N�𔭐������ĐU��񂵏�Ԃ���������B
		if (SwingMgr::Instance()->isSwingPlayer || OFFSET_INERTIA / 2.0f < afterSwingDelay) {

			CrashMgr::Instance()->Crash(pos, crashDevice);
			SuperiorityGauge::Instance()->AddPlayerGauge(5.0f);
			SwingMgr::Instance()->isSwingPlayer = false;

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
			CrashMgr::Instance()->Crash(pos, crashDevice);
			SuperiorityGauge::Instance()->AddPlayerGauge(10);

		}
		// �E�B���h�E�㉺
		if (windowSize.y <= pos.y + scale.y - ScrollMgr::Instance()->scrollAmount.y || pos.y - scale.y - ScrollMgr::Instance()->scrollAmount.y <= 0) {

			stuckWindowTimer = STRUCK_WINDOW_TIMER;
			CrashMgr::Instance()->Crash(pos, crashDevice);
			SuperiorityGauge::Instance()->AddPlayerGauge(10);

		}

	}

	prevIntersectedLine = intersectedBuff;

	if (0 < stuckWindowTimer) {

		isHitMapChip = false;

	}

}
