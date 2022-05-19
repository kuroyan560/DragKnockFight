#include "StageSelectScene.h"
#include"IntoTheAbyss/SelectStage.h"
#include"IntoTheAbyss/StageMgr.h"
#include"IntoTheAbyss/CharacterManager.h"

StageSelectScene::StageSelectScene()
{
	changeScene = std::make_shared<SceneCange>();
	stageNum = 0;
}

void StageSelectScene::OnInitialize()
{
	charactersSelect = false;
	CharacterManager::Instance()->CharactersSelectInit();
	stageSelect.Init();
	screenShot.Init();
	rightArrow.Init(Vec2<float>(1180.0f, static_cast<float>(WinApp::Instance()->GetWinCenter().y)), 0);
	leftArrow.Init(Vec2<float>(100.0f, static_cast<float>(WinApp::Instance()->GetWinCenter().y)), DirectX::XM_PI);
}

void StageSelectScene::OnUpdate()
{

	// 画面のズームアウトの判定をスクショのズームアウトの判定にも適応させる。
	screenShot.SetZoomFlag(stageSelect.GetZoomOutFlag());

	if (isSkip)
	{
		KuroEngine::Instance().ChangeScene(2, changeScene);
		SelectStage::Instance()->resetStageFlag = true;
	}

	if (charactersSelect)
	{
		//キャラクター選択更新
		CharacterManager::Instance()->CharactersSelectUpdate();

		//ゲームシーンに移動する
		if (UsersInput::Instance()->ControllerOnTrigger(0, XBOX_BUTTON::A))
		{
			//KuroEngine::Instance().ChangeScene(2, changeScene);
			//SelectStage::Instance()->resetStageFlag = true;
		}
		//ステージ選択へ戻る
		if (UsersInput::Instance()->ControllerOnTrigger(0, XBOX_BUTTON::B))
		{
			charactersSelect = false;
			screenShot.SetZoomFlag(false);
			stageSelect.SetZoomFlag(false);
			rightArrow.SetZoom(false);
			leftArrow.SetZoom(false);
		}
	}
	else
	{
		//キャラクター選択へ
		if (UsersInput::Instance()->ControllerOnTrigger(0, XBOX_BUTTON::A))
		{
			charactersSelect = true;
			screenShot.SetZoomFlag(true);		// ズームアウトさせる
			stageSelect.SetZoomFlag(true);		// ズームアウトさせる
			rightArrow.SetZoom(true);
			leftArrow.SetZoom(true);
		}
		//タイトルシーンに移動する
		if (UsersInput::Instance()->ControllerOnTrigger(0, XBOX_BUTTON::B))
		{
			KuroEngine::Instance().ChangeScene(0, changeScene);
		}

		//ステージ番号を増やす
		if (UsersInput::Instance()->ControllerOnTrigger(0, XBOX_BUTTON::RB))
		{
			++stageNum;
			screenShot.Next();
			stageSelect.SetExp(Vec2<float>(0, 40), Vec2<float>(0.2f, 0.2f));
			screenShot.SetExp(Vec2<float>(0, 0), Vec2<float>(0.1f, 0.1f));
		}
		//ステージ番号を減らす
		if (UsersInput::Instance()->ControllerOnTrigger(0, XBOX_BUTTON::LB))
		{
			--stageNum;
			screenShot.Prev();
			stageSelect.SetExp(Vec2<float>(0, 40), Vec2<float>(0.2f, 0.2f));
			screenShot.SetExp(Vec2<float>(0, 0), Vec2<float>(0.1f, 0.12f));
		}


		if (StageMgr::Instance()->GetMaxStageNumber() - 1 <= stageNum)
		{
			stageNum = StageMgr::Instance()->GetMaxStageNumber() - 1;
		}
		if (stageNum <= 0)
		{
			stageNum = 0;
		}
		SelectStage::Instance()->SelectStageNum(stageNum);
	}

	screenShot.Update();
	stageSelect.Update();
	rightArrow.Update();
	leftArrow.Update();
}

void StageSelectScene::OnDraw()
{
	KuroEngine::Instance().Graphics().SetRenderTargets({ D3D12App::Instance()->GetBackBuffRenderTarget() });
	KuroEngine::Instance().Graphics().SetRenderTargets({ D3D12App::Instance()->GetBackBuffRenderTarget() });
	stageSelect.Draw();
	screenShot.Draw();
	rightArrow.Draw();
	leftArrow.Draw();
}

void StageSelectScene::OnImguiDebug()
{
	if (charactersSelect)
	{
		CharacterManager::Instance()->CharactersSelectDraw();
	}
	else
	{
		ImGui::Begin("StageSelect");
		ImGui::Text("StageNumber:%d", stageNum);
		ImGui::Text("MaxStageNumber:%d", StageMgr::Instance()->GetMaxStageNumber() - 1);
		ImGui::Text("Stick Up:Add Number");
		ImGui::Text("Stick Down:Subtract a number");
		ImGui::Text("Bbutton:TitleScene");
		ImGui::Text("Abutton:Done");
		ImGui::End();
	}

	stageSelect.ImGuiDraw();
	//screenShot.ImGuiDraw();
}

void StageSelectScene::OnFinalize()
{
}
