#include "Vec.h"
#include <vector>
#include "Intersected.h"
#include"AreaCollider.h"
#include"IBossPattern.h"
#include<array>
#include<memory>
#include"Bullet.h"
#include"BulletCollision.h"

#include"StagingInterFace.h"
using namespace std;

#include"CharacterInterFace.h"

// プレイヤーと引っ張り合うボスクラス
class Boss : public CharacterInterFace
{
public:
	/*===== メンバ変数 =====*/
	Vec2<float> moveVel;		// 移動量
	Vec2<float> swingInertiaVec;// 振り回しの慣性の移動方向
	float swingInertia;			// 振り回しの慣性
	int afterSwingDelay;		// 振り回しのあとにボスを少し動けない状態にするためのタイマー
	INTERSECTED_LINE prevIntersectedLine;

	//画像
	static const enum DIR { FRONT, BACK, DIR_NUM };
	int graphHandle[DIR_NUM];

	//ボスのパターン制御-----------------------
	enum E_BossPattern
	{
		BOSS_PATTERN_NONE = -1,
		BOSS_PATTERN_NORMALMOVE,
		BOSS_PATTERN_ATTACK
	};
	E_BossPattern bossPatternNow, oldBossPattern;
	BossPatternData patternData;
	std::array<std::unique_ptr<IBossPattern>, 2>bossPattern;
	int patternTimer;
	bool atackModeFlag;
	//ボスのパターン制御-----------------------

	bool initPaticleFlag;
	int moveTimer;
	Vec2<float>initScale;
public:
	/*===== 定数 =====*/

	//const Vec2<float> SCALE = { 80.0f,80.0f };
	const float OFFSET_VEL = 2.0f;
	const float OFFSET_INERTIA = 30.0f;		// 振り回しの慣性
	const int AFTER_SWING_DELAY = 15;
	const int STRUCK_WINDOW_TIMER = 120.0f;

	std::array<Bullet, 200> bullts;

public:
	/*===== メンバ関数 =====*/

	// コンストラクタ
	Boss();

private:
	// 初期化処理
	void OnInit()override;

	// 更新処理
	void OnUpdate(const std::vector<std::vector<int>>& MapData)override;

	// 描画処理
	void OnDraw()override;

	void OnCheckHit(const std::vector<std::vector<int>>& MapData, const Vec2<float>& LineCenterPos)override {};
	virtual void OnHitMapChip(const HIT_DIR& Dir)override {};

public:
	bool Appear()override;
};