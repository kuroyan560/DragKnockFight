#pragma once
#include "Vec.h"

// 適当敵クラス
class Enemy {

private:

	/*-- メンバ変数 --*/

	Vec2<float> pos;
	bool isHit;
	//Vec2<float> forwardVec;


private:

	/*-- 定数 --*/

	//const float SPEED = 5.0f;
	const Vec2<float> SIZE = { 100.0f,100.0f };


public:

	/*-- メンバ関数 --*/

	// コンストラクタ
	Enemy();

	// 初期化処理
	void Init();

	// 更新処理
	void Update(const Vec2<float>& playerPos);

	// 描画処理
	void Draw();

	// 弾との当たり判定
	void CheckHitBullet();


};