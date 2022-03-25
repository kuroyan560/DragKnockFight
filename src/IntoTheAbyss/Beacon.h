#pragma once
#include "Vec.h"

// 短槍識別用列挙体
enum PIKE_ID {

	PIKE_TELEPORT,		// 瞬間移動の短槍
	PIKE_TIMESTOP,		// 時間停止の短槍

};

// ビーコンクラス
class Beacon {

public:

	/*===== メンバ変数 =====*/

	Vec2<float> pos;				// 座標
	Vec2<float> prevFramePos;		// 前フレームの座標
	Vec2<float> forwardVec;		// 移動方向ベクトル
	Vec2<float> gimmickVel;
	Vec2<float> prevFrameScroll;	// 前フレームのウィンドウスクロール量 ウィンドウに刺さったときにウィンドウに合わせて動かすために必要
	float* stopTargetPos;
	float stopPos;
	float speed;			// 移動速度
	bool isActive;			// 有効化されているか(発射されたか)フラグ
	bool isHitWall;			// 壁に当たっているかのフラグ
	bool isHitWindow;		// ウィンドウにヒットしたかどうかのフラグ

	// 短槍を識別するためのID
	PIKE_ID id;


public:

	/*===== 定数 =====*/

	const float MAX_SPEED = 20.0f;	// 移動速度
	const float SCALE = 10.0f;		// 大きさ


public:

	/*===== メンバ関数 =====*/

	// コンストラクタ
	Beacon();

	// 初期化処理
	void Init();

	// 生成処理
	void Generate(const Vec2<float>& generatePos, const Vec2<float>& forwadVec, const PIKE_ID& id);

	// 更新処理
	void Update();

	// 描画処理
	void Draw();

	// 再び動かす。
	void MoveAgain() {
		if (stopTargetPos != nullptr)*stopTargetPos = stopPos;
	}

};