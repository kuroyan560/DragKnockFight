#pragma once
#include "StaminaItem.h"
#include "Singleton.h"
#include "Vec.h"
#include <array>

class StaminaItemMgr : public Singleton<StaminaItemMgr> {

private:

	/*===== メンバ変数 =====*/

	static const int ITEM_COUNT = 100;
	std::array<StaminaItem, ITEM_COUNT> item;

	float areaLeftPos;		// 戦闘エリアの左の座標
	float areaRighPos;		// 戦闘エリアの右の座標

	int sponeTimer = 0;
	const int SPONE_TIMER = 60;	// アイテムが自動スポーンするタイマー
	const int SPONE_COUNT = 3;	// 一回のスポーンでアイテムがすぽーんする数

	const int GENERATE_CRASH = 3;		// クラッシュ時に生成するアイテム
	const int ADD_GENERATE_CRASH = 10;	// クラッシュ時に生成するアイテム
	const float HEAL_AMOUNT = 20;	// アイテム1個のヒール量
	const float CRASH_VEL = 20;		// クラッシュした際にアイテムが広がる量(演出用)

public:

	// アイテムを生成する際に、どの種類の生成を行うかの識別用
	enum class GENERATE_STATUS
	{
		CRASH,
		SPONE,
	};


public:

	/*===== メンバ変数 =====*/

	void SetArea(const float& LeftUpPos, const float& RightDownPos);
	void Init();
	void GenerateCrash(const Vec2<float>& GeneratePos, GENERATE_STATUS Status, Vec2<float>* CharaPos, StaminaItem::CHARA_ID CharaID, const Vec2<float>& SwingCharaPos);
	void GenerateSpone(const Vec2<float>& GeneratePos);
	void Update(const Vec2<float>& LeftUp, const Vec2<float>& RightDown);
	void Draw();

	// 当たり判定を行い、スタミナの回復量を取得する。
	int CheckHit(Vec2<float>* CharaPos, const float& CharaRadius, const float& PilotRadius, StaminaItem::CHARA_ID CharaID, const Vec2<float>* PilotPos = nullptr);

	// アイテム配列のゲッタ
	inline const std::array<StaminaItem, ITEM_COUNT> GetItemArray() { return item; }
	inline const Vec2<float> GetPos(const int& Index){return item[Index].GetPos();}

};