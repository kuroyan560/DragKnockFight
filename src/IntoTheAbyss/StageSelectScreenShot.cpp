#include "StageSelectScreenShot.h"
#include"TexHandleMgr.h"
#include"../Engine/DrawFunc.h"

StageSelectScreenShot::StageSelectScreenShot()
{
	//screenShotHandle[0] = TexHandleMgr::LoadGraph("resource/ChainCombat/select_scene/stage_screen_shot/0.png");
	//screenShotHandle[1] = TexHandleMgr::LoadGraph("resource/ChainCombat/select_scene/stage_screen_shot/1.png");
	//screenShotHandle[2] = TexHandleMgr::LoadGraph("resource/ChainCombat/select_scene/stage_screen_shot/2.png");
	//selectNum = 0;
}

void StageSelectScreenShot::Init()
{
	//screenShotLerpData.Init(Vec2<float>(640.0f, 360.0f), Vec2<float>(1.12f, 1.0f));
}

void StageSelectScreenShot::Update()
{
	/*if (zoomOutFlag)
	{
		screenShotLerpData.lerpPos = { 640.0f,144.0f };
		screenShotLerpData.lerpSize = { 0.4f,0.4f };
	}
	else
	{
		screenShotLerpData.lerpPos = { 640.0f,360.0f };
		screenShotLerpData.lerpSize = { 1.12f,1.0f };
	}
	screenShotLerpData.Lerp();*/
}

void StageSelectScreenShot::Draw()
{
	//DrawFunc::DrawRotaGraph2D(screenShotLerpData.pos, screenShotLerpData.size, 0.0f, TexHandleMgr::GetTexBuffer(screenShotHandle[selectNum]));
}

void StageSelectScreenShot::Next()
{
//	++selectNum;
}

void StageSelectScreenShot::Prev()
{
	//--selectNum;
}

void StageSelectScreenShot::ImGuiDraw()
{
	/*ImGui::Begin("StageSelectImage");
	ImGui::InputFloat("PosX", &screenShotLerpData.pos.x);
	ImGui::InputFloat("PosY", &screenShotLerpData.pos.y);
	ImGui::InputFloat("SizeX", &screenShotLerpData.size.x);
	ImGui::InputFloat("SizeY", &screenShotLerpData.size.y);
	ImGui::Checkbox("ZoomOut", &zoomOutFlag);
	ImGui::End();*/
}
