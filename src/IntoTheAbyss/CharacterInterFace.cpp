#include "CharacterInterFace.h"
#include"CrashMgr.h"
#include"WinApp.h"
#include"SuperiorityGauge.h"
#include"ScrollMgr.h"

void CharacterInterFace::SwingUpdate()
{
	static const float ADD_EASINGTIMER_MINIMUM = 0.025f;	// �C�[�W���O�^�C�}�[�ɉ��Z����� �ŏ��l
	static const float ADD_EASINGTIMER_MAX = 0.05f;	// �C�[�W���O�^�C�}�[�ɉ��Z����� �ő�l

	// �C�[�W���O�ʂ����߂�B
	float easingChange = ADD_EASINGTIMER_MAX - ADD_EASINGTIMER_MINIMUM;
	float addEasingTimer = ADD_EASINGTIMER_MINIMUM + (fabs(fabs(swingEndVec.y) - fabs(swingStartVec.y)) * easingChange);

	// [�␳]X���̃x�N�g����0�������狭���I��0.01f�ɂ���B
	if (fabs(swingStartVec.x) <= 0.001f) swingStartVec.x = 0.001f;

	Vec2<float>easeNowVec = { 0,0 };

	// �C�[�W���O�^�C�}�[���X�V����B
	if (swingEaseRate < 1.0f) {

		// �C�[�W���O�^�C�}�[���X�V����B
		swingEaseRate += addEasingTimer;

		// ���E�l�𒴂��Ȃ��悤�ɂ���B
		if (1.0f <= swingEaseRate) swingEaseRate = 1.0f;

		// �C�[�W���O�^�C�}�[���狁�߂���C�[�W���O�ʂ����߂�B
		auto easeAmount = KuroMath::Ease(Out, Cubic, swingEaseRate, 0.0f, 1.0f);

		// ���݂̃x�N�g�������߂�B
		easeNowVec.y = swingStartVec.y + (easeAmount * (swingEndVec.y - swingStartVec.y));
		if (easeNowVec.y < 0) {
			easeNowVec.x = 1.0f + easeNowVec.y;
		}
		else {
			easeNowVec.x = 1.0f - easeNowVec.y;
		}
		easeNowVec.Normalize();
	}

	const float lineLength = LINE_LENGTH * 2 + addLineLength + partner.lock()->addLineLength;
	partner.lock()->pos = this->pos + easeNowVec * lineLength;

	// �C�[�W���O�^�C�}�[�����E�𒴂�����t���O��܂�B
	if (1.0f <= swingEaseRate) {
		nowSwing = false;
		swingInertiaVec = (pos - prevPos).GetNormal();
		swingInertia = (prevPos - pos).Length();
		afterSwingDelay = AFTER_SWING_DELAY;
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
	SuperiorityGauge::Instance()->AddPlayerGauge(10.0f);
}

void CharacterInterFace::CrashUpdate()
{
	//�U��񂳂�Ă���
	if (partner.lock()->nowSwing)
	{
		//���ʐU��񂵂���
		static const float CRASH_SWING_RATE = 0.15f;
		if (CRASH_SWING_RATE < partner.lock()->swingEaseRate)
		{
			Vec2<float>vec = { 0,0 };
			if (mapChipHit[LEFT])vec.x = -1.0f;
			else if (mapChipHit[RIGHT])vec.x = 1.0f;
			if (mapChipHit[TOP])vec.y = -1.0f;
			else if (mapChipHit[BOTTOM])vec.y = 1.0f;

			Crash(vec);
		}

		partner.lock()->nowSwing = false;
	}

	// �}�b�v�`�b�v�ɂ������Ă����Ԃŉ�ʊO�ɏo����
	if (stuckWindowTimer <= 0)
	{
		Vec2<int> windowSize = WinApp::Instance()->GetWinCenter();
		windowSize *= Vec2<int>(2, 2);

		// �E�B���h�E���E
		bool winLeft = pos.x - size.x - ScrollMgr::Instance()->scrollAmount.x <= 0;
		bool winRight = windowSize.x <= pos.x + size.x - ScrollMgr::Instance()->scrollAmount.x;
		if (winRight || winLeft) {

			stuckWindowTimer = STRUCK_WINDOW_TIMER;
			Crash({ winRight ? 1.0f : -1.0f , 0.0f });
		}

		// �E�B���h�E�㉺
		bool winTop = pos.y - size.y - ScrollMgr::Instance()->scrollAmount.y <= 0;
		bool winBottom = windowSize.y <= pos.y + size.y - ScrollMgr::Instance()->scrollAmount.y;
		if (winBottom || winTop) {

			stuckWindowTimer = STRUCK_WINDOW_TIMER;
			Crash({ 0.0f,winBottom ? 1.0f : -1.0f });
		}
	}
}

void CharacterInterFace::SwingPartner()
{
	//�U��񂵏��������ɑ����Ă���ꍇ�́A�d�˂ĐU��񂹂Ȃ�
	if (partner.lock()->nowSwing || nowSwing)return;

	//����Ƃ̃x�N�g���擾
	Vec2<float>dir = GetPartnerPos() - pos;
	dir.Normalize();

	//�U��񂵂̃f�b�h���C��
	static const float SWING_DEAD_LINE = 0.3f;
	if (fabs(dir.y) <= SWING_DEAD_LINE)return;

	//�U��񂵂̊J�n�x�N�g��
	swingStartVec = dir;
	// [�␳]X���̃x�N�g����0�������狭���I��0.01f�ɂ���B
	if (fabs(swingStartVec.x) <= 0.001f) swingStartVec.x = 0.001f;

	//�U��񂵂̏I���x�N�g��
	swingEndVec = swingStartVec * Vec2<float>(1.0f, -1.0f);

	//�U��񂵃C�[�W���O�����Z�b�g
	swingEaseRate = 0.0f;

	//�U��񂵃t���O�̗L����
	nowSwing = true;
}

void CharacterInterFace::Init(const Vec2<float>& GeneratePos, const Vec2<float>& Size)
{
	pos = GeneratePos;
	vel = { 0,0 };
	size = Size;

	stuckWindowTimer = 0;

	nowSwing = false;

	stackMapChip = false;
	for (auto& flg : mapChipHit)flg = false;

	stagingDevice.Init();

	addLineLength = 0.0f;

	swingInertia = 0;
	swingInertiaVec = {};
	afterSwingDelay = 0;

	OnInit();
}

void CharacterInterFace::Update(const std::vector<std::vector<int>>& MapData, const Vec2<float>& LineCenterPos)
{
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
	//���肪�U��񂵂Ă��Ȃ��Ƃ�
	else if (!partner.lock()->nowSwing)
	{
		OnUpdate(MapData);
		//�E�B���h�E�̈����������Ă��锻��̃^�C�}�[�X�V
		if (0 < stuckWindowTimer) {

			--stuckWindowTimer;
		}
	}


	//�����蔻��
	CheckHit(MapData, LineCenterPos);

	//�����������Ă���
	if (stackMapChip)
	{
		CrashUpdate();
	}

	//�}�b�v�`�b�v�ƈ����������Ă���t���O�����낷
	stackMapChip = false;

	//�}�b�v�`�b�v�Ƃ̓����蔻�肪�������Ƃ��̏����Ăяo��
	for (int i = 0; i < HIT_DIR_NUM; ++i)
	{
		if (mapChipHit[i])
		{
			OnHitMapChip((HIT_DIR)i);
			mapChipHit[i] = false;
		}
	}
}

void CharacterInterFace::Draw()
{
	OnDraw();
}

#include"Intersected.h"
#include"MapChipCollider.h"
void CharacterInterFace::CheckHit(const std::vector<std::vector<int>>& MapData, const Vec2<float>& LineCenterPos)
{
	/*===== �����蔻�� =====*/

	//�e�L�����̓��L�̓����蔻��
	OnCheckHit(MapData, LineCenterPos);

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
	if (!isHitTop && intersectedLine == INTERSECTED_TOP) isHitTop = true;
	posBuff = pos - Vec2<float>(size.x - offset, 0);
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(posBuff, size, MapData, INTERSECTED_TOP);
	pos.y = posBuff.y;
	if (!isHitTop && intersectedLine == INTERSECTED_TOP) isHitTop = true;
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(pos, size, MapData, INTERSECTED_TOP);
	if (!isHitTop && intersectedLine == INTERSECTED_TOP) isHitTop = true;

	// ������
	posBuff = pos + Vec2<float>(size.x - offset, 0);
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(posBuff, size, MapData, INTERSECTED_BOTTOM);
	pos.y = posBuff.y;
	if (!isHitBottom && intersectedLine == INTERSECTED_BOTTOM) isHitBottom = true;
	posBuff = pos - Vec2<float>(size.x - offset, 0);
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(posBuff, size, MapData, INTERSECTED_BOTTOM);
	pos.y = posBuff.y;
	if (!isHitBottom && intersectedLine == INTERSECTED_BOTTOM) isHitBottom = true;
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(pos, size, MapData, INTERSECTED_BOTTOM);
	if (!isHitBottom && intersectedLine == INTERSECTED_BOTTOM) isHitBottom = true;

	// �E����
	posBuff = pos + Vec2<float>(0, size.y - offset);
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(posBuff, size, MapData, INTERSECTED_RIGHT);
	pos.x = posBuff.x;
	if (!isHitRight && intersectedLine == INTERSECTED_RIGHT) isHitRight = true;
	posBuff = pos - Vec2<float>(0, size.y - offset);
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(posBuff, size, MapData, INTERSECTED_RIGHT);
	pos.x = posBuff.x;
	if (!isHitRight && intersectedLine == INTERSECTED_RIGHT) isHitRight = true;
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(pos, size, MapData, INTERSECTED_RIGHT);
	if (!isHitRight && intersectedLine == INTERSECTED_RIGHT) isHitRight = true;

	// ������
	posBuff = pos + Vec2<float>(0, size.y - offset);
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(posBuff, size, MapData, INTERSECTED_LEFT);
	pos.x = posBuff.x;
	if (!isHitLeft && intersectedLine == INTERSECTED_LEFT) isHitLeft = true;
	posBuff = pos - Vec2<float>(0, size.y - offset);
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(posBuff, size, MapData, INTERSECTED_LEFT);
	pos.x = posBuff.x;
	if (!isHitLeft && intersectedLine == INTERSECTED_LEFT) isHitLeft = true;
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(pos, size, MapData, INTERSECTED_LEFT);
	if (!isHitLeft && intersectedLine == INTERSECTED_LEFT) isHitLeft = true;

	// �}�b�v�`�b�v�Ɠ������Ă�����
	if (isHitTop || isHitRight || isHitLeft || isHitBottom) {

		// �}�b�v�`�b�v�Ɠ������Ă���ꏊ�ɂ���āA�����������Ă��邩�ǂ����𒲂ׂ�B

		// ����Ƃ̊p�x
		const auto partnerPos = partner.lock()->pos;
		float partnerAngle = atan2(partnerPos.y - pos.y, partnerPos.x - pos.x);
		partnerAngle = fabs(partnerAngle);

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
	}
}

