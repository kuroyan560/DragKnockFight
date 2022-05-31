#pragma once
#include "Vec.h"
#include "CharacterManager.h"
#include "Camera.h"
#include "Singleton.h"
#include <array>

#include "RoundFinishParticle.h"
#include"EavaluationDataMgr.h"

class RoundFinishEffect : public Singleton<RoundFinishEffect> {

public:

	/*===== メンバ変数 =====*/

	enum class EFFECT_STATUS {

		NUM1_ZOOMIN,			// ①二人の真ん中にズームインする。
		NUM2_ENEMY_SHAKE,		// ②敵をシェイクさせる。
		NUM3_ENEMY_EXP,			// ③敵を爆発させる。
		NUM4_RETURN_DEFPOS,		// ⑤各キャラを規定の座標に戻す。
		NUM5_RETURN_PLAYER_DEFPOS,

	};

	std::array<int, 3> perfectGraph;
	int goodGraph;
	int greatGraph;
	int excellentGraph;

	int useGraph;

	int perfectAnimTimer;
	int perfectAnimIndex;
	float perfectMoveAmount;
	const int PERFECT_ANIM_TIMER = 10;

	EFFECT_STATUS status;		// ステータス

	int timer;					// 各タイマー
	bool isEnd;					// 演出が終わったかのフラグ
	bool finishLap;
	bool changeMap;
	Vec2<float> shakeAmount;	// シェイクさせる処理全般に使用するやつ

	bool isPerfect;				// すべてのブロックを壊したかのフラグ
	Vec2<float> perfectPos;		// [Perfect]の座標
	Vec2<float> perfectExp;		// [Perfect]の拡縮

	float cameraZoom;

	// 各ステータスの時間
	const int NUM1_ZOOMIN_TIMER = 60;
	const int NUM2_ENEMY_SHAKE_TIMER = 60;
	const int NUM3_ENEMY_EXP_TIMER = 160;
	const int NUM4_RETURN_DEFPOS_TIMER = 90;
	const int NUM5_RETURN_PLAYER_DEF_POS = 120;

	
	Sound soundType;
	std::array<int, 4>evaluationSoundHandle;

	bool addScoreFlag;
public:

	// コンストラクタ
	RoundFinishEffect();

	// 初期化処理
	void Init();

	// 開始処理
	void Start(const bool& IsPerfect, const float& Rate, const float& CameraZoom);

	// 更新処理
	void Update(const Vec2<float>& LineCenterPos);

	// 描画処理
	void Draw();

};