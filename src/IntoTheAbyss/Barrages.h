#pragma once
#include "BarrageBase.h"

// 弾幕クラスごとにファイルを作るととんでもない数になりそうなので、このファイルに纏める。
// Ctrl + M + O で閉じてもらったほうが良いかもです！

// 渦巻き弾幕クラス
#pragma region WhirlpoolBarrage

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

#pragma endregion


// 2Way渦巻き弾幕クラス
#pragma region Whirlpool2WayBarrage

class Whirlpool2WayBarrage : public BarrageBase {

private:

	/*===== メンバ変数 =====*/

	bool isEnd;			// 終わったかどうか。Update関数で返す値。
	int timer;			// 弾幕の間隔などに使用する。
	int roundCounter;	// 何周したか。 一定以上周ったら終わりにする。
	float angle;		// 弾幕を発射する角度。

	const int SHOOT_DELAY = 2;				// 弾を発射する間隔。
	const int MAX_ROUND = 1;				// 周回数 
	const float SHOOT_COUNT_PER_ROUND = 10;	// 一周で撃つ弾の数。
	const float ANGLE = DirectX::XM_2PI / SHOOT_COUNT_PER_ROUND;
	const float SPEED = 8.0f;				// 弾速

public:

	/*===== メンバ関数 =====*/

	void Start() override;
	void Init() override;
	bool Update(BulletMgrBase& BulletMgr, const Vec2<float>& Pos, const Vec2<float>& TargetPos, const int& GraphHandle)override;

};

#pragma endregion


// 円形弾幕クラス
#pragma region CircularBarrage

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

#pragma endregion


// 敵の方向に数発撃つ弾幕
#pragma region TargetShotBarrage

class TargetShotBarrage : public BarrageBase {

private:

	/*===== メンバ変数 =====*/

	bool isEnd;			// 終わったかどうか。Update関数で返す値。
	int timer;			// 弾幕の間隔などに使用する。
	int shotCounter;	// 撃った数

	const int SHOOT_DELAY = 5;				// 弾を発射する間隔。
	const int SHOOT_COUNT = 5;
	const float SPEED = 15.0f;				// 弾速

public:

	/*===== メンバ関数 =====*/

	void Start() override;
	void Init() override;
	bool Update(BulletMgrBase& BulletMgr, const Vec2<float>& Pos, const Vec2<float>& TargetPos, const int& GraphHandle)override;

};

#pragma endregion


// 敵の方向に数発撃つ弾幕 3Way
#pragma region TargetShot3WayBarrage

class TargetShot3WayBarrage : public BarrageBase {

private:

	/*===== メンバ変数 =====*/

	bool isEnd;			// 終わったかどうか。Update関数で返す値。
	int timer;			// 弾幕の間隔などに使用する。
	int shotCounter;	// 撃った数

	const int SHOOT_DELAY = 5;				// 弾を発射する間隔。
	const int SHOOT_COUNT = 5;
	const float SPEED = 15.0f;				// 弾速
	const float AngleDispersion = 0.5f;		// 角度の分散具合 ラジアン

public:

	/*===== メンバ関数 =====*/

	void Start() override;
	void Init() override;
	bool Update(BulletMgrBase& BulletMgr, const Vec2<float>& Pos, const Vec2<float>& TargetPos, const int& GraphHandle)override;

};

#pragma endregion


// 敵の方向に複数個の弾を散らばらせて撃つ
#pragma region ShotGunBarrage

class ShotGunBarrage : public BarrageBase {

private:

	/*===== メンバ変数 =====*/

	bool isEnd;			// 終わったかどうか。Update関数で返す値。

	const int SHOOT_COUNT = 10;				// 撃つ数
	const float MAX_SPEED = 15.0f;			// 弾速の最大値
	const float MIN_SPEED = 5.0f;			// 弾速の最小値
	const float AngleDispersion = 0.5f;		// 角度の分散具合 ラジアン

public:

	/*===== メンバ関数 =====*/

	void Start() override;
	void Init() override;
	bool Update(BulletMgrBase& BulletMgr, const Vec2<float>& Pos, const Vec2<float>& TargetPos, const int& GraphHandle)override;

};

#pragma endregion


// 敵の方向に波状の弾幕を飛ばす。
#pragma region WaveBarrage

class WaveBarrage : public BarrageBase {

private:

	/*===== メンバ変数 =====*/

	bool isEnd;			// 終わったかどうか。Update関数で返す値。
	int timer;			// 弾幕の間隔などに使用する。
	float sinTimer;		// sin波に使用する。

	const float TIMER = 60.0f;				// 撃ち切るまでの合計時間
	const float FRAME_TIMER = 0.2f;			// Sin波に加算する値
	const float SPEED = 8.0f;				// 弾速
	const float AngleDispersion = 1.0f;		// 角度の分散具合 ラジアン
	const int SHOOT_DELAY = 5;				// 弾を撃つ遅延

public:

	/*===== メンバ関数 =====*/

	void Start() override;
	void Init() override;
	bool Update(BulletMgrBase& BulletMgr, const Vec2<float>& Pos, const Vec2<float>& TargetPos, const int& GraphHandle)override;

};

#pragma endregion