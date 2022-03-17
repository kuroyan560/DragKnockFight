#pragma once
#include"KuroEngine.h"
#include"LightManager.h"

#include"IntoTheAbyss/Game.h"

class Sprite;
class Sprite_Shadow;
class TextureBuffer;
class GaussianBlur;

class GameScene : public BaseScene
{
	LightManager ligMgr;
	Light::Direction dirLig;
	Light::Point ptLig;
	Light::Spot spotLig;
	Light::HemiSphere hemiLig;

	std::shared_ptr<Sprite_Shadow>backGround;
	std::shared_ptr<Sprite>test;
	std::shared_ptr<Sprite_Shadow>test_shadow;
	std::shared_ptr<Sprite>lightRangeSprite;

	static const int SPLIT = 8;
	static const int SPLIT_SPRITE_NUM = SPLIT * SPLIT;
	//static const int SPLIT_X = 6;
	//static const int SPLIT_Y = 3;
	//static const int SPLIT_SPRITE_NUM = SPLIT_X * SPLIT_Y;
	std::shared_ptr<Sprite_Shadow>splitSprites[SPLIT_SPRITE_NUM];

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