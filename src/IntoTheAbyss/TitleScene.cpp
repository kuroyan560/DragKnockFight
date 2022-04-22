#include "TitleScene.h"
#include "StageMgr.h"
#include "DrawFunc.h"
#include "TexHandleMgr.h"
#include "SceneCange.h"
#include "KuroMath.h"
#include"DebugKeyManager.h"

TitleScene::TitleScene()
{
	changeScene = std::make_shared<SceneCange>();
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
}

void TitleScene::OnUpdate()
{
	static int SE = -1;
	if (SE == -1)
	{
		SE = AudioApp::Instance()->LoadAudio("resource/ChainCombat/sound/select.wav");
		AudioApp::Instance()->ChangeVolume(SE, 0.13f);
	}

	//ステージセレクトに移動する
	if (UsersInput::Instance()->ControllerOnTrigger(0, XBOX_BUTTON::A))
	{
		KuroEngine::Instance().ChangeScene(1, changeScene);
		AudioApp::Instance()->PlayWave(SE);
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


	if (UsersInput::Instance()->ControllerOnTrigger(0, B))
	{
		AudioApp::Instance()->PlayWave(SE);
		KuroEngine::Instance().ChangeScene(4, changeScene);
	}

}

void TitleScene::OnDraw()
{
	KuroEngine::Instance().Graphics().SetRenderTargets({ D3D12App::Instance()->GetBackBuffRenderTarget() });
	KuroEngine::Instance().Graphics().SetRenderTargets({ D3D12App::Instance()->GetBackBuffRenderTarget() });

	// 枠を描画
	DrawFunc::DrawGraph(Vec2<float>(0, 0), TexHandleMgr::GetTexBuffer(frameHandle), AlphaBlendMode_Trans);

	float easingAmount = KuroMath::Ease(InOut, Sine, easingTimer, 0.0f, 1.0f);

	// 星を描画
	DrawFunc::DrawGraph(Vec2<float>(0, 30) + Vec2<float>(0, easingAmount * -EASING_MOVE_STAR), TexHandleMgr::GetTexBuffer(starHandle), AlphaBlendMode_Trans);

	// 背景キャラ二人を描画
	DrawFunc::DrawGraph(LACY_POS + Vec2<float>(0, easingAmount * EASING_MOVE_CHAR), TexHandleMgr::GetTexBuffer(lacyHandle), AlphaBlendMode_Trans);
	DrawFunc::DrawGraph(LUNA_POS + Vec2<float>(0, easingAmount * EASING_MOVE_CHAR), TexHandleMgr::GetTexBuffer(lunaHandle), AlphaBlendMode_Trans);

	// 背景キャラロボット二体を描画
	DrawFunc::DrawGraph(LACY_ROBOT_POS + Vec2<float>(0, easingAmount * EASING_MOVE_CHAR), TexHandleMgr::GetTexBuffer(lacyRobotHandle), AlphaBlendMode_Trans);
	DrawFunc::DrawGraph(LUNA_ROBOT_POS + Vec2<float>(0, easingAmount * EASING_MOVE_CHAR), TexHandleMgr::GetTexBuffer(lunaRobotHandle), AlphaBlendMode_Trans);

	// タイトルアイコン画像を描画
	DrawFunc::DrawGraph(TITLE_POS + Vec2<float>(0, easingAmount * -EASING_MOVE_TITLE), TexHandleMgr::GetTexBuffer(titleHandle), AlphaBlendMode_Trans);

	// PRESSENTERの画像を描画
	if (isPressStartDraw) {
		DrawFunc::DrawGraph(PRESS_START_POS, TexHandleMgr::GetTexBuffer(pressStartHandle), AlphaBlendMode_Trans);
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
