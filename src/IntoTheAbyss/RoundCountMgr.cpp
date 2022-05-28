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
	angle = 0;
	exp = { 3,3 };
	alpha = 0;

}

void RoundCount::Generate(const Vec2<float>& Pos)
{

	/*===== 生成処理 =====*/

	pos = Pos;
	isActive = true;
	isFill = false;
	angle = 0;
	exp = { 10,10 };
	alpha = 0;

}

void RoundCount::Update()
{

	/*===== 更新処理 =====*/

	if (isFill) {

		angle += (DirectX::XM_2PI * 2.0f - angle) / 5.0f;
		alpha += (255 - alpha) / 5.0f;
		exp.x += (1.0f - exp.x) / 5.0f;
		exp.y += (1.0f - exp.y) / 5.0f;

	}

}

void RoundCount::Draw(const int& FrameGraph, const int& InnerGraph)
{

	/*===== 描画処理 =====*/

	// 外側を描画
	DrawFunc::DrawRotaGraph2D(pos, Vec2<float>(1.0f, 1.0f), 0, TexHandleMgr::GetTexBuffer(FrameGraph));

	// 内側を描画
	if (isFill) {
		DrawFunc::DrawRotaGraph2D(pos, exp, 0, TexHandleMgr::GetTexBuffer(InnerGraph), Color(255, 255, 255, alpha));
	}

}

RoundCountMgr::RoundCountMgr()
{

	/*===== コンストラクタ =====*/

	//nowRound = 0;
	//maxRound = 0;
	frameGraph = TexHandleMgr::LoadGraph("resource/ChainCombat/UI/roundGaugeFrame.png");
	innerGraph = TexHandleMgr::LoadGraph("resource/ChainCombat/UI/roundGauge.png");
	roundGraph = TexHandleMgr::LoadGraph("resource/ChainCombat/UI/roundSmall.png");

}

void RoundCountMgr::Init(const int& MaxRound)
{

	/*===== 初期化処理 =====*/

	Vec2<float> UI_OFFSET_POS = Vec2<float>(1140, 100);
	const float UI_OFFSET_SIZE = 45.0f;

	int indexCounter = 0;

	for (auto& index : counter) {

		index.Init();

		// 生成する座標を求める。
		Vec2<float> generatePos = UI_OFFSET_POS + Vec2<float>(indexCounter * UI_OFFSET_SIZE, 0);

		// 現在のラウンド数のUIを生成。
		index.Generate(generatePos);

		++indexCounter;

	}

	nowRound = 0;
	maxRound = 3;

}

void RoundCountMgr::Update()
{

	/*===== 更新処理 =====*/

	for (auto& index : counter) {

		index.Update();

	}

}

void RoundCountMgr::Draw()
{

	/*===== 描画処理 =====*/

	Vec2<float> UI_OFFSET_POS = Vec2<float>(1024, 102);
	DrawFunc::DrawRotaGraph2D(UI_OFFSET_POS, Vec2<float>(1, 1), 0, TexHandleMgr::GetTexBuffer(roundGraph));

	for (auto& index : counter) {

		index.Draw(frameGraph, innerGraph);

	}

}

void RoundCountMgr::RoundIncrement()
{

	/*===== nowRoundをインクリメント =====*/

	// インクリメントしてMaxRoundより大きくならないようにする。
	++nowRound;
	if (maxRound <= nowRound) nowRound = maxRound;

	counter[nowRound - 1].SetFill();

}
