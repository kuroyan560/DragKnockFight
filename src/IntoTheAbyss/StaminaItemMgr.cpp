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
				item[index].Generate(GeneratePos, randomVec, HEAL_AMOUNT, CRASH_VEL, StaminaItem::STAMINA_ITEM_ID::CRASH_ITEM, true, CharaPos);

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

int StaminaItemMgr::CheckHit(const Vec2<float>& CharaPos, const float& CharaRadius, StaminaItem::CHARA_ID CharaID)
{

	/*===== 当たり判定を行い、スタミナの回復量を取得する =====*/

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
		// ここに取得されていなかったら移動量を調整したりどうたらこうたらって処理を書く。

	}

	return healAmount;

}
