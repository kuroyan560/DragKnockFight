#include "Vec.h"
#include "Singleton.h"

// スローを管理するクラス
class SlowMgr : public Singleton<SlowMgr> {

public:

	/*===== メンバ変数 =====*/

	float slowAmount;		// スローの値 0~1の範囲


public:

	/*===== 定数 =====*/


public:

	/*===== メンバ関数 =====*/

	// コンストラクタ
	SlowMgr() {
		slowAmount = 1.0f;
	}

	// 初期化処理
	inline void Init() {
		slowAmount = 1.0f;
	}

};