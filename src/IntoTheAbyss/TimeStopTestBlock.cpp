#include "TimeStopTestBlock.h"
#include "ScrollMgr.h"
#include "ShakeMgr.h"
#include "MapChipCollider.h"

TimeStopTestBlock::TimeStopTestBlock()
{

	/*===== コンストラクタ =====*/

	pos = { 100,100 };
	isTimeStop = false;

}

void TimeStopTestBlock::Update(const Vec2<float>& playerPos)
{

	/*===== 更新処理 =====*/

	// プレイヤーの位置に近付ける。
	Vec2<float> targetPos = playerPos;
	targetPos.y -= 10.0f;

	// 時間停止の短槍が刺さっていなかったら近付ける。
	if (isTimeStop) return;

	// 近付ける。
	pos.x += (targetPos.x - pos.x) / 100.0f;
	pos.y += (targetPos.y - pos.y) / 100.0f;

}

#include"DrawFunc.h"
#include"TexHandleMgr.h"
void TimeStopTestBlock::Draw(const int& blockGraph)
{

	/*===== 描画処理 =====*/

	// 座標を計算。
	const float centerX = pos.x * ScrollMgr::Instance()->zoom - ScrollMgr::Instance()->scrollAmount.x * ScrollMgr::Instance()->zoom;
	const float centerY = pos.y * ScrollMgr::Instance()->zoom - ScrollMgr::Instance()->scrollAmount.y * ScrollMgr::Instance()->zoom;

	// マップを描画
	//DrawExtendGraph(centerX - MAP_CHIP_HALF_SIZE * ScrollMgr::Instance()->zoom + ShakeMgr::Instance()->shakeAmount.x * ScrollMgr::Instance()->zoom,
	//	centerY - MAP_CHIP_HALF_SIZE * ScrollMgr::Instance()->zoom + ShakeMgr::Instance()->shakeAmount.y * ScrollMgr::Instance()->zoom,
	//	centerX + MAP_CHIP_HALF_SIZE * ScrollMgr::Instance()->zoom + ShakeMgr::Instance()->shakeAmount.x * ScrollMgr::Instance()->zoom,
	//	centerY + MAP_CHIP_HALF_SIZE * ScrollMgr::Instance()->zoom + ShakeMgr::Instance()->shakeAmount.y * ScrollMgr::Instance()->zoom,
	//	blockGraph, FALSE);

	Vec2<float>leftUp = { centerX - MAP_CHIP_HALF_SIZE * ScrollMgr::Instance()->zoom + ShakeMgr::Instance()->shakeAmount.x * ScrollMgr::Instance()->zoom,
		centerY - MAP_CHIP_HALF_SIZE * ScrollMgr::Instance()->zoom + ShakeMgr::Instance()->shakeAmount.y * ScrollMgr::Instance()->zoom };
	Vec2<float>rightBottom = { centerX + MAP_CHIP_HALF_SIZE * ScrollMgr::Instance()->zoom + ShakeMgr::Instance()->shakeAmount.x * ScrollMgr::Instance()->zoom,
		centerY + MAP_CHIP_HALF_SIZE * ScrollMgr::Instance()->zoom + ShakeMgr::Instance()->shakeAmount.y * ScrollMgr::Instance()->zoom };
	
	// マップを描画
	DrawFunc::DrawExtendGraph2D(leftUp, rightBottom, TexHandleMgr::GetTexBuffer(blockGraph));

}
