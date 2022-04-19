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

static const enum PLAYABLE_CHARACTER_NAME { PLAYABLE_LUNA, PLAYABLE_LACY, PLAYABLE_BOSS_0, PLAYABLE_CHARACTER_NUM, PLAYER_CHARACTER_NUM = PLAYABLE_LACY + 1 };
static const enum WHICH_TEAM { LEFT_TEAM, RIGHT_TEAM, TEAM_NUM };

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

	const int AFTER_SWING_DELAY = 15;
	Vec2<float> swingInertiaVec;// 振り回しの慣性の移動方向
	float swingInertia;			// 振り回しの慣性
	int afterSwingDelay;		// 振り回しのあとにボスを少し動けない状態にするためのタイマー

	//演出などの動きの関係で動きを止める
	bool canMove;
	//演出などの関係で当たり判定をとらなくなる
	bool hitCheck;

	//左か右か
	WHICH_TEAM team;

	//スタン用タイマー
	int stanTimer;
	//ダメージ用タイマー
	int damageTimer;

protected:
	BulletMgrBase bulletMgr;
	bool nowSwing;
	Vec2<float> nowSwingVec;		// 現在の角度
	Vec2<float> swingTargetVec;		// 目標地点
	float addSwingAngle;			// 振り回しで回転させる量 だんだん増える。
	bool isSwingClockWise;			// この振り回しが時計回りかどうか true...時計回り...右回転  false...反時計回り...左回転
	const float ADD_SWING_ANGLE = 0.002f;
	const float MAX_SWING_ANGLE = 0.07f;


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

	//[共通関数]
	//振り回し
	void SwingPartner(const Vec2<float>& SwingTargetVec);
	//ゲッタ類
	const Vec2<float>& GetPartnerPos()
	{
		return partner.lock()->pos;
	}
	//振り回しによる慣性
	Vec2<float>GetSwingInertia() { return swingInertiaVec * swingInertia; }
	//振り回し直後の硬直中か
	bool GetSwingRigor() { return 0 < afterSwingDelay; }
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
	int gripPowerTimer;			// 握力タイマー
	const int MAX_GRIP_POWER_TIMER = 180;

	void RegisterCharacterInfo(const std::shared_ptr<CharacterInterFace>Partner, const WHICH_TEAM& Team)
	{
		partner = Partner;
		team = Team;
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

	inline void FinishSwing() { nowSwing = false; }
};

