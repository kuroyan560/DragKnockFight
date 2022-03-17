#pragma once
#include "Vec.h"
#include "Player.h"
#include <vector>

using namespace std;

// ドッスンクラス識別ID
enum DossunID {

	DOSSUN_OFF_POWER,	// 電源オフのドッスン
	DOSSUN_LOW_POWER,	// 低出力のドッスン
	DOSSUN_HIGH_POWER,	// 高出力のドッスン

};

// ドッスンクラス
class DossunBlock {

public:

	/*===== メンバ変数 =====*/

	Vec2<float> pos;			// 座標
	Vec2<float> size;			// 大きさ
	Vec2<float> moveDir;		// 移動方向
	float speed;		// 移動速度
	int isMoveTimer;	// 移動するまでのタイマー(プレイヤーがこのブロックに触れてから指定フレーム後に動き出すようにするために必要。)
	int changeDirTimer;	// 壁と当たったときに方向転換するタイマー
	bool isHitPlayer;	// プレイヤーと当たっているかどうか。
	DossunID id;		// どのドッスンかのID


public:

	/*===== 定数 =====*/

	const int IS_MOVE_TIMER = 60;			// プレイヤーがブロックに触れてから動き出すまでのタイマー
	const int CHANGE_DIR_TIMER = 180;		// 壁にあたったら方向転換するタイマー
	const float LOW_POWER_ADD_SPEED = 0.5f;	// 低出力ドッスンの移動速度の加算量
	const float LOW_POWER_MAX_SPEED = 5.0f;	// 低出力ドッスンの移動速度の最大量


public:

	/*===== メンバ関数 =====*/

	// コンストラクタ
	DossunBlock();

	// 初期化処理
	void Init();

	// 生成処理
	void Generate(Vec2<float> generatePos, Vec2<float> endPos, const Vec2<float>& size, const DossunID& dossunID);

	// 更新処理
	void Update();

	// 描画処理
	void Draw();

	// 当たり判定
	void CheckHit(Player& player, const vector<vector<int>>& mapData);

};