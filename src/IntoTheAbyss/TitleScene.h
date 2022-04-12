#pragma once
#include"KuroEngine.h"
#include"SceneCange.h"

class TitleScene : public BaseScene
{

	int frameHandle;		// 外枠のハンドル
	int starHandle;			// 星のハンドル
	int charHandle;			// キャラのハンドル
	int robotHandle;		// ロボットのハンドル
	int titleHandle;		// タイトルロゴのハンドル
	int pressStartHandle;	// PRESSSTARTのハンドル


public:
	TitleScene();
	void OnInitialize()override;
	void OnUpdate()override;
	void OnDraw()override;
	void OnImguiDebug()override;
	void OnFinalize()override;

	SceneCange* changeScene;
};

