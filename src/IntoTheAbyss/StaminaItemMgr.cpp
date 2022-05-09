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

	// 戦闘エリアの中心地点。
	float areaHalfPos = areaRighPos / 2.0f;
	float rate = 0;

	// 位置関係に応じて生成する量を増やす。
	switch (CharaID)
	{
	case StaminaItem::CHARA_ID::LEFT:

		// 半分より左に行ってなかったら処理を飛ばす。
		if (areaHalfPos < SwingCharaPos.x) break;

		// 増やすべき割合を求める。
		rate = SwingCharaPos.x / areaHalfPos;

		// 増やす。
		generateCount += (1.0f - rate) * ADD_GENERATE_CRASH;

		break;
	case StaminaItem::CHARA_ID::RIGHT:

		// 半分より右に行ってなかったら処理を飛ばす。
		if (SwingCharaPos.x < areaHalfPos) break;

		// 増やすべき割合を求める。
		rate = (SwingCharaPos.x - areaHalfPos) / areaHalfPos;

		// 増やす。
		generateCount += rate * ADD_GENERATE_CRASH;

		break;
	default:
		break;
	}

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

void StaminaItemMgr::Update()
{

	/*===== 更新処理 =====*/

	// 自動スポーンに関する更新を行う。
	++sponeTimer;
	if (SPONE_TIMER <= sponeTimer) {

		Vec2<float> randomSponePos = { KuroFunc::GetRand(0.0f, 2000.0f), KuroFunc::GetRand(0.0f,1000.0f) };

		GenerateSpone(randomSponePos);

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

int StaminaItemMgr::CheckHit(Vec2<float>* CharaPos, const float& CharaRadius, const float& PilotRadius, StaminaItem::CHARA_ID CharaID, const Vec2<float>* PilotPos)
{

	/*===== 当たり判定を行い、スタミナの回復量を取得する =====*/

	if (CharaPos == nullptr) return 0;

	int healAmount = 0;

	for (int index = 0; index < ITEM_COUNT; ++index) {

		// 未生成だったら処理を飛ばす。
		if (!item[index].GetIsActive()) continue;

		// 当たり判定を行う。
		bool isHit = item[index].CheckHit(CharaPos, CharaRadius, PilotRadius, CharaID, PilotPos);

		if(isHit){
		int a = 0;
		}

		// 当たっていて、アイテムが取得されている状態だったら回復量を加算する。
		if (isHit && item[index].GetIsAcquired()) {

			healAmount += item[index].GetHealAmount();

			// アイテムを消す。
			item[index].Init();

		}
		// 取得されていなかったら。
		if (isHit && !item[index].GetIsAcquired()) {

			// 取得された状態にする。
			item[index].Acquire(CharaPos, CharaID);
		}

	}

	return healAmount;

}
