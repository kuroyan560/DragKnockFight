#pragma once
#include "Vec.h"
#include "GimmickLoader.h"
#include "SightCollisionStorage.h"
#include <vector>

using namespace std;


// ドッスンクラス
class DossunBlock {

public:

	/*===== メンバ変数 =====*/

	Vec2<float> pos;			// 座標
	Vec2<float> size;			// 大きさ
	Vec2<float> moveDir;		// 移動方向
	SightCollisionData sightData;// 昇順の判定に使用するデータ
	float speed;		// 移動速度
	int isMoveTimer;	// 移動するまでのタイマー(プレイヤーがこのブロックに触れてから指定フレーム後に動き出すようにするために必要。)
	int changeDirTimer;	// 壁と当たったときに方向転換するタイマー
	int noCheckHitTimer;// 方向転換してから一定時間判定を無効化するためのタイマー
	int alpha;			// アルファ値
	bool isHitPlayer;	// プレイヤーと当たっているかどうか。
	bool isMove;		// 動き出したかどうか
	bool isReturn;		// 初期位置に戻っているかどうか
	bool* isTimeStopPikeAlive;
	bool isFirstMove;	// isMoveがtrueになってから最初の1F目は移動させないために使用
	E_GIMMICK id;		// どのドッスンかのID


public:

	/*===== 定数 =====*/

	const int IS_MOVE_TIMER = 60;			// プレイヤーがブロックに触れてから動き出すまでのタイマー
	const int CHANGE_DIR_TIMER = 180;		// 壁にあたったら方向転換するタイマー
	const float LOW_POWER_ADD_SPEED = 0.5f;	// 低出力ドッスンの移動速度の加算量
	const float LOW_POWER_MAX_SPEED = 7.5f;	// 低出力ドッスンの移動速度の最大量


public:

	/*===== メンバ関数 =====*/

	// コンストラクタ
	DossunBlock();

	// 初期化処理
	void Init();

	// 生成処理
	void Generate(Vec2<float> generatePos, Vec2<float> endPos, const Vec2<float>& size, const E_GIMMICK& dossunID);

	// 更新処理
	void Update();

	// 描画処理
	void Draw();

	// 当たり判定
	void CheckHit(const vector<vector<int>>& mapData);

};