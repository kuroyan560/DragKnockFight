#include "StageSelectImage.h"
#include"../Engine/DrawFunc.h"
#include"../IntoTheAbyss/TexHandleMgr.h"
#include"IntoTheAbyss/StageSelectOffsetPosDebug.h"

StageSelectImage::StageSelectImage()
{
	backGroundHandle = TexHandleMgr::LoadGraph("resource/ChainCombat/select_scene/frame.png");

	backGroundLerpData.Init(Vec2<float>(640.0f, 900.0f), Vec2<float>(2.8f, 2.5f));
	zoomOutFlag = false;
}

void StageSelectImage::Init()
{
	backGroundLerpData.Init(Vec2<float>(640, 900), Vec2<float>(2.8f, 2.5f));
	backGroundLerpData.lerpPos = { 640,900 };
	backGroundLerpData.lerpSize = { 2.8f,2.5f };
	expData.Init(Vec2<float>(0, 0), Vec2<float>(0, 0));
	zoomOutFlag = false;
}

void StageSelectImage::Update()
{

	if (zoomOutFlag)
	{
		backGroundLerpData.lerpPos = { 1280.0f / 2.0f,720.0f / 2.0f };
		backGroundLerpData.lerpSize = { 1.0f,1.0f };
	}
	else
	{
		backGroundLerpData.lerpPos = { 640,900 };
		backGroundLerpData.lerpSize = { 2.8f,2.5f };		// メモ：座標はY+40 スケールは+0.2f
	}
	backGroundLerpData.Lerp();

	// 拡縮のための大きさを0に戻す。
	expData.lerpPos = Vec2<float>(0, 0);
	expData.lerpSize = Vec2<float>(0, 0);
	expData.Lerp();
}

void StageSelectImage::Draw()
{
	Vec2<float> debugPos = StageSelectOffsetPosDebug::Instance()->pos;
	DrawFunc::DrawRotaGraph2D(backGroundLerpData.pos + expData.pos + debugPos, backGroundLerpData.size + expData.size, 0.0f, TexHandleMgr::GetTexBuffer(backGroundHandle));
}

void StageSelectImage::ImGuiDraw()
{
	ImGui::Begin("StageSelectImage2");
	ImGui::InputFloat("PosX", &backGroundLerpData.lerpPos.x);
	ImGui::InputFloat("PosY", &backGroundLerpData.lerpPos.y);
	ImGui::InputFloat("SizeX", &backGroundLerpData.size.x);
	ImGui::InputFloat("SizeY", &backGroundLerpData.size.y);
	ImGui::Checkbox("ZoomOut", &zoomOutFlag);
	ImGui::End();
}
