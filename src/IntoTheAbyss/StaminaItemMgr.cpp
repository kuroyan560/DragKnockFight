#include "StaminaItemMgr.h"
#include "KuroMath.h"
#include "KuroFunc.h"

void StaminaItemMgr::Init()
{

	for (int index = 0; index < ITEM_COUNT; ++index) {

		item[index].Init();

	}

}

void StaminaItemMgr::Generate(const Vec2<float>& GeneratePos, GENERATE_STATUS Status, Vec2<float>* CharaPos, Color CharaColor, StaminaItem::CHARA_ID CharaID)
{

	/*===== 生成処理 =====*/

	// ステータスに寄って処理を分ける。
	switch (Status)
	{
	case StaminaItemMgr::GENERATE_STATUS::CRASH:

		for (int generate = 0; generate < GENERATE_CRASH; ++generate) {

			for (int index = 0; index < ITEM_COUNT; ++index) {

				// 生成済みだったら処理を飛ばす。
				if (item[index].GetIsActive()) continue;

				//ランダムな角度を求める。
				Vec2<float> randomVec;
				randomVec = { KuroFunc::GetRand(-100.0f,100.0f),KuroFunc::GetRand(-100.0f,100.0f) };
				randomVec.Normalize();

				// 生成する。
				item[index].Generate(GeneratePos, randomVec, HEAL_AMOUNT, CRASH_VEL, StaminaItem::STAMINA_ITEM_ID::CRASH_ITEM, true, CharaPos,CharaColor);

				break;

			}

		}


		break;
	case StaminaItemMgr::GENERATE_STATUS::SPONE:

		for (int index = 0; index < ITEM_COUNT; ++index) {

			// 生成済みだったら処理を飛ばす。
			if (item[index].GetIsActive()) continue;

			// 生成する。
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

	/*===== 更新処理 =====*/

	// 自動スポーンに関する更新を行う。
	++sponeTimer;
	if (SPONE_TIMER <= sponeTimer) {

		Vec2<float> randomSponePos = { KuroFunc::GetRand(0.0f, 2000.0f), KuroFunc::GetRand(0.0f,1000.0f) };

		Generate(randomSponePos, StaminaItemMgr::GENERATE_STATUS::SPONE);

		sponeTimer = 0;

	}

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

int StaminaItemMgr::CheckHit(Vec2<float>* CharaPos, const float& CharaRadius, StaminaItem::CHARA_ID CharaID, Color CharaColor)
{

	/*===== 当たり判定を行い、スタミナの回復量を取得する =====*/

	if (CharaPos == nullptr) return 0;

	int healAmount = 0;

	for (int index = 0; index < ITEM_COUNT; ++index) {

		// 未生成だったら処理を飛ばす。
		if (!item[index].GetIsActive()) continue;

		// 当たり判定を行う。
		bool isHit = item[index].CheckHit(CharaPos, CharaRadius, CharaID);

		// 当たっていて、アイテムが取得されている状態だったら回復量を加算する。
		if (isHit && item[index].GetIsAcquired()) {

			healAmount += item[index].GetHealAmount();

			// アイテムを消す。
			item[index].Init();

		}
		// 取得されていなかったら。
		if (isHit && !item[index].GetIsAcquired()) {

			// 取得された状態にする。
			item[index].Acquire(CharaPos, CharaID, CharaColor);

		}

	}

	return healAmount;

}
