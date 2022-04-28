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
	hitoriHandle = TexHandleMgr::LoadGraph("resource/ChainCombat/select_scene/players/singlePlay_str.png");
	futariHandle = TexHandleMgr::LoadGraph("resource/ChainCombat/select_scene/players/multiPlay_str.png");
	arrowHandle = TexHandleMgr::LoadGraph("resource/ChainCombat/select_scene/players/arrow.png");
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
	invisibleArrowTimer = 0;
	isInvisibleArrow = false;
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

		// マルチプレイを選択中だったら
		bool isFirstHalf = easingTimer < 0.5f && 0.0f < easingTimer;
		// 前半部分だったら
		if (isFirstHalf) {

			// マルチだったら
			if (isMulti) {

				// マルチの画像を表示
				DrawFunc::DrawGraph(multiTexturePos, TexHandleMgr::GetTexBuffer(multiHandle));
				// シングルの画像を表示
				DrawFunc::DrawGraph(singleTexturePos, TexHandleMgr::GetTexBuffer(singleHandle));

			}
			else {

				// シングルの画像を表示
				DrawFunc::DrawGraph(singleTexturePos, TexHandleMgr::GetTexBuffer(singleHandle));
				// マルチの画像を表示
				DrawFunc::DrawGraph(multiTexturePos, TexHandleMgr::GetTexBuffer(multiHandle));

			}

		}
		else if (isMulti) {

			// シングルの画像を表示
			DrawFunc::DrawGraph(singleTexturePos, TexHandleMgr::GetTexBuffer(singleHandle));
			// マルチの画像を表示
			DrawFunc::DrawGraph(multiTexturePos, TexHandleMgr::GetTexBuffer(multiHandle));

		}
		// シングルプレイを選択中だったら
		else if (isSingle) {

			// マルチの画像を表示
			DrawFunc::DrawGraph(multiTexturePos, TexHandleMgr::GetTexBuffer(multiHandle));
			// シングルの画像を表示
			DrawFunc::DrawGraph(singleTexturePos, TexHandleMgr::GetTexBuffer(singleHandle));

		}

		// 何人プレイかを描画
		if (isMulti) {

			// 二人プレイを描画
			DrawFunc::DrawGraph(PLAY_STR_POS, TexHandleMgr::GetTexBuffer(futariHandle));

		}
		else {

			// 一人プレイを描画
			DrawFunc::DrawGraph(PLAY_STR_POS, TexHandleMgr::GetTexBuffer(hitoriHandle));

		}

		// 矢を描画。
		if (!isInvisibleArrow) {

			const Vec2<float> LEFT_ARROW_POS = { 220,620 };
			const Vec2<float> RIGHT_ARROW_POS = { 1200, 620 };

			DrawFunc::DrawRotaGraph2D(LEFT_ARROW_POS, { 1,1 }, 3.14f, TexHandleMgr::GetTexBuffer(arrowHandle));
			DrawFunc::DrawRotaGraph2D(RIGHT_ARROW_POS, { 1,1 }, 0.0f, TexHandleMgr::GetTexBuffer(arrowHandle));

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

			if (0 < inputLeftVec.x) {

				// 右側に入力したら。
				multiTextureInterimPos = BEHIND_INTERIM_POS;
				multiTextureEndPos = CURRENT_DEF_POS;
				singleTextureInterimPos = CURRENT_INTERIM_POS;
				singleTextureEndPos = BEHIND_DEF_POS;

			}
			else {

				// 左側に入力したら。
				multiTextureInterimPos = CURRENT_INTERIM_POS;
				multiTextureEndPos = BEHIND_DEF_POS;
				singleTextureInterimPos = BEHIND_INTERIM_POS;
				singleTextureEndPos = CURRENT_DEF_POS;


			}

		}
		// 今のフラグがマルチプレイ描画中だったら。
		else {

			// シングルプレイにする。
			isMulti = false;
			isSingle = true;

			if (0 < inputLeftVec.x) {

				// 右側に入力したら。
				multiTextureInterimPos = CURRENT_INTERIM_POS;
				multiTextureEndPos = BEHIND_DEF_POS;
				singleTextureInterimPos = BEHIND_INTERIM_POS;
				singleTextureEndPos = CURRENT_DEF_POS;

			}
			else {

				// 左側に入力したら。
				multiTextureInterimPos = BEHIND_INTERIM_POS;
				multiTextureEndPos = BEHIND_DEF_POS;
				singleTextureInterimPos = CURRENT_INTERIM_POS;
				singleTextureEndPos = CURRENT_DEF_POS;


			}

		}

		// イージングタイマーを進める。こうすることで自動的にイージングが始まる。
		easingTimer += ADD_EASING_TIMER;

		// 矢印を一旦消す。
		isInvisibleArrow = true;
		invisibleArrowTimer = 0;

	}

	// イージングタイマーが起動していたらイージングを進める。
	if (easingTimer != 0) {

		// 画像を更新。
		UpdateEasingMultiSingle(easingTimer, singleTexturePos, multiTexturePos,
			CURRENT_DEF_POS, singleTextureInterimPos, singleTextureEndPos,
			BEHIND_DEF_POS, multiTextureInterimPos, multiTextureEndPos);

	}

	// イージングタイマーを更新。
	easingTimer += ADD_EASING_TIMER;
	if (1.0f <= easingTimer) {

		easingTimer = 0;

	}

	// 矢不可視タイマーを更新。
	++invisibleArrowTimer;
	if (INVISIBLE_ARROW_TIMER <= invisibleArrowTimer) {

		isInvisibleArrow = isInvisibleArrow ? false : true;
		invisibleArrowTimer = 0;

	}

}

void StageSelectScene::UpdateEasingMultiSingle(const float& EasingRate, Vec2<float>& SinglePos, Vec2<float>& MultiPos, const Vec2<float>& SingleDefPos, const Vec2<float>& SingleIntermPos, const Vec2<float>& SingleEndPos, const Vec2<float>& MultiDefPos, const Vec2<float>& MultiIntermPos, const Vec2<float>& MultiEndPos)
{

	// イージングのタイマーの値によって今が前半か後半かを取得する。
	bool isFirstHalf = EasingRate < 0.5f;

	// 前半だったら。
	if (isFirstHalf) {

		float easingRate = easingTimer / 0.5f;

		float easingAmount = KuroMath::Ease(Out, Sine, easingRate, 0.0f, 1.0f);

		// シングルプレイを背面から正面に持ってくる。
		SinglePos = SingleDefPos + (SingleIntermPos - SingleDefPos) * easingAmount;

		// マルチプレイを全面から背面に持ってくる。
		MultiPos = MultiDefPos + (MultiIntermPos - MultiDefPos) * easingAmount;

	}
	// 後半だったら。
	else {

		float easingRate = (easingTimer - 0.5f) / 0.5f;

		float easingAmount = KuroMath::Ease(In, Sine, easingRate, 0.0f, 1.0f);

		// シングルプレイを背面から正面に持ってくる。
		SinglePos = SingleIntermPos + (SingleEndPos - SingleIntermPos) * easingAmount;

		// マルチプレイを全面から背面に持ってくる。
		MultiPos = MultiIntermPos + (MultiEndPos - MultiIntermPos) * easingAmount;

	}

}
