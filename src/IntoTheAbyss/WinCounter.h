#pragma once
#include"Singleton.h"
#include"Vec.h"
#include"CharacterInterFace.h"
class WinCounter : public Singleton<WinCounter>
{
	friend class Singleton<WinCounter>;
	WinCounter();

	//勝利数の画像
	int winCountGraph_Left;
	int winCountGraph_Right;
	//KnockOut 画像
	int knockOutGraph_Left;
	int knockOutGraph_Right;

	//試合関連
	int countRound;						//ラウンド数
	int countLeftWin, countRightWin;	//勝利数
	int drawCountLeft, drawCountRight;	//勝利数描画数

	// KnockOut 文字のアニメーション
	Vec2<float>knockOutPos;
	Vec2<float>knockOutAppearPos;
	Vec2<float>knockOutDisappearPos;
	float kncokOutScale;
	float knockOutRadian;
	int knockOutSpinVec;
	int knockOutGraph;
	int knockOutTimer;

	//勝利数カウント演出
	bool animation;

	bool gameFinish = false;

	Vec2<float>GetWinCountPos(const bool& Left, const int& Num);

public:
	void Reset()
	{
		countRound = 0;
		countLeftWin = 0;
		countRightWin = 0;
		drawCountLeft = 0;
		drawCountRight = 0;
		animation = false;
		gameFinish = false;
	}
	void Update();
	void Draw();

	const int& GetNowRound() { return countRound + 1; }
	const int& GetLeftWin() { return countLeftWin; }
	const int& GetRightWin() { return countRightWin; }
	const bool& GetNowAnimation() { return animation; }
	const bool& GetGameFinish() { return gameFinish; }

	WHICH_TEAM Winner()
	{
		if (countLeftWin < countRightWin)return RIGHT_TEAM;
		return LEFT_TEAM;
	}

	void RoundFinish(const Vec2<float>& FinishPos, const bool& WinnerIsLeft);
};

