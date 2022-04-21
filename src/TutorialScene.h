#pragma once
#include"KuroEngine.h"
#include"../src/IntoTheAbyss/SceneCange.h"
#include"../src/IntoTheAbyss/StoryMgr.h"

class TutorialScene :public BaseScene
{
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

