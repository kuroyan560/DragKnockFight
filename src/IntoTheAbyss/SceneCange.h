#pragma once
#include"KuroEngine.h"

class SceneCange :public SceneTransition
{
public:
	SceneCange();
	~SceneCange();
	void OnStart()override;
	bool OnUpdate()override;
	void OnDraw()override;
};
