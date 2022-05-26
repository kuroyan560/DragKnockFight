#pragma once
#include "Vec.h"
#include "Singleton.h"
#include <array>

class RoundFinishParticle {

public:

	/*===== メンバ変数 =====*/

	Vec2<float> pos;
	Vec2<float> dir;
	float changeLength;
	int useGraphHanlde;
	bool isActive;
	bool isReturn;

	const float MIN_MOVE_LENGTH = 150.0f;
	const float MAX_MOVE_LENGTH = 200.0f;


public:

	/*===== メンバ関数 =====*/

	// コンストラクタ
	RoundFinishParticle();

	// 初期化処理
	void Init();

	// 生成処理
	void Generate(const Vec2<float>& CharaPos, const int& UseGraphHandle);

	// 更新処理
	void Update(const Vec2<float>& LeftCharaPos);

	// 描画処理
	void Draw();

};

class RoundFinishParticleMgr : public Singleton<RoundFinishParticleMgr> {

public:

	/*===== メンバ変数 =====*/

	static const int MAX_PARTICLE = 70;
	std::array<RoundFinishParticle, MAX_PARTICLE> particles;

	std::array<int, 3> particleGraph;

	int easingTimer;
	const int EASING_TIMER = 90;


public:

	/*===== メンバ関数 ======*/

	// 初期化処理
	void Init();

	// 生成処理
	void Generate(const Vec2<float>& CharaPos);

	// 更新処理
	void Update(const Vec2<float>& LeftCharaPos);

	// 描画処理
	void Draw();

	// 戻る。
	void SetReturn();


};