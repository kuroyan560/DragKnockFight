#include "TutorialScene.h"
#include"IntoTheAbyss/TexHandleMgr.h"
#include"KuroFunc.h"
#include<sys/stat.h>
#include"IntoTheAbyss/CharacterManager.h"
#include"IntoTheAbyss/Tutorial.h"

int TutorialScene::LoadPngFile(const std::string& Dir, const int& Num)
{
	const std::string path = Dir + std::to_string(Num) + ".png";

	//ファイルが存在しない
	if (!KuroFunc::ExistFile(path))return 0;

	return TexHandleMgr::LoadGraph(path);
}

bool TutorialScene::ExistDir(const std::string& Dir)
{
	static struct stat statBuff;
	return stat(Dir.c_str(), &statBuff) == 0;
}


TutorialScene::TutorialScene()
{
	changeScene = std::make_shared<SceneCange>();

	static const std::string DIR = "resource/ChainCombat/tutorial/";
	std::vector<PictureStoryHandleData> picStorys;

	while (1)
	{
		const int storyIdx = picStorys.size();
		const std::string dir = DIR + std::to_string(storyIdx) + "/";

		//ディレクトリが存在しない
		if (!ExistDir(dir))break;

		//画像の読み込み
		std::vector<int>picHandles;
		while (1)
		{
			const int picIdx = picHandles.size();

			//PNGファイル読み込み
			int png = LoadPngFile(dir, picIdx);

			//PNGファイルが存在しない
			if (!png)break;

			picHandles.emplace_back(png);
		}

		//文章画像読み込み
		std::vector<int>strHandles;
		while (1)
		{
			const int strIdx = strHandles.size();

			//PNGファイル読み込み
			int png = LoadPngFile(dir + "str/", strIdx);

			//PNGファイルが存在しない
			if (!png)break;

			strHandles.emplace_back(png);
		}

		picStorys.emplace_back(PictureStoryHandleData(picHandles, strHandles));
	}

	pictureStory.Init(picStorys);
}

void TutorialScene::OnInitialize()
{
	CharacterManager::Instance()->PracticeMode();
	pictureStory.InitScene();
	game.Init(true);
}

void TutorialScene::OnUpdate()
{
	game.Update(true);

	pictureStory.Update();

	if (pictureStory.goToTitleSceneFlag)
	{
		KuroEngine::Instance().ChangeScene(0, changeScene);
	}
}

void TutorialScene::OnDraw()
{
	KuroEngine::Instance().Graphics().SetRenderTargets({ D3D12App::Instance()->GetBackBuffRenderTarget() });
	game.Draw();
	pictureStory.Draw();
}

void TutorialScene::OnImguiDebug()
{
}

void TutorialScene::OnFinalize()
{
	Tutorial::SetStaticActiveAll(true);
}
