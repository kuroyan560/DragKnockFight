#include "CircularBarrage.h"
#include "BulletMgrBase.h"

void CircularBarrage::Start()
{

	/*===== 初期化処理 =====*/

	isEnd = false;
	timer = 0;
	angle = 0;
	roundCounter = 0;

}

void CircularBarrage::Init()
{

	isEnd = true;
	timer = 0;
	angle = 0;
	roundCounter = 0;

}

bool CircularBarrage::Update(BulletMgrBase& BulletMgr, const Vec2<float>& Pos, const int& GraphHandle)
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
