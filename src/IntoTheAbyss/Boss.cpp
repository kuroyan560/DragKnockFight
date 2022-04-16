#include "Boss.h"
#include "ShakeMgr.h"
#include "ScrollMgr.h"
#include "DrawFunc.h"
#include "MapChipCollider.h"
#include "WinApp.h"
#include "UsersInput.h"
#include "SuperiorityGauge.h"
#include "SuperiorityGauge.h"
#include "SlowMgr.h"
#include "StunEffect.h"

#include"TexHandleMgr.h"
#include"BossPatternNormalMove.h"
#include"BossPatternAttack.h"
#include"BossPatternSwing.h"
#include"BossBulletManager.h"
#include"CrashMgr.h"
#include"CharacterInterFace.h"

#include"ParticleMgr.h"
#include"DebugKeyManager.h"

#include"DebugParameter.h"


static const Vec2<float> SCALE = { 80.0f,80.0f };
Boss::Boss() :CharacterInterFace(SCALE)
{
	graphHandle[FRONT] = TexHandleMgr::LoadGraph("resource/ChainCombat/boss/enemy.png");
	graphHandle[BACK] = TexHandleMgr::LoadGraph("resource/ChainCombat/boss/enemy_back.png");

	bossPattern[0] = std::make_unique<BossPatternNormalMove>();
	bossPattern[1] = std::make_unique<BossPatternAttack>();
	bossPattern[2] = std::make_unique<BossPatternSwing>();


	//�p�^�[���ɓn���f�[�^�̏�����
	patternData.moveVel = &moveVel;


}

void Boss::OnInit()
{
	/*===== �������� =====*/

	initScale = SCALE * 5.0f;
	size = initScale;
	moveVel = { 0,0 };
	swingInertia = 0;
	swingInertiaVec = {};
	afterSwingDelay = 0;
	prevIntersectedLine = INTERSECTED_NONE;
	moveTimer = 0;
	initPaticleFlag = false;

	bossPatternNow = BOSS_PATTERN_NORMALMOVE;
	patternTimer = 0;
}

#include"Camera.h"
void Boss::OnUpdate(const std::vector<std::vector<int>> &MapData)
{
	/*===== �X�V���� =====*/

	//if (Camera::Instance()->Active())
	//{
	//	moveVel = { 0,0 };
	//	return;
	//}

	for (int i = 0; i < patternData.nearLimmitLine.size(); ++i)
	{
		patternData.nearLimmitLine[i].startPos = pos;

		float angle = i * (360.0f / patternData.nearLimmitLine.size());
		float dir = Angle::ConvertToRadian(angle);
		float distance = 150.0f;
		patternData.nearLimmitLine[i].endPos = pos + Vec2<float>(cosf(dir), sinf(dir)) * distance;
	}

	for (int i = 0; i < patternData.farLimmitLine.size(); ++i)
	{
		patternData.farLimmitLine[i].startPos = pos;

		float angle = i * (360.0f / patternData.farLimmitLine.size());
		float dir = Angle::ConvertToRadian(angle);
		float distance = 350.0f;
		patternData.farLimmitLine[i].endPos = pos + Vec2<float>(cosf(dir), sinf(dir)) * distance;
	}

	if (bossPatternNow != BOSS_PATTERN_NORMALMOVE)
	{
		moveVel = { 10.0f,0.0f };
	}



	for (int i = 0; i < patternData.bulltData.size(); ++i)
	{
		if (patternData.bulltData[i].initFlag)
		{
			patternData.bulltData[i].Reset();
		}
	}

	// [�U��񂵒����U��񂳂ꒆ��������] �X�V�������s��Ȃ��B�@�@�Ղ̎����ł��B
	bool isSwingNow = this->GetNowSwing() || partner.lock()->GetNowSwing();

	// [�d����] [�X�^�����o��] �͓������Ȃ�
	if (0 < afterSwingDelay || StunEffect::Instance()->isActive) {
		// �������Ȃ��B
	}
	else if (isSwingNow) {

	}
	else if (GetCanMove()) {

		//�{�X��AI-----------------------
		++patternTimer;
		//���̏�ԑI��
		if (120 <= patternTimer)
		{
			Vec2<float> dir = GetPartnerPos() - pos;
			dir.Normalize();

			while (1)
			{
				int random = KuroFunc::GetRand(BOSS_PATTERN_MAX - 1);
				//�U��񂵂̏�����������Ă��Ȃ����͐U��񂵂������Ȃ�
				if (fabs(dir.y) <= 0.3f && random == BOSS_PATTERN_SWING)
				{
					continue;
				}
				bossPatternNow = static_cast<E_BossPattern>(random);
				break;
			}
			patternTimer = 0;
		}
		//�{�X��AI-----------------------
		//bossPatternNow = BOSS_PATTERN_NORMALMOVE;

		//�{�X�̋���
		if (bossPatternNow != oldBossPattern)
		{
			bossPattern[bossPatternNow]->Init();
		}
		bossPattern[bossPatternNow]->Update(&patternData);
		oldBossPattern = bossPatternNow;

		//�{�X�̒e
		for (int i = 0; i < patternData.bulltData.size(); ++i)
		{
			if (patternData.bulltData[i].initFlag)
			{
				BossBulletManager::Instance()->Generate(pos, patternData.bulltData[i].dir, patternData.bulltData[i].speed);
			}
		}
	}
	DebugParameter::Instance()->bossDebugData.bossNowStatus = static_cast<E_BossPattern>(bossPatternNow);


	//�U��񂵂̊J�n�x�N�g�����擾�B
	//if (DebugKeyManager::Instance()->DebugKeyTrigger(DIK_O, "SwingBoss", TO_STRING(DIK_O)))
	if (patternData.swingFlag)
	{
		Vec2<float> dir = GetPartnerPos() - pos;
		dir.Normalize();

		// �U��񂵂̃g���K�[����
		if (0.3f < fabs(dir.y))
		{
			SwingPartner();
		}
	}


	DebugParameter::Instance()->bossDebugData.moveVel = moveVel;

	// �ړ��ʂ̑��ʂ����߂�B
	auto swingAffect = GetSwingInertia();
	if (GetSwingRigor())
	{
		// �U��񂵒���̍d�����c���Ă���ꍇ�́A�����݂̂��ړ��ʂƂ���B
		vel = swingAffect;
	}
	else
	{
		vel = moveVel + swingAffect;
	}
}

#include"DrawFunc_FillTex.h"
#include"D3D12App.h"
void Boss::OnDraw()
{
	/*===== �`�揈�� =====*/
	//DrawFunc::DrawBox2D(pos - scale - scrollShakeAmount, pos + scale - scrollShakeAmount, Color(230, 38, 113, 255), DXGI_FORMAT_R8G8B8A8_UNORM, true);
	DIR dir = FRONT;
	if (vel.y < 0)dir = BACK;

	auto drawPos = pos + stagingDevice.GetShake();
	auto drawScale = size * stagingDevice.GetExtRate();
	static auto CRASH_TEX = D3D12App::Instance()->GenerateTextureBuffer(Color(255, 0, 0, 255));

	if (DebugParameter::Instance()->bossDebugData.drawBossFlag)
	{
		DrawFunc_FillTex::DrawExtendGraph2D(ScrollMgr::Instance()->Affect(drawPos - drawScale), ScrollMgr::Instance()->Affect(drawPos + drawScale),
			TexHandleMgr::GetTexBuffer(graphHandle[dir]), CRASH_TEX, stagingDevice.GetFlashAlpha());
	}

	//���C�Ƃ̔���m�F
	if (DebugParameter::Instance()->bossDebugData.drawNearRayFlag)
	{
		for (int i = 0; i < patternData.nearLimmitLine.size(); ++i)
		{
			Vec2<float>drawStartPos = ScrollMgr::Instance()->Affect(patternData.nearLimmitLine[i].startPos);
			Vec2<float>drawEndPos = ScrollMgr::Instance()->Affect(patternData.nearLimmitLine[i].endPos);
			if (patternData.nearLimmitLine[i].hitFlag)
			{
				DrawFunc::DrawLine2D(drawStartPos, drawEndPos, Color(255, 0, 0, 255));
			}
			else
			{
				DrawFunc::DrawLine2D(drawStartPos, drawEndPos, Color(255, 255, 255, 255));
			}
		}
	}

	if (DebugParameter::Instance()->bossDebugData.drawFarRayFlag)
	{
		for (int i = 0; i < patternData.farLimmitLine.size(); ++i)
		{
			Vec2<float>drawStartPos = ScrollMgr::Instance()->Affect(patternData.farLimmitLine[i].startPos);
			Vec2<float>drawEndPos = ScrollMgr::Instance()->Affect(patternData.farLimmitLine[i].endPos);
			if (patternData.farLimmitLine[i].hitFlag)
			{
				DrawFunc::DrawLine2D(drawStartPos, drawEndPos, Color(0, 255, 0, 255));
			}
			else
			{
				DrawFunc::DrawLine2D(drawStartPos, drawEndPos, Color(0, 0, 255, 255));
			}
		}
	}
}

//void Boss::CheckHit(const vector<vector<int>>& mapData, bool& isHitMapChip, const Vec2<float>& playerPos, const Vec2<float>& lineCenterPos)
//{
//
//	/*===== �����蔻�� =====*/
//
//	// �{�X���}�b�v�`�b�v�Ɠ������Ă��邩�̃t���O
//	INTERSECTED_LINE intersectedBuff = {};
//
//	// �}�b�v�`�b�v�ڐ��łǂ��ɓ���������
//	bool isHitTop = false;
//	bool isHitRight = false;
//	bool isHitLeft = false;
//	bool isHitBottom = false;
//
//	bool onGround = false;
//	// �ړ��ʂ����ɂ����}�b�v�`�b�v�Ƃ̓����蔻����s���B
//	Vec2<float> moveDir = pos - prevPos;
//	moveDir.Normalize();
//	INTERSECTED_LINE intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheVel(pos, prevPos, moveDir * scale, scale, onGround, mapData, false);
//	isHitTop = intersectedLine == INTERSECTED_TOP;
//	isHitRight = intersectedLine == INTERSECTED_RIGHT;
//	isHitLeft = intersectedLine == INTERSECTED_LEFT;
//	isHitBottom = intersectedLine == INTERSECTED_BOTTOM;
//
//	// ����
//	Vec2<float> velPosBuff = pos - scale;
//	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheVel(velPosBuff, prevPos - scale, {}, {}, onGround, mapData, false);
//	pos = velPosBuff + scale;
//	if (intersectedLine == INTERSECTED_TOP) isHitTop = true;
//	if (intersectedLine == INTERSECTED_RIGHT) isHitRight = true;
//	if (intersectedLine == INTERSECTED_LEFT) isHitLeft = true;
//	if (intersectedLine == INTERSECTED_BOTTOM) isHitBottom = true;
//
//	// �E��
//	velPosBuff = pos + Vec2<float>(scale.x, -scale.y);
//	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheVel(velPosBuff, prevPos + Vec2<float>(scale.x, -scale.y), {}, {}, onGround, mapData, false);
//	pos = velPosBuff - Vec2<float>(scale.x, -scale.y);
//	if (intersectedLine == INTERSECTED_TOP) isHitTop = true;
//	if (intersectedLine == INTERSECTED_RIGHT) isHitRight = true;
//	if (intersectedLine == INTERSECTED_LEFT) isHitLeft = true;
//	if (intersectedLine == INTERSECTED_BOTTOM) isHitBottom = true;
//
//	// �E��
//	velPosBuff = pos + scale;
//	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheVel(velPosBuff, prevPos + scale, {}, {}, onGround, mapData, false);
//	pos = velPosBuff - scale;
//	if (intersectedLine == INTERSECTED_TOP) isHitTop = true;
//	if (intersectedLine == INTERSECTED_RIGHT) isHitRight = true;
//	if (intersectedLine == INTERSECTED_LEFT) isHitLeft = true;
//	if (intersectedLine == INTERSECTED_BOTTOM) isHitBottom = true;
//
//	// ����
//	velPosBuff = pos + Vec2<float>(-scale.x, scale.y);
//	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheVel(velPosBuff, prevPos + Vec2<float>(-scale.x, scale.y), {}, {}, onGround, mapData, false);
//	pos = velPosBuff - Vec2<float>(-scale.x, scale.y);
//	if (intersectedLine == INTERSECTED_TOP) isHitTop = true;
//	if (intersectedLine == INTERSECTED_RIGHT) isHitRight = true;
//	if (intersectedLine == INTERSECTED_LEFT) isHitLeft = true;
//	if (intersectedLine == INTERSECTED_BOTTOM) isHitBottom = true;
//
//	// �X�P�[�������ɂ����}�b�v�`�b�v�Ƃ̓����蔻����s���B
//
//	// �����
//	float offset = 5.0f;
//	Vec2<float> posBuff = pos + Vec2<float>(scale.x - offset, 0);
//	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(posBuff, scale, mapData, INTERSECTED_TOP);
//	pos.y = posBuff.y;
//	if (!isHitTop && intersectedLine == INTERSECTED_TOP) isHitTop = true;
//	posBuff = pos - Vec2<float>(scale.x - offset, 0);
//	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(posBuff, scale, mapData, INTERSECTED_TOP);
//	pos.y = posBuff.y;
//	if (!isHitTop && intersectedLine == INTERSECTED_TOP) isHitTop = true;
//	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(pos, scale, mapData, INTERSECTED_TOP);
//	if (!isHitTop && intersectedLine == INTERSECTED_TOP) isHitTop = true;
//
//	// ������
//	posBuff = pos + Vec2<float>(scale.x - offset, 0);
//	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(posBuff, scale, mapData, INTERSECTED_BOTTOM);
//	pos.y = posBuff.y;
//	if (!isHitBottom && intersectedLine == INTERSECTED_BOTTOM) isHitBottom = true;
//	posBuff = pos - Vec2<float>(scale.x - offset, 0);
//	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(posBuff, scale, mapData, INTERSECTED_BOTTOM);
//	pos.y = posBuff.y;
//	if (!isHitBottom && intersectedLine == INTERSECTED_BOTTOM) isHitBottom = true;
//	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(pos, scale, mapData, INTERSECTED_BOTTOM);
//	if (!isHitBottom && intersectedLine == INTERSECTED_BOTTOM) isHitBottom = true;
//
//	// �E����
//	posBuff = pos + Vec2<float>(0, scale.y - offset);
//	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(posBuff, scale, mapData, INTERSECTED_RIGHT);
//	pos.x = posBuff.x;
//	if (!isHitRight && intersectedLine == INTERSECTED_RIGHT) isHitRight = true;
//	posBuff = pos - Vec2<float>(0, scale.y - offset);
//	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(posBuff, scale, mapData, INTERSECTED_RIGHT);
//	pos.x = posBuff.x;
//	if (!isHitRight && intersectedLine == INTERSECTED_RIGHT) isHitRight = true;
//	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(pos, scale, mapData, INTERSECTED_RIGHT);
//	if (!isHitRight && intersectedLine == INTERSECTED_RIGHT) isHitRight = true;
//
//	// ������
//	posBuff = pos + Vec2<float>(0, scale.y - offset);
//	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(posBuff, scale, mapData, INTERSECTED_LEFT);
//	pos.x = posBuff.x;
//	if (!isHitLeft && intersectedLine == INTERSECTED_LEFT) isHitLeft = true;
//	posBuff = pos - Vec2<float>(0, scale.y - offset);
//	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(posBuff, scale, mapData, INTERSECTED_LEFT);
//	pos.x = posBuff.x;
//	if (!isHitLeft && intersectedLine == INTERSECTED_LEFT) isHitLeft = true;
//	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(pos, scale, mapData, INTERSECTED_LEFT);
//	if (!isHitLeft && intersectedLine == INTERSECTED_LEFT) isHitLeft = true;
//
//
//	// �}�b�v�`�b�v�Ɠ������Ă�����
//	if (isHitTop || isHitRight || isHitLeft || isHitBottom) {
//
//		// �}�b�v�`�b�v�Ɠ������Ă���ꏊ�ɂ���āA�����������Ă��邩�ǂ����𒲂ׂ�B
//
//		// �v���C���[�Ƃ̊p�x
//		float playerAngle = atan2(playerPos.y - pos.y, playerPos.x - pos.x);
//		playerAngle = fabs(playerAngle);
//
//		if (isHitTop) {
//
//			// �}�b�v�`�b�v�̏�ɂ������Ă����Ƃ������Ƃ́A�v���C���[���������ɂ����OK�I
//			// �������̋�̓I�Ȓl��
//			const float MIN_ANGLE = 0.785398f;
//			const float MAX_ANGLE = 2.35619f;
//
//			// �p�x�����̒l�̊Ԃ�������
//			if (MIN_ANGLE <= playerAngle && playerAngle <= MAX_ANGLE) {
//
//				// �����������Ă���B
//				isHitMapChip = true;
//				intersectedBuff = INTERSECTED_BOTTOM;
//
//			}
//
//		}
//
//		if (isHitBottom) {
//
//			// �}�b�v�`�b�v�̉��ɂ������Ă����Ƃ������Ƃ́A�v���C���[��������ɂ����OK�I
//			// ������̋�̓I�Ȓl��
//			const float MIN_ANGLE = 3.92699f;
//			const float MAX_ANGLE = 5.49779f;
//
//			// �p�x�����̒l�̊Ԃ�������
//			if (MIN_ANGLE <= playerAngle && playerAngle <= MAX_ANGLE) {
//
//				// �����������Ă���B
//				isHitMapChip = true;
//				intersectedBuff = INTERSECTED_TOP;
//
//			}
//
//		}
//
//		if (isHitRight) {
//
//			// �}�b�v�`�b�v�̉E�ɂ������Ă����Ƃ������Ƃ́A�v���C���[���������ɂ����OK�I
//			// �������̋�̓I�Ȓl��
//			const float MIN_ANGLE = 2.35619f;
//			const float MAX_ANGLE = 3.92699f;
//
//			// �p�x�����̒l�̊Ԃ�������
//			if (MIN_ANGLE <= playerAngle && playerAngle <= MAX_ANGLE) {
//
//				// �����������Ă���B
//				isHitMapChip = true;
//				intersectedBuff = INTERSECTED_RIGHT;
//
//			}
//
//		}
//
//		if (isHitLeft) {
//
//			// �}�b�v�`�b�v�̍��ɂ������Ă����Ƃ������Ƃ́A�v���C���[���E�����ɂ����OK�I
//			// �E�����̋�̓I�Ȓl��
//			const float MIN_ANGLE = 0.785398f;
//			const float MAX_ANGLE = 5.49779f;
//
//			// �p�x�����̒l�̊Ԃ�������
//			if (MAX_ANGLE <= playerAngle || playerAngle <= MIN_ANGLE) {
//
//				// �����������Ă���B
//				isHitMapChip = true;
//				intersectedBuff = INTERSECTED_LEFT;
//
//			}
//
//		}
//
//		// Swing�̃t���[�����S���o�߂��Ă��Ȃ������珈�����΂��B
//		if (SwingMgr::Instance()->easingTimer <= 0.05f) {
//
//		}
//		// ���ȉ���������_���[�W��^���Ȃ��B
//		if (SwingMgr::Instance()->easingTimer <= 0.15f) {
//
//			// �U��񂳂�Ă����Ԃ�������A�V�F�C�N�𔭐������ĐU��񂵏�Ԃ���������B
//			Vec2<float>vec = { 0,0 };
//			if (SwingMgr::Instance()->isSwingPlayer || OFFSET_INERTIA / 2.0f < afterSwingDelay) {
//
//				if (isHitLeft)vec.x = -1.0f;
//				else if (isHitRight)vec.x = 1.0f;
//				if (isHitTop)vec.y = -1.0f;
//				else if (isHitBottom)vec.y = 1.0f;
//
//				/*Crash(vec);
//
//				SuperiorityGauge::Instance()->AddPlayerGauge(5.0f);*/
//				SwingMgr::Instance()->isSwingPlayer = false;
//
//			}
//
//		}
//		else {
//
//			// �U��񂳂�Ă����Ԃ�������A�V�F�C�N�𔭐������ĐU��񂵏�Ԃ���������B
//			Vec2<float>vec = { 0,0 };
//			if (SwingMgr::Instance()->isSwingPlayer || OFFSET_INERTIA / 2.0f < afterSwingDelay) {
//
//				if (isHitLeft)vec.x = -1.0f;
//				else if (isHitRight)vec.x = 1.0f;
//				if (isHitTop)vec.y = -1.0f;
//				else if (isHitBottom)vec.y = 1.0f;
//
//				Crash(vec);
//
//				SuperiorityGauge::Instance()->AddPlayerGauge(10.0f);
//				SwingMgr::Instance()->isSwingPlayer = false;
//
//			}
//
//		}
//
//	}
//
//	// �}�b�v�`�b�v�ɂ������Ă����Ԃŉ�ʊO�ɏo����
//	if (isHitMapChip && stuckWindowTimer <= 0) {
//
//		Vec2<int> windowSize = WinApp::Instance()->GetWinCenter();
//		windowSize *= Vec2<int>(2, 2);
//
//		// �{�X�ƃv���C���[�̋���
//		float distanceX = fabs(lineCenterPos.x - pos.x);
//		float disntaceY = fabs(lineCenterPos.y - pos.y);
//
//		// �E�B���h�E���E
//		bool winLeft = pos.x - scale.x - ScrollMgr::Instance()->scrollAmount.x <= 0;
//		bool winRight = windowSize.x <= pos.x + scale.x - ScrollMgr::Instance()->scrollAmount.x;
//		if (winRight || winLeft) {
//
//			stuckWindowTimer = STRUCK_WINDOW_TIMER;
//			Crash({ winRight ? 1.0f : -1.0f , 0.0f });
//			SuperiorityGauge::Instance()->AddPlayerGauge(10);
//
//		}
//		// �E�B���h�E�㉺
//		bool winTop = pos.y - scale.y - ScrollMgr::Instance()->scrollAmount.y <= 0;
//		bool winBottom = windowSize.y <= pos.y + scale.y - ScrollMgr::Instance()->scrollAmount.y;
//		if (winBottom || winTop) {
//
//			stuckWindowTimer = STRUCK_WINDOW_TIMER;
//			Crash({ 0.0f,winBottom ? 1.0f : -1.0f });
//			SuperiorityGauge::Instance()->AddPlayerGauge(10);
//
//		}
//
//	}
//
//	prevIntersectedLine = intersectedBuff;
//
//	if (0 < stuckWindowTimer) {
//
//		isHitMapChip = false;
//		--stuckWindowTimer;
//
//	}
//
//}

bool Boss::Appear()
{
	if (20 <= moveTimer)
	{
		return true;
	}

	if (SCALE.x < size.x && SCALE.y < size.y)
	{
		float time = 30.0f;
		size -= initScale / time;
	}
	else
	{
		if (!initPaticleFlag)
		{
			Vec2<float>radian(cosf(Angle::ConvertToRadian(0.0f)), sinf(Angle::ConvertToRadian(0.0f)));
			ParticleMgr::Instance()->Generate(pos, radian, BULLET);

			radian = { cosf(Angle::ConvertToRadian(90.0f)), sinf(Angle::ConvertToRadian(90.0f)) };
			ParticleMgr::Instance()->Generate(pos, radian, BULLET);

			radian = { cosf(Angle::ConvertToRadian(180.0f)), sinf(Angle::ConvertToRadian(180.0f)) };
			ParticleMgr::Instance()->Generate(pos, radian, BULLET);

			radian = { cosf(Angle::ConvertToRadian(270.0f)), sinf(Angle::ConvertToRadian(270.0f)) };
			ParticleMgr::Instance()->Generate(pos, radian, BULLET);
			initPaticleFlag = true;
		}

		size = { 80.0f,80.0f };
		++moveTimer;
	}

	return false;
}
