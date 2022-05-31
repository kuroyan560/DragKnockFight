#pragma once
#include"KuroEngine.h"
#include"../Engine/Sprite.h"

class SceneTransitionDrawScreenShot :public SceneTransition
{
public:
	SceneTransitionDrawScreenShot();
	~SceneTransitionDrawScreenShot();
	void OnStart()override;
	bool OnUpdate()override;
	void OnDraw()override;

private:
	bool startFlag;
	int alpha;
	Sprite screenDraw;
};
