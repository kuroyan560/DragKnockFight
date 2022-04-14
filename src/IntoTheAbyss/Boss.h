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

// プレイヤーと引っ張り合うボスクラス
class Boss {

	void Crash(const Vec2<float>& MyVec);

public:

	/*===== メンバ変数 =====*/

	Vec2<float> pos;			// 座標
	Vec2<float> prevPos;		// 前フレームの座標
	Vec2<float> scale;			// 大きさ
	Vec2<float> vel;			// そのフレームの移動量の総量
	Vec2<float> moveVel;		// 移動量
	Vec2<float> swingInertiaVec;// 振り回しの慣性の移動方向
	float swingInertia;			// 振り回しの慣性
	int stuckWindowTimer;		// ウィンドウに挟まったタイマー
	int afterSwingDelay;		// 振り回しのあとにボスを少し動けない状態にするためのタイマー
	INTERSECTED_LINE prevIntersectedLine;

	//画像
	static const enum DIR { FRONT, BACK, DIR_NUM };
	int graphHandle[DIR_NUM];

	//クラッシュ演出補助
	StagingInterFace stagingDevice;


	//ボスのパターン制御-----------------------
	enum E_BossPattern
	{
		BOSS_PATTERN_NONE = -1,
		BOSS_PATTERN_NORMALMOVE,
		BOSS_PATTERN_ATTACK,
		BOSS_PATTERN_SWING,
		BOSS_PATTERN_MAX
	};
	E_BossPattern bossPatternNow, oldBossPattern;
	BossPatternData patternData;
	std::array<std::unique_ptr<IBossPattern>, BOSS_PATTERN_MAX>bossPattern;
	int patternTimer;
	//ボスのパターン制御-----------------------

	bool allowToMoveFlag;
	bool initPaticleFlag;
	int moveTimer;
	Vec2<float>initScale;
public:

	/*===== 定数 =====*/

	const Vec2<float> SCALE = { 80.0f,80.0f };
	const float OFFSET_VEL = 7.0f;
	const float OFFSET_INERTIA = 30.0f;		// 振り回しの慣性
	const int AFTER_SWING_DELAY = 15;
	const int STRUCK_WINDOW_TIMER = 120.0f;

	const float SWING_DISTANCE_DEADLINE = 200.0f;	//プレイヤーを振り回す距離のデットライン

	std::array<Bullet, 200> bullts;

	//陣地との判定
	Square areaHitBox;
	std::unique_ptr<SphereCollision> bulletHitBox;

public:
	bool readyToStartRoundEffectFlag;
	/*===== メンバ関数 =====*/

	// コンストラクタ
	Boss();

	// 初期化処理
	void Init(const Vec2<float>& generatePos);

	// 更新処理
	void Update();

	// 描画処理
	void Draw();

	// 当たり判定
	void CheckHit(const vector<vector<int>> &mapData, bool &isHitMapChip, const Vec2<float> &playerPos, const Vec2<float> &lineCenterPos);


	bool AllowToMove()
	{
		return allowToMoveFlag;
	};

	void AiPattern();
};