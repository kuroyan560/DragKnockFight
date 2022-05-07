#include "StaminaItem.h"
#include "ScrollMgr.h"
#include "DrawFunc.h"
#include"TexHandleMgr.h"
#include "BulletCollision.h"

int StaminaItem::GRAPH_HANDLE[STAR_COLOR::GRAPH_NUM] = { -1 };

StaminaItem::StaminaItem()
{
	if (GRAPH_HANDLE[0] == -1)
	{
		TexHandleMgr::LoadDivGraph("resource/ChainCombat/background_star.png", 4, { 4,1 }, GRAPH_HANDLE);
	}

	/*===== ������ =====*/

	isActive = false;

	collisionData = std::make_shared<SphereCollision>();
	collisionData->center = &pos;
	collisionData->radius = HIT_RADIUS;

}

void StaminaItem::Init()
{

	/*===== ������ =====*/

	isActive = false;

}

void StaminaItem::Generate(const Vec2<float>& GeneratePos, const Vec2<float>& ForwardVec, const float& HealAmount, const float& Vel, STAMINA_ITEM_ID ItemID, const bool& IsAcquired, Vec2<float>* CharaPos, CHARA_ID CharaID)
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
	graph = GRAPH_HANDLE[STAR_COLOR::NONE];
	isActive = true;

	// ���߂���擾����Ă����Ԃ�������Q�Ƃ�ۑ�����B
	if (isAcquired && CharaPos != nullptr) {

		charaPos = CharaPos;

		if (CharaID == CHARA_ID::LEFT)
		{
			graph = GRAPH_HANDLE[STAR_COLOR::LEFT];
		}
		else
		{
			graph = GRAPH_HANDLE[STAR_COLOR::RIGHT];
		}
	}

	flashRad = Angle(KuroFunc::GetRand(0, 360));
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

		// �擾����Ă��Ȃ�������_��
		if (!isAcquired)
		{
			flashRad += Angle::ConvertToRadian(1.0f);
			if (Angle(180) < flashRad)flashRad = 0.0f;
			break;
		}

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

	//DrawFunc::DrawCircle2D(ScrollMgr::Instance()->Affect(pos), DRAW_RADIUS * Camera::Instance()->zoom, itemColor, isAcquired);
	const float extDiffer = isAcquired ? 1.0f : 0.75f;
	const Vec2<float>extRate = { Camera::Instance()->zoom * extDiffer,Camera::Instance()->zoom * extDiffer };
	static const float ALPHA_MIN = 0.6f;
	const float alpha = isAcquired ? 1.0f : (sin(flashRad) * (1.0f - ALPHA_MIN) + ALPHA_MIN);
	DrawFunc::DrawRotaGraph2D(ScrollMgr::Instance()->Affect(pos), extRate, 0.0f, TexHandleMgr::GetTexBuffer(graph), Color(1.0f, 1.0f, 1.0f, alpha));

}

bool StaminaItem::CheckHit(Vec2<float>* CharaPos, const float& CharaRadius, const float& PilotRadius, CHARA_ID CharaID, const Vec2<float>* PilotPos)
{
	if (CharaPos == nullptr && PilotPos) return false;

	// �ړ����������瓖���蔻����s��Ȃ��B
	if (0 < vel) return false;

	// �������Ă�����B
	if (isAcquired) {

		// ���ݕۑ�����Ă���L�����N�^�[��ID�ƍ��n����Ă���ID�������������瓖���蔻����s���B
		if (charaID == CharaID) {

			// 2�_�Ԃ̋��������߂�B
			float distance = (pos - *CharaPos).Length();

			// 2�_�Ԃ̋��������a����������������B
			if (distance <= CharaRadius) {

				return true;

			}

			//�p�C���b�g�Ƃ̔���
			if (PilotPos != nullptr)
			{
				distance = PilotPos->Distance(pos);

				if (distance <= PilotRadius)return true;
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

		//�p�C���b�g�Ƃ̔���
		if (PilotPos != nullptr)
		{
			distance = PilotPos->Distance(pos);

			if (distance <= PilotRadius)return true;
		}
	}

	return false;
}

void StaminaItem::Acquire(Vec2<float>* CharaPos, CHARA_ID CharaID)
{

	/*===== �擾���ꂽ��Ԃɂ��� =====*/

	isAcquired = true;
	charaPos = CharaPos;
	forwardVec = pos - *CharaPos;
	forwardVec.Normalize();
	vel = ACQUIRED_VEL;
	charaID = CharaID;
	if (CharaID == CHARA_ID::LEFT)
	{
		graph = GRAPH_HANDLE[STAR_COLOR::LEFT];
	}
	else
	{
		graph = GRAPH_HANDLE[STAR_COLOR::RIGHT];
	}

}
