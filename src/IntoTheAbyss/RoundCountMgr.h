#pragma once
#include "Vec.h"
#include "Singleton.h"
#include <array>

// ラウンドUI単体
class RoundCount {

public:

	/*===== メンバ変数 =====*/

	Vec2<float> pos;		// 座標
	Vec2<float> exp;
	int alpha;
	float angle;
	bool isActive;			// 描画するか
	bool isFill;			// 埋まっているか(内側も描画するか)

	bool appearFlag;
	Vec2<float>initPos, vel;
	float t;
public:

	/*===== メンバ関数 =====*/

	// コンストラクタ
	RoundCount();

	// 初期化処理
	void Init();

	// 生成処理
	void Generate(const Vec2<float>& Pos);

	// 更新処理
	void Update();

	// 描画処理
	void Draw(const int& FrameGraph, const int& InnerGraph);

	// 埋める。
	inline void SetFill() { isFill = true; }

	void Appear();
};

// ラウンドUI管理クラス
class RoundCountMgr : public Singleton<RoundCountMgr> {

public:

	/*===== メンバ変数 =====*/

	std::array<RoundCount, 3> counter;

	int nowRound;		// ラウンド数 ここで使用するラウンド数は1~の値 配列の要素数として使用する際には-1する必要あり。 この値はラウンドが終わった後に加算される。
	int maxRound;

	int frameGraph;
	int innerGraph;
	int roundGraph;

	Vec2<float>startPos;
public:

	/*===== メンバ関数 =====*/

	// コンストラクタ
	RoundCountMgr();

	// 初期化処理
	void Init(int MaxRound);

	// 更新処理
	void Update();

	// 描画処理
	void Draw();

	// ラウンドをインクリメント
	void RoundIncrement();

	void Appear();

};