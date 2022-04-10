#include "Vec.h"
#include "Singleton.h"

// スタンした際の演出クラス
class StunEffect : public Singleton<StunEffect> {

private:

	/*===== メンバ変数 =====*/

	Vec2<float>* focusPos;	// フォーカスする対象の座標
	int focusTime;			// フォーカスする時間

public:

	bool isActive;			// この演出が有効化されているかのフラグ


private:

	/*===== 定数 =====*/

	const int FOCUS_TIME = 120;
	const float FOCUS_ZOOM = 1.7f;


public:

	/*===== メンバ関数 =====*/

	// コンストラクタ
	StunEffect();

	// 初期化処理
	void Init();

	// 生成処理
	void Activate(Vec2<float>& focusPos);

	// 更新処理
	void Update();

};