#include "TitleScene.h"
#include"StageMgr.h"

TitleScene::TitleScene()
{
	changeScene = new SceneCange();
	StageMgr::Instance();
}

void TitleScene::OnInitialize()
{
}

void TitleScene::OnUpdate()
{
	//ステージセレクトに移動する
	if (UsersInput::Instance()->OnTrigger(XBOX_BUTTON::A))
	{
		KuroEngine::Instance().ChangeScene(1, changeScene);
	}
}

void TitleScene::OnDraw()
{
	KuroEngine::Instance().Graphics().SetRenderTargets({ D3D12App::Instance()->GetBackBuffRenderTarget() });
	KuroEngine::Instance().Graphics().SetRenderTargets({ D3D12App::Instance()->GetBackBuffRenderTarget() });
}

void TitleScene::OnImguiDebug()
{
	ImGui::Begin("TitleScene");
	ImGui::Text("Abutton:StageSelect");
	ImGui::End();
}

void TitleScene::OnFinalize()
{
}
