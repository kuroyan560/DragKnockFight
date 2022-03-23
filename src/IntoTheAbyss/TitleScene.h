#pragma once
#include"KuroEngine.h"
#include"SceneCange.h"

class TitleScene : public BaseScene
{
public:
	TitleScene();
	void OnInitialize()override;
	void OnUpdate()override;
	void OnDraw()override;
	void OnImguiDebug()override;
	void OnFinalize()override;

	SceneCange *changeScene;
};

