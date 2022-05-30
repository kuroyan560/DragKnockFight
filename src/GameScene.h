#pragma once
#include"KuroEngine.h"
#include"LightManager.h"
#include"IntoTheAbyss/Game.h"
#include"IntoTheAbyss/SceneCange.h"
#include"IntoTheAbyss/CRT.h"


class Sprite;
class GaussianBlur;
class RenderTarget;

class GameScene : public BaseScene
{
	Game game;
	float addValue;
	bool isSS;		// スクショを撮るかのフラグ
	std::shared_ptr<SceneCange>sceneChange;
	CRT crt;
public:
	GameScene();
	void OnInitialize()override;
	void OnUpdate()override;
	void OnDraw()override;
	void OnImguiDebug()override;
	void OnFinalize()override;
};