#include "CharacterInterFace.h"
#include"CrashMgr.h"
#include"WinApp.h"
#include"SuperiorityGauge.h"
#include"ScrollMgr.h"
#include"DebugParameter.h"
#include"StunEffect.h"
#include"FaceIcon.h"
#include "ResultTransfer.h"

void CharacterInterFace::SwingUpdate()
{

	/*===== �U��񂵒��ɌĂ΂�鏈�� =====*/

	// �p�x�ɉ��Z����ʂ��X�V�B
	addSwingAngle += ADD_SWING_ANGLE;

	// ���E�𒴂��Ă�����C���B
	if (MAX_SWING_ANGLE < addSwingAngle) {

		addSwingAngle = MAX_SWING_ANGLE;

	}

	// ���݂̊p�x�����߂�B
	float nowAngle = atan2f(GetPartnerPos().y - pos.y, GetPartnerPos().x - pos.x);

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
	if (isSwingClockWise && crossResult < 0) {

		// �U��񂵏I���I
		FinishSwing();
	}
	// [�ŏ��������v���] ���� [���݂����v���] ��������
	if (!isSwingClockWise && 0 < crossResult) {

		// �U��񂵏I���I
		FinishSwing();
	}

}

void CharacterInterFace::Crash(const Vec2<float>& MyVec)
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
	//SuperiorityGauge::Instance()->AddGauge(team, -10.0f);
	stagingDevice.StopSpin();

	OnCrash();
}

void CharacterInterFace::CrashUpdate()
{
	//�U��񂳂�Ă���
	if (partner.lock()->nowSwing)
	{
		//���ʐU��񂵂���
		static const float CRASH_SWING_RATE = 0.15f;
		if (ADD_SWING_ANGLE * 2.0f < partner.lock()->addSwingAngle)
		{
			Vec2<float>vec = { 0,0 };
			if (mapChipHit[LEFT])vec.x = -1.0f;
			else if (mapChipHit[RIGHT])vec.x = 1.0f;
			if (mapChipHit[TOP])vec.y = -1.0f;
			else if (mapChipHit[BOTTOM])vec.y = 1.0f;

			Crash(vec);
		}

		partner.lock()->FinishSwing();
	}
}

void CharacterInterFace::SwingPartner(const Vec2<float>& SwingTargetVec)
{
	static int SE = -1;
	if (SE == -1)
	{
		SE = AudioApp::Instance()->LoadAudio("resource/ChainCombat/sound/swing.wav");
		AudioApp::Instance()->ChangeVolume(SE, 0.13f);
	}

	//�U��񂵏��������ɑ����Ă���ꍇ�́A�d�˂ĐU��񂹂Ȃ�
	if (partner.lock()->nowSwing || nowSwing)return;

	partner.lock()->OnSwinged();

	AudioApp::Instance()->PlayWave(SE);

	// �ڕW�n�_�̃x�N�g����ۑ��B
	swingTargetVec = SwingTargetVec;

	// ���݂̃t���[���̑����Ƃ̃x�N�g�������߂�B
	nowSwingVec = GetPartnerPos() - pos;
	nowSwingVec.Normalize();

	// �O�ς̍��E����ɂ��ǂ��瑤�ɐU��񂷂��𔻒f����B ���̒l�����A���̒l���E�B
	float crossResult = nowSwingVec.Cross(swingTargetVec);
	if (crossResult < 0) {

		// �}�C�i�X�Ȃ̂ō�(�����v���)�B
		isSwingClockWise = false;


	}
	else if (0 <= crossResult) {

		// �v���X�Ȃ̂ŉE(���v���)�B
		isSwingClockWise = true;

	}

	// �p�x�ւ̉��Z�ʂ��������B
	addSwingAngle = 0;

	//�U��񂵃t���O�̗L����
	nowSwing = true;

	partner.lock()->stagingDevice.StartSpin(isSwingClockWise);
}

void CharacterInterFace::SaveHitInfo(bool& isHitTop, bool& isHitBottom, bool& isHitLeft, bool& isHitRight, const INTERSECTED_LINE& intersectedLine)
{
	if (intersectedLine == INTERSECTED_LINE::INTERSECTED_TOP) isHitTop = true;
	if (intersectedLine == INTERSECTED_LINE::INTERSECTED_BOTTOM) isHitBottom = true;
	if (intersectedLine == INTERSECTED_LINE::INTERSECTED_LEFT) isHitLeft = true;
	if (intersectedLine == INTERSECTED_LINE::INTERSECTED_RIGHT) isHitRight = true;
}

void CharacterInterFace::Init(const Vec2<float>& GeneratePos)
{
	pos = GeneratePos;
	vel = { 0,0 };

	stackWindowTimer = 0;

	nowSwing = false;

	stackMapChip = false;
	for (auto& flg : mapChipHit)flg = false;

	stagingDevice.Init();

	addLineLength = 0.0f;

	swingInertia = 0;
	swingInertiaVec = {};
	afterSwingDelay = 0;

	//�o�ꉉ�o�̂��ߍŏ��͓����Ȃ�
	canMove = false;
	//�o�ꉉ�o�̂��ߍŏ��͓����蔻��Ƃ�Ȃ�
	hitCheck = false;

	bulletMgr.Init();

	stanTimer = 0;

	stagingDevice.Init();

	OnInit();

	isHold = false;

	gripPowerTimer = MAX_GRIP_POWER_TIMER;

	isGripPowerEmpty = false;

	advancedEntrySwingTimer = 0;
	isAdvancedEntrySwing = false;

}

void CharacterInterFace::Update(const std::vector<std::vector<int>>& MapData, const Vec2<float>& LineCenterPos)
{

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

	prevPos = pos;

	// �������X�V�B
	if (0 < swingInertia) {
		swingInertia -= swingInertia / 5.0f;
	}

	// �U��񂵒���̍d���̃^�C�}�[���X�V�B
	if (0 < afterSwingDelay) --afterSwingDelay;

	//���g���U��񂵒�
	if (nowSwing)
	{
		SwingUpdate();
	}

	if (SuperiorityGauge::Instance()->GetGaugeData(team).gaugeValue)
	{
		OnUpdate(MapData);
	}

	OnUpdateNoRelatedSwing();

	//�E�B���h�E�̈����������Ă��锻��̃^�C�}�[�X�V
	if (0 < stackWindowTimer) {

		--stackWindowTimer;
	}

	//�e�̍X�V
	bulletMgr.Update();

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
}

#include "DrawFunc.h"
#include"TexHandleMgr.h"
void CharacterInterFace::Draw()
{
	OnDraw();
	bulletMgr.Draw();
}

void CharacterInterFace::DrawUI()
{
	//���̓Q�[�W�`��
	static Color GAUGE_COLOR[TEAM_NUM] = { Color(47,255,139,255),Color(239,1,144,255) };
	static Color GAUGE_SHADOW_COLOR[TEAM_NUM] = { Color(41,166,150,255),Color(162,27,108,255) };
	static const int STAMINA_GAUGE_WIDTH = 110;
	static const int STAMINA_GAUGE_HEIGHT = 5;
	static const int STAMINA_GAUGE_OFFSET_Y = -64;
	static const Vec2<float> STAMINA_SHADOW_OFFSET_SIZE = { 4.0f,4.0f };
	{
		const float drawWidth = STAMINA_GAUGE_WIDTH * ((float)gripPowerTimer / MAX_GRIP_POWER_TIMER);
		const Vec2<float>leftUp = pos + Vec2<float>(-STAMINA_GAUGE_WIDTH / 2.0f, STAMINA_GAUGE_OFFSET_Y - STAMINA_GAUGE_HEIGHT);
		const Vec2<float>rightBottom = pos + Vec2<float>(-STAMINA_GAUGE_WIDTH / 2.0f + drawWidth, STAMINA_GAUGE_OFFSET_Y + STAMINA_GAUGE_HEIGHT);


		if (isHold && 0.0f < drawWidth)DrawFunc::DrawBox2D(ScrollMgr::Instance()->Affect(leftUp - STAMINA_SHADOW_OFFSET_SIZE), ScrollMgr::Instance()->Affect(rightBottom + STAMINA_SHADOW_OFFSET_SIZE), GAUGE_SHADOW_COLOR[team], D3D12App::Instance()->GetBackBuffFormat(), true);
		DrawFunc::DrawBox2D(ScrollMgr::Instance()->Affect(leftUp), ScrollMgr::Instance()->Affect(rightBottom), GAUGE_COLOR[team], D3D12App::Instance()->GetBackBuffFormat(), true);
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

	// �ړ��ʂ����ɂ����}�b�v�`�b�v�Ƃ̓����蔻����s���B
	Vec2<float> moveDir = pos - prevPos;
	float velOffset = 3.0f;
	moveDir.Normalize();
	INTERSECTED_LINE intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheVel(pos, prevPos, moveDir * size, size, MapData, false);
	isHitTop = intersectedLine == INTERSECTED_TOP;
	isHitRight = intersectedLine == INTERSECTED_RIGHT;
	isHitLeft = intersectedLine == INTERSECTED_LEFT;
	isHitBottom = intersectedLine == INTERSECTED_BOTTOM;

	// ����
	Vec2<float> velPosBuff = pos - size + Vec2<float>(velOffset, velOffset);
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheVel(velPosBuff, prevPos - size + Vec2<float>(velOffset, velOffset), {}, {}, MapData, false);
	pos = velPosBuff + size - Vec2<float>(velOffset, velOffset);
	if (intersectedLine == INTERSECTED_TOP) isHitTop = true;
	if (intersectedLine == INTERSECTED_RIGHT) isHitRight = true;
	if (intersectedLine == INTERSECTED_LEFT) isHitLeft = true;
	if (intersectedLine == INTERSECTED_BOTTOM) isHitBottom = true;

	// �E��
	velPosBuff = pos + Vec2<float>(size.x, -size.y) + Vec2<float>(-velOffset, velOffset);
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheVel(velPosBuff, prevPos + Vec2<float>(size.x, -size.y) + Vec2<float>(-velOffset, velOffset), {}, {}, MapData, false);
	pos = velPosBuff - Vec2<float>(size.x, -size.y) - Vec2<float>(-velOffset, velOffset);
	if (intersectedLine == INTERSECTED_TOP) isHitTop = true;
	if (intersectedLine == INTERSECTED_RIGHT) isHitRight = true;
	if (intersectedLine == INTERSECTED_LEFT) isHitLeft = true;
	if (intersectedLine == INTERSECTED_BOTTOM) isHitBottom = true;

	// �E��
	velPosBuff = pos + size + Vec2<float>(-velOffset, -velOffset);
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheVel(velPosBuff, prevPos + size + Vec2<float>(-velOffset, -velOffset), {}, {}, MapData, false);
	pos = velPosBuff - size - Vec2<float>(-velOffset, -velOffset);
	if (intersectedLine == INTERSECTED_TOP) isHitTop = true;
	if (intersectedLine == INTERSECTED_RIGHT) isHitRight = true;
	if (intersectedLine == INTERSECTED_LEFT) isHitLeft = true;
	if (intersectedLine == INTERSECTED_BOTTOM) isHitBottom = true;

	// ����
	velPosBuff = pos + Vec2<float>(-size.x, size.y) + Vec2<float>(velOffset, -velOffset);
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheVel(velPosBuff, prevPos + Vec2<float>(-size.x, size.y) + Vec2<float>(velOffset, -velOffset), {}, {}, MapData, false);
	pos = velPosBuff - Vec2<float>(-size.x, size.y) - Vec2<float>(velOffset, -velOffset);
	if (intersectedLine == INTERSECTED_TOP) isHitTop = true;
	if (intersectedLine == INTERSECTED_RIGHT) isHitRight = true;
	if (intersectedLine == INTERSECTED_LEFT) isHitLeft = true;
	if (intersectedLine == INTERSECTED_BOTTOM) isHitBottom = true;

	// �X�P�[�������ɂ����}�b�v�`�b�v�Ƃ̓����蔻����s���B

	// �����
	float offset = 5.0f;
	Vec2<float> posBuff = pos + Vec2<float>(size.x - offset, 0);
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(posBuff, size, MapData, INTERSECTED_TOP);
	pos.y = posBuff.y;
	SaveHitInfo(isHitTop, isHitBottom, isHitLeft, isHitRight, intersectedLine);
	posBuff = pos - Vec2<float>(size.x - offset, 0);
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(posBuff, size, MapData, INTERSECTED_TOP);
	pos.y = posBuff.y;
	SaveHitInfo(isHitTop, isHitBottom, isHitLeft, isHitRight, intersectedLine);
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(pos, size, MapData, INTERSECTED_TOP);
	SaveHitInfo(isHitTop, isHitBottom, isHitLeft, isHitRight, intersectedLine);

	// ������
	posBuff = pos + Vec2<float>(size.x - offset, 0);
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(posBuff, size, MapData, INTERSECTED_BOTTOM);
	pos.y = posBuff.y;
	SaveHitInfo(isHitTop, isHitBottom, isHitLeft, isHitRight, intersectedLine);
	posBuff = pos - Vec2<float>(size.x - offset, 0);
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(posBuff, size, MapData, INTERSECTED_BOTTOM);
	pos.y = posBuff.y;
	SaveHitInfo(isHitTop, isHitBottom, isHitLeft, isHitRight, intersectedLine);
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(pos, size, MapData, INTERSECTED_BOTTOM);
	SaveHitInfo(isHitTop, isHitBottom, isHitLeft, isHitRight, intersectedLine);

	// �E����
	posBuff = pos + Vec2<float>(0, size.y - offset);
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(posBuff, size, MapData, INTERSECTED_RIGHT);
	pos.x = posBuff.x;
	SaveHitInfo(isHitTop, isHitBottom, isHitLeft, isHitRight, intersectedLine);
	posBuff = pos - Vec2<float>(0, size.y - offset);
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(posBuff, size, MapData, INTERSECTED_RIGHT);
	pos.x = posBuff.x;
	SaveHitInfo(isHitTop, isHitBottom, isHitLeft, isHitRight, intersectedLine);
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(pos, size, MapData, INTERSECTED_RIGHT);
	SaveHitInfo(isHitTop, isHitBottom, isHitLeft, isHitRight, intersectedLine);

	// ������
	posBuff = pos + Vec2<float>(0, size.y - offset);
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(posBuff, size, MapData, INTERSECTED_LEFT);
	pos.x = posBuff.x;
	SaveHitInfo(isHitTop, isHitBottom, isHitLeft, isHitRight, intersectedLine);
	posBuff = pos - Vec2<float>(0, size.y - offset);
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(posBuff, size, MapData, INTERSECTED_LEFT);
	pos.x = posBuff.x;
	SaveHitInfo(isHitTop, isHitBottom, isHitLeft, isHitRight, intersectedLine);
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(pos, size, MapData, INTERSECTED_LEFT);
	SaveHitInfo(isHitTop, isHitBottom, isHitLeft, isHitRight, intersectedLine);


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
		if (partner.lock()->addSwingAngle <= ADD_SWING_ANGLE * 1.0f) {

		}
		else if (partner.lock()->addSwingAngle <= ADD_SWING_ANGLE * 3.0f) {

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

		}
		else {

			// �U��񂳂�Ă����Ԃ�������A�V�F�C�N�𔭐������ĐU��񂵏�Ԃ���������B
			Vec2<float>vec = { 0,0 };
			if (partner.lock()->GetNowSwing()) {

				if (isHitLeft)vec.x = -1.0f;
				else if (isHitRight)vec.x = 1.0f;
				if (isHitTop)vec.y = -1.0f;
				else if (isHitBottom)vec.y = 1.0f;

				Crash(vec);
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

			}
		}

		// �ǂ͂��݂̔���
		if (stackWindowTimer <= 0) {

			Vec2<int> windowSize = WinApp::Instance()->GetWinCenter();
			windowSize *= Vec2<int>(2, 2);

			// �E�B���h�E���E
			bool winLeft = pos.x - size.x - ScrollMgr::Instance()->scrollAmount.x <= 0;
			bool winRight = windowSize.x <= pos.x + size.x - ScrollMgr::Instance()->scrollAmount.x;
			if (winRight || winLeft) {

				stackWindowTimer = STACK_WINDOW_TIMER;


				//CrashMgr::Instance()->Crash(pos, crashDevice, { false,true });
				//SuperiorityGauge::Instance()->AddPlayerGauge(DebugParameter::Instance()->gaugeData->enemyClashDamageValue);
				SuperiorityGauge::Instance()->AddGauge(team, -20);
				Crash({ winRight ? 1.0f : -1.0f , 0.0f });

				// �`�[���ɉ����ăN���b�V���������Z����ϐ���ς���B
				if (team == WHICH_TEAM::LEFT_TEAM) {
					++ResultTransfer::Instance()->leftCrashCount;
				}
				else {
					++ResultTransfer::Instance()->rightCrashCount;
				}
			}
			// �E�B���h�E�㉺
			bool winTop = pos.y - size.y - ScrollMgr::Instance()->scrollAmount.y <= 0;
			bool winBottom = windowSize.y <= pos.y + size.y - ScrollMgr::Instance()->scrollAmount.y;
			if (winBottom || winTop) {

				stackWindowTimer = STACK_WINDOW_TIMER;

				//CrashMgr::Instance()->Crash(pos, crashDevice, { true,false });
				//SuperiorityGauge::Instance()->AddPlayerGauge(DebugParameter::Instance()->gaugeData->enemyClashDamageValue);
				SuperiorityGauge::Instance()->AddGauge(team, -20);
				Crash({ 0.0f,winBottom ? 1.0f : -1.0f });

				// �`�[���ɉ����ăN���b�V���������Z����ϐ���ς���B
				if (team == WHICH_TEAM::LEFT_TEAM) {
					++ResultTransfer::Instance()->leftCrashCount;
				}
				else {
					++ResultTransfer::Instance()->rightCrashCount;
				}
			}
		}

	}

	//prevIntersectedLine = intersectedBuff;


}

void CharacterInterFace::FinishSwing()
{
	nowSwing = false;
	partner.lock()->stagingDevice.StopSpin();
	partner.lock()->OnSwingedFinish();
}