#pragma once
#include"KuroEngine.h"
#include"IntoTheAbyss/SceneCange.h"
#include"IntoTheAbyss/StageSelectImage.h"
#include"IntoTheAbyss/StageSelectScreenShot.h"

class StageSelectScene : public BaseScene
{
	//ビッサミ提出用
	bool isSkip = false;

	//キャラクター選択中フラグ
	bool charactersSelect;

	StageSelectImage stageSelect;
	//StageSelectScreenShot screenShot;
public:
	StageSelectScene();
	void OnInitialize()override;
	void OnUpdate()override;
	void OnDraw()override;
	void OnImguiDebug()override;
	void OnFinalize()override;

	std::shared_ptr<SceneCange>changeScene;

	int stageNum;
};

