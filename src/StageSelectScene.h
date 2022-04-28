#pragma once
#include"KuroEngine.h"
#include"IntoTheAbyss/SceneCange.h"

class StageSelectScene : public BaseScene
{
	//ビッサミ提出用
	bool isSkip = false;

	//キャラクター選択中フラグ
	bool charactersSelect;

	// マルチシングル選択画面かフラグ
	bool isSelectMultiSingle;

	// マルチかシングルか
	bool isMulti;
	bool isSingle;

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

