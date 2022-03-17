#pragma once
#include "Vec.h"

// 適当敵クラス
class Enemy {

private:

	/*-- メンバ変数 --*/

	Vec2<float> pos;
	Vec2<float> forwardVec;


private:

	/*-- 定数 --*/

	const float SPEED = 5.0f;
	const Vec2<float> SIZE = { 20.0f,20.0f };


public:

	/*-- メンバ関数 --*/

	// コンストラクタ
	Enemy();

	// 初期化処理
	void Init();

	// 更新処理
	void Update();

	// 描画処理
	void Draw();


};