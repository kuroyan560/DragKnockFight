#include "StrongSwingUI.h"

void StrongSwingUI::Disappear()
{
	if (disappear)return;

	timer = 0;
	disappear = true;
}

#include"DrawFunc.h"
#include"WinApp.h"
#include"TexHandleMgr.h"
void StrongSwingUI::Draw(const int& MyIdx)
{
	static const int STRONG_SWING_COUNT_GRAPH = TexHandleMgr::LoadGraph("resource/ChainCombat/UI/canSwingCount.png");
	if (disappear)
	{

	}
	else
	{
		Vec2<float>drawPos(30, WinApp::Instance()->GetExpandWinSize().y);
		const float offsetY = 70;
		drawPos.y -= offsetY * (MyIdx + 1);
		DrawFunc::DrawGraph(drawPos, TexHandleMgr::GetTexBuffer(STRONG_SWING_COUNT_GRAPH));
	}
}
