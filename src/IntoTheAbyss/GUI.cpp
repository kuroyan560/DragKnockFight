#include "GUI.h"
#include"TexHandleMgr.h"
#include"DrawFunc_Color.h"
#include"WinApp.h"

GUI::GUI()
{
	spearTeleGraph = TexHandleMgr::LoadGraph("resource/IntoTheAbyss/UI/spear_ui_tele.png");
	spearTimeGraph = TexHandleMgr::LoadGraph("resource/IntoTheAbyss/UI/spear_ui_time.png");
}

void GUI::Draw()
{
	DrawFunc_Color::DrawGraph({ 0.0f,0.0f }, TexHandleMgr::GetTexBuffer(spearTimeGraph));
	DrawFunc_Color::DrawGraph({ WinApp::Instance()->GetExpandWinSize().x - 252.0f,0.0f }, TexHandleMgr::GetTexBuffer(spearTimeGraph));
}
