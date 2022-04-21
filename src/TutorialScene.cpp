#include "TutorialScene.h"
#include"IntoTheAbyss/TexHandleMgr.h"
#include"../src/Engine/DrawFunc.h"

TutorialScene::TutorialScene()
{
	changeScene = std::make_shared<SceneCange>();

	std::vector<int>handle;
	std::vector<int>handle2;

	handle.push_back(TexHandleMgr::LoadGraph("resource/ChainCombat/tutorial/0/0.png"));
	handle.push_back(TexHandleMgr::LoadGraph("resource/ChainCombat/tutorial/0/1.png"));
	handle.push_back(TexHandleMgr::LoadGraph("resource/ChainCombat/tutorial/0/2.png"));
	handle2.push_back(TexHandleMgr::LoadGraph("resource/ChainCombat/tutorial/0/str/0.png"));
	handle2.push_back(TexHandleMgr::LoadGraph("resource/ChainCombat/tutorial/0/str/1.png"));

	std::vector<PictureStoryHandleData> data;
	//1シーン目の画像追加
	data.push_back(PictureStoryHandleData(handle, handle2));
	//2シーン目の画像追加(テスト)
	//data.push_back(PictureStoryHandleData(handle, handle2));
	pictureStory.Init(data);

	aButtonHandle = TexHandleMgr::LoadGraph("resource/ChainCombat/star.png");
	pos.x = static_cast<float>(WinApp::Instance()->GetWinSize().x - 70);
	pos.y = static_cast<float>(WinApp::Instance()->GetWinSize().y - 70);
	drawButtonFlag = true;
	flashTimer = 0;
}

void TutorialScene::OnInitialize()
{
	pictureStory.InitScene();
}

void TutorialScene::OnUpdate()
{
	pictureStory.Update();

	if (pictureStory.goToTitleSceneFlag)
	{
		KuroEngine::Instance().ChangeScene(0, changeScene);
	}

	++flashTimer;
	if (30 <= flashTimer)
	{
		drawButtonFlag = !drawButtonFlag;
		flashTimer = 0;
	}
}

void TutorialScene::OnDraw()
{
	KuroEngine::Instance().Graphics().SetRenderTargets({ D3D12App::Instance()->GetBackBuffRenderTarget() });
	KuroEngine::Instance().Graphics().SetRenderTargets({ D3D12App::Instance()->GetBackBuffRenderTarget() });

	pictureStory.Draw();

	if (drawButtonFlag)
	{
		DrawFunc::DrawRotaGraph2D(pos, Vec2<float>(1.0f, 1.0f), 0.0f, TexHandleMgr::GetTexBuffer(aButtonHandle));
	}
}

void TutorialScene::OnImguiDebug()
{
}

void TutorialScene::OnFinalize()
{
}
