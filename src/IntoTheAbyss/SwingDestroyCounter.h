#include "Vec.h"
#include <array>

// SwingingDestroyCounterで使用する数字1つ
class DestroyCounter {

public:

	/*===== メンバ変数 =====*/

	enum class STATUS{
	
		ENTRY,		// 開始状態
		STAY,		// 待機状態
		EXIT,		// 退出状態
	
	};

	Vec2<float> pos;	// 描画座標
	int disit;			// 数字
	float easingTimer;	// 挙動にイージングをかけるためのタイマー
	bool isActive;		// 有効化されているかのフラグ
	STATUS status;		// 現在のステータス

	const float ADD_EASING_TIMER = 0.05f;
	const float EASING_CHANGE = 100.0f;


public:

	/*===== メンバ関数 =====*/

	// コンストラクタ
	DestroyCounter();

	// 初期化処理
	void Init();

	// 入場させる。
	void SetExtry(const int& Disit);

	// 退出させる。
	void SetExit();

	// 更新処理
	void Update(const Vec2<float>& CharaPos);

	// 描画処理
	void Draw();

};

// 1回のフリ回しで何ブロック壊したかを表示する。
class SwingDestroyCounter {

private:

	/*===== メンバ変数 =====*/

	static const int DESTROY_COUNTER = 20;
	std::array<DestroyCounter, DESTROY_COUNTER> destroyCounter;		// UIに使用する数字1つ。

	int nowCounter;		// 現在の破壊数。これの値を元にUIのクラスの数字を設定する。


public:

	/*===== メンバ変数 =====*/

	// コンストラクタ
	SwingDestroyCounter();

	// 初期化
	void Init();

	// 更新処理
	void Update();

	// 描画処理
	void Draw();

	// 値を加算。
	void Increment();

};