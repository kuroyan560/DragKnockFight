#pragma once
#include "BarrageBase.h"

// 弾幕クラスごとにファイルを作るととんでもない数になりそうなので、このファイルに纏める。

// 渦巻き弾幕クラス
class WhirlpoolBarrage : public BarrageBase {

private:

	/*===== メンバ変数 =====*/

	bool isEnd;			// 終わったかどうか。Update関数で返す値。
	int timer;			// 弾幕の間隔などに使用する。
	int roundCounter;	// 何周したか。 一定以上周ったら終わりにする。
	float angle;		// 弾幕を発射する角度。

	const int SHOOT_DELAY = 2;				// 弾を発射する間隔。
	const int MAX_ROUND = 1;
	const float SHOOT_COUNT_PER_ROUND = 10;	// 一周で撃つ弾の数。
	const float ANGLE = DirectX::XM_2PI / SHOOT_COUNT_PER_ROUND;
	const float SPEED = 8.0f;				// 弾速

public:

	/*===== メンバ関数 =====*/

	void Start() override;
	void Init() override;
	bool Update(BulletMgrBase& BulletMgr, const Vec2<float>& Pos, const Vec2<float>& TargetPos, const int& GraphHandle)override;

};


// 円形弾幕クラス
class CircularBarrage : public BarrageBase {

private:

	/*===== メンバ変数 =====*/

	bool isEnd;			// 終わったかどうか。Update関数で返す値。

	const float SHOOT_COUNT_PER_ROUND = 10;	// 一周で撃つ弾の数。
	const float ANGLE = DirectX::XM_2PI / SHOOT_COUNT_PER_ROUND;
	const float SPEED = 8.0f;				// 弾速

public:

	/*===== メンバ関数 =====*/

	void Start() override;
	void Init() override;
	bool Update(BulletMgrBase& BulletMgr, const Vec2<float>& Pos, const Vec2<float>& TargetPos, const int& GraphHandle)override;

};

// 敵の方向に数発撃つ弾幕
class TargetShotBarrage : public BarrageBase {

private:

	/*===== メンバ変数 =====*/

	bool isEnd;			// 終わったかどうか。Update関数で返す値。
	int timer;			// 弾幕の間隔などに使用する。
	int shotCounter;	// 撃った数

	const int SHOOT_DELAY = 5;				// 弾を発射する間隔。
	const int SHOOT_COUNT = 3;
	const float SPEED = 10.0f;				// 弾速

public:

	/*===== メンバ関数 =====*/

	void Start() override;
	void Init() override;
	bool Update(BulletMgrBase& BulletMgr, const Vec2<float>& Pos, const Vec2<float>& TargetPos, const int& GraphHandle)override;

};

// 敵の方向に複数個の弾を散らばらせて撃つ
class ShotGunBarrage : public BarrageBase {

private:

	/*===== メンバ変数 =====*/

	bool isEnd;			// 終わったかどうか。Update関数で返す値。

	const int SHOOT_COUNT = 10;				// 撃つ数
	const float MAX_SPEED = 15.0f;			// 弾速の最大値
	const float AngleDispersion = 0.5f;		// 角度の分散具合 ラジアン
	const float MIN_SPEED = 5.0f;			// 弾速の最小値

public:

	/*===== メンバ関数 =====*/

	void Start() override;
	void Init() override;
	bool Update(BulletMgrBase& BulletMgr, const Vec2<float>& Pos, const Vec2<float>& TargetPos, const int& GraphHandle)override;

};