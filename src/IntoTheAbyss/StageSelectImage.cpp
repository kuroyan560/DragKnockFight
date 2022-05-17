#include "StageSelectImage.h"
#include"../Engine/DrawFunc.h"
#include"../IntoTheAbyss/TexHandleMgr.h"

StageSelectImage::StageSelectImage()
{
	backGroundHandle = TexHandleMgr::LoadGraph("resource/ChainCombat/select_scene/frame.png");

	backGroundLerpData.Init(Vec2<float>(640.0f, 900.0f), Vec2<float>(2.8f, 2.5f));
	zoomOutlag = false;
}

void StageSelectImage::Init()
{
}

void StageSelectImage::Update()
{
	if (zoomOutlag)
	{
		backGroundLerpData.lerpPos = { 1280.0f / 2.0f,720.0f / 2.0f };
		backGroundLerpData.lerpSize = { 1.0f,1.0f };
	}
	else
	{
		backGroundLerpData.lerpPos = { 640,900 };
		backGroundLerpData.lerpSize = { 2.8f,2.5f };
	}
	backGroundLerpData.Lerp();
}

void StageSelectImage::Draw()
{
	DrawFunc::DrawRotaGraph2D(backGroundLerpData.pos, backGroundLerpData.size, 0.0f, TexHandleMgr::GetTexBuffer(backGroundHandle));
}

void StageSelectImage::ImGuiDraw()
{
	ImGui::Begin("StageSelectImage2");
	ImGui::InputFloat("PosX", &backGroundLerpData.lerpPos.x);
	ImGui::InputFloat("PosY", &backGroundLerpData.lerpPos.y);
	ImGui::InputFloat("SizeX", &backGroundLerpData.size.x);
	ImGui::InputFloat("SizeY", &backGroundLerpData.size.y);
	ImGui::Checkbox("ZoomOut", &zoomOutlag);
	ImGui::End();
}
