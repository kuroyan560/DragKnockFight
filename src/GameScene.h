#pragma once
#include"KuroEngine.h"
#include"LightManager.h"
#include"IntoTheAbyss/Game.h"
#include"IntoTheAbyss/SceneCange.h"
#include"IntoTheAbyss/CRT.h"
#include"IntoTheAbyss/SceneTransitionDrawScreenShot.h"


class Sprite;
class GaussianBlur;
class RenderTarget;

class GameScene : public BaseScene
{
	Game game;
	float addValue;
	bool isSS;		// �X�N�V�����B�邩�̃t���O
	//std::shared_ptr<SceneCange>sceneChange;
	std::shared_ptr<SceneTransitionDrawScreenShot>sceneChange;
	CRT crt;
public:
	GameScene();
	void OnInitialize()override;
	void OnUpdate()override;
	void OnDraw()override;
	void OnImguiDebug()override;
	void OnFinalize()override;
};