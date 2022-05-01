#include "Vec.h"

// スタミナクラス
class Stamina {

private:
	/*===== メンバ変数 =====*/

	float nowGauge;		// 現在のゲージ量 0 ~ 100 の値
	bool isActivate;	// ゲージ量が最大値に達していて、振り回しやダッシュが行える状態かのフラグ


private:

	/*===== 定数 =====*/

	const float MAX_GAUGE = 100.0f;		// ゲージ量の最大値。


public:

	/*===== メンバ変数 =====*/

	Stamina();
	void Init();
	void Empty();
	void Update();
	void Draw(const Vec2<float>& DrawPos, const float& Width, const float& Height);

	// 各種アクセッタ
	inline const bool& GetIsActivate() { return isActivate; }
	inline const float& GetNowGauge() { return nowGauge; }
	float AddNowGauge(const float& Add);

};