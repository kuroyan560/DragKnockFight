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

	auto img = D3D12App::Instance()->GenerateTextureBuffer("resource/player.png");

	test = std::make_shared<Sprite>(img, "test");
	test->mesh.SetSize({ 136.0f,144.0f });
	test->mesh.SetAnchorPoint({ 0.5f,0.5f });
	static const Vec2<float>OFFSET = { 128.0f,0.0f };
	test->transform.SetPos(WinApp::Instance()->GetExpandWinCenter() - OFFSET);

	auto normalMap = D3D12App::Instance()->GenerateTextureBuffer("resource/player_n.png");

	test_shadow = std::make_shared<Sprite_Shadow>(img, normalMap, nullptr, "test_shadow");
	test_shadow->mesh.SetSize({ 136.0f * 2.0f,144.0f * 2.0f });
	test_shadow->mesh.SetAnchorPoint({ 0.5f,0.5f });
	test_shadow->transform.SetPos(WinApp::Instance()->GetExpandWinCenter() + OFFSET);

	dirLig.SetDir({ 0.0f,0.0f,1.0f });
	dirLig.SetColor(Color(0.5f, 0.5f, 0.5f, 0.5f));

	static const float RANGE = 256.0f;
	ptLig.SetInfluenceRange(RANGE);

	//spotLig.SetInfluenceRange(RANGE);
	spotLig.SetPos({ WinApp::Instance()->GetExpandWinCenter().x,WinApp::Instance()->GetExpandWinCenter().y,-1.0f });
	spotLig.SetInfluenceRange(256.0f);
	//spotLig.SetAngle(Angle(10));

	//ligMgr.RegisterDirLight(&dirLig);
	ligMgr.RegisterPointLight(&ptLig);
	//ligMgr.RegisterSpotLight(&spotLig);
	ligMgr.RegisterHemiSphereLight(&hemiLig);

	auto lightRange = D3D12App::Instance()->GenerateTextureBuffer("resource/range.png");

	lightRangeSprite = std::make_shared<Sprite>(lightRange, "LightRange");
	lightRangeSprite->mesh.SetSize({ RANGE,RANGE });
	lightRangeSprite->mesh.SetAnchorPoint({ 0.5f,0.5f });


	auto splitImg = D3D12App::Instance()->GenerateTextureBuffer("resource/test.png", SPLIT_SPRITE_NUM, { SPLIT,SPLIT });
	for (int y = 0; y < SPLIT; ++y)
	{
		for (int x = 0; x < SPLIT; ++x)
		{
			int idx = y * SPLIT + x;
			splitSprites[idx] = std::make_shared<Sprite_Shadow>(splitImg[idx], nullptr, nullptr, ("SplitSprite - " + std::to_string(idx)).c_str());
			splitSprites[idx]->transform.SetPos({ x * 70.0f,y * 70.0f });
		}
	}

	//auto splitImg = D3D12App::Instance()->GenerateTextureBuffer("resource/IntoTheAbyss/chip_sheet.png", SPLIT_SPRITE_NUM, { SPLIT_X,SPLIT_Y });
	//for (int y = 0; y < SPLIT_Y; ++y)
	//{
	//	for (int x = 0; x < SPLIT_X; ++x)
	//	{
	//		int idx = y * SPLIT_X + x;
	//		splitSprites[idx] = std::make_shared<Sprite_Shadow>(splitImg[idx], nullptr, nullptr, ("SplitSprite - " + std::to_string(idx)).c_str());
	//		splitSprites[idx]->transform.SetPos({ x * 70.0f,y * 70.0f });
	//	}
	//}

	gaussianBlur = std::make_shared<GaussianBlur>(WinApp::Instance()->GetWinSize() , DXGI_FORMAT_R32G32B32A32_FLOAT);

	emissiveMap = D3D12App::Instance()->GenerateRenderTarget(DXGI_FORMAT_R32G32B32A32_FLOAT, Color(0.0f, 0.0f, 0.0f, 1.0f),
		WinApp::Instance()->GetWinSize(), L"EmissiveMap");
}

void GameScene::OnInitialize()
{
}

void GameScene::OnUpdate()
{
	Vec3<float>pos = { 0.0f,0.0f,-20.0f };
	pos.x = UsersInput::Instance()->GetMousePos().x;
	pos.y = UsersInput::Instance()->GetMousePos().y;

	lightRangeSprite->transform.SetPos({ pos.x,pos.y });

	ptLig.SetPos(pos);

	pos.z = 0.0f;
	spotLig.SetTarget(pos);
	//test_shadow->transform.SetPos({ pos.x,pos.y });

	game.Update();

}

void GameScene::OnDraw()
{
	KuroEngine::Instance().Graphics().SetRenderTargets({ D3D12App::Instance()->GetBackBuffRenderTarget(),emissiveMap });
	for (int i = 0; i < SPLIT_SPRITE_NUM; ++i)
	{
		splitSprites[i]->Draw(ligMgr);
	}
	backGround->Draw(ligMgr);
	test_shadow->Draw(ligMgr);

	gaussianBlur->Register(emissiveMap);
	gaussianBlur->DrawResult(AlphaBlendMode_Add);

	KuroEngine::Instance().Graphics().SetRenderTargets({ D3D12App::Instance()->GetBackBuffRenderTarget() });
	test->Draw();
	//lightRangeSprite->Draw();

	game.Draw();
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
