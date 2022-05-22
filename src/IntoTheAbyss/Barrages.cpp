#include "Barrages.h"

#pragma region WhirlpoolBarrage

void WhirlpoolBarrage::Start()
{

	/*===== 初期化処理 =====*/

	isEnd = false;
	timer = 0;
	angle = 0;
	roundCounter = 0;

}

void WhirlpoolBarrage::Init()
{

	isEnd = true;
	timer = 0;
	angle = 0;
	roundCounter = 0;

}

bool WhirlpoolBarrage::Update(BulletMgrBase& BulletMgr, const Vec2<float>& Pos, const Vec2<float>& TargetPos, const int& GraphHandle)
{

	/*===== 更新処理 =====*/

	if (isEnd) return true;

	// タイマーを更新して、弾を撃つ処理を更新する。
	++timer;
	if (SHOOT_DELAY < timer) {

		timer = 0;

		// 弾を生成。
		BulletMgr.Generate(GraphHandle, Pos, angle, SPEED);

		// 角度を更新。
		angle += ANGLE;

		// 角度が一周を超えたら一周した判定を行う。
		if (DirectX::XM_2PI <= angle) {

			angle = 0;
			++roundCounter;

		}

	}

	// 一定以上周ったら処理を終える。
	if (MAX_ROUND <= roundCounter) {

		isEnd = true;

	}

	return isEnd;
}

#pragma endregion


#pragma region CircularBarrage

void CircularBarrage::Start()
{

	/*===== 開始 =====*/

	isEnd = false;

}

void CircularBarrage::Init()
{

	/*===== 強制終了 =====*/

	isEnd = true;

}

bool CircularBarrage::Update(BulletMgrBase& BulletMgr, const Vec2<float>& Pos, const Vec2<float>& TargetPos, const int& GraphHandle)
{

	/*===== 更新処理 =====*/

	if (isEnd) return true;

	float shootAngle = 0;

	// 周りに弾を生成する。
	for (int index = 0; index < SHOOT_COUNT_PER_ROUND; ++index) {

		// 弾を生成して角度を更新する。
		BulletMgr.Generate(GraphHandle, Pos, shootAngle, SPEED);

		shootAngle += ANGLE;

	}

	// 一回撃ったら終わり。
	isEnd = true;

	return isEnd;
}

#pragma endregion


#pragma region TargetShotBarrage

void TargetShotBarrage::Start()
{

	/*===== 開始 =====*/

	isEnd = false;
	timer = 0;
	shotCounter = 0;

}

void TargetShotBarrage::Init()
{

	/*===== 強制終了 =====*/

	isEnd = true;

}

bool TargetShotBarrage::Update(BulletMgrBase& BulletMgr, const Vec2<float>& Pos, const Vec2<float>& TargetPos, const int& GraphHandle)
{

	/*===== 更新処理 =====*/

	if (isEnd) return true;

	// タイマーを更新して弾を撃つ。
	++timer;
	if (SHOOT_DELAY <= timer) {

		timer = 0;

		++shotCounter;

		BulletMgr.Generate(GraphHandle, Pos, atan2f(TargetPos.y - Pos.y, TargetPos.x - Pos.x), SPEED);

	}

	// 一定数撃ったら終わり。
	if (SHOOT_COUNT <= shotCounter) isEnd = true;

	return isEnd;
}

#pragma endregion

void ShotGunBarrage::Start()
{

	/*===== 開始 =====*/

	isEnd = false;

}

void ShotGunBarrage::Init()
{

	/*===== 強制終了 =====*/

	isEnd = true;

}

bool ShotGunBarrage::Update(BulletMgrBase& BulletMgr, const Vec2<float>& Pos, const Vec2<float>& TargetPos, const int& GraphHandle)
{

	/*===== 更新処理 =====*/

	if (isEnd) return true;

	// キャラ間の角度。
	float baseAngle = atan2f(TargetPos.y - Pos.y, TargetPos.x - Pos.x);

	// 指定の数分弾を撃つ。
	for (int index = 0; index < SHOOT_COUNT; ++index) {

		// 散らばらせるために角度を多少ずらす。
		float shootAngle = baseAngle + (KuroFunc::GetRand(AngleDispersion * 2.0f) - AngleDispersion);

		// 移動速度もランダムで求める。
		float speed = KuroFunc::GetRand(MAX_SPEED - MIN_SPEED) + MIN_SPEED;

		// 弾を生成する。
		BulletMgr.Generate(GraphHandle, Pos, shootAngle, speed);

	}

	// 一回撃ったら終わり。
	isEnd = true;

	return isEnd;

}
