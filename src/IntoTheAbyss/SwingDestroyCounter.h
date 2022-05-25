#include "Vec.h"
#include <array>

// SwingingDestroyCounterで使用する数字1つ
class DestroyCounter {

public:

	/*===== メンバ変数 =====*/

	enum class STATUS {

		NONE,
		ENTRY,		// 開始状態
		STAY,		// 待機状態
		EXIT,		// 退出状態

	};

	enum class DISIT_ID {

		DISIT_NONE,
		DISIT1,		// 1の位
		DISIT10,	// 10の位
		DISIT100,	// 100の位

	};

	Vec2<float> pos;	// 描画座標
	int disit;			// 数字
	int exitReservationTimer;
	int exitTimer;		// STAY状態から自動的に消えるまでのタイマー
	int alpha;
	float easingTimer;	// 挙動にイージングをかけるためのタイマー
	bool isActive;		// 有効化されているかのフラグ
	bool isExitReservation;	// Exit予約
	STATUS status;		// 現在のステータス
	DISIT_ID disitID;	// 桁数

	const float ADD_EASING_TIMER = 0.05f;
	const float EASING_CHANGE = 100.0f;
	const float FONT_SIZE = 128.0f;
	const int EXIT_TIMER = 120;


public:

	/*===== メンバ関数 =====*/

	// コンストラクタ
	DestroyCounter();

	// 初期化処理
	void Init();

	// 入場させる。
	void SetEntry(const int& Disit, const DISIT_ID DisitID);

	// 退出させる。
	void SetExit();

	// 更新処理
	void Update(const Vec2<float>& CharaPos);

	// 描画処理
	void Draw(std::array<int, 12> NumberHandle);

};

// 1回のフリ回しで何ブロック壊したかを表示する。
class SwingDestroyCounter {

private:

	/*===== メンバ変数 =====*/

	static const int DESTROY_COUNTER = 600;
	std::array<DestroyCounter, DESTROY_COUNTER> destroyCounter;		// UIに使用する数字1つ。
	std::array<int, 12> numberHandle;
	std::array<int, 12> numberHandle_yellow;

	int nowCounter;		// 現在の破壊数。これの値を元にUIのクラスの数字を設定する。


public:

	/*===== メンバ変数 =====*/

	// コンストラクタ
	SwingDestroyCounter();

	// 初期化
	void Init();

	// 生存しているすべてのカウンターをExit状態にする。
	void AllExit();

	// 更新処理
	void Update(const Vec2<float>& CharaPos);

	// 描画処理
	void Draw();

	// 値を加算。
	void Increment();

	bool IsZero() { return !nowCounter; }

private:

	// 指定の桁数のIndexを取得。
	void CheckDisitStayID(const DestroyCounter::DISIT_ID DisitID);

};