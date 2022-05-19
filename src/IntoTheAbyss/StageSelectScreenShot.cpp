#include "StageSelectScreenShot.h"
#include"TexHandleMgr.h"
#include"../Engine/DrawFunc.h"
#include"IntoTheAbyss/StageSelectOffsetPosDebug.h"

StageSelectScreenShot::StageSelectScreenShot()
{
	screenShotHandle[0] = TexHandleMgr::LoadGraph("resource/ChainCombat/select_scene/stage_screen_shot/0.png");
	screenShotHandle[1] = TexHandleMgr::LoadGraph("resource/ChainCombat/select_scene/stage_screen_shot/1.png");
	screenShotHandle[2] = TexHandleMgr::LoadGraph("resource/ChainCombat/select_scene/stage_screen_shot/2.png");
	stageNumberHandle[0] = TexHandleMgr::LoadGraph("resource/ChainCombat/select_scene/stage_tag/0.png");
	stageNumberHandle[1] = TexHandleMgr::LoadGraph("resource/ChainCombat/select_scene/stage_tag/1.png");
	stageNumberHandle[2] = TexHandleMgr::LoadGraph("resource/ChainCombat/select_scene/stage_tag/2.png");
	selectNum = 0;
}

void StageSelectScreenShot::Init()
{
	screenShotLerpData.Init(Vec2<float>(640.0f, 360.0f), Vec2<float>(1.12f, 1.0f), Vec2<float>(0.0f, 0.0f), Vec2<float>(0.0f, 0.0f));
	screenShotLerpData.timer = 1.0f;
	stageNumberData.Init(Vec2<float>(static_cast<float>(WinApp::Instance()->GetWinCenter().x), 610.0f), Vec2<float>(1.0f, 1.0f), Vec2<float>(0.0f, 0.0f), Vec2<float>(0.0f, 0.0f));
	stageNumberData.timer = 1.0f;
	expData.Init(Vec2<float>(0, 0), Vec2<float>(0, 0), Vec2<float>(0.0f, 0.0f), Vec2<float>(0.0f, 0.0f));
	stageNumberExpData.Init(Vec2<float>(0, 0), Vec2<float>(0, 0), Vec2<float>(0.0f, 0.0f), Vec2<float>(0.0f, 0.0f));
	zoomOutFlag = false;
	timer = 0;
}

void StageSelectScreenShot::Update()
{
	if (zoomOutFlag)
	{

		screenShotLerpData.lerpPos = { 640.0f,144.0f };
		screenShotLerpData.lerpSize = { 0.4f,0.4f };

		screenShotLerpData.startPos = { 640.0f,360.0f };
		screenShotLerpData.startSize = { 1.12f,1.0f };

		// ズームアウトしていたらステージ数の場所を下の方に吹っ飛ばす。
		stageNumberData.lerpPos = Vec2<float>(stageNumberData.lerpPos.x, 2000);
		stageNumberData.lerpSize = Vec2<float>(1.0f, 1.0f);
		stageNumberData.startPos = Vec2<float>(stageNumberData.lerpPos.x, 610.0f);
		stageNumberData.startSize = Vec2<float>(1.0f, 1.0f);

	}
	else
	{

		screenShotLerpData.lerpPos = { 640.0f,360.0f };
		screenShotLerpData.lerpSize = { 1.12f,1.0f };

		screenShotLerpData.startPos = { 640.0f,144.0f };
		screenShotLerpData.startSize = { 0.4f,0.4f };

		// ズームインしていたらステージ数の場所を元の位置に戻す。
		stageNumberData.startPos = Vec2<float>(stageNumberData.lerpPos.x, 2000);
		stageNumberData.startSize = Vec2<float>(1.0f, 1.0f);
		stageNumberData.lerpPos = Vec2<float>(stageNumberData.lerpPos.x, 610.0f);
		stageNumberData.lerpSize = Vec2<float>(1.0f, 1.0f);

	}
	screenShotLerpData.EaseInOut();

	// 拡縮のための大きさを0に戻す。
	expData.lerpPos = Vec2<float>(0, 0);
	expData.lerpSize = Vec2<float>(0, 0);
	expData.Lerp();
	stageNumberExpData.lerpPos = Vec2<float>(0, 0);
	stageNumberExpData.lerpSize = Vec2<float>(0, 0);
	stageNumberExpData.Lerp();
	stageNumberData.EaseInOut();

	// リサージュ曲線に使用するタイマーを更新。
	timer += 0.01f;
	//if (1.0f < timer) timer = 0;
}

void StageSelectScreenShot::Draw()
{
	// 描画するリサージュ曲線のいちを求める。
	float lissajousMove = 10.0f;
	Vec2<float> lissajousCurve = Vec2<float>(cosf(1.0f * timer) * lissajousMove, sinf(2.0f * timer) * lissajousMove);


	Vec2<float> debugPos = StageSelectOffsetPosDebug::Instance()->pos;
	DrawFunc::DrawRotaGraph2D(screenShotLerpData.pos + expData.pos + debugPos, screenShotLerpData.size + expData.size, 0.0f, TexHandleMgr::GetTexBuffer(screenShotHandle[selectNum]));
	DrawFunc::DrawRotaGraph2D(stageNumberData.pos + stageNumberExpData.pos + debugPos + lissajousCurve, stageNumberData.size + stageNumberExpData.size, 0.0f, TexHandleMgr::GetTexBuffer(stageNumberHandle[selectNum]));
}

void StageSelectScreenShot::Next()
{
	++selectNum;

	// ステージの番号のUIを縮小させる。
	stageNumberExpData.size = Vec2<float>(-1.0f, -1.0f);

	// 最大値に達したら最初のハンドルに戻す。
	if (screenShotHandle.size() - 1 < selectNum) selectNum = 0;

}

void StageSelectScreenShot::Prev()
{
	--selectNum;

	// ステージの番号のUIを縮小させる。
	stageNumberExpData.size = Vec2<float>(-1.0f, -1.0f);

	// 最小値に達したら最初のハンドルに戻す。
	if (selectNum < 0) selectNum = screenShotHandle.size() - 1;
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

void StageSelectScreenShot::SetZoomFlag(const bool& Zoom)
{

	zoomOutFlag = Zoom;

	screenShotLerpData.timer = 0.0f;
	stageNumberData.timer = 0.0f;

}
