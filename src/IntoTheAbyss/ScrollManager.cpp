#include "ScrollManager.h"
#include"Camera.h"

void ScrollManager::Init(const Vec2<float> POS, const Vec2<float> &MAP_MAX_SIZE)
{
	mapSize = MAP_MAX_SIZE;

	Vec2<float>startPos = CaluStartScrollLine(Vec2<float>(1280 / 2.0f, 720 / 2.0f));
	Vec2<float>endPos = CaluEndScrollLine(Vec2<float>(1280 / 2.0f, 720 / 2.0f));
	honraiScrollAmount = POS - startPos;
	scrollAmount = honraiScrollAmount;
	initFlag = true;
}

void ScrollManager::Update()
{
	scrollAmount.x += (honraiScrollAmount.x - scrollAmount.x - Camera::Instance()->scrollAffect.x) / 5.0f;
	scrollAmount.y += (honraiScrollAmount.y - scrollAmount.y - Camera::Instance()->scrollAffect.y) / 5.0f;
}

void ScrollManager::CalucurateScroll(const Vec2<float> &VEL)
{
	//ワープした際は前フレーム計算の影響が出ないように処理を飛ばす
	if (!initFlag)
	{
		honraiScrollAmount -= VEL;
	}
	initFlag = false;


	//スクロールの制限
	Vec2<float>startPos = CaluStartScrollLine(Vec2<float>(1280 / 2.0f, 720 / 2.0f));
	Vec2<float>endPos = CaluEndScrollLine(Vec2<float>(1280 / 2.0f, 720 / 2.0f));

	float adj = 50.0f / 2.0f;
	Vec2<float>scrollMaxValue = (endPos - startPos) + Vec2<float>(adj, adj);
	Vec2<float>scrollMinValue = { -adj,-adj };

	if (honraiScrollAmount.x <= scrollMinValue.x)
	{
		honraiScrollAmount.x = scrollMinValue.x;
	}
	if (honraiScrollAmount.y <= scrollMinValue.y)
	{
		honraiScrollAmount.y <= scrollMinValue.y;
	}
	if (scrollMaxValue.x <= honraiScrollAmount.x)
	{
		honraiScrollAmount.x = scrollMaxValue.x;
	}
	if (scrollMaxValue.y <= honraiScrollAmount.y)
	{
		honraiScrollAmount.y <= scrollMaxValue.y;
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
