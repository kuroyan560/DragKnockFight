#include "MiniMap.h"
#include"../Engine/DrawFunc.h"
#include"../IntoTheAbyss/TexHandleMgr.h"
MiniMap::MiniMap()
{
	lineHandle = TexHandleMgr::LoadGraph("resource/IntoTheAbyss/UI/position_gauge.png");
	nowPosHandle = TexHandleMgr::LoadGraph("resource/IntoTheAbyss/UI/position_gauge_now.png");

	linePos.x = 1280.0f / 2.0f;

	miniX = 0.0f;
	massX = 0.0f;
}

void MiniMap::CalucurateCurrentPos(const Vec2<float> &POS)
{
	nowValue = POS.x / maxValue;
}

void MiniMap::Init(const float &MAX_VALUE)
{
	maxValue = MAX_VALUE;
}

void MiniMap::Update()
{
	float miniDivX = 0.0f;
	float massDivX = 1.0f;

	if (nowValue <= miniDivX)
	{
		nowValue = miniDivX;
	}
	if (massDivX <= nowValue)
	{
		nowValue = massDivX;
	}

	nowPos.x = 0.0f + nowValue * 1185.0f;
	nowPos.y = 10.0f;
}

void MiniMap::Draw()
{
	DrawFunc::DrawRotaGraph2D(linePos, Vec2<float>(1.0f, 1.0f), 0.0f, TexHandleMgr::GetTexBuffer(lineHandle));
	DrawFunc::DrawRotaGraph2D(nowPos, Vec2<float>(1.0f, 1.0f), 0.0f, TexHandleMgr::GetTexBuffer(nowPosHandle));
}
