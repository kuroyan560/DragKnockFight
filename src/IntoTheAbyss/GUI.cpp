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
	static const Color PAINT = Color(0.3f, 0.3f, 0.3f, 0.8f);
	DrawFunc_Color::DrawGraph({ 0.0f,0.0f }, TexHandleMgr::GetTexBuffer(spearTimeGraph), PAINT, { false,false }, { 0.0f,0.0f }, { 1.0f,1.0f - spearTimeRate });
	DrawFunc_Color::DrawGraph({ WinApp::Instance()->GetExpandWinSize().x - 252.0f,0.0f }, TexHandleMgr::GetTexBuffer(spearTeleGraph), PAINT, { false,false }, { 0.0f,0.0f }, { 1.0f,1.0f - spearTeleRate });
}
