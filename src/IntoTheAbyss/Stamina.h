#include "Vec.h"
#include "Color.h"
#include <memory>
#include <vector>
#include"RunOutOfStaminaEffect.h"

// スタミナクラス
class Stamina {

private:
	/*===== メンバ変数 =====*/

	float nowGauge;			// 現在のゲージ量 0 ~ 100 の値
	bool isActivate;		// ゲージ量が最大値に達していて、振り回しやダッシュが行える状態かのフラグ
	Vec2<float> expAmount;	// 回復した際にちょっと拡大する演出に使用する変数。
	Color innerColor;		// ゲージの内側の色
	Color outerColor;		// ゲージの外側の色


private:

	/*===== 定数 =====*/

	const float MAX_GAUGE = 100.0f;		// ゲージ量の最大値。
	const float EXP_AMOUNT = 3.0f;		// 拡大量。
	const float BIG_EXP_AMOUNT = 6.0f;	// ゲージが満タンになったときの拡大量。


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
	void SetExp(const bool& isBig = false);
	void SetColor(const Color& InnerColor, const Color& OuterColor);

};

// スタミナ管理クラス
class StaminaMgr {

public:
	static const int STAMINA_NUM = 5;
	static int STAMINA_HEAL_SE[STAMINA_NUM];

	/*===== メンバ変数 =====*/

	std::vector<Stamina> stamina;
	RunOutOfStaminaEffect outOfStaminaEffect;
	bool emptyTrigger;

public:

	/*===== メンバ関数 =====*/

	StaminaMgr();
	void Init();
	void Update(const bool& Heal, const Vec2<float>& CharacterPos);
	void Draw(const Vec2<float>& CharaPos);
	
	// 色を設定。
	void SetColor(const Color& InnerColor, const Color& OuterColor);

	// スタミナを消費
	void ConsumesStamina(const int& ConsumesStamina);

	// スタミナを加算
	void AddStamina(const int& AddStamina);

	// 指定したアクションができるか。
	bool CheckCanAction(const int& ConsumesStamina);

	// バー単位ではなくゲージ消費(パイロット引き離し用)、足りなかったらfalseを返す
	void ConsumesStaminaByGauge(const float& CounsumeStaminaGauge);

	// スタミナゲージのサイズを設定する。
	void Resize(const int& Size);

};