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

	/*===== 生成処理 =====*/

	// 生成するデフォルト量をセット。
	int generateCount = GENERATE_CRASH;

	generateCount = GENERATE_CRASH;

	for (int generate = 0; generate < generateCount; ++generate) {

		for (int index = 0; index < ITEM_COUNT; ++index) {

			// 生成済みだったら処理を飛ばす。
			if (item[index].GetIsActive()) continue;

			//ランダムな角度を求める。
			Vec2<float> randomVec;
			randomVec = { KuroFunc::GetRand(-100.0f,100.0f),KuroFunc::GetRand(-100.0f,100.0f) };
			randomVec.Normalize();

			// 生成する。
			item[index].Generate(GeneratePos, randomVec, HEAL_AMOUNT, CRASH_VEL, StaminaItem::STAMINA_ITEM_ID::CRASH_ITEM, true, CharaPos, CharaID);

			break;

		}

	}

}

void StaminaItemMgr::GenerateSpone(const Vec2<float>& GeneratePos)
{

	/*===== 生成処理 =====*/

	for (int index = 0; index < ITEM_COUNT; ++index) {

		// 生成済みだったら処理を飛ばす。
		if (item[index].GetIsActive()) continue;

		// 生成する。
		item[index].Generate(GeneratePos, Vec2<float>(0, 0), HEAL_AMOUNT, 0, StaminaItem::STAMINA_ITEM_ID::SPONE_ITEM);

		break;

	}

}

void StaminaItemMgr::Update(const Vec2<float>& LeftUp, const Vec2<float>& RightDown)
{

	/*===== 更新処理 =====*/

	// 自動スポーンに関する更新を行う。
	//++sponeTimer;
	//if (SPONE_TIMER <= sponeTimer) {

	//	// 規定の数アイテムをすぽーんさせる。
	//	for (int generateCount = 0; generateCount < SPONE_COUNT; ++generateCount) {

	//		Vec2<float> randomSponePos = { KuroFunc::GetRand(LeftUp.x, RightDown.x), KuroFunc::GetRand(LeftUp.y,RightDown.y) };

	//		GenerateSpone(randomSponePos);

	//	}

	//	sponeTimer = 0;

	//}

	for (int index = 0; index < ITEM_COUNT; ++index) {

		// 未生成だったら処理を飛ばす。
		if (!item[index].GetIsActive())continue;

		item[index].Update();

	}

}

void StaminaItemMgr::Draw()
{

	/*===== 描画処理 =====*/

	for (int index = 0; index < ITEM_COUNT; ++index) {

		// 未生成だったら処理を飛ばす。
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

	/*===== 当たり判定を行い、スタミナの回復量を取得する =====*/

	if (CharaPos == nullptr) return 0;

	int healAmount = 0;
	int getNum = 0;

	for (int index = 0; index < ITEM_COUNT; ++index) {

		// 未生成だったら処理を飛ばす。
		if (!item[index].GetIsActive()) continue;

		// 当たり判定を行う。
		bool isHit = item[index].CheckHit(CharaPos, CharaRadius, PilotRadius, CharaID, PilotPos);

		if (isHit) {
			int a = 0;
		}

		// 当たっていて、アイテムが取得されている状態だったら回復量を加算する。
		if (isHit && item[index].GetIsAcquired()) {

			healAmount += item[index].GetHealAmount();
			getNum++;

			// アイテムを消す。
			item[index].Init();

		}
		// 取得されていなかったら。
		if (isHit && !item[index].GetIsAcquired()) {

			// 取得された状態にする。
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
