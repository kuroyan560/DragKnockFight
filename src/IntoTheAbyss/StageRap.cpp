#include "StageRap.h"
#include"TexHandleMgr.h"

StageRap::StageRap()
{
	TexHandleMgr::LoadDivGraph("resource/ChainCombat/UI/num.png", 12, { 12, 1 }, numberHandle.data());
	slashHandle = numberHandle[10];
}

void StageRap::Init(int RAP_MAX_NUM)
{
	rapNum = 1;
	rapMaxNum = RAP_MAX_NUM;
	pos = { 580.0f,40.0f };
	size = { 0.8f,0.8f };
}

void StageRap::Update()
{
	if (rapMaxNum <= rapNum)
	{
		rapNum = rapMaxNum;
	}
	if (rapNum <= 0)
	{
		rapNum = 0;
	}
}

void StageRap::Draw()
{
	DrawFunc::DrawRotaGraph2D(pos, size, 0.0f, TexHandleMgr::GetTexBuffer(numberHandle[rapNum]));

	{
		Vec2<float> lPos = pos + Vec2<float>(50.0f, 0.0f);
		DrawFunc::DrawRotaGraph2D(lPos, size, 0.0f, TexHandleMgr::GetTexBuffer(slashHandle));
	}

	{
		Vec2<float> lPos = pos + Vec2<float>(100.0f, 0.0f);
		DrawFunc::DrawRotaGraph2D(lPos, size, 0.0f, TexHandleMgr::GetTexBuffer(numberHandle[rapMaxNum]));
	}
}

void StageRap::Increment()
{
	++rapNum;
}
