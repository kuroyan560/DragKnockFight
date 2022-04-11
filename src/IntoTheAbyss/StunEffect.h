#include "Vec.h"
#include "Singleton.h"

// スタンした際の演出クラス
class StunEffect : public Singleton<StunEffect> {

private:

	/*===== メンバ変数 =====*/

	Vec2<float>* focusPos;		// フォーカスする対象の座標
	int focusTime;				// フォーカスする時間

	// 以下BREAK画像用のメンバ変数
	Vec2<float> breakUIPos;		// [BREAK]画像を描画する座標
	Vec2<float> iconPos;		// アイコンの座標
	Vec2<float> breakUIPosDef;	// [BREAK]画像を描画する座標のデフォルトの値
	float breakUIRadian;		// [BREAK]画像のラジアン
	float breakUIExtRate;		// [BREAK]画像の拡大率
	int breakUITimer;			// [BREAK]画像の各処理に使用するタイマー
	int enemyBreakHandle;		// 敵の[BREAK]の画像ハンドル
	int playerBreakHandle;		// プレイヤーの[BRWAK]の画像ハンドル
	int useHandle;				// 実際に描画する画像ハンドル
	bool isBoss;

	enum BREAK_UI_STATUS {
		STATUS_GENERATE,		// 生成して中央に向かうシーン
		STATUS_STOP,			// 中央について止まっているシーン
		STATUS_ICON,			// 中心からアイコンに向かっていくシーン
	};
	BREAK_UI_STATUS status;


public:

	bool isActive;				// この演出が有効化されているかのフラグ


private:

	/*===== 定数 =====*/

	const int BREAK_UI_GENERATE_TIMER = 50;
	const int BREAK_UI_STOP_TIMER = 60;
	const int BREAK_UI_ICON_TIMER = 30;
	const int FOCUS_TIME = 120;
	const float FOCUS_ZOOM = 1.7f;
	const float DEF_RADIAN = 3600.0f;
	const float EXT_RATE = 2.0f;				// 画像の大きさ
	const float ADD_EXT_RATE = 0.5f;			// アイコンに向かう際にちょっとだけ拡大する量


public:

	/*===== メンバ関数 =====*/

	// コンストラクタ
	StunEffect();

	// 初期化処理
	void Init();

	// 生成処理
	void Activate(Vec2<float>& focusPos, const Vec2<float>& stunPos, const Vec2<float>& iconPos, const bool& isBoss);

	// 更新処理
	void Update();

	// 描画処理
	void Draw();

};