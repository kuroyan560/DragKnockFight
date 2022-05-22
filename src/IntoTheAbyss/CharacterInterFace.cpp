#include "CharacterInterFace.h"
#include"CrashMgr.h"
#include"WinApp.h"
#include"SuperiorityGauge.h"
#include"ScrollMgr.h"
#include"DebugParameter.h"
#include"StunEffect.h"
#include"FaceIcon.h"
#include "ResultTransfer.h"
#include "AfterImage.h"
#include "CrashEffectMgr.h"
#include "Stamina.h"
#include"CharacterManager.h"

const Color CharacterInterFace::TEAM_COLOR[TEAM_NUM] =
{
	Color(47,255,139,255),
	Color(239,1,144,255)
};

void CharacterInterFace::SwingUpdate()
{
	ADD_SWING_ANGLE = DebugParameter::Instance()->GetBossData().swingAngle;
	MAX_SWING_ANGLE = DebugParameter::Instance()->GetBossData().swingMax;

	/*===== �U��񂵒��ɌĂ΂�鏈�� =====*/

	// �p�x�ɉ��Z����ʂ��X�V�B
	addSwingAngle += ADD_SWING_ANGLE * addSwingRate;

	// �U��񂵂̌o�ߎ��Ԃ�ݒ�B
	++swingTimer;

	// ���̃^�C�~���O�ŃX�^�~�i�������B
	if (swingTimer == 5) {
		//����͂���ŊԂɍ��킹��B
		if (CharacterManager::Instance()->Right()->GetCharacterName() == PLAYABLE_BOSS_0)
		{
			// �X�^�~�i������
			staminaGauge->ConsumesStamina(DebugParameter::Instance()->GetBossData().staminaSwing);
		}
		else
		{
			// �X�^�~�i������
			staminaGauge->ConsumesStamina(SWING_STAMINA);
		}
	}

	// ���E�𒴂��Ă�����C���B
	if (MAX_SWING_ANGLE < addSwingAngle) {

		addSwingAngle = MAX_SWING_ANGLE;

	}

	// ���݂̊p�x�����߂�B
	float nowAngle = atan2f(GetPartnerPos().y - pos.y, GetPartnerPos().x - pos.x);

	// �p�x��-�������狸������B
	if (nowAngle < 0) {

		float angleBuff = 3.14f - fabs(nowAngle);

		nowAngle = 3.14f + angleBuff;

	}

	// ���݂̊p�x�Ɋp�x�̉��Z�ʂ𑫂��B
	if (isSwingClockWise) {

		// ���v��肾������
		nowAngle += addSwingAngle;

	}
	else {

		// �����v��肾������
		nowAngle -= addSwingAngle;

	}

	// ���̊p�x�����݂̃x�N�g���Ƃ��Ďg�p����B
	nowSwingVec = { cosf(nowAngle), sinf(nowAngle) };

	// �����������ŐU��񂵂Ă��܂��I
	float partnerDistance = (pos - partner.lock()->pos).Length();

	partner.lock()->pos = pos + nowSwingVec * partnerDistance;

	// ���݂̃x�N�g���ƍŏI�ڕW�̃x�N�g���ŊO�ς̍��E������s���A�ʂ�z���Ă�����U��񂵂��I����B ���̒l����(�����v���)�A���̒l���E(���v���)�B
	float crossResult = nowSwingVec.Cross(swingTargetVec);

	// [�ŏ������v���] ���� [���݂������v���] ��������
	if (isSwingClockWise && crossResult < 0 && 15 < swingTimer) {

		// �p�[�g�i�[�Ɋ�����^����B
		partner.lock()->vel = (partner.lock()->pos - partner.lock()->prevPos) * 0.5f;

		// �U��񂵏I���I
		FinishSwing();
	}
	// [�ŏ��������v���] ���� [���݂����v���] ��������
	if (!isSwingClockWise && 0 < crossResult && 15 < swingTimer) {

		// �p�[�g�i�[�Ɋ�����^����B
		partner.lock()->vel = (partner.lock()->pos - partner.lock()->prevPos) * 0.5f;

		// �U��񂵏I���I
		FinishSwing();
	}


	// �R���ݏ�Ԃ��폜�B ���̐V�d�l�̐U��񂵂ōs���Ƃ����炱�̕ϐ��͂���Ȃ��̂ō폜����B
	//isHold = false;

}

void CharacterInterFace::Crash(const Vec2<float>& MyVec, const int& SmokeCol)
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

	CrashMgr::Instance()->Crash(pos, stagingDevice, ext, smokeVec, SmokeCol);
	//SuperiorityGauge::Instance()->AddGauge(team, -10.0f);
	stagingDevice.StopSpin();

	OnCrash();
	partner.lock()->OnPartnerCrash();

	if (stanTimer)return;	//�X�^�����Ȃ�_���[�W�ɂ���ύX�Ȃ�
	static const int DAMAGE_TOTAL_TIME = 90;

	damageTimer = DAMAGE_TOTAL_TIME;
	FaceIcon::Instance()->Change(team, FACE_STATUS::DAMAGE);
}

void CharacterInterFace::CrashUpdate()
{
	//�U��񂳂�Ă���
	//if (partner.lock()->nowSwing)
	//{
	//	//���ʐU��񂵂���
	//	static const float CRASH_SWING_RATE = 0.15f;
	//	if (ADD_SWING_ANGLE * 2.0f < partner.lock()->addSwingAngle)
	//	{
	//		Vec2<float>vec = { 0,0 };
	//		if (mapChipHit[LEFT])vec.x = -1.0f;
	//		else if (mapChipHit[RIGHT])vec.x = 1.0f;
	//		if (mapChipHit[TOP])vec.y = -1.0f;
	//		else if (mapChipHit[BOTTOM])vec.y = 1.0f;

	//		Crash(vec);
	//	}

	//	partner.lock()->FinishSwing();
	//}
}

void CharacterInterFace::SwingPartner(const Vec2<float>& SwingTargetVec, const bool& IsClockWise)
{

	// �U��񂵂̗\�������X�V����ۂɎg�p����ϐ����Z�b�g�B
	CWSwingSegmentMgr.SetCharaStartPos(pos);
	CCWSwingSegmentMgr.SetCharaStartPos(pos);

	static const int SE = AudioApp::Instance()->LoadAudio("resource/ChainCombat/sound/swing.wav", 0.13f);


	//�U��񂵏��������ɑ����Ă���ꍇ�́A�d�˂ĐU��񂹂Ȃ�
	if (partner.lock()->nowSwing || nowSwing)return;

	//�p�C���b�g�؂藣�����Ȃ�g���Ȃ�
	if (isPilotDetached)return;

	partner.lock()->OnSwinged();

	AudioApp::Instance()->PlayWave(SE);

	// �ڕW�n�_�̃x�N�g����ۑ��B
	swingTargetVec = SwingTargetVec;

	// ���݂̃t���[���̑����Ƃ̃x�N�g�������߂�B
	nowSwingVec = GetPartnerPos() - pos;
	nowSwingVec.Normalize();

	float pi45 = Angle::ConvertToRadian(42);

	// �O�ς̍��E����ɂ��ǂ��瑤�ɐU��񂷂��𔻒f����B ���̒l�����A���̒l���E�B
	float crossResult = nowSwingVec.Cross(swingTargetVec);
	if (!IsClockWise) {

		// �}�C�i�X�Ȃ̂ō�(�����v���)�B
		isSwingClockWise = false;

		// ���v���̕��ɖڕW�n�_�x�N�g�����C���B
		float targetAngle = atan2f(SwingTargetVec.y, SwingTargetVec.x);

		targetAngle += pi45;

		//swingTargetVec = {cosf(targetAngle),sinf(targetAngle)};

	}
	else if (IsClockWise) {

		// �v���X�Ȃ̂ŉE(���v���)�B
		isSwingClockWise = true;

		// ���v���̕��ɖڕW�n�_�x�N�g�����C���B
		float targetAngle = atan2f(SwingTargetVec.y, SwingTargetVec.x);

		targetAngle -= pi45;

		//swingTargetVec = {cosf(targetAngle),sinf(targetAngle)};

	}

	// �p�x�ւ̉��Z�ʂ��������B
	addSwingAngle = 0;

	//�U��񂵃t���O�̗L����
	nowSwing = true;

	swingTimer = 0;

	partner.lock()->stagingDevice.StartSpin(isSwingClockWise);

	// �p�x�ɉ��Z����ʂ̊��������߂�B
	float partnerDistance = (pos - partner.lock()->pos).Length();

	const float MAX_LENGTH = 150.0f;

	// �������K��l�ȏゾ������1.0f��������B
	if (MAX_LENGTH < partnerDistance) {

		addSwingRate = 1.0f;

	}
	else {

		// ���������߂�B
		addSwingRate = (partnerDistance / MAX_LENGTH) * 2.0f + 1.0f;

	}

}

void CharacterInterFace::SetPilotDetachedFlg(const bool& Flg)
{
	//�p�C���b�g�łȂ�����
	return;

	if (isPilotDetached == Flg)return;
	//�p�C���b�g�؂藣���ɂ͍Œ�ł��X�^�~�i�o�[�P���K�v
	if (Flg && !staminaGauge->CheckCanAction(1))return;

	static const float PILOT_RETURN_DIST_BASE = 128.0f;
	static const int PILOT_RETURN_TOTAL_TIME_BASE = 5;	// PILOT_RETURN_DIST_BASE �̋��������̃t���[���ŋA�鑬��

	//�؂藣�����u��
	if (Flg)
	{
		if (!IsPilotOutSide())
		{
			pilotPos = pos;	//���{�̍��W����X�^�[�g
		}
		OnPilotLeave();
	}
	//���{�ɖ߂鏈���̏�����
	else
	{
		pilotReturnTimer = 0;
		pilotReturnStartPos = pilotPos;
		pilotReturnTotalTime = max(PILOT_RETURN_TOTAL_TIME_BASE * (pilotPos.Distance(pos) / PILOT_RETURN_DIST_BASE), 1);
	}
	isPilotDetached = Flg;
}

void CharacterInterFace::SaveHitInfo(bool& isHitTop, bool& isHitBottom, bool& isHitLeft, bool& isHitRight, const INTERSECTED_LINE& intersectedLine, Vec2<int>& hitChipIndex, const Vec2<int>& hitChipIndexBuff)
{
	auto mapChipDrawData = StageMgr::Instance()->GetLocalDrawMap();
	(*mapChipDrawData)[hitChipIndexBuff.y][hitChipIndexBuff.x].shocked = 1.0f;

	if (intersectedLine == INTERSECTED_LINE::INTERSECTED_TOP)isHitTop = true;
	if (intersectedLine == INTERSECTED_LINE::INTERSECTED_BOTTOM) isHitBottom = true;
	if (intersectedLine == INTERSECTED_LINE::INTERSECTED_LEFT) isHitLeft = true;
	if (intersectedLine == INTERSECTED_LINE::INTERSECTED_RIGHT) isHitRight = true;
	if (intersectedLine != INTERSECTED_LINE::INTERSECTED_NONE) hitChipIndex = hitChipIndexBuff;
}

void CharacterInterFace::Appear()
{
	if (CompleteAppear())
	{
		return;
	}

	//�T�C�Y��1.0f�ɂȂ�܂œ����Ȃ�
	if (1.0f < appearExtRate.x && 1.0f < appearExtRate.y)
	{
		float time = 30.0f;
		appearExtRate.x -= INIT_SIZE / time;
		appearExtRate.y -= INIT_SIZE / time;
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

		appearExtRate = { 1.0f,1.0f };
		++moveTimer;
	}
}

void CharacterInterFace::InitSwingLineSegmetn()
{
	CWSwingSegmentMgr.Init();
	CCWSwingSegmentMgr.Init();
}

void CharacterInterFace::Init(const Vec2<float>& GeneratePos, const bool& Appear)
{
	if (pilotGraph != -1)
	{
		pilotSize = TexHandleMgr::GetTexBuffer(pilotGraph)->GetGraphSize().Float();
	}
	if (Appear)appearExtRate = { INIT_SIZE,INIT_SIZE };
	else appearExtRate = { 1.0f,1.0f };
	initPaticleFlag = false;
	moveTimer = 0;

	pos = GeneratePos;
	vel = { 0,0 };

	stackWindowTimer = 0;

	nowSwing = false;

	stackMapChip = false;
	for (auto& flg : mapChipHit)flg = false;

	stagingDevice.Init();

	addLineLength = 0.0f;

	//�o�ꉉ�o�̂��ߍŏ��͓����Ȃ�
	canMove = false;
	//�o�ꉉ�o�̂��ߍŏ��͓����蔻��Ƃ�Ȃ�
	hitCheck = false;

	bulletMgr.Init();

	stanTimer = 0;
	elecTimer = 0;

	stagingDevice.Init();

	OnInit();

	isHold = false;

	advancedEntrySwingTimer = 0;
	isAdvancedEntrySwing = false;

	damageTimer = 0;

	static const int RETICLE_GRAPH[TEAM_NUM] = { TexHandleMgr::LoadGraph("resource/ChainCombat/reticle_player.png"),TexHandleMgr::LoadGraph("resource/ChainCombat/reticle_enemy.png") };
	int team = GetWhichTeam();

	CWSwingSegmentMgr.Setting(true, rbHandle, arrowHandle, lineHandle, RETICLE_GRAPH[team]);
	CCWSwingSegmentMgr.Setting(false, lbHandle, arrowHandle, lineHandle, RETICLE_GRAPH[team]);
	isInputSwingRB = false;

	addSwingRate = 0;

	isPilotDetached = false;
	pilotReturnTimer = 0;
	pilotReturnTotalTime = 0;
	gaugeReturnTimer = 0;

	// �e�L�����ɂ���ăX�^�~�i�Q�[�W�̃f�t�H���g�ʂ����肷��\��B
	//staminaGauge = std::make_shared<StaminaMgr>();
	staminaGauge->Init();

	// �L�����Ɋ���ăX�^�~�i�Q�[�W�̐F��ݒ肷��B
	bool isLeft = GetWhichTeam() == WHICH_TEAM::LEFT_TEAM;
	Color innerColor = Color();
	Color outerColor = Color();
	if (isLeft) {

		// ��
		outerColor = Color(0x29, 0xC9, 0xB4, 0xFF);
		innerColor = Color(0x2F, 0xFF, 0x8B, 0xFF);

	}
	else {

		// ��
		outerColor = Color(0xA2, 0x1B, 0x6C, 0xFF);
		innerColor = Color(0xEF, 0x01, 0x90, 0xFF);

	}
	staminaGauge->SetColor(innerColor, outerColor);

	prevSwingFlag = false;


	bulletMgr.Init();
	barrage->Init();
	barrageDelayTimer = 0;

	healAuraEaseRate = 1.0f;
}

#include "SlowMgr.h"
void CharacterInterFace::Update(const std::vector<std::vector<int>>& MapData, const Vec2<float>& LineCenterPos, const bool& isRoundStartEffect)
{

	if (team == WHICH_TEAM::RIGHT_TEAM && 0.8f <= SlowMgr::Instance()->slowAmount) {

		// �e�̍X�V����
		bulletMgr.Update();

	}


	// �U��񂵒���������������X�V����B
	if (nowSwing) {

		CWSwingSegmentMgr.UpdateSwing(pos);
		CCWSwingSegmentMgr.UpdateSwing(pos);

	}

	// ���肪�U��񂵂Ă�����A������̏Ə��������B
	if (partner.lock()->GetNowSwing() || 0 < stanTimer || 0 < damageTimer) {
		CWSwingSegmentMgr.Init();
		CCWSwingSegmentMgr.Init();
	}

	//�X�^����ԍX�V
	if (stanTimer)
	{
		stanTimer--;
		//�X�^���I��
		if (stanTimer <= 0)
		{
			canMove = true;
			FaceIcon::Instance()->Change(team, FACE_STATUS::DEFAULT);
			SuperiorityGauge::Instance()->Init();
			OnBreakFinish();
		}
	}
	//�_���[�W��ԍX�V�i�琧��j
	if (damageTimer)
	{
		damageTimer--;
		if (damageTimer <= 0)
		{
			FaceIcon::Instance()->Change(team, FACE_STATUS::DEFAULT);
		}
	}

	// ���ɂ������Ĉ�莞�ԓ����Ȃ��t���O�������Ă����烊�^�[��
	if (0 < elecTimer) {
		--elecTimer;
		return;
	}

	//���g���U��񂵒�
	if (nowSwing)
	{
		SwingUpdate();

	}

	//�p�C���b�g�؂藣����
	if (isPilotDetached)
	{
		OnPilotControl();

		const auto vec = pilotPos - pos;
		if (PILOT_RANGE < vec.Length())
		{
			pilotPos = pos + vec.GetNormal() * PILOT_RANGE;
		}

		//�X�^�~�i����
		staminaGauge->ConsumesStaminaByGauge(1.0f);
		//if (!staminaGauge->ConsumesStaminaByGauge(0.5f))
		//�X�^�~�i������ۂȂ�
		if (staminaGauge->emptyTrigger)
		{
			//�����I�Ƀp�C���b�g��߂�
			SetPilotDetachedFlg(false);
		}
	}
	//�p�C���b�g���߂��Ă��鏈��
	if (pilotReturnTimer < pilotReturnTotalTime)
	{
		pilotReturnTimer++;
		pilotPos = KuroMath::Ease(Out, Circ, pilotReturnTimer, pilotReturnTotalTime, pilotReturnStartPos, Vec2<float>(pos));

		//�p�C���b�g�����{�ɂ��ǂ蒅��
		if (pilotReturnTotalTime <= pilotReturnTimer)
		{
			OnPilotReturn();
		}
	}


	if (SuperiorityGauge::Instance()->GetGaugeData(team).gaugeValue)
	{
		MOVE_SPEED_PLAYER = DebugParameter::Instance()->playerData[0].playerSpeed;
		OnUpdate(MapData);
	}

	OnUpdateNoRelatedSwing();

	//�E�B���h�E�̈����������Ă��锻��̃^�C�}�[�X�V
	if (0 < stackWindowTimer) {

		--stackWindowTimer;
	}

	//�����������Ă���
	if (stackMapChip)
	{
		CrashUpdate();
	}

	//�}�b�v�`�b�v�Ƃ̓����蔻�肪�������Ƃ��̏����Ăяo��
	for (int i = 0; i < HIT_DIR_NUM; ++i)
	{
		if (mapChipHit[i])
		{
			OnHitMapChip((HIT_DIR)i);
			mapChipHit[i] = false;
		}
	}

	//���o�⏕�X�V
	stagingDevice.Update();

	// �U��񂵉����p�̃N���X���X�V�B
	/*if (nowSwing) {
		CCWSwingSegmentMgr.Update(pos, Vec2<float>(partner.lock()->pos - pos).GetNormal(), Vec2<float>(pos - partner.lock()->pos).Length(), !isInputSwingRB, true, MapData);
		CWSwingSegmentMgr.Update(pos, Vec2<float>(partner.lock()->pos - pos).GetNormal(), Vec2<float>(pos - partner.lock()->pos).Length(), isInputSwingRB, true, MapData);
	}
	else {
		CCWSwingSegmentMgr.Update(pos, Vec2<float>(partner.lock()->pos - pos).GetNormal(), Vec2<float>(pos - partner.lock()->pos).Length(), false, false, MapData);
		CWSwingSegmentMgr.Update(pos, Vec2<float>(partner.lock()->pos - pos).GetNormal(), Vec2<float>(pos - partner.lock()->pos).Length(), false, false, MapData);
	}*/

	// �̊��Q�[�W���f�t�H���g�ɖ߂��^�C�}�[��0��������
	if (gaugeReturnTimer <= 0 && !GetNowBreak()) {

		static const int DEF_GAUGE = 50;
		static const float RETURN_AMOUNT = 0.1f;

		// �E���̃`�[���ŁA�E���̃Q�[�W�ʂ��f�t�H���g�ȉ���������E���̃Q�[�W�𑫂��B
		float nowGaugeValue = SuperiorityGauge::Instance()->GetGaugeData(RIGHT_TEAM).gaugeValue;
		if (GetWhichTeam() == RIGHT_TEAM && nowGaugeValue < DEF_GAUGE) {

			SuperiorityGauge::Instance()->AddGauge(RIGHT_TEAM, RETURN_AMOUNT);

			// �����ߒ��Ō��E�𒴂��Ȃ��悤�ɂ���B
			nowGaugeValue = SuperiorityGauge::Instance()->GetGaugeData(RIGHT_TEAM).gaugeValue;
			if (DEF_GAUGE < nowGaugeValue) {

				SuperiorityGauge::Instance()->AddGauge(RIGHT_TEAM, DEF_GAUGE - nowGaugeValue);

			}

		}

		// �����̃`�[���ŁA�����̃Q�[�W�ʂ��f�t�H���g�ȉ��������獶���̃Q�[�W�𑫂��B
		nowGaugeValue = SuperiorityGauge::Instance()->GetGaugeData(LEFT_TEAM).gaugeValue;
		if (GetWhichTeam() == LEFT_TEAM && nowGaugeValue < DEF_GAUGE) {

			SuperiorityGauge::Instance()->AddGauge(LEFT_TEAM, RETURN_AMOUNT);

			// �����ߒ��Ō��E�𒴂��Ȃ��悤�ɂ���B
			nowGaugeValue = SuperiorityGauge::Instance()->GetGaugeData(LEFT_TEAM).gaugeValue;
			if (DEF_GAUGE < nowGaugeValue) {

				SuperiorityGauge::Instance()->AddGauge(LEFT_TEAM, DEF_GAUGE - nowGaugeValue);

			}

		}

	}
	else {

		--gaugeReturnTimer;

	}

	if (team == WHICH_TEAM::LEFT_TEAM) {

		// �Ђ���̃`�[��(�v���C���[)��������
		staminaAutoHealAmount = 1.5f;

	}
	else {

		// �݂��̃`�[��(�G)��������
		staminaAutoHealAmount = DebugParameter::Instance()->GetBossData().staminaHealAmount;

	}

	staminaGauge->Update(!isPilotDetached, pos, staminaAutoHealAmount);


	// �E�̃L�����������珈�����s���B
	if (team == WHICH_TEAM::RIGHT_TEAM && !isRoundStartEffect) {

		// �e���̍X�V����
		static const int BULLET_HANDLE = TexHandleMgr::LoadGraph("resource/ChainCombat/boss/bullet_enemy.png");
		bool isEnd = barrage->Update(bulletMgr, CharacterManager::Instance()->Right()->pos, CharacterManager::Instance()->Left()->pos, BULLET_HANDLE);

		// �e���̍X�V���I����Ă�����B
		if (isEnd) {

			// �^�C�}�[���X�V�B
			++barrageDelayTimer;

			// �^�C�}�[���K��l�ɒB������B
			if (BARRAGE_DELAY_TIMER <= barrageDelayTimer) {

				// �����𐶐����āA���̗����̒e�����Z�b�g����B
				int random = KuroFunc::GetRand(6);

				switch (random)
				{
				case 0:

					barrage = std::make_unique<WhirlpoolBarrage>();

					break;

				case 1:

					barrage = std::make_unique<Whirlpool2WayBarrage>();

					break;

				case 2:

					barrage = std::make_unique<CircularBarrage>();

					break;

				case 3:

					barrage = std::make_unique<TargetShotBarrage>();

					break;

				case 4:

					barrage = std::make_unique<TargetShot3WayBarrage>();

					break;

				case 5:

					barrage = std::make_unique<ShotGunBarrage>();

					break;

				case 6:

					barrage = std::make_unique<WaveBarrage>();

					break;
				default:
					break;
				}

				barrage->Start();

			}

		}
		else {

			barrageDelayTimer = 0;

		}


	}


}

#include "DrawFunc.h"
#include"TexHandleMgr.h"
void CharacterInterFace::Draw(const bool& isRoundStartEffect)
{
	// �c����`��
	if (!GetNowBreak()) {
		CWSwingSegmentMgr.Draw(team);
		CCWSwingSegmentMgr.Draw(team);
	}
	OnDraw(isRoundStartEffect);

	static int HEAL_GRAPH[TEAM_NUM] = { 0 };
	if (!HEAL_GRAPH[0])
	{
		TexHandleMgr::LoadDivGraph("resource/ChainCombat/healAura.png", TEAM_NUM, { TEAM_NUM,1 }, HEAL_GRAPH);
	}
	if (healAuraEaseRate < 1.0f)
	{
		healAuraEaseRate += 0.02f;
		if (1.0f < healAuraEaseRate)healAuraEaseRate = 1.0f;
	}
	float auraAlpha = KuroMath::Ease(In, Cubic, healAuraEaseRate, 1.0f, 0.0f);
	float exp = ScrollMgr::Instance()->zoom;
	DrawFunc::DrawRotaGraph2D(ScrollMgr::Instance()->Affect(pos), { exp,exp }, 0.0f, TexHandleMgr::GetTexBuffer(HEAL_GRAPH[team]), Color(1.0f, 1.0f, 1.0f, auraAlpha),
		{ 0.5f,0.5f }, { false,false }, AlphaBlendMode_Add);

	static const int LINE_GRAPH[TEAM_NUM] =
	{
		TexHandleMgr::LoadGraph("resource/ChainCombat/chain_player.png"),
		TexHandleMgr::LoadGraph("resource/ChainCombat/chain_enemy.png")
	};
	static const int CHAIN_THICKNESS = 4;

	//�p�C���b�g�`��
	if (IsPilotOutSide() && pilotGraph != -1)
	{
		const auto pilotDrawPos = ScrollMgr::Instance()->Affect(pilotPos);
		DrawFunc::DrawLine2DGraph(ScrollMgr::Instance()->Affect(pos), ScrollMgr::Instance()->Affect(pilotPos),
			TexHandleMgr::GetTexBuffer(LINE_GRAPH[team]), CHAIN_THICKNESS * ScrollMgr::Instance()->zoom);

		const auto sizeHalf = pilotSize * ScrollMgr::Instance()->zoom / 2.0f;
		DrawFunc::DrawExtendGraph2D(pilotDrawPos - sizeHalf, pilotDrawPos + sizeHalf, TexHandleMgr::GetTexBuffer(pilotGraph), Color(), { pilotDrawMiror,false });
	}

	bulletMgr.Draw();
}

void CharacterInterFace::DrawUI()
{
	//���̓Q�[�W�`��
	static Color GAUGE_COLOR[TEAM_NUM] = { Color(47,255,139,255),Color(239,1,144,255) };
	static Color GAUGE_SHADOW_COLOR[TEAM_NUM] = { Color(41,166,150,255),Color(162,27,108,255) };

	if (GetCharacterName() != PLAYABLE_BOSS_0)
	{
		staminaGauge->Draw(pos);
	}


	OnDrawUI();
}

void CharacterInterFace::Break()
{
	static const int STAN_TOTAL_TIME = 120;
	stanTimer = STAN_TOTAL_TIME + StunEffect::GetTotalTime();
	OnBreak();
}

void CharacterInterFace::Damage()
{
	static const int DAMAGE_TOTAL_TIME = 90;
	stagingDevice.Flash(DAMAGE_TOTAL_TIME, 0.7f);
	stagingDevice.Shake(DAMAGE_TOTAL_TIME / 2, 2, 3.0f);

	if (stanTimer)return;	//�X�^�����Ȃ�_���[�W�ɂ���ύX�Ȃ�

	damageTimer = DAMAGE_TOTAL_TIME;
	FaceIcon::Instance()->Change(team, FACE_STATUS::DAMAGE);
}

#include"Intersected.h"
#include"MapChipCollider.h"
#include "StaminaItemMgr.h"
#include <IntoTheAbyss/StageMgr.h>
void CharacterInterFace::CheckHit(const std::vector<std::vector<int>>& MapData, const Vec2<float>& LineCenterPos)
{

	/*===== �����蔻�� =====*/

	//�����蔻��
	if (!hitCheck)return;

	// �ǂ͂��݂���̓����蔻�薳��������B
	if (0 < stackWindowTimer) {

		--stackWindowTimer;
		stackMapChip = false;

		return;

	}


	//�e�L�����̓��L�̓����蔻��
	//OnCheckHit(MapData, LineCenterPos);

	// �{�X���}�b�v�`�b�v�Ɠ������Ă��邩�̃t���O
	INTERSECTED_LINE intersectedBuff = {};

	// �}�b�v�`�b�v�ڐ��łǂ��ɓ���������
	bool isHitTop = false;
	bool isHitRight = false;
	bool isHitLeft = false;
	bool isHitBottom = false;
	Vec2<int> hitChipIndex;
	Vec2<int> hitChipIndexBuff;		// �ꎟ�ۑ��p

	// �ړ��ʂ����ɂ����}�b�v�`�b�v�Ƃ̓����蔻����s���B
	Vec2<float> moveDir = pos - prevPos;
	float velOffset = 3.0f;
	moveDir.Normalize();
	INTERSECTED_LINE intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheVel(pos, prevPos, moveDir, size, MapData, hitChipIndexBuff);
	isHitTop = intersectedLine == INTERSECTED_TOP;
	isHitRight = intersectedLine == INTERSECTED_RIGHT;
	isHitLeft = intersectedLine == INTERSECTED_LEFT;
	isHitBottom = intersectedLine == INTERSECTED_BOTTOM;
	if (intersectedLine != INTERSECTED_NONE) hitChipIndex = hitChipIndexBuff;

	// ����
	Vec2<float> velPosBuff = pos - size + Vec2<float>(velOffset, velOffset);
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheVel(velPosBuff, prevPos - size + Vec2<float>(velOffset, velOffset), {}, {}, MapData, hitChipIndexBuff);
	pos = velPosBuff + size - Vec2<float>(velOffset, velOffset);
	SaveHitInfo(isHitTop, isHitBottom, isHitLeft, isHitRight, intersectedLine, hitChipIndex, hitChipIndexBuff);

	// �E��
	velPosBuff = pos + Vec2<float>(size.x, -size.y) + Vec2<float>(-velOffset, velOffset);
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheVel(velPosBuff, prevPos + Vec2<float>(size.x, -size.y) + Vec2<float>(-velOffset, velOffset), {}, {}, MapData, hitChipIndexBuff);
	pos = velPosBuff - Vec2<float>(size.x, -size.y) - Vec2<float>(-velOffset, velOffset);
	SaveHitInfo(isHitTop, isHitBottom, isHitLeft, isHitRight, intersectedLine, hitChipIndex, hitChipIndexBuff);

	// �E��
	velPosBuff = pos + size + Vec2<float>(-velOffset, -velOffset);
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheVel(velPosBuff, prevPos + size + Vec2<float>(-velOffset, -velOffset), {}, {}, MapData, hitChipIndexBuff);
	pos = velPosBuff - size - Vec2<float>(-velOffset, -velOffset);
	SaveHitInfo(isHitTop, isHitBottom, isHitLeft, isHitRight, intersectedLine, hitChipIndex, hitChipIndexBuff);

	// ����
	velPosBuff = pos + Vec2<float>(-size.x, size.y) + Vec2<float>(velOffset, -velOffset);
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheVel(velPosBuff, prevPos + Vec2<float>(-size.x, size.y) + Vec2<float>(velOffset, -velOffset), {}, {}, MapData, hitChipIndexBuff);
	pos = velPosBuff - Vec2<float>(-size.x, size.y) - Vec2<float>(velOffset, -velOffset);
	SaveHitInfo(isHitTop, isHitBottom, isHitLeft, isHitRight, intersectedLine, hitChipIndex, hitChipIndexBuff);

	// �X�P�[�������ɂ����}�b�v�`�b�v�Ƃ̓����蔻����s���B

	// �����
	float offset = 5.0f;
	Vec2<float> posBuff = pos + Vec2<float>(size.x - offset, 0);
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(posBuff, size, MapData, INTERSECTED_TOP, hitChipIndexBuff);
	pos.y = posBuff.y;
	SaveHitInfo(isHitTop, isHitBottom, isHitLeft, isHitRight, intersectedLine, hitChipIndex, hitChipIndexBuff);
	posBuff = pos - Vec2<float>(size.x - offset, 0);
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(posBuff, size, MapData, INTERSECTED_TOP, hitChipIndexBuff);
	pos.y = posBuff.y;
	SaveHitInfo(isHitTop, isHitBottom, isHitLeft, isHitRight, intersectedLine, hitChipIndex, hitChipIndexBuff);
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(pos, size, MapData, INTERSECTED_TOP, hitChipIndexBuff);
	SaveHitInfo(isHitTop, isHitBottom, isHitLeft, isHitRight, intersectedLine, hitChipIndex, hitChipIndexBuff);

	// ������
	posBuff = pos + Vec2<float>(size.x - offset, 0);
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(posBuff, size, MapData, INTERSECTED_BOTTOM, hitChipIndexBuff);
	pos.y = posBuff.y;
	SaveHitInfo(isHitTop, isHitBottom, isHitLeft, isHitRight, intersectedLine, hitChipIndex, hitChipIndexBuff);
	posBuff = pos - Vec2<float>(size.x - offset, 0);
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(posBuff, size, MapData, INTERSECTED_BOTTOM, hitChipIndexBuff);
	pos.y = posBuff.y;
	SaveHitInfo(isHitTop, isHitBottom, isHitLeft, isHitRight, intersectedLine, hitChipIndex, hitChipIndexBuff);
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(pos, size, MapData, INTERSECTED_BOTTOM, hitChipIndexBuff);
	SaveHitInfo(isHitTop, isHitBottom, isHitLeft, isHitRight, intersectedLine, hitChipIndex, hitChipIndexBuff);

	// �E����
	posBuff = pos + Vec2<float>(0, size.y - offset);
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(posBuff, size, MapData, INTERSECTED_RIGHT, hitChipIndexBuff);
	pos.x = posBuff.x;
	SaveHitInfo(isHitTop, isHitBottom, isHitLeft, isHitRight, intersectedLine, hitChipIndex, hitChipIndexBuff);
	posBuff = pos - Vec2<float>(0, size.y - offset);
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(posBuff, size, MapData, INTERSECTED_RIGHT, hitChipIndexBuff);
	pos.x = posBuff.x;
	SaveHitInfo(isHitTop, isHitBottom, isHitLeft, isHitRight, intersectedLine, hitChipIndex, hitChipIndexBuff);
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(pos, size, MapData, INTERSECTED_RIGHT, hitChipIndexBuff);
	SaveHitInfo(isHitTop, isHitBottom, isHitLeft, isHitRight, intersectedLine, hitChipIndex, hitChipIndexBuff);

	// ������
	posBuff = pos + Vec2<float>(0, size.y - offset);
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(posBuff, size, MapData, INTERSECTED_LEFT, hitChipIndexBuff);
	pos.x = posBuff.x;
	SaveHitInfo(isHitTop, isHitBottom, isHitLeft, isHitRight, intersectedLine, hitChipIndex, hitChipIndexBuff);
	posBuff = pos - Vec2<float>(0, size.y - offset);
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(posBuff, size, MapData, INTERSECTED_LEFT, hitChipIndexBuff);
	pos.x = posBuff.x;
	SaveHitInfo(isHitTop, isHitBottom, isHitLeft, isHitRight, intersectedLine, hitChipIndex, hitChipIndexBuff);
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(pos, size, MapData, INTERSECTED_LEFT, hitChipIndexBuff);
	SaveHitInfo(isHitTop, isHitBottom, isHitLeft, isHitRight, intersectedLine, hitChipIndex, hitChipIndexBuff);


	//�}�b�v�`�b�v�ƈ����������Ă���t���O�����낷
	stackMapChip = false;

	// �}�b�v�`�b�v�Ɠ������Ă�����
	if (isHitTop || isHitRight || isHitLeft || isHitBottom) {

		// �}�b�v�`�b�v�Ɠ������Ă���ꏊ�ɂ���āA�����������Ă��邩�ǂ����𒲂ׂ�B

		// ����Ƃ̊p�x
		const auto partnerPos = partner.lock()->pos;
		float partnerAngle = atan2(partnerPos.y - pos.y, partnerPos.x - pos.x);

		// �p�x��-��������180�x�ȏ�
		if (partnerAngle < 0) {

			float angleBuff = 3.14f - fabs(partnerAngle);

			partnerAngle = 3.14f + angleBuff;

		}

		if (isHitTop) {

			mapChipHit[BOTTOM] = true;

			// �}�b�v�`�b�v�̏�ɂ������Ă����Ƃ������Ƃ́A�v���C���[���������ɂ����OK�I
			// �������̋�̓I�Ȓl��
			const float MIN_ANGLE = 0.785398f;
			const float MAX_ANGLE = 2.35619f;

			// �p�x�����̒l�̊Ԃ�������
			if (MIN_ANGLE <= partnerAngle && partnerAngle <= MAX_ANGLE) {

				// �����������Ă���B
				stackMapChip = true;
				intersectedBuff = INTERSECTED_BOTTOM;

			}

		}

		if (isHitBottom) {

			mapChipHit[TOP] = true;

			// �}�b�v�`�b�v�̉��ɂ������Ă����Ƃ������Ƃ́A�v���C���[��������ɂ����OK�I
			// ������̋�̓I�Ȓl��
			const float MIN_ANGLE = 3.92699f;
			const float MAX_ANGLE = 5.49779f;

			// �p�x�����̒l�̊Ԃ�������
			if (MIN_ANGLE <= partnerAngle && partnerAngle <= MAX_ANGLE) {

				// �����������Ă���B
				stackMapChip = true;
				intersectedBuff = INTERSECTED_TOP;

			}

		}

		if (isHitRight) {

			mapChipHit[LEFT] = true;

			// �}�b�v�`�b�v�̉E�ɂ������Ă����Ƃ������Ƃ́A�v���C���[���������ɂ����OK�I
			// �������̋�̓I�Ȓl��
			const float MIN_ANGLE = 2.35619f;
			const float MAX_ANGLE = 3.92699f;

			// �p�x�����̒l�̊Ԃ�������
			if (MIN_ANGLE <= partnerAngle && partnerAngle <= MAX_ANGLE) {

				// �����������Ă���B
				stackMapChip = true;
				intersectedBuff = INTERSECTED_RIGHT;
			}

		}

		if (isHitLeft) {

			mapChipHit[RIGHT] = true;

			// �}�b�v�`�b�v�̍��ɂ������Ă����Ƃ������Ƃ́A�v���C���[���E�����ɂ����OK�I
			// �E�����̋�̓I�Ȓl��
			const float MIN_ANGLE = 0.785398f;
			const float MAX_ANGLE = 5.49779f;

			// �p�x�����̒l�̊Ԃ�������
			if (MAX_ANGLE <= partnerAngle || partnerAngle <= MIN_ANGLE) {

				// �����������Ă���B
				stackMapChip = true;
				intersectedBuff = INTERSECTED_LEFT;

			}
		}




		// ���ȉ���������_���[�W��^���Ȃ��B
		if (partner.lock()->addSwingAngle <= ADD_SWING_ANGLE * 2.0f) {

			// �G�ꂽ�����ł��̐F�ɂȂ�悤�ɂ���B
			MapChipType hitChipData = StageMgr::Instance()->GetLocalMapChipType(hitChipIndex);
			if (hitChipData == MapChipType::MAPCHIP_BLOCK_COLOR_LEFT) {

				// �������E���̃L������������B
				if (team == WHICH_TEAM::RIGHT_TEAM) {

					// �u���b�N���E�̐F�ɂ���B
					StageMgr::Instance()->WriteMapChipData(hitChipIndex, MapChipData::MAPCHIP_TYPE_STATIC_COLOR_RIGHT);

				}

			}
			else if (hitChipData == MapChipType::MAPCHIP_BLOCK_COLOR_RIGHT) {

				// �����������̃L������������B
				if (team == WHICH_TEAM::LEFT_TEAM) {

					// �u���b�N���E�̐F�ɂ���B
					StageMgr::Instance()->WriteMapChipData(hitChipIndex, MapChipData::MAPCHIP_TYPE_STATIC_COLOR_LEFT);

				}

			}


		}
		else if (partner.lock()->addSwingAngle <= ADD_SWING_ANGLE * 4.0f) {

			// �U��񂳂�Ă����Ԃ�������A�V�F�C�N�𔭐������ĐU��񂵏�Ԃ���������B
			Vec2<float>vec = { 0,0 };
			if (partner.lock()->GetNowSwing()) {

				if (isHitLeft)vec.x = -1.0f;
				else if (isHitRight)vec.x = 1.0f;
				if (isHitTop)vec.y = -1.0f;
				else if (isHitBottom)vec.y = 1.0f;

				/*Crash(vec);

				SuperiorityGauge::Instance()->AddPlayerGauge(5.0f);*/
				partner.lock()->FinishSwing();

			}

			// �Q�[�W���f�t�H���g�ɖ߂�܂ł̃^�C�}�[���X�V�B
			gaugeReturnTimer = GAUGE_RETURN_TIMER;

		}
		else {

			// �U��񂳂�Ă����Ԃ�������A�V�F�C�N�𔭐������ĐU��񂵏�Ԃ���������B
			Vec2<float>vec = { 0,0 };
			if (partner.lock()->GetNowSwing()) {

				int smokeCol = 0;

				if (isHitLeft)vec.x = -1.0f;
				else if (isHitRight)vec.x = 1.0f;
				if (isHitTop)vec.y = -1.0f;
				else if (isHitBottom)vec.y = 1.0f;

				//CrashMgr::Instance()->Crash(pos, crashDevice, ext);
				//SuperiorityGauge::Instance()->AddPlayerGauge(DebugParameter::Instance()->gaugeData->swingDamageValue);
				SuperiorityGauge::Instance()->AddGauge(team, -10);
				partner.lock()->FinishSwing();

				// �`�[���ɉ����ăN���b�V���������Z����ϐ���ς���B
				if (team == WHICH_TEAM::LEFT_TEAM) {
					++ResultTransfer::Instance()->leftCrashCount;
				}
				else {
					++ResultTransfer::Instance()->rightCrashCount;
				}

				// �N���b�V�����o��ǉ��B
				CrashEffectMgr::Instance()->Generate(pos, GetTeamColor());

				// ���������ǂ����̃`�[�������擾�B
				StaminaItem::CHARA_ID charaID;
				if (team != WHICH_TEAM::LEFT_TEAM) charaID = StaminaItem::CHARA_ID::LEFT;
				if (team == WHICH_TEAM::LEFT_TEAM) charaID = StaminaItem::CHARA_ID::RIGHT;

				// ���������`�b�v���F�u���b�N��������B
				MapChipType hitChipData = StageMgr::Instance()->GetLocalMapChipType(hitChipIndex);
				auto mapChipDrawData = StageMgr::Instance()->GetLocalDrawMap();


				if (hitChipData == MapChipType::MAPCHIP_BLOCK_COLOR_LEFT) {

					smokeCol = 1;
					(*mapChipDrawData)[hitChipIndex.y][hitChipIndex.x].expEaseRate = 0.0f;

					// �������E���̃L������������B
					if (team == WHICH_TEAM::RIGHT_TEAM) {

						// �A�C�e���𐶐�����B
						StaminaItemMgr::Instance()->GenerateCrash(pos, StaminaItemMgr::GENERATE_STATUS::CRASH, &partner.lock()->pos, StaminaItem::CHARA_ID::LEFT, partner.lock()->pos);

					}
					// �����������̃L������������B
					else {

						// �A�C�e���𐶐�����B
						StaminaItemMgr::Instance()->GenerateCrash(pos, StaminaItemMgr::GENERATE_STATUS::CRASH, &pos, StaminaItem::CHARA_ID::LEFT, partner.lock()->pos);

					}

				}
				else if (hitChipData == MapChipType::MAPCHIP_BLOCK_COLOR_RIGHT) {

					smokeCol = 2;
					(*mapChipDrawData)[hitChipIndex.y][hitChipIndex.x].expEaseRate = 0.0f;

					// �������E���̃L������������B
					if (team == WHICH_TEAM::RIGHT_TEAM) {

						// �A�C�e���𐶐�����B
						StaminaItemMgr::Instance()->GenerateCrash(pos, StaminaItemMgr::GENERATE_STATUS::CRASH, &pos, StaminaItem::CHARA_ID::RIGHT, partner.lock()->pos);

					}
					// �����������̃L������������B
					else {

						// �A�C�e���𐶐�����B
						StaminaItemMgr::Instance()->GenerateCrash(pos, StaminaItemMgr::GENERATE_STATUS::CRASH, &partner.lock()->pos, StaminaItem::CHARA_ID::RIGHT, partner.lock()->pos);

					}

				}

				// �N���b�V��������B
				Crash(vec, smokeCol);

				CWSwingSegmentMgr.Init();
				CCWSwingSegmentMgr.Init();

			}

			// �Q�[�W���f�t�H���g�ɖ߂�܂ł̃^�C�}�[���X�V�B
			gaugeReturnTimer = GAUGE_RETURN_TIMER;

		}

	}

	// ���������蔻��̍X�V
	CheckHitStuck(MapData);

}

void CharacterInterFace::CheckHitStuck(const std::vector<std::vector<int>>& MapData)
{

	/*===== ���������蔻��̍X�V���� =====*/

	// �}�b�v�`�b�v�̔ԍ��̃f�[�^
	SizeData mapChipSizeData = StageMgr::Instance()->GetMapChipSizeData(MAPCHIP_TYPE_STATIC_BLOCK);

	// �}�b�v�`�b�v�̗v�f��
	const int MAX_X = MapData[0].size() - 1;
	const int MAX_Y = MapData.size() - 1;

	// ��������ʁB
	float searchOffset = MAP_CHIP_SIZE;

	// �㑤�̔�����s���B
	bool isHitTop = false;

	// �㑤�̍��W���擾����B
	Vec2<float> searchPos = pos + Vec2<float>(0, -size.y - searchOffset);

	// �������W�̃}�b�v�`�b�v�ԍ������߂�B
	Vec2<float> searchChipIndex = { searchPos.x / MAP_CHIP_SIZE, searchPos.y / MAP_CHIP_SIZE };

	// �������W�̃}�b�v�`�b�v�ԍ����z��O�������珈�����΂��B
	if (!(searchChipIndex.x < 0 || MAX_X < searchChipIndex.x || searchChipIndex.y < 0 || MAX_Y < searchChipIndex.y)) {

		// ���̃}�b�v�`�b�v�̔ԍ����u���b�N���ǂ������`�F�b�N�B
		isHitTop = mapChipSizeData.min <= MapData[searchChipIndex.y][searchChipIndex.x] && MapData[searchChipIndex.y][searchChipIndex.x] <= mapChipSizeData.max;

	}

	// �����̔�����s���B
	bool isHitBottom = false;

	// �����̍��W���擾����B
	searchPos = pos + Vec2<float>(0, size.y + searchOffset);

	// �������W�̃}�b�v�`�b�v�ԍ������߂�B
	searchChipIndex = { searchPos.x / MAP_CHIP_SIZE, searchPos.y / MAP_CHIP_SIZE };

	// �������W�̃}�b�v�`�b�v�ԍ����z��O�������珈�����΂��B
	if (!(searchChipIndex.x < 0 || MAX_X < searchChipIndex.x || searchChipIndex.y < 0 || MAX_Y < searchChipIndex.y)) {

		// ���̃}�b�v�`�b�v�̔ԍ����u���b�N���ǂ������`�F�b�N�B
		isHitBottom = mapChipSizeData.min <= MapData[searchChipIndex.y][searchChipIndex.x] && MapData[searchChipIndex.y][searchChipIndex.x] <= mapChipSizeData.max;

	}

	// �E���̔�����s���B
	bool isHitRight = false;

	// �E���̍��W���擾����B
	searchPos = pos + Vec2<float>(size.x + searchOffset, 0);

	// �������W�̃}�b�v�`�b�v�ԍ������߂�B
	searchChipIndex = { searchPos.x / MAP_CHIP_SIZE, searchPos.y / MAP_CHIP_SIZE };
	Vec2<float> posIndex = { pos.x / MAP_CHIP_SIZE, pos.y / MAP_CHIP_SIZE };

	// �������W�̃}�b�v�`�b�v�ԍ����z��O�������珈�����΂��B
	if (!(searchChipIndex.x < 0 || MAX_X < searchChipIndex.x || searchChipIndex.y < 0 || MAX_Y < searchChipIndex.y)) {

		// ���̃}�b�v�`�b�v�̔ԍ����u���b�N���ǂ������`�F�b�N�B
		isHitRight = mapChipSizeData.min <= MapData[searchChipIndex.y][searchChipIndex.x] && MapData[searchChipIndex.y][searchChipIndex.x] <= mapChipSizeData.max;

	}

	// �����̔�����s���B
	bool isHitLeft = false;

	// �����̍��W���擾����B
	searchPos = pos + Vec2<float>(-size.x - searchOffset, 0);

	// �������W�̃}�b�v�`�b�v�ԍ������߂�B
	searchChipIndex = { searchPos.x / MAP_CHIP_SIZE, searchPos.y / MAP_CHIP_SIZE };

	// �������W�̃}�b�v�`�b�v�ԍ����z��O�������珈�����΂��B
	if (!(searchChipIndex.x < 0 || MAX_X < searchChipIndex.x || searchChipIndex.y < 0 || MAX_Y < searchChipIndex.y)) {

		// ���̃}�b�v�`�b�v�̔ԍ����u���b�N���ǂ������`�F�b�N�B
		isHitLeft = mapChipSizeData.min <= MapData[searchChipIndex.y][searchChipIndex.x] && MapData[searchChipIndex.y][searchChipIndex.x] <= mapChipSizeData.max;

	}

	// ����Ƃ̊p�x
	const auto partnerPos = partner.lock()->pos;
	float partnerAngle = atan2(partnerPos.y - pos.y, partnerPos.x - pos.x);

	// �p�x��-��������180�x�ȏ�
	if (partnerAngle < 0) {

		float angleBuff = 3.14f - fabs(partnerAngle);

		partnerAngle = 3.14f + angleBuff;

	}

	if (isHitBottom) {

		// �}�b�v�`�b�v�̏�ɂ������Ă����Ƃ������Ƃ́A�v���C���[���������ɂ����OK�I
		// �������̋�̓I�Ȓl��
		const float MIN_ANGLE = 0.785398f;
		const float MAX_ANGLE = 2.35619f;

		// �p�x�����̒l�̊Ԃ�������
		if (MIN_ANGLE <= partnerAngle && partnerAngle <= MAX_ANGLE) {

			// �����������Ă���B
			stackMapChip = true;

		}

	}

	if (isHitTop) {

		// �}�b�v�`�b�v�̉��ɂ������Ă����Ƃ������Ƃ́A�v���C���[��������ɂ����OK�I
		// ������̋�̓I�Ȓl��
		const float MIN_ANGLE = 3.92699f;
		const float MAX_ANGLE = 5.49779f;

		// �p�x�����̒l�̊Ԃ�������
		if (MIN_ANGLE <= partnerAngle && partnerAngle <= MAX_ANGLE) {

			// �����������Ă���B
			stackMapChip = true;

		}

	}

	if (isHitLeft) {

		// �}�b�v�`�b�v�̉E�ɂ������Ă����Ƃ������Ƃ́A�v���C���[���������ɂ����OK�I
		// �������̋�̓I�Ȓl��
		const float MIN_ANGLE = 2.35619f;
		const float MAX_ANGLE = 3.92699f;

		// �p�x�����̒l�̊Ԃ�������
		if (MIN_ANGLE <= partnerAngle && partnerAngle <= MAX_ANGLE) {

			// �����������Ă���B
			stackMapChip = true;
		}

	}

	if (isHitRight) {

		// �}�b�v�`�b�v�̍��ɂ������Ă����Ƃ������Ƃ́A�v���C���[���E�����ɂ����OK�I
		// �E�����̋�̓I�Ȓl��
		const float MIN_ANGLE = 0.785398f;
		const float MAX_ANGLE = 5.49779f;

		// �p�x�����̒l�̊Ԃ�������
		if (MAX_ANGLE <= partnerAngle || partnerAngle <= MIN_ANGLE) {

			// �����������Ă���B
			stackMapChip = true;

		}
	}

}

void CharacterInterFace::FinishSwing()
{
	nowSwing = false;
	partner.lock()->stagingDevice.StopSpin();
	partner.lock()->OnSwingedFinish();
	CWSwingSegmentMgr.Init();
	CCWSwingSegmentMgr.Init();
	addSwingAngle = 0;
	swingTimer = 0;

}

void CharacterInterFace::HealStamina(const int& HealAmount)
{
	if (HealAmount)healAuraEaseRate = 0.0f;
	staminaGauge->AddStamina(HealAmount);
	OnStaminaHeal(HealAmount);
}

void CharacterInterFace::OverWriteMapChipValueAround(const Vec2<int>& MapChipIndex, const MapChipType& DstType, const MapChipData& SrcData)
{

	// �w�肳�ꂽ�C���f�b�N�X�̍����̃`�b�v���������E���̃u���b�N���𒲂ׂ�B
	if (StageMgr::Instance()->GetLocalMapChipType(MapChipIndex + Vec2<int>(-1, 0)) == DstType) {
		// �g�Q�u���b�N�𞙖�����Ԃɂ�����B
		StageMgr::Instance()->WriteMapChipData(MapChipIndex + Vec2<int>(-1, 0), SrcData);
	}

	// �w�肳�ꂽ�C���f�b�N�X�̍����̃`�b�v���������E���̃u���b�N���𒲂ׂ�B
	if (StageMgr::Instance()->GetLocalMapChipType(MapChipIndex + Vec2<int>(1, 0)) == DstType) {
		// �g�Q�u���b�N�𞙖�����Ԃɂ�����B
		StageMgr::Instance()->WriteMapChipData(MapChipIndex + Vec2<int>(1, 0), SrcData);
	}

	// �w�肳�ꂽ�C���f�b�N�X�̍����̃`�b�v���������E���̃u���b�N���𒲂ׂ�B
	if (StageMgr::Instance()->GetLocalMapChipType(MapChipIndex + Vec2<int>(0, -1)) == DstType) {
		// �g�Q�u���b�N�𞙖�����Ԃɂ�����B
		StageMgr::Instance()->WriteMapChipData(MapChipIndex + Vec2<int>(0, -1), SrcData);
	}

	// �w�肳�ꂽ�C���f�b�N�X�̍����̃`�b�v���������E���̃u���b�N���𒲂ׂ�B
	if (StageMgr::Instance()->GetLocalMapChipType(MapChipIndex + Vec2<int>(0, 1)) == DstType) {
		// �g�Q�u���b�N�𞙖�����Ԃɂ�����B
		StageMgr::Instance()->WriteMapChipData(MapChipIndex + Vec2<int>(0, 1), SrcData);
	}

}

CharacterInterFace::CharacterInterFace(const Vec2<float>& HonraiSize) : size(HonraiSize)
{
	areaHitBox.center = &pos;
	areaHitBox.size = size;
	bulletHitSphere.center = &pos;
	bulletHitSphere.radius = size.x;
	rbHandle = TexHandleMgr::LoadGraph("resource/ChainCombat/UI/button_RB.png");
	lbHandle = TexHandleMgr::LoadGraph("resource/ChainCombat/UI/button_LB.png");
	lineHandle = TexHandleMgr::LoadGraph("resource/ChainCombat/UI/swing_line.png");
	arrowHandle = TexHandleMgr::LoadGraph("resource/ChainCombat/UI/swing_arrow.png");
	staminaGauge = std::make_shared<StaminaMgr>();
	bulletMgr.Init();
	barrage = std::make_unique<CircularBarrage>();
	barrage->Init();
}

