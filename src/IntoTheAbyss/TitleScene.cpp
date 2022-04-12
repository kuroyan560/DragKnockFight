#include "TitleScene.h"
#include "StageMgr.h"
#include "DrawFunc.h"
#include "TexHandleMgr.h"
#include "SceneCange.h"

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
	titleHandle = TexHandleMgr::LoadGraph("resource/ChainCombat/title_scene/star.png");
	pressStartHandle = TexHandleMgr::LoadGraph("resource/ChainCombat/title_scene/star.png");

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

	// 枠を描画
	DrawFunc::DrawGraph(Vec2<float>(0, 0), TexHandleMgr::GetTexBuffer(frameHandle), AlphaBlendMode_Trans);

	// 星を描画
	DrawFunc::DrawGraph(Vec2<float>(0, 0), TexHandleMgr::GetTexBuffer(starHandle), AlphaBlendMode_Trans);

	// 背景キャラ二人を描画
	DrawFunc::DrawGraph(LUNA_POS, TexHandleMgr::GetTexBuffer(lunaHandle), AlphaBlendMode_Trans);
	DrawFunc::DrawGraph(LACY_POS, TexHandleMgr::GetTexBuffer(lacyHandle), AlphaBlendMode_Trans);

	// 背景キャラロボット二体を描画
	DrawFunc::DrawGraph(LUNA_ROBOT_POS, TexHandleMgr::GetTexBuffer(lunaRobotHandle), AlphaBlendMode_Trans);
	DrawFunc::DrawGraph(LACY_ROBOT_POS, TexHandleMgr::GetTexBuffer(lacyRobotHandle), AlphaBlendMode_Trans);

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
