#pragma once
#include"Vec.h"
#include<memory>
#include<vector>
#include"StagingInterFace.h"
#include"AreaCollider.h"
#include"../IntoTheAbyss/BulletCollision.h"
#include<array>
#include "Intersected.h"
#include"BulletMgrBase.h"
#include"SwingLineSegmentMgr.h"
#include"TexHandleMgr.h"
#include"CharacterInfo.h"
#include"StageMgr.h"
#include"Barrages.h"
#include "SwingDestroyCounter.h"

class StaminaMgr;

class CharacterInterFace
{
private:
	void SwingUpdate();
	void Crash(const Vec2<float>& MyVec, const int& SmokeCol);
	void CrashUpdate();

	//画面挟みクラッシュ判定
	const int STACK_WINDOW_TIMER = 120.0f;// ウィンドウに挟まったタイマー
	int stackWindowTimer;		// ウィンドウに挟まったタイマー

	//陣地との判定
	Square areaHitBox;

	//弾との当たり判定
	SphereCollision bulletHitSphere;

	int swingTimer;				// 振り回しの経過時間

	//演出などの動きの関係で動きを止める
	bool canMove;
	//演出などの関係で当たり判定をとらなくなる
	bool hitCheck;

	//左か右か
	WHICH_TEAM team;

	PLAYABLE_CHARACTER_NAME characterName;

	//スタン用タイマー
	int stanTimer;
	//ダメージ用タイマー
	int damageTimer;
	// トゲブロックにあたったとき用タイマー これに値が入っているときは動けない。
	int elecTimer;

	//パイロット切り離し
	bool isPilotDetached = false;	// パイロット切り離し中かのフラグ
	Vec2<float>pilotReturnStartPos;	//パイロットがロボに戻る直前の座標
	int pilotReturnTimer;	//パイロットが戻る処理の時間計測タイマー
	int pilotReturnTotalTime;	//パイロットがロボに戻るまでの時間

	// 優勢ゲージがデフォルトに戻るまでのタイマー
	int gaugeReturnTimer;		// クラッシュなどした際に数値が代入され、0の時にデフォルトのゲージ量に戻る。
	const int GAUGE_RETURN_TIMER = 60;


	int moveTimer;

	// 弾クラス
	BulletMgrBase bulletMgr;				// 弾クラス
	std::unique_ptr<BarrageBase> barrage;	// 弾幕クラス
	int barrageDelayTimer;					// 弾幕間のタイマー
	const int BARRAGE_DELAY_TIMER = 120;	// 弾幕間の遅延タイマー

	//スタミナ回復時オーラ
	float healAuraEaseRate = 0.0f;


protected:
	//登場演出
	bool initPaticleFlag;

	bool nowSwing;
	bool isAdvancedEntrySwing;		// 振り回し先行入力
	int advancedEntrySwingTimer;	// 振り回しの先行入力を受け付けてから、無効化されるまでのタイマー
	const int ADVANCED_ENTRY_SWING_TIMER = 30;
	Vec2<float> nowSwingVec;		// 現在の角度
	Vec2<float> swingTargetVec;		// 目標地点
	float addSwingAngle;			// 振り回しで回転させる量 だんだん増える。
	float allSwingAngle;			// どのくらい回転量を加算したか。
	bool isSwingClockWise;			// この振り回しが時計回りかどうか true...時計回り...右回転  false...反時計回り...左回転
	bool isDestroyMode;				// 壁を一気に破壊するモード
	float addSwingRate;				// 振り回し速度に加算する量 近い時に回転が遅い問題を解決する。
	float ADD_SWING_ANGLE = 0.02f;
	float MAX_SWING_ANGLE = 0.13f;

	//振り回し可視化用線分クラス
	SwingLineSegmentMgr CWSwingSegmentMgr;	// 時計回り
	SwingLineSegmentMgr CCWSwingSegmentMgr;	// 反時計回り
	bool isInputSwingRB;					// RBでスイングしたかどうか falseだったらLB
	int rbHandle;
	int lbHandle;
	int reticleHandle;

	//パイロットがロボから離れられる最大距離
	const float PILOT_RANGE = 300.0f;
	Vec2<float>pilotPos;	// パイロットの座標
	Vec2<float>pilotSize = { 64.0f,64.0f };	//パイロットのサイズ
	int pilotGraph = -1;
	bool pilotDrawMiror = false;

	// 照準の画像
	int stopReticleHandle;
	Vec2<float> reticleExp;
	float reticleRad;
	int reticleAlpha;

public:

	static bool isDebugModeStrongSwing;		// デバッグ用で壁に当たるまでスイング
	Vec2<float>size;	//サイズ
	Vec2<float> bounceVel;	// バウンドで紐が伸びるやつの移動量 velをそのまま使うと色々バグりそうなので新設しました。
	int CONSECUTIVE_SWING_TIMER;

	int nowStrongSwingCount;		// 今貫通振り回しを何回しているか。
	int maxStrongSwingCount;		// 何回貫通振り回しができるか。

protected:
	static const enum HIT_DIR { LEFT, RIGHT, TOP, BOTTOM, HIT_DIR_NUM };
	std::array<int, HIT_DIR_NUM>mapChipHit;
	bool stackMapChip;

	//試合開始時に呼び出される
	CharacterInterFace(const Vec2<float>& HonraiSize);


	std::weak_ptr<CharacterInterFace>partner;
	StagingInterFace stagingDevice;
	static const int INIT_SIZE = 5;
	Vec2<float>appearExtRate;	//登場演出

	//[キャラごとに違う関数]
	virtual void OnInit() = 0;
	virtual void OnUpdate(const std::vector<std::vector<int>>& MapData) = 0;
	virtual void OnUpdateNoRelatedSwing() = 0;	//スウィング中でも通る処理
	virtual void OnDraw(const bool& isRoundStartEffect) = 0;
	virtual void OnDrawUI() = 0;
	virtual void OnHitMapChip(const HIT_DIR& Dir) = 0;
	virtual void OnBreak() = 0;
	virtual void OnBreakFinish() = 0;
	virtual void OnSwinged() = 0;
	virtual void OnSwingedFinish() = 0;
	virtual void OnCrash() = 0;	//”自分”がクラッシュした瞬間
	virtual void OnPartnerCrash() = 0;	//”相手”がクラッシュした瞬間
	virtual void OnPilotLeave() = 0;	//パイロットがロボから離れた瞬間
	virtual void OnPilotControl() = 0;		//パイロットを動かす処理
	virtual void OnPilotReturn() = 0;	//パイロットがロボに戻った瞬間
	virtual void OnStaminaHeal(const int& HealAmount) = 0;

	//[共通関数]
	//振り回し
	void SwingPartner(const Vec2<float>& SwingTargetVec, const bool& IsClockWise);
	//パイロット切り離し
	void SetPilotDetachedFlg(const bool& Flg);

	//ゲッタ類
	const Vec2<float>& GetPartnerPos()
	{
		return partner.lock()->pos;
	}
	//左チームか右チームか
	const WHICH_TEAM& GetWhichTeam() { return team; }
	//パイロットがロボの外にいるか
	bool IsPilotOutSide() { return isPilotDetached || pilotReturnTimer < pilotReturnTotalTime; }

	// 当たり判定情報保存。
	void SaveHitInfo(bool& isHitTop, bool& isHitBottom, bool& isHitLeft, bool& isHitRight, const INTERSECTED_LINE& intersectedLine, vector<Vec2<int>>& hitChipIndex, const Vec2<int>& hitChipIndexBuff);

public:
	//登場演出が完了したか
	void Appear();
	bool CompleteAppear() { return 20 <= moveTimer; }

	// 前フレームの座標を保存。
	inline void SavePrevFramePos() {
		prevPrevPos = prevPos;
		prevPos = pos;
	}

	// 振り回しの予測線を消す。
	void InitSwingLineSegmetn();

	// 移動量を初期化。
	inline void InitVel() { vel = {}; }

public:
	int destroyTimer;				// 破壊してからすぐに振り回しを終わらせないためのタイマー
	const int DESTROY_TIMER = 5;
	bool canSwingEnd;				// 振り回しを終えることができるかのフラグ DestroyModeのときは振り回しを終わらないようにする。
	static const int LINE_LENGTH = 150;
	static int ADD_LINE_LENGTH_VEL;	// 移動量に応じて伸びるaddLineLengthの最大量
	float MOVE_SPEED_PLAYER = 15.0f;			// 移動速度
	float addLineLength;	//紐
	Vec2<float> pos;			// 座標
	Vec2<float>vel;
	Vec2<float> prevPos;		// 前フレームの座標
	Vec2<float> prevPrevPos;		// 前のフレームの前フレームの座標 名前安直すぎて申し訳ない…
	bool isHold;				// つかんでいるかフラグ
	std::shared_ptr<StaminaMgr> staminaGauge;	// スタミナゲージクラス
	const int SWING_STAMINA = 2;	// 振り回し時の消費スタミナ
	const int DASH_STAMINA = 1;		// ダッシュ時の消費スタミナ
	bool goAreaFlag;

	SwingDestroyCounter swingDestroyCounter;	// 一回の振り回しで何回壊したかのクラス

	float staminaAutoHealAmount;

	bool isStopPartner;		// 相方を止めておくためのフラグ
	bool isPrevStopPartner;

	Vec2<float>shakeValue;
	bool prevSwingFlag;

	// ラインド終了時に初期化したい変数を初期化する関数
	void InitRoundFinish();

	void RegisterCharacterInfo(const std::shared_ptr<CharacterInterFace>Partner, const WHICH_TEAM& Team, const PLAYABLE_CHARACTER_NAME& Name)
	{
		partner = Partner;
		team = Team;
		characterName = Name;
	}
	void Init(const Vec2<float>& GeneratePos, const bool& Appear);	//ラウンド開始時に呼び出される
	void Update(const std::vector<std::vector<int>>& MapData, const Vec2<float>& LineCenterPos, const bool& isRoundStartEffect, const bool& isRoundFinishEffect, const int& NowStageNum, const int& NowRoomNum);
	void Draw(const bool& isRoundStartEffect);
	void DrawUI();
	//当たり判定
	void CheckHit(const std::vector<std::vector<int>>& MapData, const Vec2<float>& LineCenterPos, const bool& isRoundFinish);
	void CheckHitStuck(const std::vector<std::vector<int>>& MapData);

	//スタン
	void Break();
	//ダメージ
	void Damage();

	const Square& GetAreaHitBox() { return areaHitBox; }
	const SphereCollision& GetBulletHitSphere() { return bulletHitSphere; }

	const int& GetStackWinTimer() { return stackWindowTimer; }
	const bool& GetNowSwing() { return nowSwing; }
	const bool& GetNowStuckWin() { return 0 < stackWindowTimer; }
	const bool& GetCanMove() { return canMove && !stanTimer; }
	const bool& GetStackFlag() { return stackMapChip; }
	const bool& GetNowBreak() { return stanTimer; }
	const bool& GetSwingClockWise() { return isSwingClockWise; }	// true…時計回り false…反時計回り
	BulletMgrBase& GetBulletMgr() { return bulletMgr; }

	//パイロットの座標（切り離ししてないときはnullptrを返す）
	const Vec2<float>* GetPilotPosPtr()
	{
		if (!isPilotDetached)return nullptr;
		return &pilotPos;
	}

	void SetCanMove(const bool& Flg) { canMove = Flg; }
	void SetHitCheck(const bool& Flg) { hitCheck = Flg; }

	void FinishSwing();

	const PLAYABLE_CHARACTER_NAME& GetCharacterName() { return characterName; }
	static const Color TEAM_COLOR[TEAM_NUM];
	const Color& GetTeamColor()
	{
		return TEAM_COLOR[team];
	}

	//ノックアウトされた側
	virtual void OnKnockOut() = 0;

	/// <summary>
	/// 時計回りに壁に当たるかどうか
	/// </summary>
	/// <returns></returns>
	bool ClockwiseHitsTheWall()
	{
		return CWSwingSegmentMgr.IsHitWall();
	}
	/// <summary>
	/// 反時計回り回した際に当たるかどうか
	/// </summary>
	/// <returns></returns>
	bool CounterClockwiseHitsTheWall()
	{
		return CCWSwingSegmentMgr.IsHitWall();
	}

	// マップチップが指定のインデックスだったら指定の値に塗り替える。 上下左右も。
	void OverWriteMapChipValueAround(const Vec2<int>& MapChipIndex, const MapChipType& DstType, const MapChipData& SrcData);

	void HealStamina(const int& HealAmount);
};

