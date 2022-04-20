#include "ScreenEdgeEffect.h"
#include "../Engine/WinApp.h"

ScreenEdgeEffect::ScreenEdgeEffect()
{
}

void ScreenEdgeEffect::Init()
{
	Vec2<float>winSize, winHalfSize;
	winSize.x = static_cast<float>(WinApp::Instance()->GetWinSize().x);
	winSize.y = static_cast<float>(WinApp::Instance()->GetWinSize().y);

	winHalfSize = winSize;
	winHalfSize /= 2.0f;

	float distance = 120.0f;
	float adjDistance = 80.0f;

	screenEdge[UP].Init(distance, Vec2<float>(winHalfSize.x, -adjDistance), 0.0f);
	screenEdge[DOWN].Init(distance, Vec2<float>(winHalfSize.x, winSize.y + adjDistance), 0.0f);
	screenEdge[LEFT].Init(distance, Vec2<float>(0.0f - adjDistance, winHalfSize.y), 90.0f);
	screenEdge[RIGHT].Init(distance, Vec2<float>(winSize.x + adjDistance, winHalfSize.y), 90.0f);
}

void ScreenEdgeEffect::Update()
{
	for (int i = 0; i < screenEdge.size(); ++i)
	{
		screenEdge[i].CaluPos(nowDistance);
		screenEdge[i].Update();
	}

	if (leftWinFlag)
	{
		screenEdge[UP].StartWinEffect();
		screenEdge[DOWN].StartFinishEffect();
		screenEdge[LEFT].StartWinEffect();
		screenEdge[RIGHT].StartFinishEffect();
	}
	if (rightWinFlag)
	{
		screenEdge[UP].StartWinEffect();
		screenEdge[DOWN].StartFinishEffect();
		screenEdge[LEFT].StartFinishEffect();
		screenEdge[RIGHT].StartWinEffect();
	}
}

void ScreenEdgeEffect::Draw()
{
	for (int i = 0; i < screenEdge.size(); ++i)
	{
		screenEdge[i].Draw();
	}
}

void ScreenEdgeEffect::CheckPos(const float &LINE_CENTER_POS)
{
	nowDistance = LINE_CENTER_POS;
}
