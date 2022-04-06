#include "Vec.h"

// プレイヤーと引っ張り合うボスクラス
class Boss {

public:

	/*===== メンバ変数 =====*/

	Vec2<float> pos;
	Vec2<float> scale;
	Vec2<float> forwardVec;
	float vel;


public:

	/*===== 定数 =====*/

	const Vec2<float> SCALE = { 50.0f,50.0f };
	const float OFFSET_VEL = 5.0f;


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

};