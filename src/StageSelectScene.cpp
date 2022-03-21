#include "StageSelectScene.h"
#include"IntoTheAbyss/SelectStage.h"
#include"IntoTheAbyss/StageMgr.h"

StageSelectScene::StageSelectScene()
{
	changeScene = new SceneCange();
	stageNum = 0;
}

void StageSelectScene::OnInitialize()
{
}

void StageSelectScene::OnUpdate()
{
	//ゲームシーンに移動する
	if (UsersInput::Instance()->OnTrigger(XBOX_BUTTON::A))
	{
		KuroEngine::Instance().ChangeScene(2, changeScene);
	}
	//タイトルシーンに移動する
	if (UsersInput::Instance()->OnTrigger(XBOX_BUTTON::B))
	{
		KuroEngine::Instance().ChangeScene(0, changeScene);
	}

	//ステージ番号を増やす
	if (UsersInput::Instance()->OnTrigger(XBOX_STICK::L_UP))
	{
		++stageNum;
	}
	//ステージ番号を減らす
	if (UsersInput::Instance()->OnTrigger(XBOX_STICK::L_DOWN))
	{
		--stageNum;
	}


	if (StageMgr::Instance()->GetMaxStageNumber() - 1 <= stageNum)
	{
		stageNum = StageMgr::Instance()->GetMaxStageNumber() - 1;
	}
	if (stageNum <= 0)
	{
		stageNum = 0;
	}
	SelectStage::Instance()->Select(stageNum);
}

void StageSelectScene::OnDraw()
{
	KuroEngine::Instance().Graphics().SetRenderTargets({ D3D12App::Instance()->GetBackBuffRenderTarget() });
	KuroEngine::Instance().Graphics().SetRenderTargets({ D3D12App::Instance()->GetBackBuffRenderTarget() });
}

void StageSelectScene::OnImguiDebug()
{
	ImGui::Begin("StageSelectScene");
	ImGui::Text("StageNumber:%d", stageNum);
	ImGui::Text("MaxStageNumber:%d", StageMgr::Instance()->GetMaxStageNumber() - 1);
	ImGui::Text("Stick Up:Add Number");
	ImGui::Text("Stick Down:Subtract a number");
	ImGui::Text("Bbutton:TitleScene");
	ImGui::Text("Abutton:Done");
	ImGui::End();
}

void StageSelectScene::OnFinalize()
{
}
