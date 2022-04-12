#pragma once
#include"KuroEngine.h"
#include"SceneCange.h"
#include"IntoTheAbyss/Game.h"
#include"WinApp.h"
#include<memory>
#include"Vec.h"

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


private:

	// 初期座標
	const Vec2<float> WINDOW_SIZE = { 1280.0f,720.0f };
	const Vec2<float> LUNA_POS = { 0.0f,80.0f };
	const Vec2<float> LACY_POS = { WINDOW_SIZE.x - 410.0f, 80.0f };
	const Vec2<float> LUNA_ROBOT_POS = { 160.0f, 300.0f };
	const Vec2<float> LACY_ROBOT_POS = { WINDOW_SIZE.x / 2.0f, 360.0f };


public:
	TitleScene();
	void OnInitialize()override;
	void OnUpdate()override;
	void OnDraw()override;
	void OnImguiDebug()override;
	void OnFinalize()override;

	std::shared_ptr<SceneCange> changeScene;
};

