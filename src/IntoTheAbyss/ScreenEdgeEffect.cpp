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
	float adjDistance = 40.0f;

	screenEdge[UP].Init(distance, Vec2<float>(winHalfSize.x, -adjDistance), 0.0f);
	screenEdge[DOWN].Init(distance, Vec2<float>(winHalfSize.x, winSize.y + adjDistance), 0.0f);
	screenEdge[LEFT].Init(distance, Vec2<float>(0.0f - adjDistance, winHalfSize.y), 90.0f);
	screenEdge[RIGHT].Init(distance, Vec2<float>(winSize.x + adjDistance, winHalfSize.y), 90.0f);

	leftWinFlag = false;
	rightWinFlag = false;
	winFadeOutTimer = 120;
}

void ScreenEdgeEffect::Update()
{
	for (int i = 0; i < screenEdge.size(); ++i)
	{
		screenEdge[i].CaluPos(nowDistance);
		screenEdge[i].Update();
	}

	if (rightWinFlag)
	{
		screenEdge[UP].StartWinEffect(winFadeOutTimer);
		screenEdge[DOWN].StartFinishEffect();
		screenEdge[LEFT].StartWinEffect(winFadeOutTimer);
		screenEdge[RIGHT].StartFinishEffect();
	}
	if (leftWinFlag)
	{
		screenEdge[UP].StartWinEffect(winFadeOutTimer);
		screenEdge[DOWN].StartFinishEffect();
		screenEdge[LEFT].StartFinishEffect();
		screenEdge[RIGHT].StartWinEffect(winFadeOutTimer);
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

void ScreenEdgeEffect::LeftPlayerWin(const int &FADE_OUT_TIEMR)
{
	leftWinFlag = true;
	winFadeOutTimer = FADE_OUT_TIEMR;
}

void ScreenEdgeEffect::RightPlayerWin(const int &FADE_OUT_TIEMR)
{
	rightWinFlag = true;
	winFadeOutTimer = FADE_OUT_TIEMR;
}
