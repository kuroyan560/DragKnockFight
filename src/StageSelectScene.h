#pragma once
#include"KuroEngine.h"
#include"IntoTheAbyss/SceneCange.h"

class StageSelectScene : public BaseScene
{
	//�L�����N�^�[�I�𒆃t���O
	bool charactersSelect;
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

