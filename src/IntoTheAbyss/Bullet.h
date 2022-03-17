#pragma once
#include "Vec.h"
#include <vector>

using namespace std;

class MovingBlock;

// 弾クラス
class Bullet {

public:

	/*-- 定数 --*/

	const float MAX_RADIUS = 5.0f;
	const float MAX_SPEED = 30.0f;
	const int DEAD_TIMER = 120;

	enum SHOT_HAND {
		R_HAND,
		L_HAND,
	};


public:

	/*-- メンバ変数 --*/

	Vec2<float> pos;			// 座標
	Vec2<float> forwardVec;	// 移動方向ベクトル
	float alpha;
	float speed;
	int deadTimer;		// 死ぬまでの時間
	bool isActive;		// 生存しているかのフラグ
	bool isFirstFrame;
	SHOT_HAND handID;	// 撃たれた手


public:

	/*-- メンバ関数 --*/

	// コンストラクタ
	Bullet();

	// 初期化処理
	void Init();

	// 生成処理
	void Generate(const Vec2<float>& generatePos, const Vec2<float> forwardVec, const bool isFirstShot, const SHOT_HAND& id);

	// 更新処理
	void Update();

	// 描画処理
	void Draw();

	// 生存フラグのゲッタ
	inline const bool& GetIsActive() { return isActive; }


};