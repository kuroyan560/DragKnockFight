#pragma once
#include "Vec.h"

enum ENEMY_ID {

	ENEMY_BOSS,
	ENEMY_SMALL,

};

// 適当敵クラス
class Enemy {

public:

	/*-- メンバ変数 --*/

	Vec2<float> pos;
	Vec2<float> size;
	bool isHit;
	ENEMY_ID id;
	bool isActive;
	int hitPoint;
	//Vec2<float> forwardVec;


private:

	/*-- 定数 --*/

	//const float SPEED = 5.0f;
	const Vec2<float> SIZE_BIG = { 100.0f,100.0f };
	const Vec2<float> SIZE_SMALL = { 30.0f,30.0f };
	const float KNOCK_BACK = 30.0f;
	const int HITPOINT_SMALL = 5;


public:

	/*-- メンバ関数 --*/

	// コンストラクタ
	Enemy();

	// 初期化処理
	void Init();

	// 生成処理
	void Generate(const ENEMY_ID& id);

	// 更新処理
	void Update(const Vec2<float>& playerPos);

	// 描画処理
	void Draw();

	// 弾との当たり判定
	void CheckHitBullet();


};