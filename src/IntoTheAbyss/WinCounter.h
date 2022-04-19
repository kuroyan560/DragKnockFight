#pragma once
#include"Singleton.h"
#include"Vec.h"
#include"CharacterInterFace.h"
class WinCounter : public Singleton<WinCounter>
{
	friend class Singleton<WinCounter>;
	WinCounter();

	//�������̉摜
	int winCountGraph_Left;
	int winCountGraph_Right;
	//KnockOut �摜
	int knockOutGraph_Left;
	int knockOutGraph_Right;

	//�����֘A
	int countRound;						//���E���h��
	int countLeftWin, countRightWin;	//������
	int drawCountLeft, drawCountRight;	//�������`�搔

	// KnockOut �����̃A�j���[�V����
	Vec2<float>knockOutPos;
	Vec2<float>knockOutAppearPos;
	Vec2<float>knockOutDisappearPos;
	float kncokOutScale;
	float knockOutRadian;
	int knockOutSpinVec;
	int knockOutGraph;
	int knockOutTimer;

	//�������J�E���g���o
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

