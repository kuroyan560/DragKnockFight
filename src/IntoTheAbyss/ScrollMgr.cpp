#include "ScrollMgr.h"
#include "ViewPort.h"
#include"../Engine/WinApp.h"

void ScrollMgr::Init(const Vec2<float> POS, const Vec2<float> &MAP_MAX_SIZE, const Vec2<float> &ADJ)
{
	mapSize = MAP_MAX_SIZE;
	adjLine = ADJ;


	windowSize = WinApp::Instance()->GetWinSize();
	windowHalfSize = Vec2<float>(windowSize.x / 2.0f, windowSize.y / 2.0f);



	Vec2<float>startPos = CaluStartScrollLine(windowHalfSize - adjLine);
	Vec2<float>endPos = CaluEndScrollLine(windowHalfSize + adjLine);
	honraiScrollAmount = (POS + adjLine) - startPos;
	scrollAmount = honraiScrollAmount;
	initFlag = true;
}

void ScrollMgr::Update()
{
	scrollAmount.x += (honraiScrollAmount.x - scrollAmount.x - Camera::Instance()->scrollAffect.x) / 5.0f;
	scrollAmount.y += (honraiScrollAmount.y - scrollAmount.y - Camera::Instance()->scrollAffect.y) / 5.0f;
}

void ScrollMgr::CalucurateScroll(const Vec2<float> &VEL, const Vec2<float> &PLAYER_POS)
{
	//ワープした際は前フレーム計算の影響が出ないように処理を飛ばす


	//スクロールの制限
	Vec2<float>startPos = CaluStartScrollLine(windowHalfSize - adjLine);
	Vec2<float>endPos = CaluEndScrollLine(windowHalfSize + adjLine);

	//プレイヤー座標よりライン外に出たらスクロール量は変化しない

	//if (!initFlag)
	{
		honraiScrollAmount -= VEL;
	}
	initFlag = false;


	const float adj = 50.0f / 2.0f;
	const Vec2<float>scrollMaxValue = (endPos - startPos) - Vec2<float>(adj, adj);
	const Vec2<float>scrollMinValue = Vec2<float>(-adj, -adj);

	if (PLAYER_POS.x <= startPos.x - adjLine.x)
	{
		honraiScrollAmount.x = scrollMinValue.x;
	}
	if (endPos.x - adjLine.x - adj <= PLAYER_POS.x)
	{
		honraiScrollAmount.x = scrollMaxValue.x;
	}

	const bool getMinVelFlag = PLAYER_POS.y <= startPos.y - adjLine.y;
	if (getMinVelFlag)
	{
		honraiScrollAmount.y = scrollMinValue.y;
	}

	const bool getMaxVelFlag = endPos.y - adjLine.y - adj <= PLAYER_POS.y;
	if (getMaxVelFlag)
	{
		honraiScrollAmount.y = scrollMaxValue.y;
	}
}

Vec2<float> ScrollMgr::Affect(const Vec2<float> &Pos)
{
	Vec2<float> scrollShakeZoom = scrollAmount + ShakeMgr::Instance()->shakeAmount;
	scrollShakeZoom.x *= zoom;
	scrollShakeZoom.y *= zoom;
	return Pos * zoom - scrollShakeZoom;
}

void ScrollMgr::Warp(const Vec2<float> POS)
{
}
