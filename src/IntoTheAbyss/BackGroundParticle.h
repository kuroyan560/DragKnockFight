#pragma once
#include "Vec.h"
#include <array>
#include "Singleton.h"

class BackGroundParticle {

public:

	/*===== メンバ変数 =====*/

	Vec2<float> pos;
	Vec2<float> exp;
	Vec2<float> targetExp;		// EXPを補間する際の目標
	Vec2<float> stageSize;
	float speed;
	static float lissajousTimer;// リサージュ曲線に使用するタイマー
	float angle;
	int timer;					// 何かと色々使用するタイマー
	int desTimer;				// 消滅して別の場所に行くためのタイマー
	int defDesTimer;			// 消滅して別の場所に行くためのタイマーのデフォルト値
	int starIndex;
	bool isActive;

	const int MAX_DES_TIMER = 180;
	const int MAX_SHOOTING_STAR_DES_TIMER = 600;
	const float MAX_SPEED = 40.0f;
	const float MIN_SPEED = 10.0f;

	enum class STATUS {

		STAY,
		SHOOTING_STAR,
		METEOR_SWARM,

	};
	STATUS status;


public:

	/*===== メンバ関数 =====*/

	// コンストラクタ
	BackGroundParticle();

	// 初期化処理
	void Init();

	// 生成処理
	void Generate(const Vec2<float>& Pos, const STATUS& Status, const Vec2<float>& StageSize);

	// 更新処理
	void Update();

	// 描画処理
	void Draw();

	// 当たり判定
	void CheckHit(const Vec2<float>& Pos, const float& Size);

};

class BackGroundParticleMgr : public Singleton<BackGroundParticleMgr> {

public:

	/*===== メンバ変数 =====*/

	static const int PARTICLR_COUNT = 400;
	std::array<BackGroundParticle, PARTICLR_COUNT> stayStar;

	static const int SHOOTING_STAR_COUNT = 400;
	std::array<BackGroundParticle, SHOOTING_STAR_COUNT> shootingStar;

	int shootingStarGenerateTimer;
	int defShootingStarGenerateTimer;

	Vec2<float> stageSize;

	bool meterSwarmMode;		// 流星群状態
	int meterSwarmTimer;		// 流星群状態のタイマー
	Vec2<float> meterSwarmPos;	// 流星群の生成場所。
	const int METER_SWARM_TIMER = 180;
	const int METER_SWARM_SPAN = 5;


public:

	/*===== メンバ関数 =====*/

	BackGroundParticleMgr();

	// 初期化処理
	void Init();

	// 生成処理
	void StageStartGenerate(const Vec2<float>& StageSize);

	// 更新処理
	void Update();

	// 描画処理
	void Draw();

	// キャラとの当たり判定
	void CheckHit(const Vec2<float>& Pos, const float& Size);

};