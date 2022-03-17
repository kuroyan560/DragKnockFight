#pragma once
#include "Vec.h"

// 時間停止の短槍の性能を確認する用のプレイヤーを追尾するブロック
class TimeStopTestBlock {

public:

	/*===== メンバ変数 =====*/

	Vec2<float> pos;				// 現在座標
	bool isTimeStop;		// 時間停止の短槍が刺さっているかのフラグ


public:

	/*===== 定数 =====*/

	const float SCALE = 10.0f;


public:

	/*===== メンバ関数 =====*/

	// コンストラクタ
	TimeStopTestBlock();

	// 更新処理
	void Update(const Vec2<float>& playerPos);

	// 描画処理
	void Draw(const int& blockGraph);
};