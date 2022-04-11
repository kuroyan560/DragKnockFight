#include "Vec.h"
#include "Singleton.h"

// 振り回しの処理の際にプレイヤーとボスの橋渡しとなるクラス
class SwingMgr : public Singleton<SwingMgr> {

public:

	/*===== メンバ変数 =====*/

	bool isSwingPlayer;				// プレイヤーが振り回し状態かどうかのフラグ
	bool isSwingBoss;				// ボスが振り回し状態かどうかのフラグ
	float easingTimer;				// 振り回しに使用するイージングタイマー
	float easeAmount;				// タイマーから求められるイージング量
	float easeChangeAmountY;		// Y軸の変化量
	float lineLength;				// 紐の長さの総量
	Vec2<float> easingStartVec;		// 振り回しイージングの開始地点
	Vec2<float> easingEndVec;		// 振り回しイージングの終了地点
	Vec2<float> easingNowVec;		// 振り回しイージングの今の値
	Vec2<float> playerPos;			// プレイヤー座標
	Vec2<float> bossPos;			// ボス座標


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
	void Update(const Vec2<float>& playerPos, const Vec2<float>& bossPos, const float& lineLength);

	void PlaySE();
};