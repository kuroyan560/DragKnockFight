#pragma once
#include "Vec.h"
#include "TimeStopTestBlock.h"
#include "Bubble.h"
#include <memory>
#include <vector>

using namespace std;

// プレイヤーの腕の前方宣言
class PlayerHand;

// プレイヤークラス
class Player {

public:

	/*-- メンバ変数 --*/

	Vec2<float> centerPos;					// プレイヤーの中心座標
	Vec2<float> prevFrameCenterPos;		// 前フレームのプレイヤーの中心座標
	Vec2<float> vel;						// 移動量
	Vec2<float> gimmickVel;			// ギミックから与えられる移動量(ドッスンブロックに張り付いた時等。)
	float gravity;					// 重力
	bool onGround;					// プレイヤーが接地しているかのフラグ
	bool firstShot;					// 最初の一発が撃たれたかどうか trueで撃たれた判定
	bool isWallRight;				// 右の壁にくっついているか
	bool isWallLeft;				// 左の壁にくっついているか
	bool inBubble;					// シャボン玉に入っているかどうか
	int rapidFireTimerLeft;			// 連射タイマー左手
	int rapidFireTimerRight;		// 連射タイマー右手
	int gravityInvalidTimer;		// 重力無効化タイマー
	int handReturnTimer;			// 入力が終わってから腕がデフォルトの位置に戻るまでのタイマー
	int asSoonAsInputTimer;			// 移動入力が行われてから数フレーム間有効化する処理を作るためにタイマー 主にシャボン玉

	// 壁ズリフラグ
	bool isSlippingWall[4];			// 壁ズリパーティクルを出すフラグ

	// プレイヤーの腕
	unique_ptr<PlayerHand> lHand;	// 左手
	unique_ptr<PlayerHand> rHand;	// 右手
	bool isPrevFrameShotBeacon;

	int playerGraph;

public:

	/*-- 定数 --*/

	const float ADD_GRAVITY = 0.5f;				// プレイヤーにかける重力
	const float MAX_GRAVITY = 15.0f;			// プレイヤーにかける重力の最大量
	const float RECOIL_AMOUNT = 5.0f;			// 弾を撃った際の反動
	const float FIRST_RECOIL_AMOUNT = 20.0;		// 弾を撃った際の反動
	const float MAX_RECOIL_AMOUNT = 30.0f;		// 弾を撃った際の反動の最大値
	const float EXT_RATE = 0.6f;	//Player's expand rate used in Draw().
	const Vec2<float> PLAYER_SIZE = { (56 * EXT_RATE) / 2.0f,(144 * EXT_RATE) / 2.0f };			// プレイヤーのサイズ
	static Vec2<float>GetGeneratePos();
	const int RAPID_FIRE_TIMER = 21;			// 連射タイマー
	const int GRAVITY_INVALID_TIMER = 20;		// 重力無効化タイマー

	// コヨーテ的なやつのためのパラメーター
	const float STOP_DEADLINE_Y = 5.0f;		// Y軸の移動量がコレ以下だったら移動量を0にする。
	const float STOP_DEADLINE_X = 3.0f;		// X軸の移動量がコレ以下だったら移動量を0にする。

	// 手が初期位置に戻るまでのタイマー
	const int DEF_HAND_RETURN_TIMER = 180;

	// 左手の初期位置
	const float DEF_LEFT_HAND_ANGLE = 2.35619f;
	// 右手の初期位置
	const float DEF_RIGHT_HAND_ANGLE = 0.785398f;

	// 入力されてから数フレームを取得するためのタイマーのでフォルチ値
	const int AS_SOON_AS_INPUT_TIMER = 2;

	// プレイヤーの方向
	enum PLAYER_DIR {

		PLAYER_LEFT,
		PLAYER_RIGHT,
		PLAYER_TOP,
		PLAYER_BOTTOM,

	};


public:

	/*-- メンバ関数 --*/

	// コンストラクタ
	Player();
	~Player();

	// 初期化処理
	void Init(const Vec2<float>& INIT_POS);

	// 更新処理
	void Update(const vector<vector<int>> mapData);

	// 描画処理
	void Draw();

	// マップチップとの当たり判定
	void CheckHit(const vector<vector<int>> mapData, vector<Bubble>& bubble, TimeStopTestBlock& testBlock);


	// 方向ごとのマップチップとの当たり判定関数
	void HitMapChipTop();
	void HitMapChipLeft();
	void HitMapChipRight();
	void HitMapChipBottom();

	//プレイヤーの入力を禁止する
	void StopDoorLeftRight();
	void StopDoorUpDown();


private:

	/*-- クラス内で使用する関数 --*/

	// 入力処理
	void Input(const vector<vector<int>> mapData);

	// 移動処理
	void Move();

	// 重力に関する更新処理
	void UpdateGravity();

	// 壁との押し戻しに関する更新処理
	void PushBackWall();

	bool stopInputFlag;//入力を禁止するフラグ
	bool stopMoveFlag;
	bool drawCursorFlag;

	bool doorMoveLeftRightFlag;
	bool doorMoveUpDownFlag;
};