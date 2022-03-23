#pragma once
#include "Vec.h"
#include "Singleton.h"
#include"KuroFunc.h"

class ShakeMgr : public Singleton<ShakeMgr> {

public:

	/*===== メンバ変数 =====*/

	float maxShakeAmount;	// シェイク量の最大量 シェイクを始める時はコレを変える。
	Vec2<float> shakeAmount;		// そのフレーム全体のシェイク量。


public:

	/*===== 定数 =====*/

	const float FIRST_SHOT_SHAKE_AMOUNT = 5.0f;
	const float DOSSUN_LOW_POWER_SHAKE_AMOUNT = 5.0f;


public:

	/*===== メンバ関数 =====*/

	// 初期化処理
	inline void Init() {

		maxShakeAmount = 0;
		shakeAmount = {};

	}

	// 更新処理
	inline void Update() {

		// シェイク量を減らす。
		if (maxShakeAmount > 0) maxShakeAmount -= 0.5f;

		// シェイク量を更新する。
		//shakeAmount.x = GetRand(maxShakeAmount * 2.0f) - maxShakeAmount;
		//shakeAmount.y = GetRand(maxShakeAmount * 2.0f) - maxShakeAmount;
		shakeAmount.x = KuroFunc::GetRand(maxShakeAmount * 2.0f) - maxShakeAmount;
		shakeAmount.y = KuroFunc::GetRand(maxShakeAmount * 2.0f) - maxShakeAmount;

		//shakeAmount = { 0.0f,0.0f };
	}

	// シェイク量をセットする。
	inline void SetShake(const int& set) {

		if (maxShakeAmount < set) {

			maxShakeAmount = set;

		}

	}


};