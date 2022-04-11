#pragma once
#include"KuroEngine.h"
#include"LightManager.h"

#include"IntoTheAbyss/Game.h"
#include"SceneCange.h"

class ResultScene : public BaseScene
{

	int backGroundHandle;

public:
	ResultScene();
	void OnInitialize()override;
	void OnUpdate()override;
	void OnDraw()override;
	void OnImguiDebug()override;
	void OnFinalize()override;

	SceneCange* changeScene;
};