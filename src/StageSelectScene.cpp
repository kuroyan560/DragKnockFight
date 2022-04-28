#include "StageSelectScene.h"
#include"IntoTheAbyss/SelectStage.h"
#include"IntoTheAbyss/StageMgr.h"
#include"IntoTheAbyss/CharacterManager.h"
#include"Vec.h"

StageSelectScene::StageSelectScene()
{
	changeScene = std::make_shared<SceneCange>();
	stageNum = 0;
}

void StageSelectScene::OnInitialize()
{
	charactersSelect = false;
	CharacterManager::Instance()->CharactersSelectInit();
	isSelectMultiSingle = true;
	isMulti = false;
	isSingle = true;	// マルチ、シングル選択画面において最初にシングルプレイの画像を表示するためにtrueにする。
}

void StageSelectScene::OnUpdate()
{
	if (isSkip)
	{
		KuroEngine::Instance().ChangeScene(2, changeScene);
		SelectStage::Instance()->resetStageFlag = true;
	}


	// マルチ、シングル選択画面を表示する。
	if (isSelectMultiSingle) {

		// 左スティックの入力を保存する。
		Vec2<float> inputLeftVec = UsersInput::Instance()->GetLeftStickVecFuna(0);	// 1Pからの入力を受け付ける。
		inputLeftVec /= {32768.0f, 32768.0f};

		// 入力のデッドライン

		// 右側に入力されたら。
		if(0.5f < inputLeftVec.x)

	}
	// マルチ、シングル選択が画面終わり。フラグによって処理を変える。
	else {



	}



	//if (charactersSelect)
	//{
	//	//キャラクター選択更新
	//	CharacterManager::Instance()->CharactersSelectUpdate();

	//	//ゲームシーンに移動する
	//	if (UsersInput::Instance()->ControllerOnTrigger(0, XBOX_BUTTON::A))
	//	{
	//		KuroEngine::Instance().ChangeScene(2, changeScene);
	//		SelectStage::Instance()->resetStageFlag = true;
	//	}
	//	//ステージ選択へ戻る
	//	if (UsersInput::Instance()->ControllerOnTrigger(0, XBOX_BUTTON::B))
	//	{
	//		charactersSelect = false;
	//	}
	//}
	//else
	//{
	//	//キャラクター選択へ
	//	if (UsersInput::Instance()->ControllerOnTrigger(0, XBOX_BUTTON::A))
	//	{
	//		charactersSelect = true;
	//	}
	//	//タイトルシーンに移動する
	//	if (UsersInput::Instance()->ControllerOnTrigger(0, XBOX_BUTTON::B))
	//	{
	//		KuroEngine::Instance().ChangeScene(0, changeScene);
	//	}

	//	//ステージ番号を増やす
	//	if (UsersInput::Instance()->ControllerOnTrigger(0, XBOX_STICK::L_UP))
	//	{
	//		++stageNum;
	//	}
	//	//ステージ番号を減らす
	//	if (UsersInput::Instance()->ControllerOnTrigger(0, XBOX_STICK::L_DOWN))
	//	{
	//		--stageNum;
	//	}


	//	if (StageMgr::Instance()->GetMaxStageNumber() - 1 <= stageNum)
	//	{
	//		stageNum = StageMgr::Instance()->GetMaxStageNumber() - 1;
	//	}
	//	if (stageNum <= 0)
	//	{
	//		stageNum = 0;
	//	}
	//	SelectStage::Instance()->SelectStageNum(stageNum);
	//}
}

void StageSelectScene::OnDraw()
{
	KuroEngine::Instance().Graphics().SetRenderTargets({ D3D12App::Instance()->GetBackBuffRenderTarget() });
	KuroEngine::Instance().Graphics().SetRenderTargets({ D3D12App::Instance()->GetBackBuffRenderTarget() });
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
}

void StageSelectScene::OnFinalize()
{
}
