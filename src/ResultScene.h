#pragma once
#include"KuroEngine.h"
#include"LightManager.h"
#include"Vec.h"
#include"WinApp.h"
#include<vector>
#include<array>

#include"IntoTheAbyss/Game.h"
#include"InToTheAbyss/SceneCange.h"
#include"IntoTheAbyss/CharacterInterFace.h"

class ResultScene : public BaseScene
{

	int backGroundHandle;		// 背景(仮)の画像ハンドル
	int winnerFrameHandle;		// 勝者のフレームの画像ハンドル
	int resultHandle;			// リザルトの画像ハンドル
	int breakEnemyHandle;		// BREAKの画像ハンドル 敵
	int breakPlayerHandle;		// BREAKの画像ハンドル プレイヤー
	int crashEnemyHandle;		// CRASHの画像ハンドル 敵
	int crashPlayerHandle;		// CRASHの画像ハンドル プレイヤー
	int scoreHandle;			// SCOREの画像ハンドル
	int crossHandle;			// *の画像ハンドル
	std::array<int, 12> blueNumberHandle;// 青の数字の画像ハンドル
	std::array<int, 12> goldNumberHandle;// 金の数字の画像ハンドル

	// スコア
	int targetScore;				// イージングの目標値
	float scoreEffectTimer;			// スコアをガラガラ表示するために使用するタイマー
	std::array<int, 10> prevScore;	// 前フレームのスコア
	std::array<float, 10> scoreSize;// スコアのサイズ

	// 各タイマー
	int resultUITimer;			// リザルトの画像のイージングに使用するタイマー
	int breakEnemyUITimer;		// BREAKの画像ハンドル敵に使用するタイマー
	int breakPlayerUITimer;		// BREAKの画像ハンドルプレイヤーに使用するタイマー
	int crashEnemyUITimer;		// CRASHの画像ハンドル敵に使用するタイマー
	int crashPlayerUITimer;		// CRASHの画像ハンドルプレイヤーに使用するタイマー
	int scoreUITimer;			// SCOREの画像ハンドルに使用するタイマー
	int delayTimer;				// 各イージングの間の遅延タイマー

	// 各クラッシュの数字
	int breakEnemyAmount;		// 敵のブレークの数
	int breakPlayerAmount;		// プレイヤーのブレークの数
	int crashEnemyAmount;		// 敵のクラッシュの数
	int crashPlayerAmount;		// プレイヤーのクラッシュの数

	// 各イージング量
	float resultEasingAmount;		// リザルトの画像のイージング量
	float breakEnemyEasingAmount;	// BREAKの画像のイージング量
	float breakPlayerEasingAmount;	// BREAKの画像のイージング量
	float crashEnemyEasingAmount;	// CRASHの画像のイージング量
	float crashPlayerEasingAmount;	// CRASHの画像のイージング量
	float scoreEasingAmount;		// SCOREの画像のイージング量
	float scoreEffectEasingAmount;	// スコアのガラガラ表示するために使用するタイマー

	//キャラの画像
	int winnerGraph[PLAYABLE_CHARACTER_NUM];
	PLAYABLE_CHARACTER_NAME winnerName;

public:

	const Vec2<int> WINDOW_CENTER = WinApp::Instance()->GetWinCenter();

	// イージング結果の座標
	const Vec2<float> RESULT_POS = { (float)WINDOW_CENTER.x - 200.0f, 30.0f };
	const Vec2<float> BREAK_ENEMY_POS = { (float)WINDOW_CENTER.x - 110.0f, 150.0f };
	const Vec2<float> CRASH_ENEMY_POS = { (float)WINDOW_CENTER.x - 110.0f, 250.0f };
	const Vec2<float> BREAK_PLAYER_POS = { (float)WINDOW_CENTER.x - 110.0f, 350.0f };
	const Vec2<float> CRASH_PLAYER_POS = { (float)WINDOW_CENTER.x - 110.0f, 450.0f };
	const Vec2<float> SCORE_POS = { (float)WINDOW_CENTER.x - 100.0f, (float)WINDOW_CENTER.y + 180.0f };

	// 各タイマーのデフォルト値
	const int RESULT_UI_TIMER = 20;
	const int BREAK_ENEMY_UI_TIMER = 20;
	const int BREAK_PLAYER_UI_TIMER = 20;
	const int CRASH_ENEMY_UI_TIMER = 20;
	const int CRASH_PLAYER_UI_TIMER = 20;
	const int SCORE_UI_TIMER = 40;
	const int DELAY_TIMER = 30;
	const int SCORE_EFFECT_TIMER = 180;


public:
	ResultScene();
	void OnInitialize()override;
	void OnUpdate()override;
	void OnDraw()override;
	void OnImguiDebug()override;
	void OnFinalize()override;

	std::shared_ptr<SceneCange> changeScene;


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
	void DrawBREAK(const Vec2<float>& targetPosm, const float& easingTimer, const int& graphHandle, const int& breakCount);

	// [SCORE][スコア]を描画
	void DrawSCORE(const float& easingTimer, const double& scoreEffectEasingTimer);

};