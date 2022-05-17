#include "StageSelectImage.h"
#include"../Engine/DrawFunc.h"
#include"../IntoTheAbyss/TexHandleMgr.h"

StageSelectImage::StageSelectImage()
{
	backGroundHandle = TexHandleMgr::LoadGraph("resource/ChainCombat/select_scene/frame.png");

	pos = { 1280.0f / 2.0f,720.0f / 2.0f };
	lerpPos = { 640,900 };
	pos = { 640,900 };
	lerpSize = { 2.8f,2.5f };
	size = { 2.8f,2.5f };
	zoomOutlag = false;
}

void StageSelectImage::Init()
{
}

void StageSelectImage::Update()
{
	if (zoomOutlag)
	{
		lerpPos = { 1280.0f / 2.0f,720.0f / 2.0f };
		lerpSize = { 1.0f,1.0f };
	}
	else
	{
		lerpPos = { 640,900 };
		lerpSize = { 2.8f,2.5f };
	}

	Vec2<float>distance = lerpPos - pos;
	pos += distance * 0.1f;
	Vec2<float>distance2 = lerpSize - size;
	size += distance2 * 0.1f;
}

void StageSelectImage::Draw()
{
	DrawFunc::DrawRotaGraph2D(pos, size, 0.0f, TexHandleMgr::GetTexBuffer(backGroundHandle));
}

void StageSelectImage::ImGuiDraw()
{
	ImGui::Begin("StageSelectImage");
	ImGui::InputFloat("PosX", &pos.x);
	ImGui::InputFloat("PosY", &pos.y);
	ImGui::InputFloat("SizeX", &size.x);
	ImGui::InputFloat("SizeY", &size.y);
	ImGui::Checkbox("ZoomOut", &zoomOutlag);
	ImGui::End();
}
