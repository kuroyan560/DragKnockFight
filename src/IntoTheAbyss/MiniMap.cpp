#include "MiniMap.h"
#include"../Engine/DrawFunc.h"
#include"../IntoTheAbyss/TexHandleMgr.h"
#include"CharacterAIData.h"
MiniMap::MiniMap()
{
	nowPosHandle = TexHandleMgr::LoadGraph("resource/ChainCombat/UI/position_gauge_now.png");

	miniX = 0.0f;
	massX = 0.0f;
}

void MiniMap::CalucurateCurrentPos(const Vec2<float> &POS)
{
	nowValue = (POS.x - leftWall) / (maxValue - rightWall);
	nowPos.y = 131.0f;
}

void MiniMap::Init(const float &MAX_VALUE)
{
	maxValue = MAX_VALUE;
	leftWall = 200.0f;
	rightWall = 200.0f;

	nowPos.y = 131.0f;
	nowValue = 0.5f;
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

	nowPos.x = 50.0f + nowValue * 1110.0f;

	CharacterAIData::Instance()->position = nowValue;
}

void MiniMap::Draw()
{
	static const Vec2<float>OFFSET = { 0.0f,23.0f };
	DrawFunc::DrawRotaGraph2D(nowPos + OFFSET, Vec2<float>(1.0f, 1.0f), 0.0f, TexHandleMgr::GetTexBuffer(nowPosHandle));
}
