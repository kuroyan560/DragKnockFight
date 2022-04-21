#pragma once
#include "Vec.h"
#include <memory>
#include <vector>
#include"AreaCollider.h"

using namespace std;

// プレイヤーの腕の前方宣言
class PlayerHand;

#include"PlayerAnimation.h"
#include"AfterImg.h"
class LightManager;
#include"StagingInterFace.h"

#include"CharacterInterFace.h"
class Tutorial;

#include"RunOutOfStaminaEffect.h"

// プレイヤークラス
class Player :public CharacterInterFace
{

public:
	/*-- メンバ変数 --*/
	int rapidFireTimerLeft;			// 連射タイマー左手
	int rapidFireTimerRight;		// 連射タイマー右手

	// プレイヤーの腕
	unique_ptr<PlayerHand> lHand;	// 左手
	unique_ptr<PlayerHand> rHand;	// 右手

	// このキャラの色
	Color charaColor;

	//int playerGraph;

	//勢いによるストレッチ
	//const Vec2<float> MAX_STRETCH = { 16.0f,37.0f };	//最大ストレッチ量
	const Vec2<float> MAX_STRETCH = { 10.0f,31.0f };	//最大ストレッチ量
	Vec2<float>stretch_LU = { 0,0 };	//左上
	Vec2<float>stretch_RB = { 0,0 };	//右下
	Vec2<float>fromStretch_LU = { 0,0 };	//イージング用スタート値
	Vec2<float>fromStretch_RB = { 0,0 };	//イージング用スタート値
	const int STRETCH_RETURN_TIME = 17;	//ストレッチが０になるまでのフレーム数
	int stretchTimer = STRETCH_RETURN_TIME;
	const float STRETCH_DIV_RATE = 2.0f;	//ストレッチを弱くするときの割合
	const int FIRST_SHOT_RECOIL_PARTICLE_TIMER = 10.0f;

	//プレイヤーの向き
	//enum DRAW_DIR { FRONT, BACK, DIR_NUM, DEFAULT = FRONT }playerDir = DEFAULT;
	//アニメーション統括
	PlayerAnimation anim;

	//残像
	AfterImg afImg;

	int shotSE;

	Vec2<float>scale;

	const int controllerIdx;
	int bulletGraph;

	int inputInvalidTimerByCrash;	// 当たり判定無効化タイマー
	const int INPUT_INVALID_TIMER = 30;

	Vec2<float> swingVec;	// 振り回しベクトル 移動方向の逆方向に出る。

public:

	/*-- 定数 --*/

	//const float ADD_GRAVITY = 0.5f;				// プレイヤーにかける重力
	float ADD_GRAVITY = 0.1f;				// プレイヤーにかける重力
	float MAX_GRAVITY = 15.0f;			// プレイヤーにかける重力の最大量
	float RECOIL_AMOUNT = 30.0f;			// 弾を撃った際の反動
	float FIRST_RECOIL_AMOUNT = 35.0;		// 弾を撃った際の反動
	//const float FIRST_RECOIL_AMOUNT = 15.0;		// 弾を撃った際の反動
	//const float RECOIL_AMOUNT = FIRST_RECOIL_AMOUNT;			// 弾を撃った際の反動
	float MAX_RECOIL_AMOUNT = 30.0f;		// 弾を撃った際の反動の最大値
	//const float EXT_RATE = 0.6f;	//Player's expand rate used in Draw().
	//const Vec2<float> PLAYER_HIT_SIZE = { (80 * EXT_RATE) / 2.0f,(80 * EXT_RATE) / 2.0f };			// プレイヤーのサイズ
	static Vec2<float>GetGeneratePos();
	int RAPID_FIRE_TIMER = 4;			// 連射タイマー
	const int GRAVITY_INVALID_TIMER = 20;		// 重力無効化タイマー

	// コヨーテ的なやつのためのパラメーター
	const float STOP_DEADLINE_Y = 5.0f;		// Y軸の移動量がコレ以下だったら移動量を0にする。
	const float STOP_DEADLINE_X = 3.0f;		// X軸の移動量がコレ以下だったら移動量を0にする。
	const float VEL_MUL_AMOUNT = 0.95f;	//摩擦係数

	// 左手の初期位置
	const float DEF_LEFT_HAND_ANGLE = 2.35619f;
	// 右手の初期位置
	const float DEF_RIGHT_HAND_ANGLE = 0.785398f;

	// 振り回しのクールタイム
	int swingCoolTime;
	const int SWING_COOLTIME = 60;

	// 弾に関する定数
	const int BULLET_SHOT_COUNT = 5;
	const float BULLET_SHOT_ANGLE = 0.1f;

	// プレイヤーの方向
	enum PLAYER_DIR_X {
		PLAYER_LEFT,
		PLAYER_RIGHT
	}playerDirX;
	enum PLAYER_DIR_Y {
		PLAYER_FRONT,
		PLAYER_BACK
	}playerDirY;

	float sizeVel;
	bool initPaticleFlag;
	int moveTimer;

	//チュートリアルアイコン
	std::weak_ptr<Tutorial>tutorial;

public:

	/*-- メンバ関数 --*/

	// コンストラクタ
	Player(const PLAYABLE_CHARACTER_NAME& CharacterName, const int& ControllerIdx,const std::shared_ptr<Tutorial>&Tutorial);
	~Player();

private:
	// 初期化処理
	void OnInit()override;

	// 更新処理
	void OnUpdate(const vector<vector<int>>& MapData)override;

	//スウィング中も呼び出される更新処理
	void OnUpdateNoRelatedSwing()override;

	// 描画処理
	void OnDraw()override;
	void OnDrawUI()override;

	//マップチップとヒットしたとき
	void OnHitMapChip(const HIT_DIR& Dir)override;

	void OnBreak()override { isHold = false; }
	void OnBreakFinish()override { anim.ChangeAnim(DEFAULT_FRONT); }
	void OnSwinged()override
	{
		anim.ChangeAnim(SWINGED);
	}
	void OnSwingedFinish()override
	{
		if (!GetNowBreak() && !inputInvalidTimerByCrash)
		{
			anim.ChangeAnim(DEFAULT_FRONT);
		}
	}
	void OnCrash()override
	{
		// 入力受付無効化タイマーをセッティングする。
		inputInvalidTimerByCrash = INPUT_INVALID_TIMER;
	}

	bool drawCursorFlag;

	Vec2<float>size;
	Vec2<float>initSize;
private:
	/*-- クラス内で使用する関数 --*/

	// 入力処理
	void Input(const vector<vector<int>>& MapData);

	// 移動処理
	void Move();

	//弾の発射
	void Shot(const Vec2<float>& GeneratePos, const float& ForwardAngle);

	// 壁との押し戻しに関する更新処理
	void PushBackWall();

	bool stopInputFlag;//入力を禁止するフラグ
	bool stopMoveFlag;

	//ストレッチの値を計算
	void CalculateStretch(const Vec2<float>& Move);
	//ストレッチ値更新
	void UpdateStretch();
	//画像サイズからプレイヤーサイズ取得
	Vec2<float> GetPlayerGraphSize();

	// 移動量での当たり判定
	void CheckHitMapChipVel(const Vec2<float>& checkPos, const vector<vector<int>>& mapData);

public:
	bool Appear()override;
	void OnKnockOut()override { anim.ChangeAnim(KNOCK_OUT); }
};