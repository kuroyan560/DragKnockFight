#pragma once
#include "Vec.h"
#include "Singleton.h"
#include <array>

// 死亡時のエフェクトパーティクル
class PlayerDeadParticle {

public:

	/*===== メンバ変数 ====*/

	Vec2<float> pos;
	Vec2<float> fowardVec;
	float angle;
	float radius;
	float length;
	bool isActive;


public:

	/*===== 定数 =====*/

	const float MAX_SPEED = 100.0f;
	const float DEF_RADIUS = 15.0f;


public:

	/*===== メンバ関数 =====*/

	// コンストラクタ
	PlayerDeadParticle();

	// 初期化処理
	void Init();

	// 生成処理
	void Generate(const Vec2<float>& generatePos, const Vec2<float>& forwardVec);

	// 更新処理
	void Update();

	// 描画処理
	void Draw();

};

// プレイヤー死亡エフェクトクラス
class PlayerDeadEffect : public Singleton<PlayerDeadEffect> {

public:

	/*===== メンバ変数 =====*/

	Vec2<float> playerPos;		// プレイヤーの座標

	int playerTurnWhiteTimer;	// プレイヤーを白くするまでのタイマー
	float playerWhitePar;		// プレイヤーを白くするパーセンテージ タイマーを元に計算する。

	int playerExitDelayTimer;	// プレイヤーが白くなってから消えるまでのタイマー
	bool isExitPlayer;			// プレイヤーの描画を切るか

	int particleStopTimer;		// パーティクルが止まるまでのタイマー
	float particleSpeedPar;		// パーティクルの移動速度の割合

	int blackOutDelayTimer;		// 暗転するまでの遅延のタイマー

	int blackOutTimer;			// 暗転するタイマー
	bool isInitStage;			// ステージを初期化するフラグ

	int slowTimer;				// 上のタイマー4つをあわせた、完全に回りが止まるまでのタイマー

	bool isActive;				// 死亡エフェクトが有効化されているか

	static const int PARTICLE_COUNT = 4;
	std::array<PlayerDeadParticle, PARTICLE_COUNT> deadParticle;


public:

	/*===== 定数 =====*/

	const int PLAYER_TURN_WHITE_TIMER = 60;		// プレイヤーが白くなるまでのタイマー
	const int PLAYER_EXIT_TIMER = 30;			// 完全に白くなったプレイヤーが消えるまでのタイマー
	const int PARTICLE_STOP_TIMER = 10;			// パーティクルが止まるまでのタイマー
	const int BLACK_OUT_DELAY_TIMER = 30;		// 暗転するまでの遅延のタイマー
	const int BLACK_OUT_TIMER = 60;				// 暗転時間
	const int SLOW_TIMER = PLAYER_TURN_WHITE_TIMER + PLAYER_EXIT_TIMER + PARTICLE_STOP_TIMER;	// 周りが完全に止まるまでのタイマー


public:

	/*===== メンバ関数 =====*/

	// コンストラクタ
	PlayerDeadEffect();

	// 初期化処理
	void Init();

	// 有効化処理
	void Activate(Vec2<float>& playerPos);

	// 更新処理
	void Update();

	// 描画処理
	void Draw();

};