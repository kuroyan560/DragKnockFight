#pragma once
#include"KuroEngine.h"
#include"LightManager.h"

#include"IntoTheAbyss/Game.h"
#include"IntoTheAbyss/SceneCange.h"
#include<memory>

class Sprite;
class GaussianBlur;
class RenderTarget;

class GameScene : public BaseScene
{
	std::shared_ptr<Sprite>backGround;
	std::shared_ptr<GaussianBlur>gaussianBlur;
	std::shared_ptr<RenderTarget>emissiveMap;

	Game game;
	float addValue;
	bool isSS;		// スクショを撮るかのフラグ

	std::shared_ptr<SceneCange>sceneChange;
public:
	GameScene();
	void OnInitialize()override;
	void OnUpdate()override;
	void OnDraw()override;
	void OnImguiDebug()override;
	void OnFinalize()override;
};