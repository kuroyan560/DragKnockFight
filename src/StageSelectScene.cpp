#include "StageSelectScene.h"
#include"IntoTheAbyss/SelectStage.h"
#include"IntoTheAbyss/StageMgr.h"
#include"IntoTheAbyss/CharacterManager.h"
#include"Vec.h"
#include"KuroMath.h"
#include"DrawFunc_Color.h"
#include"DrawFunc.h"

StageSelectScene::StageSelectScene()
{
	changeScene = std::make_shared<SceneCange>();
	stageNum = 0;

	multiHandle = TexHandleMgr::LoadGraph("resource/ChainCombat/select_scene/players/multiPlay.png");
	singleHandle = TexHandleMgr::LoadGraph("resource/ChainCombat/select_scene/players/singlePlay.png");
}

void StageSelectScene::OnInitialize()
{
	charactersSelect = false;
	CharacterManager::Instance()->CharactersSelectInit();
	isSelectMultiSingle = true;
	isMulti = false;
	isSingle = true;	// マルチ、シングル選択画面において最初にシングルプレイの画像を表示するためにtrueにする。
	easingTimer = 0;
	singleTexturePos = CURRENT_DEF_POS;
	multiTexturePos = BEHIND_DEF_POS;
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

		// マルチ、シングルの選択画面での更新処理を行う。
		SelectMultiSingleUpdate();

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

	// マルチシングル選択画面だったら
	if (isSelectMultiSingle) {

		const Vec2<float> PLAY_STR_POS = { 1700,700 };							// 何人プレイかを描画する座標。

			// マルチプレイを選択中だったら
		if (isMulti) {

			// シングルの画像を表示
			DrawFunc::DrawGraph(singleTexturePos, TexHandleMgr::GetTexBuffer(singleHandle));
			// マルチの画像を表示
			DrawFunc::DrawGraph(multiTexturePos, TexHandleMgr::GetTexBuffer(multiHandle));

		}
		else if (isSingle) {

			// マルチの画像を表示
			DrawFunc::DrawGraph(multiTexturePos, TexHandleMgr::GetTexBuffer(multiHandle));
			// シングルの画像を表示
			DrawFunc::DrawGraph(singleTexturePos, TexHandleMgr::GetTexBuffer(singleHandle));

		}



	}
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

void StageSelectScene::SelectMultiSingleUpdate()
{

	/*===== マルチ、シングル選択画面での更新処理 =====*/


	// 左スティックの入力を保存する。
	Vec2<float> inputLeftVec = UsersInput::Instance()->GetLeftStickVecFuna(0);	// 1Pからの入力を受け付ける。
	inputLeftVec /= {32768.0f, 32768.0f};

	// どちらかに入力されたら。
	bool isEndEasing = easingTimer == 0;	// イージングが終わっているか。
	if (0.5f < fabs(inputLeftVec.x) && isEndEasing) {

		// 今のフラグが一人プレイ描画中だったら。
		if (isSingle) {

			// マルチプレイにする。
			isMulti = true;
			isSingle = false;

		}
		// 今のフラグがマルチプレイ描画中だったら。
		else {

			// シングルプレイにする。
			isMulti = false;
			isSingle = true;

		}

		// イージングタイマーを進める。こうすることで自動的にイージングが始まる。
		easingTimer += ADD_EASING_TIMER;

	}

	// イージングタイマーが起動していたらイージングを進める。
	if (easingTimer != 0) {

		// 選択中だったのがシングルプレイだったら。
		if (isSingle) {

			// イージングのタイマーの値によって今が前半か後半かを取得する。
			bool isFirstHalf = easingTimer < 0.5f;

			// 前半だったら。
			if (isFirstHalf) {

				float easingRate = easingTimer / 0.5f;

				float easingAmount = KuroMath::Ease(Out, Sine, easingRate, 0.0f, 1.0f);

				// シングルプレイを背面から正面に持ってくる。
				singleTexturePos = BEHIND_DEF_POS + (CURRENT_INTERIM_POS - BEHIND_DEF_POS) * easingAmount;

				// マルチプレイを全面から背面に持ってくる。
				multiTexturePos = CURRENT_DEF_POS + (BEHIND_INTERIM_POS - CURRENT_DEF_POS) * easingAmount;

			}
			// 後半だったら。
			else {

				float easingRate = (easingTimer - 0.5f) / 0.5f;

				float easingAmount = KuroMath::Ease(In, Sine, easingRate, 0.0f, 1.0f);

				// シングルプレイを背面から正面に持ってくる。
				singleTexturePos = CURRENT_INTERIM_POS + (CURRENT_DEF_POS - CURRENT_INTERIM_POS) * easingAmount;

				// マルチプレイを全面から背面に持ってくる。
				multiTexturePos = BEHIND_INTERIM_POS + (BEHIND_DEF_POS - BEHIND_INTERIM_POS) * easingAmount;

			}

		}
		// 選択中だったのがマルチプレイだったら。
		else {

			// イージングのタイマーの値によって今が前半か後半かを取得する。
			bool isFirstHalf = easingTimer < 0.5f;

			// 前半だったら。
			if (isFirstHalf) {

				float easingRate = easingTimer / 0.5f;

				float easingAmount = KuroMath::Ease(Out, Sine, easingRate, 0.0f, 1.0f);

				// シングルプレイを前面から背面に持っていく。
				singleTexturePos = CURRENT_DEF_POS + (BEHIND_INTERIM_POS - CURRENT_DEF_POS) * easingAmount;

				// マルチプレイを背面から前面に持っていく。
				multiTexturePos = BEHIND_DEF_POS + (CURRENT_INTERIM_POS - BEHIND_DEF_POS) * easingAmount;

			}
			// 後半だったら。
			else {

				float easingRate = (easingTimer - 0.5f) / 0.5f;

				float easingAmount = KuroMath::Ease(In, Sine, easingRate, 0.0f, 1.0f);

				// シングルプレイを前面から背面に持っていく。
				singleTexturePos = BEHIND_INTERIM_POS + (BEHIND_DEF_POS - BEHIND_INTERIM_POS) * easingAmount;

				// マルチプレイを全面から背面に持ってくる。
				multiTexturePos = CURRENT_INTERIM_POS + (CURRENT_DEF_POS - CURRENT_INTERIM_POS) * easingAmount;

			}

		}

		// イージングタイマーを更新。
		easingTimer += ADD_EASING_TIMER;
		if (1.0f <= easingTimer) {

			easingTimer = 0;

		}

	}

}
