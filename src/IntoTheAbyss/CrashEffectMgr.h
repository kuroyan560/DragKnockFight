#include "Vec.h"
#include "Singleton.h"
#include <array>
#include"Color.h"

// クラッシュ時の演出クラス
class ClassEffect {

private:

	/*===== メンバ変数 =====*/

	Vec2<float> pos;	// 座標
	Vec2<float> size;	// 大きさ
	int timer;			// 各段階の変化、イージングに使用するタイマー
	bool isExpY;		// Y軸拡張フラグ このクラスの処理は 「Y軸拡張段階」 「X軸拡張段階」 の2つに分かれている。
	bool isActive;		// 有効化されているかフラグ
	Color col;

	const int EXP_Y_TIMER = 10;	// Y軸拡張段階のタイマー
	const int EXP_X_TIMER = 20;	// X軸拡張段階のタイマー

	const float EXP_Y_AMOUNT = 30.0f;	// Y軸拡張段階でのY軸の拡張量
	const float EXP_X_AMOUNT = 1280.0f * 4;	// X軸拡張段階でのX軸の拡張量


public:

	// コンストラクタ
	ClassEffect() :pos({}), size({}), isExpY(false), isActive(false) {};

	// 初期化処理
	void Init();

	// 生成処理
	void Generate(const Vec2<float>& Pos,const Color& Col);

	// 更新処理
	void Update();

	// 描画処理
	void Draw();

	inline const bool& GetIsActive() { return isActive; }

};

// クラッシュ演出の管理クラス
class CrashEffectMgr : public Singleton<CrashEffectMgr > {

private:

	/*===== メンバ変数 =====*/

	static const int CRASH_EFFECT_AMOUNT = 30;
	std::array<ClassEffect, CRASH_EFFECT_AMOUNT> crashEffects;


public:

	/*===== メンバ関数 =====*/

	// 初期化処理
	void Init();

	// 生成処理
	void Generate(const Vec2<float>& Pos, const Color& Col);

	// 更新処理
	void Update();

	// 描画処理
	void Draw();

};