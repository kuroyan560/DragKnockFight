#pragma once
#include"Singleton.h"
#include"Vec.h"
#include"CharacterInterFace.h"
#include<array>

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
	std::array<int, 3> lunaHandle;			// �m�b�N�A�E�g����Luna(�����̃L����)�̉摜�n���h�� �A�j���[�V����
	std::array<int, 3> lacyHandle;			// �m�b�N�A�E�g����Luna(�����̃L����)�̉摜�n���h�� �A�j���[�V����
	std::array<int, 5> maskHandle;			// �m�b�N�A�E�g���Ƀ}�X�N�Ƃ��Ďg�p����摜�̃n���h�� �A�j���[�V����
	std::array<int, 5> maskFrameHandle;	// �m�b�N�A�E�g���Ɏg�p����}�X�N�̃t���[�� �A�j���[�V����
	int maskAnimHandle;
	int lunaAnimHandle;

	//�����֘A
	int countLeftWin, countRightWin;	//������
	int drawCountLeft, drawCountRight;	//�������`�搔

	// KnockOut �����̃A�j���[�V����
	Vec2<float>knockOutPos;				// [KnockOut!!!!]�̉摜�̍��W
	Vec2<float>maskPos;					// [KnockOut!!!!]�ɂ�����}�X�N�̍��W
	Vec2<float>charaPos;				// �}�X�N�̓����ɂ���L�����̍��W
	int knockOutSpinVec;
	int knockOutGraph;
	int knockOutTimer;

	// ������KnockOut���ǂ���
	bool isLeftKnockOut;

	//�������J�E���g���o
	bool animation;

	bool gameFinish = false;

	Vec2<float>GetWinCountPos(const bool& Left, const int& Num);

	// �m�b�N�A�E�g���̃X�e�[�^�X
	enum class KNOCK_OUT_PHASE {

		START_PHASE,		// �J�n�t�F�[�Y
		STOP_PHASE,			// �L�����J�b�g�C�����̈ꎞ��~�t�F�[�Y
		END_PHASE,			// �I���t�F�[�Y

	};
	KNOCK_OUT_PHASE knockOutPhase;

public:
	int countRound;						//���E���h��


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

