#include "CircularBarrage.h"

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

bool CircularBarrage::Update(BulletMgrBase& BulletMgr, const Vec2<float>& Pos, const int& GraphHandle)
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
