#include "TitleScene.h"
#include "StageMgr.h"
#include "DrawFunc.h"
#include "TexHandleMgr.h"
#include "SceneCange.h"
#include "KuroMath.h"
#include"DebugKeyManager.h"

TitleScene::TitleScene()
{
	changeScene = std::make_shared<CutInTransition>();
	StageMgr::Instance();

	// 各画像をロード
	frameHandle = TexHandleMgr::LoadGraph("resource/ChainCombat/title_scene/back.png");
	starHandle = TexHandleMgr::LoadGraph("resource/ChainCombat/title_scene/star.png");
	lunaHandle = TexHandleMgr::LoadGraph("resource/ChainCombat/title_scene/luna.png");
	lacyHandle = TexHandleMgr::LoadGraph("resource/ChainCombat/title_scene/lacy.png");
	lunaRobotHandle = TexHandleMgr::LoadGraph("resource/ChainCombat/title_scene/luna_robo.png");
	lacyRobotHandle = TexHandleMgr::LoadGraph("resource/ChainCombat/title_scene/lacy_robo.png");
	titleHandle = TexHandleMgr::LoadGraph("resource/ChainCombat/title_scene/title.png");
	pressStartHandle = TexHandleMgr::LoadGraph("resource/ChainCombat/title_scene/pressStart.png");

	easingTimer = 0;
	bool isUpper = true;

	isPressStartDraw = true;
	pressStartTimer = 0;

}

void TitleScene::OnInitialize()
{
	static bool FIRST = true;
	tutorialYes = FIRST;
	FIRST = false;
	tutorialQuestion = false;


	//handMgr->Init();
}

#include"CharacterManager.h"
void TitleScene::OnUpdate()
{
	static int SE = AudioApp::Instance()->LoadAudio("resource/ChainCombat/sound/select.wav", 0.13f);

	//PVEとPVP切り替え（β版プレイ会向け）
	if (UsersInput::Instance()->KeyInput(DIK_LSHIFT) && UsersInput::Instance()->KeyOnTrigger(DIK_T))
	{
		CharacterManager::Instance()->vsMode = !CharacterManager::Instance()->vsMode;
	}

	//チュートリアル選択画面
	if (tutorialQuestion)
	{
		//デバッグ時短用
		{
			//KuroEngine::Instance().ChangeScene(1, changeScene);
			//AudioApp::Instance()->PlayWave(SE);
		}

		if (UsersInput::Instance()->ControllerOnTrigger(0, XBOX_STICK::L_LEFT, 0.0f))
		{
			AudioApp::Instance()->PlayWave(SE);
			if (!tutorialSelect)tutorialSelect = true;
			else
			{
				tutorialYes = true;
			}
		}
		else if (UsersInput::Instance()->ControllerOnTrigger(0, XBOX_STICK::L_RIGHT, 0.0f))
		{
			AudioApp::Instance()->PlayWave(SE);
			if (!tutorialSelect)tutorialSelect = true;
			else
			{
				tutorialYes = false;
			}
		}

		if (tutorialSelect && UsersInput::Instance()->ControllerOnTrigger(0, XBOX_BUTTON::A))
		{
			//チュートリアルシーンへ
			if (tutorialYes)
			{
				AudioApp::Instance()->PlayWave(SE);
				KuroEngine::Instance().ChangeScene(4, changeScene);
			}
			//ゲームシーンへ
			else
			{
				KuroEngine::Instance().ChangeScene(1, changeScene);
				AudioApp::Instance()->PlayWave(SE);
			}
		}
	}
	//タイトル画面
	else
	{
		//ステージセレクトに移動する
		if (UsersInput::Instance()->ControllerOnTrigger(0, XBOX_BUTTON::A))
		{
			tutorialQuestion = true;
			tutorialSelect = false;
		}

		if (isUpper) {
			easingTimer += ADD_EASING_TIMER;
		}
		else {
			easingTimer -= ADD_EASING_TIMER;
		}
		if (1.0f < easingTimer) {
			easingTimer = 1.0f;
			isUpper = false;
		}
		if (easingTimer < 0) {
			easingTimer = 0.0f;
			isUpper = true;
		}

		++pressStartTimer;
		if (PERSSSTART_TIMER < pressStartTimer) {

			pressStartTimer = 0;
			isPressStartDraw = isPressStartDraw ? false : true;
		}
	}
}

void TitleScene::OnDraw()
{
	static const std::string DIR = "resource/ChainCombat/title_scene/";
	static const int QUESTION = TexHandleMgr::LoadGraph(DIR + "question.png");
	static const int YES_ON = TexHandleMgr::LoadGraph(DIR + "yes_on.png");
	static const int YES_OFF = TexHandleMgr::LoadGraph(DIR + "yes_off.png");
	static const int NO_ON = TexHandleMgr::LoadGraph(DIR + "no_on.png");
	static const int NO_OFF = TexHandleMgr::LoadGraph(DIR + "no_off.png");

	static const float QUES_OFFSET_Y = 100.0f;
	static const float YES_NO_OFFSET_X = 300.0f;

	KuroEngine::Instance().Graphics().SetRenderTargets({ D3D12App::Instance()->GetBackBuffRenderTarget() });

	if (tutorialQuestion)
	{
		const auto winCenter = WinApp::Instance()->GetExpandWinCenter();
		int yes = tutorialYes ? YES_ON : YES_OFF;
		int no = tutorialYes ? NO_OFF : NO_ON;
		if (!tutorialSelect)
		{
			yes = YES_OFF;
			no = NO_OFF;
		}

		DrawFunc::DrawRotaGraph2D({ winCenter.x,winCenter.y - QUES_OFFSET_Y }, { 1,1 }, 0.0f, TexHandleMgr::GetTexBuffer(QUESTION));
		DrawFunc::DrawRotaGraph2D({ winCenter.x - YES_NO_OFFSET_X,winCenter.y + QUES_OFFSET_Y }, { 1,1 }, 0.0f, TexHandleMgr::GetTexBuffer(yes));
		DrawFunc::DrawRotaGraph2D({ winCenter.x + YES_NO_OFFSET_X,winCenter.y + QUES_OFFSET_Y }, { 1,1 }, 0.0f, TexHandleMgr::GetTexBuffer(no));
	}
	else
	{
		// 枠を描画
		DrawFunc::DrawGraph(Vec2<float>(0, 0), TexHandleMgr::GetTexBuffer(frameHandle));

		float easingAmount = KuroMath::Ease(InOut, Sine, easingTimer, 0.0f, 1.0f);

		// 星を描画
		DrawFunc::DrawGraph(Vec2<float>(0, 30) + Vec2<float>(0, easingAmount * -EASING_MOVE_STAR), TexHandleMgr::GetTexBuffer(starHandle));

		// 背景キャラ二人を描画
		DrawFunc::DrawGraph(LACY_POS + Vec2<float>(0, easingAmount * EASING_MOVE_CHAR), TexHandleMgr::GetTexBuffer(lacyHandle));
		DrawFunc::DrawGraph(LUNA_POS + Vec2<float>(0, easingAmount * EASING_MOVE_CHAR), TexHandleMgr::GetTexBuffer(lunaHandle));

		// 背景キャラロボット二体を描画
		DrawFunc::DrawGraph(LACY_ROBOT_POS + Vec2<float>(0, easingAmount * EASING_MOVE_CHAR), TexHandleMgr::GetTexBuffer(lacyRobotHandle));
		DrawFunc::DrawGraph(LUNA_ROBOT_POS + Vec2<float>(0, easingAmount * EASING_MOVE_CHAR), TexHandleMgr::GetTexBuffer(lunaRobotHandle));

		// タイトルアイコン画像を描画
		DrawFunc::DrawGraph(TITLE_POS + Vec2<float>(0, easingAmount * -EASING_MOVE_TITLE), TexHandleMgr::GetTexBuffer(titleHandle));

		// PRESSENTERの画像を描画
		if (isPressStartDraw) {
			DrawFunc::DrawRotaGraph2D(PRESS_START_POS, { 1,1 }, 0, TexHandleMgr::GetTexBuffer(pressStartHandle));
		}
	}

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
