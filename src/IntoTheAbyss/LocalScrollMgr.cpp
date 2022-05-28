#include "LocalScrollMgr.h"
#include"../Engine/WinApp.h"

const float LocalScrollMgr::INIT_SCROLL = 0.4f;

void LocalScrollMgr::Init(const Vec2<float> POS, const Vec2<float> &MAP_MAX_SIZE, const Vec2<float> &ADJ)
{
	mapSize = MAP_MAX_SIZE;
	adjLine = ADJ;


	windowSize = WinApp::Instance()->GetWinSize();
	windowHalfSize = Vec2<float>(windowSize.x / 2.0f, windowSize.y / 2.0f);



	Vec2<float>startPos = CaluStartScrollLine(windowHalfSize - adjLine);
	Vec2<float>endPos = CaluEndScrollLine(windowHalfSize + adjLine);
	honraiScrollAmount = (POS + adjLine) - startPos;
	scrollAmount = honraiScrollAmount;

	resetAmount = scrollAmount;
	//zoom = INIT_SCROLL;
	warpFlag = false;
}

void LocalScrollMgr::Update(const Vec2<float> &LineCenterPos)
{
	//if (Camera::Instance()->active) {
	//	scrollAmount.x = honraiScrollAmount.x - Camera::Instance()->scrollAffect.x;
	//	scrollAmount.y = honraiScrollAmount.y - Camera::Instance()->scrollAffect.y;
	//}
	//else {
	scrollAmount.x += (honraiScrollAmount.x - scrollAmount.x) / 5.0f;
	scrollAmount.y += (honraiScrollAmount.y - scrollAmount.y) / 5.0f;
	//}

	// 線の中心にスクロール量などをかける。
	Vec2<float> scrollShakeZoom = scrollAmount + ShakeMgr::Instance()->shakeAmount;
	scrollShakeZoom.x *= zoom;
	scrollShakeZoom.y *= zoom;
	Vec2<float> lineCenterPos = LineCenterPos * zoom - scrollShakeZoom;

	// 画面の中心を取得。
	Vec2<int> windowCenter = WinApp::Instance()->GetWinCenter();

	// 線の中心と画面の中心との差分を求める。
	Vec2<float> lineCenterOffsetBuff = Vec2<float>((float)windowCenter.x, (float)windowCenter.y) - lineCenterPos;

	// 補完をかける。
	lineCenterOffset = { 512.0f,272.0f };
}

void LocalScrollMgr::CalucurateScroll(const Vec2<float> &VEL, const Vec2<float> &PLAYER_POS)
{
	//ワープした際は前フレーム計算の影響が出ないように処理を飛ばす

	//スクロールの制限
	Vec2<float>startPos = CaluStartScrollLine(windowHalfSize - adjLine);
	Vec2<float>endPos = CaluEndScrollLine(windowHalfSize + adjLine);

	//プレイヤー座標よりライン外に出たらスクロール量は変化しない

	//if (!warpFlag)
	//{
	//	honraiScrollAmount -= VEL;

	//	const float adj = 50.0f / 2.0f;
	//	const Vec2<float>scrollMaxValue = (endPos - startPos) - Vec2<float>(adj, adj);
	//	const Vec2<float>scrollMinValue = Vec2<float>(-adj, -adj);

	//	if (PLAYER_POS.x <= startPos.x - adjLine.x)
	//	{
	//		honraiScrollAmount.x = scrollMinValue.x;
	//	}
	//	if (endPos.x - adjLine.x - adj <= PLAYER_POS.x)
	//	{
	//		honraiScrollAmount.x = scrollMaxValue.x;
	//	}

	//	const bool getMinVelFlag = PLAYER_POS.y <= startPos.y - adjLine.y;
	//	if (getMinVelFlag)
	//	{
	//		honraiScrollAmount.y = scrollMinValue.y;
	//	}

	//	const bool getMaxVelFlag = endPos.y - adjLine.y - adj <= PLAYER_POS.y;
	//	if (getMaxVelFlag)
	//	{
	//		honraiScrollAmount.y = scrollMaxValue.y;
	//	}
	//}
	warpFlag = false;
}

Vec2<float> LocalScrollMgr::Affect(const Vec2<float> &Pos, const Vec2<float> &AFFECT)
{
	Vec2<float> scrollShakeZoom = scrollAmount + ShakeMgr::Instance()->shakeAmount;
	scrollShakeZoom.x *= zoom;
	scrollShakeZoom.y *= zoom;
	return Pos * zoom - scrollShakeZoom + lineCenterOffset + AFFECT;
	//return Pos * zoom - scrollShakeZoom + lineCenterOffset;
}

void LocalScrollMgr::Warp(const Vec2<float> POS)
{
	//warpFlag = true;
	//honraiScrollAmount = { 0.0f,0.0f };
	//Vec2<float>startPos = CaluStartScrollLine(windowHalfSize - adjLine);
	//honraiScrollAmount = (POS + adjLine) - startPos;
}

void LocalScrollMgr::Reset()
{
	honraiScrollAmount = resetAmount;
	scrollAmount = resetAmount;
}
