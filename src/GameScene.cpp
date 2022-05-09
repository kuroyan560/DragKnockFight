#include "GameScene.h"
#include"KuroEngine.h"
#include"Sprite.h"
#include"IntoTheAbyss/ResultTransfer.h"
#include"GaussianBlur.h"
#include"IntoTheAbyss/StageMgr.h"
#include"IntoTheAbyss/SelectStage.h"
#include"IntoTheAbyss/DebugParameter.h"
#include"IntoTheAbyss/SuperiorityGauge.h"
#include"IntoTheAbyss/GameTimer.h"
#include"IntoTheAbyss/ScoreManager.h"
#include"IntoTheAbyss/WinCounter.h"
#include"IntoTheAbyss/ResultTransfer.h"
#include"IntoTheAbyss/DebugKeyManager.h"
#include"IntoTheAbyss/DebugImGuiManager.h"

GameScene::GameScene()
{
	addValue = 10.0f;

	sceneChange = std::make_shared<SceneCange>();


	//imguiHandle = DebugImGuiManager::Instance()->Add("Round");
	//imguiRoundHandle = DebugImGuiManager::Instance()->DrawFlag();

	//DebugImGuiManager::Instance()->DrawImGui();

}

void GameScene::OnInitialize()
{
	ResultTransfer::Instance()->Init();
	isSS = false;
	game.Init();
}

void GameScene::OnUpdate()
{
	DebugParameter::Instance()->Update();
	game.Update();

	// リザルト画面へ飛ばす
	if (game.TurnResultScene()) {
		if (isSS)
		{
			KuroEngine::Instance().ChangeScene(3, sceneChange);
		}
		isSS = true;
	}

	bool changeInput = UsersInput::Instance()->KeyOnTrigger(DIK_B) || UsersInput::Instance()->ControllerOnTrigger(0, START);
	if (changeInput && false)
	{
		KuroEngine::Instance().ChangeScene(1, sceneChange);
	}
}

void GameScene::OnDraw()
{
	KuroEngine::Instance().Graphics().SetRenderTargets({ D3D12App::Instance()->GetBackBuffRenderTarget() });
	game.Draw();

	// スクショを保存。
	if (isSS) {
		KuroEngine::Instance().Graphics().CopyTexture(ResultTransfer::Instance()->backGround, D3D12App::Instance()->GetBackBuffRenderTarget());
	}

}

void GameScene::OnImguiDebug()
{
	//ImGui::Begin("StageInfo");
	//ImGui::Text("---Now---");
	//ImGui::Text("StageNumber:%d", SelectStage::Instance()->GetStageNum());
	//ImGui::Text("RoomNumber:%d\n\n", SelectStage::Instance()->GetRoomNum());
	//ImGui::Text("---Select---");

	//ImGui::Text("StageNumber:%d", game.debugStageData[0]);
	////選択できないステージ番号なら表示する
	//if (!StageMgr::Instance()->CheckStageNum(game.debugStageData[0]))
	//{
	//	ImGui::SameLine();
	//	ImGui::Text("Don't use");
	//}
	//if (game.nowSelectNum == 0)
	//{
	//	ImGui::SameLine();
	//	ImGui::Text("NowSelecting");
	//}


	//ImGui::Text("RoomNumber:%d", game.debugStageData[1]);
	////選択できない部屋番号なら表示する
	//if (!StageMgr::Instance()->CheckRoomNum(game.debugStageData[0], game.debugStageData[1]))
	//{
	//	ImGui::SameLine();
	//	ImGui::Text("Don't use");
	//}
	//if (game.nowSelectNum == 1)
	//{
	//	ImGui::SameLine();
	//	ImGui::Text("NowSelecting");
	//}

	//ImGui::Text("Up Down:SelectNumber");
	//ImGui::Text("Left Right:SelectStageOrRoom");
	//ImGui::Text("B Key / Home Button :StageSelectScene");
	//ImGui::Text("Return / A Button:Done");
	//ImGui::Text("Space / Back Button:Reset");
	//ImGui::End();


	/*ImGui::Begin("Timer");
	ImGui::Text("GravityMode");
	if (game.player.isZeroGravity)
	{
		ImGui::SameLine();
		ImGui::Text("  Now!!!");
	}
	ImGui::Text("NoGravityMode");
	if (!game.player.isZeroGravity)
	{
		ImGui::SameLine();
		ImGui::Text("Now!!!");
	}
	ImGui::Text("MaxTimer%d", game.player.CHANGE_GRAVITY_TIMER);
	ImGui::Text("NowTimer%d", game.player.changeGravityTimer);
	ImGui::End();*/
	game.navi.ImGuiDraw();

	DebugImGuiManager::Instance()->DrawImGui();

	SuperiorityGauge::Instance()->DebugValue(&addValue);

	//game.playerHomeBase->Debug();
	//game.enemyHomeBase->Debug();

	ImGui::Begin("Round");
	ImGui::Text("RoundNum%d", WinCounter::Instance()->GetNowRound());
	ImGui::Text("PlayerWin:%d,EnemyWin:%d", WinCounter::Instance()->GetLeftWin(), WinCounter::Instance()->GetRightWin());
	ImGui::End();

	DebugParameter::Instance()->DrawImGui();

	DebugKeyManager::Instance()->DrawImGui();


	//GameTimer::Instance()->Debug();
	//ScoreManager::Instance()->Debug();
}

void GameScene::OnFinalize()
{
}
