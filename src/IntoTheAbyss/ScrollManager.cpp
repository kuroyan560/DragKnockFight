#include "ScrollManager.h"
#include"Camera.h"

void ScrollManager::Init(const Vec2<float> POS, const Vec2<float> &MAP_MAX_SIZE, const Vec2<float> &ADJ)
{
	mapSize = MAP_MAX_SIZE;
	adjLine = ADJ;
	Vec2<float>startPos = CaluStartScrollLine(Vec2<float>(1280 / 2.0f, 720 / 2.0f - adjLine.y));
	Vec2<float>endPos = CaluEndScrollLine(Vec2<float>(1280 / 2.0f, 720 / 2.0f + adjLine.y));
	honraiScrollAmount = (endPos - POS);
	scrollAmount = honraiScrollAmount;
	initFlag = true;
}

void ScrollManager::Update()
{
	scrollAmount.x += (honraiScrollAmount.x - scrollAmount.x - Camera::Instance()->scrollAffect.x) / 5.0f;
	scrollAmount.y += (honraiScrollAmount.y - scrollAmount.y - Camera::Instance()->scrollAffect.y) / 5.0f;
}

void ScrollManager::CalucurateScroll(const Vec2<float> &VEL, const Vec2<float> &PLAYER_POS)
{
	//ワープした際は前フレーム計算の影響が出ないように処理を飛ばす


	//スクロールの制限
	Vec2<float>startPos = CaluStartScrollLine(Vec2<float>(1280 / 2.0f, 720 / 2.0f));
	Vec2<float>endPos = CaluEndScrollLine(Vec2<float>(1280 / 2.0f, 720 / 2.0f));

	//プレイヤー座標よりライン外に出たらスクロール量は変化しない

	//if (!initFlag)
	{
		honraiScrollAmount -= VEL;
	}
	initFlag = false;


	const float adj = 50.0f / 2.0f;
	const Vec2<float>scrollMaxValue = (endPos - startPos) - Vec2<float>(adj, adj);
	const Vec2<float>scrollMinValue = Vec2<float>(-adj, -adj);

	if (PLAYER_POS.x <= startPos.x)
	{
		honraiScrollAmount.x = scrollMinValue.x;
	}
	if (endPos.x <= PLAYER_POS.x)
	{
		honraiScrollAmount.x = scrollMaxValue.x;
	}

	const bool getMinVelFlag = PLAYER_POS.y <= startPos.y;
	if (getMinVelFlag)
	{
		honraiScrollAmount.y = scrollMinValue.y;
	}

	const bool getMaxVelFlag = endPos.y - adjLine.y <= PLAYER_POS.y;
	if (getMaxVelFlag)
	{
		honraiScrollAmount.y = scrollMaxValue.y;
	}
}

Vec2<float> ScrollManager::Affect(const Vec2<float> &Pos)
{
	Vec2<float> scrollShakeZoom = scrollAmount + ShakeMgr::Instance()->shakeAmount;
	scrollShakeZoom.x *= zoom;
	scrollShakeZoom.y *= zoom;
	return Pos * zoom - scrollShakeZoom;
}

void ScrollManager::Warp(const Vec2<float> POS)
{
}
