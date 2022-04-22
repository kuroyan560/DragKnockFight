#pragma once
#include"KuroEngine.h"
#include"../src/IntoTheAbyss/SceneCange.h"
#include"../src/IntoTheAbyss/StoryMgr.h"

class TutorialScene :public BaseScene
{
	int LoadPngFile(const std::string& Dir, const int& Num);
	bool ExistDir(const std::string& Dir);
public:
	TutorialScene();
	void OnInitialize()override;
	void OnUpdate()override;
	void OnDraw()override;
	void OnImguiDebug()override;
	void OnFinalize()override;

	std::shared_ptr<SceneCange> changeScene;

	StoryMgr pictureStory;
};

