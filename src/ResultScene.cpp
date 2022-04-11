#include "ResultScene.h"
#include "IntoTheAbyss/TexHandleMgr.h"
#include "DrawFunc.h"
#include "WinApp.h"

ResultScene::ResultScene()
{
}

void ResultScene::OnInitialize()
{
	backGroundHandle = TexHandleMgr::LoadGraph("resource/ChainCombat/title_scene/back.png");
}

void ResultScene::OnUpdate()
{
}

void ResultScene::OnDraw()
{
	KuroEngine::Instance().Graphics().SetRenderTargets({ D3D12App::Instance()->GetBackBuffRenderTarget() });

	Vec2<float> windowSize = { (float)WinApp::Instance()->GetWinSize().x, (float)WinApp::Instance()->GetWinSize().y };
	DrawFunc::DrawExtendGraph2D(Vec2<float>(0, 0), windowSize, TexHandleMgr::GetTexBuffer(backGroundHandle));

}

void ResultScene::OnImguiDebug()
{
}

void ResultScene::OnFinalize()
{
}
