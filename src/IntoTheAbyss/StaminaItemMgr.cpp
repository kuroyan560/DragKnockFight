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

	generateCount = GENERATE_CRASH;

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

void StaminaItemMgr::Update(const Vec2<float>& LeftUp, const Vec2<float>& RightDown)
{

	/*===== �X�V���� =====*/

	// �����X�|�[���Ɋւ���X�V���s���B
	//++sponeTimer;
	//if (SPONE_TIMER <= sponeTimer) {

	//	// �K��̐��A�C�e�������ہ[�񂳂���B
	//	for (int generateCount = 0; generateCount < SPONE_COUNT; ++generateCount) {

	//		Vec2<float> randomSponePos = { KuroFunc::GetRand(LeftUp.x, RightDown.x), KuroFunc::GetRand(LeftUp.y,RightDown.y) };

	//		GenerateSpone(randomSponePos);

	//	}

	//	sponeTimer = 0;

	//}

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

#include"AudioApp.h"
int StaminaItemMgr::CheckHit(Vec2<float>* CharaPos, const float& CharaRadius, const float& PilotRadius, StaminaItem::CHARA_ID CharaID, const Vec2<float>* PilotPos)
{
	static bool SE_INIT = false;
	static int SE[2];
	static const float SE_VOL = 1.0f;

	if (!SE_INIT)
	{
		SE[0] = AudioApp::Instance()->LoadAudio("resource/ChainCombat/sound/itemGet_0.wav", SE_VOL);
		SE[1] = AudioApp::Instance()->LoadAudio("resource/ChainCombat/sound/itemGet_1.wav", SE_VOL);

		SE_INIT = true;
	}

	/*===== �����蔻����s���A�X�^�~�i�̉񕜗ʂ��擾���� =====*/

	if (CharaPos == nullptr) return 0;

	int healAmount = 0;
	int getNum = 0;

	for (int index = 0; index < ITEM_COUNT; ++index) {

		// �������������珈�����΂��B
		if (!item[index].GetIsActive()) continue;

		// �����蔻����s���B
		bool isHit = item[index].CheckHit(CharaPos, CharaRadius, PilotRadius, CharaID, PilotPos);

		if (isHit) {
			int a = 0;
		}

		// �������Ă��āA�A�C�e�����擾����Ă����Ԃ�������񕜗ʂ����Z����B
		if (isHit && item[index].GetIsAcquired()) {

			healAmount += item[index].GetHealAmount();
			getNum++;

			// �A�C�e���������B
			item[index].Init();

		}
		// �擾����Ă��Ȃ�������B
		if (isHit && !item[index].GetIsAcquired()) {

			// �擾���ꂽ��Ԃɂ���B
			item[index].Acquire(CharaPos, CharaID);
		}

	}

	std::vector<int>seHandles;
	for (int i = 0; i < getNum; ++i)
	{
		//seHandles.emplace_back(SE[KuroFunc::GetRand(1)]);
	}
	AudioApp::Instance()->PlayWaveArray(seHandles);

	return healAmount;

}
