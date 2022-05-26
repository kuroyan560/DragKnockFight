#pragma once
#include "Vec.h"
#include "Singleton.h"

class DistanceCounter : public Singleton<DistanceCounter> {

public:

	/*===== メンバ関数 =====*/

	float distance;		// 2キャラ間の距離
	Vec2<float> lineCenterPos;	// 2キャラ間の中心点の座標

	const float DEAD_LINE = 300.0f;		// 当たったら終わりの距離


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