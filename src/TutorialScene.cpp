#include "TutorialScene.h"
#include"IntoTheAbyss/TexHandleMgr.h"
#include"KuroFunc.h"
#include<sys/stat.h>
#include"IntoTheAbyss/CharacterManager.h"
#include"IntoTheAbyss/Tutorial.h"

int TutorialScene::LoadPngFile(const std::string& Dir, const int& Num)
{
	const std::string path = Dir + std::to_string(Num) + ".png";

	//�t�@�C�������݂��Ȃ�
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

		//�f�B���N�g�������݂��Ȃ�
		if (!ExistDir(dir))break;

		//�摜�̓ǂݍ���
		std::vector<int>picHandles;
		while (1)
		{
			const int picIdx = picHandles.size();

			//PNG�t�@�C���ǂݍ���
			int png = LoadPngFile(dir, picIdx);

			//PNG�t�@�C�������݂��Ȃ�
			if (!png)break;

			picHandles.emplace_back(png);
		}

		//���͉摜�ǂݍ���
		std::vector<int>strHandles;
		while (1)
		{
			const int strIdx = strHandles.size();

			//PNG�t�@�C���ǂݍ���
			int png = LoadPngFile(dir + "str/", strIdx);

			//PNG�t�@�C�������݂��Ȃ�
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
