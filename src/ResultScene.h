#pragma once
#include"KuroEngine.h"
#include"LightManager.h"
#include"Vec.h"
#include"WinApp.h"

#include"IntoTheAbyss/Game.h"
#include"SceneCange.h"

class ResultScene : public BaseScene
{

	int backGroundHandle;		// 背景(仮)の画像ハンドル
	int winnerFrameHandle;		// 勝者のフレームの画像ハンドル
	int resultHandle;			// リザルトの画像ハンドル
	int breakEnemyHandle;		// BREAKの画像ハンドル 敵
	int breakPlayerHandle;		// BREAKの画像ハンドル プレイヤー
	int scoreHandle;			// SCOREの画像ハンドル

	// 各タイマー
	int resultUITimer;			// リザルトの画像のイージングに使用するタイマー
	int breakEnemyUITimer;		// BREAKの画像ハンドル敵に使用するタイマー
	int breakPlayerUITimer;		// BREAKの画像ハンドルプレイヤーに使用するタイマー
	int delayTimer;				// 各イージングの間の遅延タイマー

	// 各イージング量
	float resultEasingAmount;		// リザルトの画像のイージング量
	float breakEnemyEasingAmount;	// BREAKの画像のイージング量
	float breakPlayerEasingAmount;	// BREAKの画像のイージング量

	// 各タイマーのデフォルト値
	const int RESULT_UI_TIMER = 20;
	const int BREAK_ENEMY_UI_TIMER = 20;
	const int BREAK_PLAYER_UI_TIMER = 20;
	const int DELAY_TIMER = 30;


public:

	const Vec2<int> WINDOW_CENTER = WinApp::Instance()->GetWinCenter();

	// イージング結果の座標
	const Vec2<float> RESULT_POS = { (float)WINDOW_CENTER.x - 80.0f, 30.0f };
	const Vec2<float> BREAK_ENEMY_POS = { (float)WINDOW_CENTER.x + 10.0f, 150.0f };
	const Vec2<float> BREAK_PLAYER_POS = { (float)WINDOW_CENTER.x + 90.0f, 250.0f };


public:
	ResultScene();
	void OnInitialize()override;
	void OnUpdate()override;
	void OnDraw()override;
	void OnImguiDebug()override;
	void OnFinalize()override;

	SceneCange* changeScene;
};