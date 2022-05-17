#include "StageSelectScreenShot.h"
#include"TexHandleMgr.h"
#include"../Engine/DrawFunc.h"

StageSelectScreenShot::StageSelectScreenShot()
{
	screenShotHandle[0] = TexHandleMgr::LoadGraph("resource/ChainCombat/select_scene/stage_screen_shot/0.png");
	screenShotHandle[1] = TexHandleMgr::LoadGraph("resource/ChainCombat/select_scene/stage_screen_shot/1.png");
	screenShotHandle[2] = TexHandleMgr::LoadGraph("resource/ChainCombat/select_scene/stage_screen_shot/2.png");
	selectNum = 0;
}

void StageSelectScreenShot::Init()
{

}

void StageSelectScreenShot::Update()
{
}

void StageSelectScreenShot::Draw()
{
	DrawFunc::DrawRotaGraph2D(pos, size, 0.0f, TexHandleMgr::GetTexBuffer(screenShotHandle[selectNum]));
}

void StageSelectScreenShot::Next()
{
	++selectNum;
}

void StageSelectScreenShot::Prev()
{
	--selectNum;
}

void StageSelectScreenShot::ImGuiDraw()
{
	ImGui::Begin("StageSelectImage");
	ImGui::InputFloat("PosX", &pos.x);
	ImGui::InputFloat("PosY", &pos.y);
	ImGui::InputFloat("SizeX", &size.x);
	ImGui::InputFloat("SizeY", &size.y);
	ImGui::Checkbox("ZoomOut", &zoomOutFlag);
	ImGui::End();
}
