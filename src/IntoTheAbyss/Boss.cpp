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
#include"BossPatternNormalMove.h"
#include"BossPatternAttack.h"
#include"BossBulletManager.h"
#include"CrashMgr.h"

#include"ParticleMgr.h"

void Boss::Crash(const Vec2<float>& MyVec)
{
	Vec2<bool>ext = { true,true };
	if (MyVec.x == 0.0f)ext.y = false;
	if (MyVec.y == 0.0f)ext.x = false;

	Vec2<float>smokeVec;
	smokeVec = { 0,0 };

	if (0.0f < MyVec.x)smokeVec.x = -1.0f;
	else if (MyVec.x < 0.0f)smokeVec.x = 1.0f;
	if (0.0f < MyVec.y)smokeVec.y = -1.0f;
	else if (MyVec.y < 0.0f)smokeVec.y = 1.0f;

	CrashMgr::Instance()->Crash(pos, stagingDevice, ext, smokeVec);
}

Boss::Boss()
{

	graphHandle[FRONT] = TexHandleMgr::LoadGraph("resource/ChainCombat/enemy.png");
	graphHandle[BACK] = TexHandleMgr::LoadGraph("resource/ChainCombat/enemy_back.png");

	areaHitBox.center = &pos;


	bossPattern[0] = std::make_unique<BossPatternNormalMove>();
	bossPattern[1] = std::make_unique<BossPatternAttack>();


	//�p�^�[���ɓn���f�[�^�̏�����
	patternData.bossPos = &moveVel;
	patternData.stuckWindowTimer = &stuckWindowTimer;

	bossPatternNow = BOSS_PATTERN_NORMALMOVE;
	patternTimer = 0;

	bulletHitBox = std::make_unique<SphereCollision>();
	bulletHitBox->center = &pos;
	bulletHitBox->radius = 40.0f;

	Init();
}

void Boss::Init()
{
	pos = {};
	scale = {};
	vel = {};
	moveVel = {};
	stagingDevice.Init();
}

void Boss::Generate(const Vec2<float>& generatePos)
{

	/*===== �������� =====*/

	pos = generatePos;
	initScale = SCALE * 5.0f;
	scale = initScale;
	vel = { OFFSET_VEL,0 };
	moveVel = { OFFSET_VEL,0 };
	swingInertia = 0;
	swingInertiaVec = {};
	afterSwingDelay = 0;
	prevIntersectedLine = INTERSECTED_NONE;
	stuckWindowTimer = 0;
	areaHitBox.size = scale;
	allowToMoveFlag = false;
	moveTimer = 0;
	initPaticleFlag = false;
}

#include"Camera.h"
void Boss::Update()
{
	/*===== �X�V���� =====*/
	stagingDevice.Update();

	// �O�t���[���̍��W��ۑ�
	prevPos = pos;

	if (80.0f < scale.x && 80.0f < scale.y)
	{
		float time = 30.0f;
		scale -= initScale / time;
	}

	if (scale.x <= 80.0f && scale.y <= 80.0f)
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

		scale = { 80.0f,80.0f };
		++moveTimer;
	}
	if (20 <= moveTimer)
	{
		allowToMoveFlag = true;
	}


	//�ʏ�T�C�Y�ɂȂ�܂œ����Ȃ�
	if (scale.x <= SCALE.x && scale.y <= SCALE.y && !readyToStartRoundEffectFlag)
	{
		if (Camera::Instance()->Active())
		{
			moveVel = { 0,0 };
			return;
		}

		for (int i = 0; i < patternData.bulltData.size(); ++i)
		{
			if (patternData.bulltData[i].initFlag)
			{
				patternData.bulltData[i].Reset();
			}
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

			//�{�X��AI-----------------------
			++patternTimer;
			if (120 <= patternTimer)
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
			//�{�X��AI-----------------------




			//�{�X�̋���
			if (bossPatternNow != oldBossPattern)
			{
				bossPattern[bossPatternNow]->Init();
			}
			bossPattern[bossPatternNow]->Update(&patternData);
			oldBossPattern = bossPatternNow;
			if (UsersInput::Instance()->Input(DIK_0)) {
				vel.x = OFFSET_VEL * 5.0f;
			}

			//�{�X�̒e
			for (int i = 0; i < patternData.bulltData.size(); ++i)
			{
				if (patternData.bulltData[i].initFlag)
				{
					//BossBulletManager::Instance()->Generate(pos, patternData.bulltData[i].dir, patternData.bulltData[i].speed);
				}
			}
		}
		// �ړ��ʂ̑��ʂ����߂�B

		if (0 < afterSwingDelay) {

			// �U��񂵒���̍d�����c���Ă���ꍇ�́A�����݂̂��ړ��ʂƂ���B
			vel = swingInertiaVec * swingInertia;

		}
		else {

			vel = moveVel + swingInertiaVec * swingInertia;

		}
	}


	BossBulletManager::Instance()->Update();

}

#include"DrawFunc_FillTex.h"
#include"D3D12App.h"
void Boss::Draw()
{
	/*===== �`�揈�� =====*/
	BossBulletManager::Instance()->Draw();
	//DrawFunc::DrawBox2D(pos - scale - scrollShakeAmount, pos + scale - scrollShakeAmount, Color(230, 38, 113, 255), DXGI_FORMAT_R8G8B8A8_UNORM, true);
	DIR dir = FRONT;
	if (vel.y < 0)dir = BACK;

	auto drawPos = pos + stagingDevice.GetShake();
	auto drawScale = scale * stagingDevice.GetExtRate();
	static auto CRASH_TEX = D3D12App::Instance()->GenerateTextureBuffer(Color(255, 0, 0, 255));
	DrawFunc_FillTex::DrawExtendGraph2D(ScrollMgr::Instance()->Affect(drawPos - drawScale), ScrollMgr::Instance()->Affect(drawPos + drawScale),
		TexHandleMgr::GetTexBuffer(graphHandle[dir]), CRASH_TEX, stagingDevice.GetFlashAlpha());

	DrawFunc::DrawBox2D(ScrollMgr::Instance()->Affect(drawPos - scale), ScrollMgr::Instance()->Affect(drawPos + scale), Color(), DXGI_FORMAT_R8G8B8A8_UNORM);
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

	// ����
	Vec2<float> velPosBuff = pos - scale;
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheVel(velPosBuff, prevPos - scale, {}, {}, onGround, mapData, false);
	pos = velPosBuff + scale;
	if (intersectedLine == INTERSECTED_TOP) isHitTop = true;
	if (intersectedLine == INTERSECTED_RIGHT) isHitRight = true;
	if (intersectedLine == INTERSECTED_LEFT) isHitLeft = true;
	if (intersectedLine == INTERSECTED_BOTTOM) isHitBottom = true;

	// �E��
	velPosBuff = pos + Vec2<float>(scale.x, -scale.y);
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheVel(velPosBuff, prevPos + Vec2<float>(scale.x, -scale.y), {}, {}, onGround, mapData, false);
	pos = velPosBuff - Vec2<float>(scale.x, -scale.y);
	if (intersectedLine == INTERSECTED_TOP) isHitTop = true;
	if (intersectedLine == INTERSECTED_RIGHT) isHitRight = true;
	if (intersectedLine == INTERSECTED_LEFT) isHitLeft = true;
	if (intersectedLine == INTERSECTED_BOTTOM) isHitBottom = true;

	// �E��
	velPosBuff = pos + scale;
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheVel(velPosBuff, prevPos + scale, {}, {}, onGround, mapData, false);
	pos = velPosBuff - scale;
	if (intersectedLine == INTERSECTED_TOP) isHitTop = true;
	if (intersectedLine == INTERSECTED_RIGHT) isHitRight = true;
	if (intersectedLine == INTERSECTED_LEFT) isHitLeft = true;
	if (intersectedLine == INTERSECTED_BOTTOM) isHitBottom = true;

	// ����
	velPosBuff = pos + Vec2<float>(-scale.x, scale.y);
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheVel(velPosBuff, prevPos + Vec2<float>(-scale.x, scale.y), {}, {}, onGround, mapData, false);
	pos = velPosBuff - Vec2<float>(-scale.x, scale.y);
	if (intersectedLine == INTERSECTED_TOP) isHitTop = true;
	if (intersectedLine == INTERSECTED_RIGHT) isHitRight = true;
	if (intersectedLine == INTERSECTED_LEFT) isHitLeft = true;
	if (intersectedLine == INTERSECTED_BOTTOM) isHitBottom = true;

	// �X�P�[�������ɂ����}�b�v�`�b�v�Ƃ̓����蔻����s���B

	// �����
	float offset = 5.0f;
	Vec2<float> posBuff = pos + Vec2<float>(scale.x - offset, 0);
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(posBuff, scale, mapData, INTERSECTED_TOP);
	pos.y = posBuff.y;
	if (!isHitTop && intersectedLine == INTERSECTED_TOP) isHitTop = true;
	posBuff = pos - Vec2<float>(scale.x - offset, 0);
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(posBuff, scale, mapData, INTERSECTED_TOP);
	pos.y = posBuff.y;
	if (!isHitTop && intersectedLine == INTERSECTED_TOP) isHitTop = true;
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(pos, scale, mapData, INTERSECTED_TOP);
	if (!isHitTop && intersectedLine == INTERSECTED_TOP) isHitTop = true;

	// ������
	posBuff = pos + Vec2<float>(scale.x - offset, 0);
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(posBuff, scale, mapData, INTERSECTED_BOTTOM);
	pos.y = posBuff.y;
	if (!isHitBottom && intersectedLine == INTERSECTED_BOTTOM) isHitBottom = true;
	posBuff = pos - Vec2<float>(scale.x - offset, 0);
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(posBuff, scale, mapData, INTERSECTED_BOTTOM);
	pos.y = posBuff.y;
	if (!isHitBottom && intersectedLine == INTERSECTED_BOTTOM) isHitBottom = true;
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(pos, scale, mapData, INTERSECTED_BOTTOM);
	if (!isHitBottom && intersectedLine == INTERSECTED_BOTTOM) isHitBottom = true;

	// �E����
	posBuff = pos + Vec2<float>(0, scale.y - offset);
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(posBuff, scale, mapData, INTERSECTED_RIGHT);
	pos.x = posBuff.x;
	if (!isHitRight && intersectedLine == INTERSECTED_RIGHT) isHitRight = true;
	posBuff = pos - Vec2<float>(0, scale.y - offset);
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(posBuff, scale, mapData, INTERSECTED_RIGHT);
	pos.x = posBuff.x;
	if (!isHitRight && intersectedLine == INTERSECTED_RIGHT) isHitRight = true;
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(pos, scale, mapData, INTERSECTED_RIGHT);
	if (!isHitRight && intersectedLine == INTERSECTED_RIGHT) isHitRight = true;

	// ������
	posBuff = pos + Vec2<float>(0, scale.y - offset);
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(posBuff, scale, mapData, INTERSECTED_LEFT);
	pos.x = posBuff.x;
	if (!isHitLeft && intersectedLine == INTERSECTED_LEFT) isHitLeft = true;
	posBuff = pos - Vec2<float>(0, scale.y - offset);
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(posBuff, scale, mapData, INTERSECTED_LEFT);
	pos.x = posBuff.x;
	if (!isHitLeft && intersectedLine == INTERSECTED_LEFT) isHitLeft = true;
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(pos, scale, mapData, INTERSECTED_LEFT);
	if (!isHitLeft && intersectedLine == INTERSECTED_LEFT) isHitLeft = true;

	// �{�X�̂����߂��Ƀ}�b�v�`�b�v�����邩�ǂ�������������B
	float sarchOffset = 10.0f;
	int sarchChipX = 0;
	int sarchChipY = 0;
	const int MAP_HEIGHT = mapData.size();

	// ��
	sarchChipY = (pos.y - scale.y - sarchOffset + MAP_CHIP_HALF_SIZE) / MAP_CHIP_SIZE;
	if (sarchChipY < 0 || MAP_HEIGHT <= sarchChipY) sarchChipY = 0;
	sarchChipX = pos.x / MAP_CHIP_SIZE;
	if (0 < mapData[sarchChipY][sarchChipX] && mapData[sarchChipY][sarchChipX] < 10) {
		isHitTop = true;
	}
	// ��
	sarchChipY = (pos.y + scale.y + sarchOffset + MAP_CHIP_HALF_SIZE) / MAP_CHIP_SIZE;
	if (sarchChipY < 0 || MAP_HEIGHT <= sarchChipY) sarchChipY = 0;
	sarchChipX = pos.x / MAP_CHIP_SIZE;
	if (0 < mapData[sarchChipY][sarchChipX] && mapData[sarchChipY][sarchChipX] < 10) {
		isHitBottom = true;
	}
	// ��
	sarchChipY = pos.y / MAP_CHIP_SIZE;
	sarchChipX = (pos.x - scale.x - sarchOffset + MAP_CHIP_HALF_SIZE) / MAP_CHIP_SIZE;
	if (sarchChipX < 0 || mapData[sarchChipY].size() <= sarchChipX) sarchChipX = 0;
	if (0 < mapData[sarchChipY][sarchChipX] && mapData[sarchChipY][sarchChipX] < 10) {
		isHitRight = true;
	}
	// �E
	sarchChipY = pos.y / MAP_CHIP_SIZE;
	sarchChipX = (pos.x + scale.x + sarchOffset + MAP_CHIP_HALF_SIZE) / MAP_CHIP_SIZE;
	if (sarchChipX < 0 || mapData[sarchChipY].size() <= sarchChipX) sarchChipX = 0;
	if (0 < mapData[sarchChipY][sarchChipX] && mapData[sarchChipY][sarchChipX] < 10) {
		isHitLeft = true;
	}


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
			if (MAX_ANGLE <= playerAngle || playerAngle <= MIN_ANGLE) {

				// �����������Ă���B
				isHitMapChip = true;
				intersectedBuff = INTERSECTED_LEFT;

			}

		}

		// �U��񂳂�Ă����Ԃ�������A�V�F�C�N�𔭐������ĐU��񂵏�Ԃ���������B
		Vec2<float>vec = { 0,0 };
		if (SwingMgr::Instance()->isSwingPlayer || OFFSET_INERTIA / 2.0f < afterSwingDelay) {

			if (isHitLeft)vec.x = -1.0f;
			else if (isHitRight)vec.x = 1.0f;
			if (isHitTop)vec.y = -1.0f;
			else if (isHitBottom)vec.y = 1.0f;

			Crash(vec);

			SuperiorityGauge::Instance()->AddPlayerGauge(5.0f);
			SwingMgr::Instance()->isSwingPlayer = false;

		}

	}

	// �}�b�v�`�b�v�ɂ������Ă����Ԃŉ�ʊO�ɏo����
	if (isHitMapChip && stuckWindowTimer <= 0) {

		Vec2<int> windowSize = WinApp::Instance()->GetWinCenter();
		windowSize *= Vec2<int>(2, 2);

		// �{�X�ƃv���C���[�̋���
		float distanceX = fabs(lineCenterPos.x - pos.x);
		float disntaceY = fabs(lineCenterPos.y - pos.y);

		// �E�B���h�E���E
		bool winLeft = pos.x - scale.x - ScrollMgr::Instance()->scrollAmount.x <= 0;
		bool winRight = windowSize.x <= pos.x + scale.x - ScrollMgr::Instance()->scrollAmount.x;
		if (winRight || winLeft) {

			stuckWindowTimer = STRUCK_WINDOW_TIMER;
			Crash({ winRight ? 1.0f : -1.0f , 0.0f });
			SuperiorityGauge::Instance()->AddPlayerGauge(10);

		}
		// �E�B���h�E�㉺
		bool winTop = pos.y - scale.y - ScrollMgr::Instance()->scrollAmount.y <= 0;
		bool winBottom = windowSize.y <= pos.y + scale.y - ScrollMgr::Instance()->scrollAmount.y;
		if (winBottom || winTop) {

			stuckWindowTimer = STRUCK_WINDOW_TIMER;
			Crash({ 0.0f,winBottom ? 1.0f : -1.0f });
			SuperiorityGauge::Instance()->AddPlayerGauge(10);

		}

	}

	prevIntersectedLine = intersectedBuff;

	if (0 < stuckWindowTimer) {

		isHitMapChip = false;
		--stuckWindowTimer;

	}

}