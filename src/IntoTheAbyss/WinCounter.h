#pragma once
#include"Singleton.h"
#include"Vec.h"
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
	int knockOutGraph;
	int knockOutTimer;
	Vec2<float>knockOutAppearPos;
	Vec2<float>knockOutDisappearPos;

	//�������J�E���g���o
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

