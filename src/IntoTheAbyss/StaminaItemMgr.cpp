#include "StaminaItemMgr.h"
#include "KuroMath.h"
#include "KuroFunc.h"

void StaminaItemMgr::SetArea(const float& LeftUpPos, const float& RightDownPos)
{
	areaLeftPos = LeftUpPos;
	areaRighPos = RightDownPos;
}

void StaminaItemMgr::Init()
{

	for (int index = 0; index < ITEM_COUNT; ++index) {

		item[index].Init();

	}

}

void StaminaItemMgr::GenerateCrash(const Vec2<float>& GeneratePos, GENERATE_STATUS Status, Vec2<float>* CharaPos, StaminaItem::CHARA_ID CharaID, const Vec2<float>& SwingCharaPos)
{

	/*===== �������� =====*/

	// ��������f�t�H���g�ʂ��Z�b�g�B
	int generateCount = GENERATE_CRASH;

	// �퓬�G���A�̒��S�n�_�B
	float areaHalfPos = areaRighPos / 2.0f;
	float rate = 0;

	// �ʒu�֌W�ɉ����Đ�������ʂ𑝂₷�B
	switch (CharaID)
	{
	case StaminaItem::CHARA_ID::LEFT:

		// ������荶�ɍs���ĂȂ������珈�����΂��B
		if (areaHalfPos < SwingCharaPos.x) break;

		// ���₷�ׂ����������߂�B
		rate = SwingCharaPos.x / areaHalfPos;

		// ���₷�B
		generateCount += (1.0f - rate) * ADD_GENERATE_CRASH;

		break;
	case StaminaItem::CHARA_ID::RIGHT:

		// �������E�ɍs���ĂȂ������珈�����΂��B
		if (SwingCharaPos.x < areaHalfPos) break;

		// ���₷�ׂ����������߂�B
		rate = (SwingCharaPos.x - areaHalfPos) / areaHalfPos;

		// ���₷�B
		generateCount += rate * ADD_GENERATE_CRASH;

		break;
	default:
		break;
	}

	for (int generate = 0; generate < generateCount; ++generate) {

		for (int index = 0; index < ITEM_COUNT; ++index) {

			// �����ς݂������珈�����΂��B
			if (item[index].GetIsActive()) continue;

			//�����_���Ȋp�x�����߂�B
			Vec2<float> randomVec;
			randomVec = { KuroFunc::GetRand(-100.0f,100.0f),KuroFunc::GetRand(-100.0f,100.0f) };
			randomVec.Normalize();

			// ��������B
			item[index].Generate(GeneratePos, randomVec, HEAL_AMOUNT, CRASH_VEL, StaminaItem::STAMINA_ITEM_ID::CRASH_ITEM, true, CharaPos, CharaID);

			break;

		}

	}

}

void StaminaItemMgr::GenerateSpone(const Vec2<float>& GeneratePos)
{

	/*===== �������� =====*/

	for (int index = 0; index < ITEM_COUNT; ++index) {

		// �����ς݂������珈�����΂��B
		if (item[index].GetIsActive()) continue;

		// ��������B
		item[index].Generate(GeneratePos, Vec2<float>(0, 0), HEAL_AMOUNT, 0, StaminaItem::STAMINA_ITEM_ID::SPONE_ITEM);

		break;

	}

}

void StaminaItemMgr::Update()
{

	/*===== �X�V���� =====*/

	// �����X�|�[���Ɋւ���X�V���s���B
	++sponeTimer;
	if (SPONE_TIMER <= sponeTimer) {

		Vec2<float> randomSponePos = { KuroFunc::GetRand(0.0f, 2000.0f), KuroFunc::GetRand(0.0f,1000.0f) };

		GenerateSpone(randomSponePos);

		sponeTimer = 0;

	}

	for (int index = 0; index < ITEM_COUNT; ++index) {

		// �������������珈�����΂��B
		if (!item[index].GetIsActive())continue;

		item[index].Update();

	}

}

void StaminaItemMgr::Draw()
{

	/*===== �`�揈�� =====*/

	for (int index = 0; index < ITEM_COUNT; ++index) {

		// �������������珈�����΂��B
		if (!item[index].GetIsActive())continue;

		item[index].Draw();

	}

}

int StaminaItemMgr::CheckHit(Vec2<float>* CharaPos, const float& CharaRadius, const float& PilotRadius, StaminaItem::CHARA_ID CharaID, const Vec2<float>* PilotPos)
{

	/*===== �����蔻����s���A�X�^�~�i�̉񕜗ʂ��擾���� =====*/

	if (CharaPos == nullptr) return 0;

	int healAmount = 0;

	for (int index = 0; index < ITEM_COUNT; ++index) {

		// �������������珈�����΂��B
		if (!item[index].GetIsActive()) continue;

		// �����蔻����s���B
		bool isHit = item[index].CheckHit(CharaPos, CharaRadius, PilotRadius, CharaID, PilotPos);

		if(isHit){
		int a = 0;
		}

		// �������Ă��āA�A�C�e�����擾����Ă����Ԃ�������񕜗ʂ����Z����B
		if (isHit && item[index].GetIsAcquired()) {

			healAmount += item[index].GetHealAmount();

			// �A�C�e���������B
			item[index].Init();

		}
		// �擾����Ă��Ȃ�������B
		if (isHit && !item[index].GetIsAcquired()) {

			// �擾���ꂽ��Ԃɂ���B
			item[index].Acquire(CharaPos, CharaID);
		}

	}

	return healAmount;

}
