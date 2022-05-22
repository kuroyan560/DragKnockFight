#pragma once
#include "BarrageBase.h"

// 円形弾幕クラス
class WhirlpoolBarrage : public BarrageBase {

private:

	/*===== メンバ変数 =====*/

	bool isEnd;			// 終わったかどうか。Update関数で返す値。
	int timer;			// 弾幕の間隔などに使用する。
	int roundCounter;	// 何周したか。 一定以上周ったら終わりにする。
	float angle;		// 弾幕を発射する角度。

	const int SHOOT_DELAY = 2;				// 弾を発射する間隔。
	const int MAX_ROUND = 3;
	const float SHOOT_COUNT_PER_ROUND = 10;	// 一周で撃つ弾の数。
	const float ANGLE = DirectX::XM_2PI / SHOOT_COUNT_PER_ROUND;
	const float SPEED = 8.0f;				// 弾速

public:

	/*===== メンバ関数 =====*/

	void Start() override;
	void Init() override;
	bool Update(BulletMgrBase& BulletMgr, const Vec2<float>& Pos, const int& GraphHandle)override;

};