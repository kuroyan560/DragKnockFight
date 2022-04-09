#pragma once
#include"Singleton.h"
#include"Vec.h"
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
	int knockOutGraph;
	int knockOutTimer;
	Vec2<float>knockOutAppearPos;
	Vec2<float>knockOutDisappearPos;

	//勝利数カウント演出
	bool animation;

public:
	void Reset()
	{
		countRound = 0;
		countLeftWin = 0;
		countRightWin = 0;
		drawCountLeft = 0;
		drawCountRight = 0;
		animation = false;
	}
	void Update();
	void Draw();

	const int& GetNowRound() { return countRound + 1; }
	const int& GetLeftWin() { return countLeftWin; }
	const int& GetRightWin() { return countRightWin; }

	void RoundFinish(const Vec2<float>& FinishPos, const bool& WinnerIsLeft);
};

