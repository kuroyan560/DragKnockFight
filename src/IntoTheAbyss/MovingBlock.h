#pragma once
#include "Vec.h"
#include <vector>
using namespace std;

// 弾を当てることで動くブロック
class MovingBlock {

public:

	/*===== メンバ変数 =====*/

	Vec2<float> pos;					// 座標
	Vec2<float> prevFramePos;			// 前フレームの座標
	Vec2<float> vel;					// 移動量
	float gravity;				// 重力
	bool onGround;				// 接地フラグ
	bool isCloseToThePlayer;	// プレイヤーに近いかどうかのフラグ 軽量化のために使用


public:

	/*===== 定数 =====*/

	const float SCALE = 25.0f;			// サイズ
	const float ADD_GRAVITY = 0.5f;		// 重力に加算する量。
	const float MAX_GRAVITY = 15.0f;	// 重力の最大量。
	const float ADD_VEL = 2.0f;			// 移動量に加算する量。
	const float MAX_VEL = 15.0f;		// 移動量の最大値。
	const float DO_PROCESS_DISTANCE = 1500;	// 処理を行う距離。 軽量化のために使用


public:

	/*===== メンバ関数 =====*/

	// コンストラクタ
	MovingBlock(const Vec2<float>& generatePos);

	// 初期化処理
	void Init();

	// 更新処理
	void Update(const Vec2<float>& playerPos);

	// 描画処理
	void Draw(const int& graphHandle);

	// 地面と当たった時の処理
	void HitGround();

	// 弾と当たった時の処理
	void HitBullet(const Vec2<float>& bulletForwardVec);

	// マップチップとの当たり判定
	void CheckHitMapChip(const vector<vector<int>>& mapData);

};