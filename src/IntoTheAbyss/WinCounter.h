#pragma once
#include"Singleton.h"
#include"Vec.h"
#include"CharacterInterFace.h"
#include<array>

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
	std::array<int, 3> lunaHandle;			// ノックアウト時のLuna(左側のキャラ)の画像ハンドル アニメーション
	std::array<int, 3> lacyHandle;			// ノックアウト時のLuna(左側のキャラ)の画像ハンドル アニメーション
	std::array<int, 5> maskHandle;			// ノックアウト時にマスクとして使用する画像のハンドル アニメーション
	std::array<int, 5> maskFrameHandle;	// ノックアウト時に使用するマスクのフレーム アニメーション
	int maskAnimHandle;
	int lunaAnimHandle;

	//試合関連
	int countLeftWin, countRightWin;	//勝利数
	int drawCountLeft, drawCountRight;	//勝利数描画数

	// KnockOut 文字のアニメーション
	Vec2<float>knockOutPos;				// [KnockOut!!!!]の画像の座標
	Vec2<float>maskPos;					// [KnockOut!!!!]にかけるマスクの座標
	Vec2<float>charaPos;				// マスクの内側にいるキャラの座標
	int knockOutSpinVec;
	int knockOutGraph;
	int knockOutTimer;

	// 左側のKnockOutかどうか
	bool isLeftKnockOut;

	//勝利数カウント演出
	bool animation;

	bool gameFinish = false;

	Vec2<float>GetWinCountPos(const bool& Left, const int& Num);

	// ノックアウト時のステータス
	enum class KNOCK_OUT_PHASE {

		START_PHASE,		// 開始フェーズ
		STOP_PHASE,			// キャラカットイン中の一時停止フェーズ
		END_PHASE,			// 終了フェーズ

	};
	KNOCK_OUT_PHASE knockOutPhase;

public:
	int countRound;						//ラウンド数


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

	void RoundFinish(const Vec2<float>& FinishPos, const bool& WinnerIsLeft, const Vec2<float>& WinnerPos);
};

