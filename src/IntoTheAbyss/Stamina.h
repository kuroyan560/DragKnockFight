#include "Vec.h"
#include <memory>
#include <vector>

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
	void Init();	// 初期化 スタミナゲージを埋める。
	void Empty();	// 初期化 スタミナゲージを空にする。
	void Update();
	void Draw(const Vec2<float>& DrawPos, const float& Width, const float& Height);

	// 各種アクセッタ
	inline const bool& GetIsActivate() { return isActivate; }
	inline const float& GetNowGauge() { return nowGauge; }
	float AddNowGauge(const float& Add);

};

// スタミナ管理クラス
class StaminaMgr{

public:

	/*===== メンバ変数 =====*/

	std::vector<Stamina> stamina;


public:

	/*===== メンバ関数 =====*/

	StaminaMgr();
	void Init();
	void Update(const bool& Heal);
	void Draw(const Vec2<float>& CharaPos);

	// スタミナを消費
	void ConsumesStamina(const int& ConsumesStamina);

	// 指定したアクションができるか。
	bool CheckCanAction(const int& ConsumesStamina);

	//バー単位ではなくゲージ消費(パイロット引き離し用)、足りなかったらfalseを返す
	bool ConsumesStaminaByGauge(const float& CounsumeStaminaGauge);
};