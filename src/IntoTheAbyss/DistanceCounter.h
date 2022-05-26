#pragma once
#include "Vec.h"
#include "Singleton.h"
#include <array>

class DistanceCounter : public Singleton<DistanceCounter> {

public:

	/*===== メンバ関数 =====*/

	float distance;		// 2キャラ間の距離
	Vec2<float> lineCenterPos;	// 2キャラ間の中心点の座標
	Vec2<float> shakeAmount;	// シェイク量
	const float MAX_SHAKE_DISTANCE = 1000.0f;	// シェイクを始める開始距離
	const float MAX_SHAKE_AMOUNT = 15.0f;		// シェイクの最大量

	const float DEAD_LINE = 300.0f;		// 当たったら終わりの距離

	std::array<int, 12> fontGraph;

	bool isExpSmall;	// 小さくするかどうかフラグ
	float exp;			// 今の大きさの割合


public:

	/*===== メンバ関数 =====*/

	// コンストラクタ
	DistanceCounter();

	// 初期化処理
	void Init();

	// 更新処理
	void Update();

	// 描画処理
	void Draw();

};