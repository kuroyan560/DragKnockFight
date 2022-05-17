#pragma once
#include"KuroEngine.h"
#include"IntoTheAbyss/SceneCange.h"
#include"IntoTheAbyss/StageSelectImage.h"
#include"IntoTheAbyss/StageSelectScreenShot.h"

class StageSelectScene : public BaseScene
{
	//�r�b�T�~��o�p
	bool isSkip = false;

	//�L�����N�^�[�I�𒆃t���O
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

