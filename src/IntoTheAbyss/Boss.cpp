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
#include"BossPatternSwing.h"
#include"BossBulletManager.h"
#include"CrashMgr.h"

#include"ParticleMgr.h"

#include"DebugParameter.h"

Boss::Boss()
{

	pos = {};
	scale = {};
	vel = {};
	moveVel = {};

	graphHandle[FRONT] = TexHandleMgr::LoadGraph("resource/ChainCombat/enemy.png");
	graphHandle[BACK] = TexHandleMgr::LoadGraph("resource/ChainCombat/enemy_back.png");

	areaHitBox.center = &pos;


	bossPattern[0] = std::make_unique<BossPatternNormalMove>();
	bossPattern[1] = std::make_unique<BossPatternAttack>();
	bossPattern[2] = std::make_unique<BossPatternSwing>();


	//�p�^�[���ɓn���f�[�^�̏�����
	patternData.moveVel = &moveVel;
	patternData.stuckWindowTimer = &stuckWindowTimer;

	bossPatternNow = BOSS_PATTERN_NORMALMOVE;
	patternTimer = 0;

	bulletHitBox = std::make_unique<SphereCollision>();
	bulletHitBox->center = &pos;
	bulletHitBox->radius = 40.0f;

	//Init();
}

void Boss::Init()
{
	pos = {};
	scale = {};
	vel = {};
	moveVel = {};
	crashDevice.Init();
}

void Boss::Generate(const Vec2<float> &generatePos)
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
	crashDevice.Update();

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
		else if (isSwingNow && SwingMgr::Instance()->isSwingPlayer) {

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
			AiPattern();
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
					BossBulletManager::Instance()->Generate(pos, patternData.bulltData[i].dir, patternData.bulltData[i].speed);
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

	auto drawPos = pos + crashDevice.GetShake();
	auto drawScale = scale * crashDevice.GetExtRate();
	static auto CRASH_TEX = D3D12App::Instance()->GenerateTextureBuffer(Color(255, 0, 0, 255));
	DrawFunc_FillTex::DrawExtendGraph2D(ScrollMgr::Instance()->Affect(drawPos - drawScale), ScrollMgr::Instance()->Affect(drawPos + drawScale),
		TexHandleMgr::GetTexBuffer(graphHandle[dir]), CRASH_TEX, crashDevice.GetFlashAlpha());
}

void Boss::CheckHit(const vector<vector<int>> &mapData, bool &isHitMapChip, const Vec2<float> &playerPos, const Vec2<float> &lineCenterPos)
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

			Vec2<bool>ext = { true,true };
			if (!isHitLeft && !isHitRight)ext.y = false;
			if (!isHitTop && !isHitBottom)ext.x = false;

			CrashMgr::Instance()->Crash(pos, crashDevice, ext);
			SuperiorityGauge::Instance()->AddPlayerGauge(DebugParameter::Instance()->gaugeData->swingDamageValue);
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
			CrashMgr::Instance()->Crash(pos, crashDevice, { false,true });
			SuperiorityGauge::Instance()->AddPlayerGauge(DebugParameter::Instance()->gaugeData->enemyClashDamageValue);
		}
		// �E�B���h�E�㉺
		if (windowSize.y <= pos.y + scale.y - ScrollMgr::Instance()->scrollAmount.y || pos.y - scale.y - ScrollMgr::Instance()->scrollAmount.y <= 0) {

			stuckWindowTimer = STRUCK_WINDOW_TIMER;
			CrashMgr::Instance()->Crash(pos, crashDevice, { true,false });
			SuperiorityGauge::Instance()->AddPlayerGauge(DebugParameter::Instance()->gaugeData->enemyClashDamageValue);

		}


	}

	prevIntersectedLine = intersectedBuff;

	if (0 < stuckWindowTimer) {

		isHitMapChip = false;

	}

}

void Boss::AiPattern()
{
	//120Flame���Ƃɂǂ̍s���p�^�[����I�����邩���߂�

	++patternTimer;
	if (120 <= patternTimer)
	{
		//�v���C���[�ƃ{�X�Ƃ̕����x�N�g���m�F
		Vec2<float>dir = SwingMgr::Instance()->playerPos - SwingMgr::Instance()->bossPos;
		float distance = dir.y - (dir.y * -1.0f);

		bool allowToSwingFlag = false;
		if (SWING_DISTANCE_DEADLINE <= fabs(distance))
		{
			allowToSwingFlag = true;
		}


		//���ɂǂ̃p�^�[����I�����邩
		//�ꕔ�̏����Ńp�^�[����I�����Ȃ��悤�ɂ���
		while (1)
		{
			int randomPattern = KuroFunc::GetRand(BOSS_PATTERN_MAX - 1);

			//�U��񂵋��Ȃ��ŐU��񂻂��Ƃ������蒼��
			if (!allowToSwingFlag && randomPattern == BOSS_PATTERN_SWING)
			{
				continue;
			}
			bossPatternNow = static_cast<E_BossPattern>(randomPattern);
			break;
		}

		patternTimer = 0;
	}
}
