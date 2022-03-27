#pragma once
#include "Singleton.h"

// スローモーション演出及びヒットストップの際のスローなどに使用する値を管理するクラス
class SlowMgr : public Singleton<SlowMgr> {

public:

	/*===== メンバ変数 =====*/

	float playerDeadSlow;	// プレイヤーのスローモーション
	float prevFramePlayerDeadSlow;	// 前フレーム


public:

	/*===== 定数 =====*/


public:

	/*===== メンバ変数 =====*/

	// コンストラクタ
	SlowMgr() {

		playerDeadSlow = 1.0f;

	}

	// 更新処理
	void Update() {

		prevFramePlayerDeadSlow = playerDeadSlow;

	}

};