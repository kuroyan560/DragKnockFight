#pragma once
#include"KuroEngine.h"
#include"LightManager.h"
#include"Vec.h"
#include"WinApp.h"
#include<vector>

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
	int crossHandle;			// *の画像ハンドル
	std::vector<int> blueNumberHandle;// 青の数字の画像ハンドル
	std::vector<int> goldNumberHandle;// 金の数字の画像ハンドル

	// 各タイマー
	int resultUITimer;			// リザルトの画像のイージングに使用するタイマー
	int breakEnemyUITimer;		// BREAKの画像ハンドル敵に使用するタイマー
	int breakPlayerUITimer;		// BREAKの画像ハンドルプレイヤーに使用するタイマー
	int delayTimer;				// 各イージングの間の遅延タイマー

	// 各クラッシュの数字
	int breakEnemyAmount;		// 敵のクラッシュの数
	int breakPlayerAmount;		// プレイヤーのクラッシュの数

	// 各イージング量
	float resultEasingAmount;		// リザルトの画像のイージング量
	float breakEnemyEasingAmount;	// BREAKの画像のイージング量
	float breakPlayerEasingAmount;	// BREAKの画像のイージング量


public:

	const Vec2<int> WINDOW_CENTER = WinApp::Instance()->GetWinCenter();

	// イージング結果の座標
	const Vec2<float> RESULT_POS = { (float)WINDOW_CENTER.x - 80.0f, 30.0f };
	const Vec2<float> BREAK_ENEMY_POS = { (float)WINDOW_CENTER.x + 10.0f, 150.0f };
	const Vec2<float> BREAK_PLAYER_POS = { (float)WINDOW_CENTER.x + 90.0f, 250.0f };

	// 各タイマーのデフォルト値
	const int RESULT_UI_TIMER = 20;
	const int BREAK_ENEMY_UI_TIMER = 20;
	const int BREAK_PLAYER_UI_TIMER = 20;
	const int DELAY_TIMER = 30;


public:
	ResultScene();
	void OnInitialize()override;
	void OnUpdate()override;
	void OnDraw()override;
	void OnImguiDebug()override;
	void OnFinalize()override;

	SceneCange* changeScene;


private:
	// 桁数を取得。
	inline int CountDisits(const int& disits) {
		return to_string(disits).size();
	}
	// 指定の桁数の数字を取得。
	inline int GetDisit(const int& disits, const int& disit) {
		return (disits % (int)pow(10, disit + 1)) / pow(10, disit);
	}

	// [BREAK]を描画
	void DrawBREAK(const Vec2<float>& windowSize, const Vec2<float>& targetPosm, const float& easingTimer, const bool& isBoss, const int& breakCount);

};