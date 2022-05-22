#pragma once
#include"KuroEngine.h"
#include"SceneCange.h"
#include"ScreenEdgeEffect.h"
#include"PictureStory.h"
#include"BehaviorPredection.h"

class TitleScene : public BaseScene
{

	// 画像ハンドル系
	int frameHandle;		// 外枠のハンドル
	int starHandle;			// 星のハンドル
	int lacyHandle;			// [lacy]のハンドル
	int lunaHandle;			// [luna]のハンドル
	int lacyRobotHandle;	// [lacy]ロボットのハンドル
	int lunaRobotHandle;	// [luna]ロボットのハンドル
	int titleHandle;		// タイトルロゴのハンドル
	int pressStartHandle;	// PRESSSTARTのハンドル

	// イージングに使用するタイマー
	float easingTimer;
	bool isUpper;
	int pressStartTimer;
	bool isPressStartDraw;


private:

	// 初期座標
	const Vec2<float> WINDOW_SIZE = { 1280.0f,720.0f };
	const Vec2<float> LUNA_POS = { 0.0f,120.0f };
	const Vec2<float> LACY_POS = { WINDOW_SIZE.x - 410.0f, 120.0f };
	const Vec2<float> LUNA_ROBOT_POS = { 125.0f, 350.0f };
	const Vec2<float> LACY_ROBOT_POS = { WINDOW_SIZE.x / 2.0f - 50.0f, 340.0f };
	const Vec2<float> TITLE_POS = { 170,80 };
	const Vec2<float> PRESS_START_POS = { 660,550 };

	// イージングに使用するタイマー
	const float ADD_EASING_TIMER = 0.01f;
	const int PERSSSTART_TIMER = 50;
	const float EASING_MOVE_STAR = 10.0f;
	const float EASING_MOVE_CHAR = 20.0f;
	const float EASING_MOVE_TITLE = 30.0f;

	//チュートリアル選択
	bool tutorialQuestion = false;
	bool tutorialSelect = false;
	bool tutorialYes = true;

public:
	TitleScene();
	void OnInitialize()override;
	void OnUpdate()override;
	void OnDraw()override;
	void OnImguiDebug()override;
	void OnFinalize()override;

	std::shared_ptr<SceneCange> changeScene;
};

