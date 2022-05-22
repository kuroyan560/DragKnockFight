#pragma once
#include "Vec.h"
#include "BulletMgrBase.h"

// 弾幕基底クラス
class BarrageBase{

public:

	/*===== メンバ関数 =====*/

	virtual void Start() = 0;
	virtual void Init() = 0;
	virtual bool Update(BulletMgrBase& BulletMgr, const Vec2<float>& Pos, const int& GraphHandle) = 0;	// trueが帰ってきたら終わり。

};