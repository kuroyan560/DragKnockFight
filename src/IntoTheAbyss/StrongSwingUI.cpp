#include "StrongSwingUI.h"

void StrongSwingUI::Disappear()
{
	if (disappear)return;

	timer = 0;
	disappear = true;
}

#include"DrawFunc.h"
#include"DrawFunc_Color.h"
#include"WinApp.h"
#include"TexHandleMgr.h"
#include"KuroMath.h"
void StrongSwingUI::Draw(const int& MyIdx)
{
	static const int STRONG_SWING_COUNT_GRAPH = TexHandleMgr::LoadGraph("resource/ChainCombat/UI/canSwingCount.png");
	static const int STRONG_SWING_COUNT_FRAME_GRAPH = TexHandleMgr::LoadGraph("resource/ChainCombat/UI/canSwingCount_frame.png");

	static const int EASE_TIME = 15;
	Vec2<float> flashExp = { 1.0f,1.0f };
	if (disappear)
	{
		if (timer == 0)flashExp = { 0.5f, 3.0f };
		if (timer == 1)flashExp = { 3.0f,0.5f };
		if (timer < EASE_TIME)timer++;
	}
	float exp = KuroMath::Ease(Out, Back, timer, EASE_TIME, 1.0f, 0.0f);

	Vec2<float>drawPos(35, WinApp::Instance()->GetExpandWinSize().y + 40);
	const float offsetY = 100;
	drawPos.y -= offsetY * (MyIdx + 1);

	DrawFunc::DrawRotaGraph2D(drawPos, { 1.0f,1.0f }, 0.0f, TexHandleMgr::GetTexBuffer(STRONG_SWING_COUNT_FRAME_GRAPH));

	if (disappear && (timer == 1 || timer == 2))
	{
		DrawFunc_Color::DrawRotaGraph2D(drawPos, flashExp, 0.0f, TexHandleMgr::GetTexBuffer(STRONG_SWING_COUNT_GRAPH), Color(1.0f, 1.0f, 1.0f, 1.0f));
	}
	else
	{
		DrawFunc::DrawRotaGraph2D(drawPos, Vec2<float>(exp, exp), 0.0f, TexHandleMgr::GetTexBuffer(STRONG_SWING_COUNT_GRAPH));
	}
}

StrongSwingTutorialUI::StrongSwingTutorialUI()
{
	std::vector<Anim>rStickAnim(1);
	rStickAnim[0].graph.resize(3);
	TexHandleMgr::LoadDivGraph("resource/ChainCombat/tutorial/r_stick.png", 3, { 3,1 }, rStickAnim[0].graph.data());
	rStickAnim[0].interval = 15;
	rStickAnim[0].loop = true;
	rStick = std::make_shared<PlayerAnimation>(rStickAnim);

	std::vector<Anim>ltButtonAnim(1);
	ltButtonAnim[0].graph.resize(2);
	TexHandleMgr::LoadDivGraph("resource/ChainCombat/tutorial/LT.png", 2, { 2,1 }, ltButtonAnim[0].graph.data());
	ltButtonAnim[0].interval = 15;
	ltButtonAnim[0].loop = true;
	ltButton = std::make_shared<PlayerAnimation>(ltButtonAnim);
}

void StrongSwingTutorialUI::Init()
{
	rStick->Init(0);
	ltButton->Init(0);
}


void StrongSwingTutorialUI::Draw(const bool& CanStrongSwing)
{
	static const int TUTORIAL_FRAME = TexHandleMgr::LoadGraph("resource/ChainCombat/tutorial/icon_frame_sqr.png");
	if (!CanStrongSwing)return;

	rStick->Update();
	ltButton->Update();

	const float exp = 0.7f;
	float x = 110;
	float subY = 60;
	DrawFunc::DrawRotaGraph2D({ x,WinApp::Instance()->GetExpandWinSize().y - subY }, { exp,exp }, 0.0f, TexHandleMgr::GetTexBuffer(rStick->GetGraphHandle()));
	DrawFunc::DrawRotaGraph2D({ x,WinApp::Instance()->GetExpandWinSize().y - subY }, { exp,exp }, 0.0f, TexHandleMgr::GetTexBuffer(TUTORIAL_FRAME));

	//x += 95;
	subY += 80;
	DrawFunc::DrawRotaGraph2D({ x,WinApp::Instance()->GetExpandWinSize().y - subY }, { exp,exp }, 0.0f, TexHandleMgr::GetTexBuffer(ltButton->GetGraphHandle()));
	DrawFunc::DrawRotaGraph2D({ x,WinApp::Instance()->GetExpandWinSize().y - subY }, { exp,exp }, 0.0f, TexHandleMgr::GetTexBuffer(TUTORIAL_FRAME));
}
