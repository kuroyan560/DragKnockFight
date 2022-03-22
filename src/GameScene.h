#pragma once
#include"KuroEngine.h"
#include"LightManager.h"

#include"IntoTheAbyss/Game.h"

class Sprite_Shadow;
class GaussianBlur;
class RenderTarget;

class GameScene : public BaseScene
{
	std::shared_ptr<Sprite_Shadow>backGround;
	std::shared_ptr<GaussianBlur>gaussianBlur;
	std::shared_ptr<RenderTarget>emissiveMap;

	Game game;
public:
	GameScene();
	void OnInitialize()override;
	void OnUpdate()override;
	void OnDraw()override;
	void OnImguiDebug()override;
	void OnFinalize()override;
};