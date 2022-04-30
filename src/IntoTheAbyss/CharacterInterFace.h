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
#include"RunOutOfStaminaEffect.h"
#include"SwingLineSegmentMgr.h"
#include"TexHandleMgr.h"
#include"CharacterInfo.h"

class CharacterInterFace
{

private:
	void SwingUpdate();
	void Crash(const Vec2<float>& MyVec);
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

	// 優勢ゲージがデフォルトに戻るまでのタイマー
	int gaugeReturnTimer;		// クラッシュなどした際に数値が代入され、0の時にデフォルトのゲージ量に戻る。
	const int GAUGE_RETURN_TIMER = 60;


protected:
	BulletMgrBase bulletMgr;
	bool nowSwing;
	bool isAdvancedEntrySwing;		// 振り回し先行入力
	int advancedEntrySwingTimer;	// 振り回しの先行入力を受け付けてから、無効化されるまでのタイマー
	const int ADVANCED_ENTRY_SWING_TIMER = 30;
	Vec2<float> nowSwingVec;		// 現在の角度
	Vec2<float> swingTargetVec;		// 目標地点
	float addSwingAngle;			// 振り回しで回転させる量 だんだん増える。
	bool isSwingClockWise;			// この振り回しが時計回りかどうか true...時計回り...右回転  false...反時計回り...左回転
	float addSwingRate;				// 振り回し速度に加算する量 近い時に回転が遅い問題を解決する。
	const float ADD_SWING_ANGLE = 0.008f;
	const float MAX_SWING_ANGLE = 0.13f;

	//振り回し可視化用線分クラス
	SwingLineSegmentMgr CWSwingSegmentMgr;	// 時計回り
	SwingLineSegmentMgr CCWSwingSegmentMgr;	// 反時計回り
	bool isInputSwingRB;					// RBでスイングしたかどうか falseだったらLB
	int rbHandle;
	int lbHandle;
	int lineHandle;
	int arrowHandle;
	int reticleHandle;


protected:
	static const enum HIT_DIR { LEFT, RIGHT, TOP, BOTTOM, HIT_DIR_NUM };
	std::array<int, HIT_DIR_NUM>mapChipHit;
	bool stackMapChip;

	//試合開始時に呼び出される
	CharacterInterFace(const Vec2<float>& HonraiSize) : size(HonraiSize)
	{
		areaHitBox.center = &pos;
		areaHitBox.size = size;
		bulletHitSphere.center = &pos;
		bulletHitSphere.radius = size.x;
		outOfStaminaEffect.Init();
		rbHandle = TexHandleMgr::LoadGraph("resource/ChainCombat/UI/button_RB.png");
		lbHandle = TexHandleMgr::LoadGraph("resource/ChainCombat/UI/button_LB.png");
		lineHandle = TexHandleMgr::LoadGraph("resource/ChainCombat/UI/swing_line.png");
		arrowHandle = TexHandleMgr::LoadGraph("resource/ChainCombat/UI/swing_arrow.png");
	}

	std::weak_ptr<CharacterInterFace>partner;
	StagingInterFace stagingDevice;
	Vec2<float>size;	//サイズ

	//[キャラごとに違う関数]
	virtual void OnInit() = 0;
	virtual void OnUpdate(const std::vector<std::vector<int>>& MapData) = 0;
	virtual void OnUpdateNoRelatedSwing() = 0;	//スウィング中でも通る処理
	virtual void OnDraw() = 0;
	virtual void OnDrawUI() = 0;
	virtual void OnHitMapChip(const HIT_DIR& Dir) = 0;
	virtual void OnBreak() = 0;
	virtual void OnBreakFinish() = 0;
	virtual void OnSwinged() = 0;
	virtual void OnSwingedFinish() = 0;
	virtual void OnCrash() = 0;

	//[共通関数]
	//振り回し
	void SwingPartner(const Vec2<float>& SwingTargetVec, const bool& IsClockWise);
	//ゲッタ類
	const Vec2<float>& GetPartnerPos()
	{
		return partner.lock()->pos;
	}
	//左チームか右チームか
	const WHICH_TEAM& GetWhichTeam() { return team; }

	// 当たり判定情報保存。
	void SaveHitInfo(bool& isHitTop, bool& isHitBottom, bool& isHitLeft, bool& isHitRight, const INTERSECTED_LINE& intersectedLine);

public:
	//登場演出が完了したか
	virtual bool Appear() = 0;

public:
	static const int LINE_LENGTH = 150;
	static const int ADD_LINE_LENGTH_VEL = 100;	// 移動量に応じて伸びるaddLineLengthの最大量
	float MOVE_SPEED_PLAYER = 15.0f;			// 移動速度
	float addLineLength;	//紐
	Vec2<float> pos;			// 座標
	Vec2<float>vel;
	Vec2<float> prevPos;		// 前フレームの座標
	bool isHold;				// つかんでいるかフラグ
	bool isGripPowerEmpty;		// 握力タイマーを使い切ってから回復するまでを判断するためのフラグ
	float gripPowerTimer;			// 握力タイマー
	const int MAX_GRIP_POWER_TIMER = 40;
	RunOutOfStaminaEffect outOfStaminaEffect;

	void RegisterCharacterInfo(const std::shared_ptr<CharacterInterFace>Partner, const WHICH_TEAM& Team, const PLAYABLE_CHARACTER_NAME& Name)
	{
		partner = Partner;
		team = Team;
		characterName = Name;
		outOfStaminaEffect.Init();
	}
	void Init(const Vec2<float>& GeneratePos);	//ラウンド開始時に呼び出される
	void Update(const std::vector<std::vector<int>>& MapData, const Vec2<float>& LineCenterPos);
	void Draw();
	void DrawUI();
	//当たり判定
	void CheckHit(const std::vector<std::vector<int>>& MapData, const Vec2<float>& LineCenterPos);

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
	BulletMgrBase& GetBulletMgr() { return bulletMgr; }

	void SetCanMove(const bool& Flg) { canMove = Flg; }
	void SetHitCheck(const bool& Flg) { hitCheck = Flg; }

	void FinishSwing();

	const PLAYABLE_CHARACTER_NAME& GetCharacterName() { return characterName; }
	const Color& GetTeamColor()
	{
		static const Color TEAM_COLOR[TEAM_NUM] =
		{
			Color(47,255,139,255),
			Color(239,1,144,255)
		};
		return TEAM_COLOR[team];
	}

	//ノックアウトされた側
	virtual void OnKnockOut() = 0;
};

