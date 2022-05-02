#include "StaminaItem.h"
#include "ScrollMgr.h"
#include "DrawFunc.h"

StaminaItem::StaminaItem()
{

	/*===== ������ =====*/

	isActive = false;

}

void StaminaItem::Init()
{

	/*===== ������ =====*/

	isActive = false;

}

void StaminaItem::Generate(const Vec2<float>& GeneratePos, const Vec2<float>& ForwardVec, const float& HealAmount, const float& Vel, STAMINA_ITEM_ID ItemID, const bool& IsAcquired, Vec2<float>* CharaPos, Color CharaColor, CHARA_ID CharaID)
{

	/*===== �������� =====*/

	// �ϐ���������B
	pos = GeneratePos;
	forwardVec = ForwardVec;
	healAmount = HealAmount;
	vel = Vel;
	isAcquired = IsAcquired;
	itemID = ItemID;
	charaID = CharaID;
	itemColor = CharaColor;
	isActive = true;

	// ���߂���擾����Ă����Ԃ�������Q�Ƃ�ۑ�����B
	if (isAcquired && CharaPos != nullptr) {

		charaPos = CharaPos;

	}

}

void StaminaItem::Update()
{

	/*===== �X�V���� =====*/

	float prevVel = 0;
	float moveAngle = 0;

	// ID�ɂ���ď����𕪂���B
	switch (itemID)
	{
	case StaminaItem::STAMINA_ITEM_ID::SPONE_ITEM:

		/*-- �X�|�[�������A�C�e���������� --*/

		// �擾����Ă��Ȃ������珈�����΂��B
		if (!isAcquired) break;

		// �ړ��ʂ�0�ɋ߂Â���B
		vel -= vel / 10.0f;
		if (vel < 1.0f) vel = 0;

		// �ړ��ʂ𑫂��B
		pos += forwardVec * vel;

		// [�ړ��ʂ�0] ���� [�������~�߂�^�C�}�[���N�����Ă��Ȃ�������] �L�����N�^�[�Ɍ������Ēǔ�������B
		if (vel <= 0 && charaPos != nullptr) {

			// �ړ����ׂ��p�x�����߂�B
			moveAngle = atan2f(charaPos->y - pos.y, charaPos->x - pos.x);

			pos += {cosf(moveAngle)* TRACING_SPEED, sinf(moveAngle)* TRACING_SPEED};

		}


		break;
	case StaminaItem::STAMINA_ITEM_ID::CRASH_ITEM:

		/*-- �N���b�V�����Ȃǂɏo��A�C�e���������� --*/

		// �ړ��ʂ�0�ɋ߂Â���B
		vel -= vel / 10.0f;
		if (vel < 0.5f) vel = 0;

		// �ړ��ʂ𑫂��B
		pos += forwardVec * vel;

		// [�ړ��ʂ�0] ���� [�������~�߂�^�C�}�[���N�����Ă��Ȃ�������] �L�����N�^�[�Ɍ������Ēǔ�������B
		if (vel <= 0 && charaPos != nullptr) {

			// �ړ����ׂ��p�x�����߂�B
			moveAngle = atan2f(charaPos->y - pos.y, charaPos->x - pos.x);

			pos += {cosf(moveAngle)* TRACING_SPEED, sinf(moveAngle)* TRACING_SPEED};

		}

		break;
	default:
		break;
	}


}

void StaminaItem::Draw()
{

	/*===== �`�揈�� =====*/

	DrawFunc::DrawCircle2D(ScrollMgr::Instance()->Affect(pos), DRAW_RADIUS * Camera::Instance()->zoom, itemColor, isAcquired);

}

bool StaminaItem::CheckHit(Vec2<float>* CharaPos, const float& CharaRadius, CHARA_ID CharaID)
{

	if (CharaPos == nullptr) return false;

	// �ړ����������瓖���蔻����s��Ȃ��B
	if (0 < vel) return false;

	// �������Ă�����B
	if (isAcquired) {

		// ���ݕۑ�����Ă���L�����N�^�[��ID�ƍ��n����Ă���ID�������������瓖���蔻����s���B
		if (charaID == CharaID) {

			// 2�_�Ԃ̋��������߂�B
			float distance = (pos - *CharaPos).Length();

			// 2�_�Ԃ̋��������a����������������B
			if (distance <= CharaRadius + HIT_RADIUS / 2.0f) {

				return true;

			}

		}

	}
	// �������Ă��Ȃ�������B
	else {

		// 2�_�Ԃ̋��������߂�B
		float distance = (pos - *CharaPos).Length();

		// 2�_�Ԃ̋��������a����������������B
		if (distance <= CharaRadius + HIT_RADIUS) {

			return true;

		}

	}

	return false;
}

void StaminaItem::Acquire(Vec2<float>* CharaPos, CHARA_ID CharaID, Color CharaColor)
{

	/*===== �擾���ꂽ��Ԃɂ��� =====*/

	isAcquired = true;
	charaPos = CharaPos;
	forwardVec = pos - *CharaPos;
	forwardVec.Normalize();
	vel = ACQUIRED_VEL;
	charaID = CharaID;
	itemColor = CharaColor;

}
