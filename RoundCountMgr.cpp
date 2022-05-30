#include "RoundCountMgr.h"
#include "DrawFunc.h"
#include "TexHandleMgr.h"

RoundCount::RoundCount()
{

	/*===== コンストラクタ =====*/

	pos = {};
	isActive = false;
	isFill = false;

}

void RoundCount::Init()
{

	/*===== 初期化処理 =====*/

	pos = {};
	isActive = false;
	isFill = false;

}

void RoundCount::Generate(const Vec2<float>& Pos)
{

	/*===== 初期化処理 =====*/

	pos = Pos;
	isActive = true;
	isFill = false;

}

void RoundCount::Draw(const int& FrameGraph, const int& InnerGraph)
{

	/*===== 描画処理 =====*/

	// 外側を描画
	DrawFunc::DrawRotaGraph2D(pos, Vec2<float>(1.0f,1.0f), 0, TexHandle)

}
