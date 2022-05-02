#include "StaminaItemMgr.h"
#include "KuroMath.h"
#include "KuroFunc.h"

void StaminaItemMgr::Init()
{

	for (int index = 0; index < ITEM_COUNT; ++index) {

		item[index].Init();

	}

}

void StaminaItemMgr::Generate(const Vec2<float>& GeneratePos, GENERATE_STATUS Status, Vec2<float>* CharaPos, StaminaItem::CHARA_ID CharaID)
{

	/*===== �������� =====*/

	// �X�e�[�^�X�Ɋ���ď����𕪂���B
	switch (Status)
	{
	case StaminaItemMgr::GENERATE_STATUS::CRASH:

		for (int generate = 0; generate < GENERATE_CRASH; ++generate) {

			for (int index = 0; index < ITEM_COUNT; ++index) {

				// �����ς݂������珈�����΂��B
				if (item[index].GetIsActive()) continue;

				//�����_���Ȋp�x�����߂�B
				Vec2<float> randomVec;
				randomVec = { KuroFunc::GetRand(-100.0f,100.0f),KuroFunc::GetRand(-100.0f,100.0f) };
				randomVec.Normalize();

				// ��������B
				item[index].Generate(GeneratePos, randomVec, HEAL_AMOUNT, CRASH_VEL, StaminaItem::STAMINA_ITEM_ID::CRASH_ITEM, true, CharaPos);

				break;

			}

		}


		break;
	case StaminaItemMgr::GENERATE_STATUS::SPONE:

		for (int index = 0; index < ITEM_COUNT; ++index) {

			// �����ς݂������珈�����΂��B
			if (item[index].GetIsActive()) continue;

			// ��������B
			item[index].Generate(GeneratePos, Vec2<float>(0, 0), HEAL_AMOUNT, 0, StaminaItem::STAMINA_ITEM_ID::SPONE_ITEM);

			break;

		}

		break;
	default:
		break;
	}


}

void StaminaItemMgr::Update()
{

	/*===== �X�V���� =====*/

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

int StaminaItemMgr::CheckHit(const Vec2<float>& CharaPos, const float& CharaRadius, StaminaItem::CHARA_ID CharaID)
{

	/*===== �����蔻����s���A�X�^�~�i�̉񕜗ʂ��擾���� =====*/

	int healAmount = 0;

	for (int index = 0; index < ITEM_COUNT; ++index) {

		// �������������珈�����΂��B
		if (!item[index].GetIsActive()) continue;

		// �����蔻����s���B
		bool isHit = item[index].CheckHit(CharaPos, CharaRadius, CharaID);

		// �������Ă��āA�A�C�e�����擾����Ă����Ԃ�������񕜗ʂ����Z����B
		if (isHit && item[index].GetIsAcquired()) {

			healAmount += item[index].GetHealAmount();

			// �A�C�e���������B
			item[index].Init();

		}
		// �����Ɏ擾����Ă��Ȃ�������ړ��ʂ𒲐�������ǂ����炱��������ď����������B

	}

	return healAmount;

}
