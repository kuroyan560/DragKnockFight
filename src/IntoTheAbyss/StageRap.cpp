#include "StageRap.h"
#include"TexHandleMgr.h"

StageRap::StageRap()
{
	TexHandleMgr::LoadDivGraph("resource/ChainCombat/UI/num.png", 12, { 12, 1 }, numberHandle.data());
	slashHandle = numberHandle[10];
}

void StageRap::Init()
{
	rapNum = 0;
	rapMaxNum = 2;
}

void StageRap::Update()
{
	pos = { 100.0f,200.0f };
}

void StageRap::Draw()
{
	DrawFunc::DrawRotaGraph2D(pos, size, 0.0f, TexHandleMgr::GetTexBuffer(numberHandle[rapNum]));

	{
		Vec2<float> lPos = pos + Vec2<float>(10.0f, 0.0f);
		DrawFunc::DrawRotaGraph2D(lPos, size, 0.0f, TexHandleMgr::GetTexBuffer(slashHandle));
	}

	{
		Vec2<float> lPos = pos + Vec2<float>(20.0f, 0.0f);
		DrawFunc::DrawRotaGraph2D(pos, size, 0.0f, TexHandleMgr::GetTexBuffer(numberHandle[rapMaxNum]));
	}
}
