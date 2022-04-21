#include "TutorialScene.h"
#include"IntoTheAbyss/TexHandleMgr.h"

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
	data.push_back(PictureStoryHandleData(handle, handle2));
	data.push_back(PictureStoryHandleData(handle, handle2));
	pictureStory.Init(data);
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
}

void TutorialScene::OnDraw()
{
	KuroEngine::Instance().Graphics().SetRenderTargets({ D3D12App::Instance()->GetBackBuffRenderTarget() });
	KuroEngine::Instance().Graphics().SetRenderTargets({ D3D12App::Instance()->GetBackBuffRenderTarget() });

	pictureStory.Draw();
}

void TutorialScene::OnImguiDebug()
{
}

void TutorialScene::OnFinalize()
{
}
