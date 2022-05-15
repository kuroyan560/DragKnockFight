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
#include"Tutorial.h"

#include"RunOutOfStaminaEffect.h"

// プレイヤークラス
class Player :public CharacterInterFace
{

public:
	/*-- メンバ変数 --*/
	int rapidFireTimerLeft;			// 連射タイマー左手
	int rapidFireTimerRight;		// 連射タイマー右手

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
	static const int TIRED_DRAW_TIME = 90;
	int tiredTimer = TIRED_DRAW_TIME;

	//残像
	AfterImg afImg;

	int shotSE;

	Vec2<float>scale;

	const int controllerIdx;
	int bulletGraph;

	int inputInvalidTimerByCrash;	// 当たり判定無効化タイマー
	const int INPUT_INVALID_TIMER = 30;

	Vec2<float> swingVec;	// 振り回しベクトル 移動方向の逆方向に出る。

	bool isPrevLeftBottom;
	bool isInputRightStick;
	bool isPrevInputRightStick;
	Vec2<float> prevInputRightStick;

	//ダッシュ時残像を出すための変数
	int dashAftImgTimer;

	//直前の通常移動量（オートパイロット用）
	Vec2<float>autoPilotMove;

public:

	/*-- 定数 --*/

	float RECOIL_AMOUNT = 15.0f;			// 弾を撃った際の反動
	float FIRST_RECOIL_AMOUNT = 15.0;		// 弾を撃った際の反動
	float MAX_RECOIL_AMOUNT = 15.0f;		// 弾を撃った際の反動の最大値
	static Vec2<float>GetGeneratePos();

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

	//チュートリアルアイコン
	Tutorial tutorial;

	//パイロットの移動速度
	Vec2<float>pilotVel;
	Vec2<float>pilotAccel;
	int playerPilotGraph[2];	//FRONT,BACK

	// ジャスト振り回しキャンセルダッシュ用の当たり判定線分。
	Vec2<float> justCancelDashStartPos;
	Vec2<float> justCancelDashEndPos;


public:

	/*-- メンバ関数 --*/

	// コンストラクタ
	Player(const PLAYABLE_CHARACTER_NAME& CharacterName, const WHICH_TEAM& Team);
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
		anim.ChangeAnim(SWINGED);
		UsersInput::Instance()->ShakeController(controllerIdx, 0.8f, 13);
	}
	void OnPartnerCrash()override
	{
		UsersInput::Instance()->ShakeController(controllerIdx, 0.8f, 13);
	}
	void OnPilotLeave()override	//パイロットがロボから離れた瞬間
	{
		pilotVel = { 0,0 };
		anim.ChangeAnim(NON_PILOT);
		tutorial.SetPilotLeave(true);
	}
	void OnPilotControl()override;		//パイロットを動かす処理
	void OnPilotReturn()override	//パイロットがロボに戻った瞬間
	{
		if (anim.GetNowAnim() != TIRED)anim.ChangeAnim(DEFAULT_FRONT);
		tutorial.SetPilotLeave(false);
	}

	bool drawCursorFlag;

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
	void OnKnockOut()override { anim.ChangeAnim(KNOCK_OUT); }
};