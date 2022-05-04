#pragma once
#include "Vec.h"
#include "Color.h"
#include"Angle.h"

// スタミナアイテム
class StaminaItem {

private:
	static const enum STAR_COLOR { RED, GREEN, YELLOW, BLUE, GRAPH_NUM, 
		LEFT = GREEN, RIGHT = RED, NONE = YELLOW };
	static int GRAPH_HANDLE[GRAPH_NUM];

	/*===== メンバ変数 =====*/

	Vec2<float> pos;		// アイテムの座標
	Vec2<float> forwardVec;	// 移動方向 主にクラッシュなどで散らばる際に使用する。
	Vec2<float>* charaPos;	// 取得したキャラの座標
	int graph;
	float vel;				// 移動量 主にクラッシュなどで散らばる際に使用する。
	float healAmount;		// このアイテムの回復量
	bool isActive;			// 有効化フラグ
	bool isAcquired;		// 取得されたかフラグ
	Angle flashRad;

public:
	enum class STAMINA_ITEM_ID {

		SPONE_ITEM,		// 自動スポーンのアイテム
		CRASH_ITEM,		// クラッシュ時に散らばるアイテム

	};
	enum class CHARA_ID {
		LEFT,
		RIGHT
	};
private:
	STAMINA_ITEM_ID itemID;
	CHARA_ID charaID;


private:

	/*===== 定数 =====*/

	const float HIT_RADIUS = 60;	// 当たり判定を行う際の半径 大きめにしている。
	const float TRACING_SPEED = 30;	// 取得されて追跡するときのスピード。(演出用？)
	const float ACQUIRED_VEL = 20;	// 通常スポーンのアイテムが取得された時に広がる移動量。(演出用)


public:

	/*===== メンバ関数 =====*/

	StaminaItem();
	void Init();
	void Generate(const Vec2<float>& GeneratePos, const Vec2<float>& ForwardVec, const float& HealAmount, const float& Vel, STAMINA_ITEM_ID ItemID, const bool& IsAcquired = false, Vec2<float>* CharaPos = nullptr, CHARA_ID CharaID = CHARA_ID::LEFT);
	void Update();
	void Draw();

	// キャラクターとの当たり判定
	bool CheckHit(Vec2<float>* CharaPos, const float& CharaRadius, CHARA_ID CharaID);

	// 取得された状態にする。
	void Acquire(Vec2<float>* CharaPos, CHARA_ID CharaID);

	// 各種アクセッタ
	inline const bool& GetIsActive() { return isActive; }
	inline const bool& GetIsAcquired() { return isAcquired; }
	inline const float& GetHealAmount() { return healAmount; }
	inline const CHARA_ID& GetCharaID() { return charaID; }

};
