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
#include"IntoTheAbyss/CRT.h"

class ResultScene : public BaseScene
{
	int backGroundHandle;		// 背景(仮)の画像ハンドル
	int winnerFrameHandle;		// 勝者のフレームの画像ハンドル
	int resultHandle;			// リザルトの画像ハンドル
	int breakEnemyHandle;		// BREAKの画像ハンドル 敵
	int roundHandle;			// ROUNDの描画
	int lunaHandle;
	int lacyHandle;
	int slashHandle;			//スラッシュ
	int goodHandle;				//Goodの画像ハンドル
	int greatHandle;			//Greatの画像ハンドル
	int excellentHandle;		//Excellentの画像ハンドル
	int evaluationNowHandle;	
	bool evaluationFlag;		
	float lissajousTimer;
	std::array<int, 3> perfectHandle; //スラッシュ
	std::array<int, 12> blueNumberHandle;// 青の数字の画像ハンドル

	//スコア
	float baseBreakCount, breakCount, prevBreakCount;

	// 背景で飛ばすキャラクターのアニメーションタイマー
	int backGroundCharaAnimTimer;
	int backGroundCharaAnimHandle;
	float backGroundCharaAngle;
	Vec2<float> backGroundCharaPos;
	Vec2<float> backGroundCharaVel;
	std::array<int, 3> useHandle;
	std::array<int, 3> redCharaHandle;
	std::array<int, 3> greenCharaHandle;
	std::array<int, 3> ironBallhandle;


	// スコア
	int targetScore;				// イージングの目標値
	float scoreEffectTimer;			// スコアをガラガラ表示するために使用するタイマー
	std::array<int, 10> prevScore;	// 前フレームのスコア
	std::array<float, 10> scoreSize;// スコアのサイズ
	bool bigFontFlag;
	float defaultSize;

	//評価
	float easeEvaluationPosY;
	int intervalTimer;
	bool timeUpFlag;
	float evaluationPosX;

	int perfectIndex,perfectInterval;

	// 各タイマー
	int resultUITimer;			// リザルトの画像のイージングに使用するタイマー
	int breakEnemyUITimer;		// BREAKの画像ハンドル敵に使用するタイマー
	int evaluationEasingTimer;
	int delayTimer;				// 各イージングの間の遅延タイマー


	// 各イージング量
	float resultEasingAmount;		// リザルトの画像のイージング量
	float breakCountEasingAmount;	// BREAKの画像のイージング量


	//キャラの画像
	int winnerGraph[PLAYABLE_CHARACTER_NUM];
	PLAYABLE_CHARACTER_NAME winnerName;

	Vec2<float> nowSize,breakSize, maxSize;

	CRT crt;
	bool endFlg = false;
	bool initSoundFlag;

	int ssIntervalTimer;

	std::array<int, 4>soundSe;
public:

	const Vec2<int> WINDOW_CENTER = WinApp::Instance()->GetWinCenter();

	// イージング結果の座標
	Vec2<float> RESULT_POS = { (float)WINDOW_CENTER.x - 200.0f, 30.0f };

	// 各タイマーのデフォルト値
	const int RESULT_UI_TIMER = 20;
	const int BREAK_COUNTUI_TIMER = 20;
	const int SCORE_EFFECT_TIMER = 20;
	const int EVALUATION_EFFECT_TIMER = 20;
	const int DELAY_TIMER = 30;

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


	std::vector<int> CountNumber(int TIME)
	{
		float score = TIME;
		std::vector<int> Number(KuroFunc::GetDigit(TIME));
		for (int i = 0; i < Number.size(); ++i)
		{
			Number[i] = -1;
		}

		int tmp = score;
		//スコア計算
		for (int i = 0; tmp > 0; ++i)
		{
			float result = tmp % 10;
			//Number.push_back(result);
			Number[i] = result;
			tmp /= 10.0f;
		}
		//0埋め
		for (int i = 0; i < Number.size(); ++i)
		{
			if (Number[i] == -1)
			{
				Number[i] = 0;
			}
		}
		std::reverse(Number.begin(), Number.end());
		return Number;
	}
	void DrawBreakCount(float scoreEasingAmount,int BREAK_NOW_COUNT, int BREAK_MAX_COUNT, float OFFSET_X, Vec2<float> LISSAJOUS);
};