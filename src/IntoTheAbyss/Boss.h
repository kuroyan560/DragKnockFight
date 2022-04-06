#include "Vec.h"
#include <vector>
#include "Intersected.h"

using namespace std;

// プレイヤーと引っ張り合うボスクラス
class Boss {

public:

	/*===== メンバ変数 =====*/

	Vec2<float> pos;			// 座標
	Vec2<float> prevPos;		// 前フレームの座標
	Vec2<float> scale;			// 大きさ
	Vec2<float> vel;			// 移動量
	int stuckWindowTimer;		// ウィンドウに挟まったタイマー
	INTERSECTED_LINE prevIntersectedLine;


public:

	/*===== 定数 =====*/

	const Vec2<float> SCALE = { 50.0f,50.0f };
	const float OFFSET_VEL = 10.0f;
	const int STRUCK_WINDOW_TIMER = 120.0f;


public:

	/*===== メンバ関数 =====*/

	// コンストラクタ
	Boss();

	// 初期化処理
	void Init();

	// 生成処理
	void Generate(const Vec2<float>& generatePos);

	// 更新処理
	void Update();

	// 描画処理
	void Draw();

	// 当たり判定
	void CheckHit(const vector<vector<int>>& mapData, bool& isHitMapChip, const Vec2<float>& playerPos);

};