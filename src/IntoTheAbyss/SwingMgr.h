#include "Vec.h"
#include "Singleton.h"

// 振り回しの処理の際にプレイヤーとボスの橋渡しとなるクラス
class SwingMgr : public Singleton<SwingMgr> {

public:

	/*===== メンバ変数 =====*/

	bool isSwingPlayer;				// プレイヤーが振り回し状態かどうかのフラグ
	bool isSwingBoss;				// ボスが振り回し状態かどうかのフラグ
	float easingTimer;				// 振り回しに使用するイージングタイマー
	Vec2<float> easingStartVec;		// 振り回しイージングの開始地点
	Vec2<float> easingEndVec;		// 振り回しイージングの終了地点


public:

	/*===== 定数 =====*/

	const float ADD_EASINGTIMER = 0.05f;	// イージングタイマーに加算する量


public:

	/*===== メンバ関数 =====*/

	// コンストラクタ
	SwingMgr();

	// 初期化処理
	void Init();

	// 更新処理
	void Update();

};