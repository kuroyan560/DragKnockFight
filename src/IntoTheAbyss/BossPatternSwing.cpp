#include "BossPatternSwing.h"
#include "../IntoTheAbyss/SwingMgr.h"

BossPatternSwing::BossPatternSwing()
{
}

void BossPatternSwing::Init()
{
}

void BossPatternSwing::Update(BossPatternData *DATA)
{

	if (!swingFlag)
	{
		// 振り回しの開始ベクトルを取得。
		SwingMgr::Instance()->easingStartVec = SwingMgr::Instance()->playerPos - SwingMgr::Instance()->bossPos;
		SwingMgr::Instance()->easingStartVec.Normalize();
		SwingMgr::Instance()->easingNowVec = SwingMgr::Instance()->easingStartVec;

		// 振り回しの終了ベクトルを取得。
		SwingMgr::Instance()->easingEndVec = (SwingMgr::Instance()->playerPos - SwingMgr::Instance()->bossPos) * Vec2<float>(1.0f, -1.0f);
		SwingMgr::Instance()->easingEndVec.Normalize();

		// 振り回しフラグを有効化する。
		SwingMgr::Instance()->isSwingPlayer = false;
		SwingMgr::Instance()->isSwingBoss = true;

		// 各タイマーを初期化。
		SwingMgr::Instance()->easingTimer = 0;
		SwingMgr::Instance()->easeAmount = 0;
		SwingMgr::Instance()->easeChangeAmountY = SwingMgr::Instance()->easingEndVec.y - SwingMgr::Instance()->easingStartVec.y;
		swingFlag = true;
	}
	//// クールタイムを設定。
	//swingCoolTime = SWING_COOLTIME;

	//SwingMgr::Instance()->PlaySE();
}
