#include "GameScene.h"
#include"KuroEngine.h"
#include"Sprite.h"
#include"Sprite_Shadow.h"
#include"GaussianBlur.h"
#include"IntoTheAbyss/StageMgr.h"

GameScene::GameScene()
{
	static const float BACK_GROUND_DEPTH = 7.0f;
	auto backColor = D3D12App::Instance()->GenerateTextureBuffer(Color(8, 27, 81, 255));
	backGround = std::make_shared<Sprite_Shadow>(backColor, nullptr, nullptr, "BackGround");
	backGround->SetDepth(BACK_GROUND_DEPTH);
	backGround->SetSpecularAffect(0.0f);
	backGround->SetLimAffect(0.0f);
	backGround->mesh.SetZLayer(BACK_GROUND_DEPTH);
	backGround->mesh.SetSize(WinApp::Instance()->GetExpandWinSize());

	gaussianBlur = std::make_shared<GaussianBlur>(WinApp::Instance()->GetWinSize() , DXGI_FORMAT_R32G32B32A32_FLOAT);

	emissiveMap = D3D12App::Instance()->GenerateRenderTarget(DXGI_FORMAT_R32G32B32A32_FLOAT, Color(0.0f, 0.0f, 0.0f, 1.0f),
		WinApp::Instance()->GetWinSize(), L"EmissiveMap");
}

void GameScene::OnInitialize()
{
}

void GameScene::OnUpdate()
{
	game.Update();
}

void GameScene::OnDraw()
{
	KuroEngine::Instance().Graphics().SetRenderTargets({ D3D12App::Instance()->GetBackBuffRenderTarget(),emissiveMap });
	backGround->Draw(game.GetLigManager());
	game.Draw();

	gaussianBlur->Register(emissiveMap);
	gaussianBlur->DrawResult(AlphaBlendMode_Add);
}

void GameScene::OnImguiDebug()
{
	ImGui::Begin("StageInfo");
	ImGui::Text("---Now---");
	ImGui::Text("StageNumber:%d",game.stageNum);
	ImGui::Text("RoomNumber:%d\n\n",game.roomNum);
	ImGui::Text("---Select---");

	ImGui::Text("StageNumber:%d", game.debugStageData[0]);
	//選択できないステージ番号なら表示する
	if (!StageMgr::Instance()->CheckStageNum(game.debugStageData[0]))
	{
		ImGui::SameLine();
		ImGui::Text("Don't use");
	}

	ImGui::Text("RoomNumber:%d", game.debugStageData[1]);
	//選択できない部屋番号なら表示する
	if (!StageMgr::Instance()->CheckRoomNum(game.debugStageData[0],game.debugStageData[1]))
	{
		ImGui::SameLine();
		ImGui::Text("Don't use");
	}

	ImGui::Text("Up Down:SelectNumber");
	ImGui::Text("Left Right:SelectStageOrRoom");
	ImGui::Text("Return:Done");
	ImGui::End();
}

void GameScene::OnFinalize()
{
}
