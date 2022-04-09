#pragma once
#include"../Common/Vec.h"
#include<vector>
#include"../Common/Singleton.h"

/// <summary>
/// �Q�[���̑S�̂̎��Ԃ��v�Z���܂�
/// </summary>
class GameTimer:public Singleton<GameTimer>
{
public:
	GameTimer();

	/// <summary>
	/// ���Ԃ̕`����W�����������܂�
	/// </summary>
	/// <param name="POS">���Ԃ̕`����W</param>
	void Init(const Vec2<float> &POS, int TIME, const Vec2<float> &COUNT_DOWN_START_POS, const Vec2<float> &COUNT_DOWN_END_POS);
	void Finalize();
	void Update();
	void Draw();

	/// <summary>
	/// ���Ԃ��v��܂�
	/// </summary>
	void Start();

	/// <summary>
	/// �t���[���P�ʂŎ��Ԃ�n���܂�
	/// </summary>
	/// <returns>�t���[����</returns>
	int GetFlame();

	/// <summary>
	/// �b�P�ʂŎ��Ԃ�n���܂�
	/// </summary>
	/// <returns>�b��</returns>
	int GetSeceond();

	/// <summary>
	/// ���Ԑ؂��m�点�܂�
	/// </summary>
	/// <returns>true...���Ԑ؂�,false...���Ԑ؂ꂶ��Ȃ�</returns>
	bool TimeUpFlag();

	/// <summary>
	/// �J�E���g�_�E�����I�����������n���܂�
	/// </summary>
	/// <returns>true...�J�E���g�_�E���I��,false...�I�����Ă��Ȃ�</returns>
	bool StartGame();


	bool FinishAllEffect();


	void Debug();

private:
	Vec2<float> timerPos;

	std::vector<int> number;

	int timer;
	float flame;
	bool startFlag;
	bool timeUpFlag;

	std::vector<int> minitueHandle;
	std::vector<int> timeHandle;
	std::vector<int> miriHandle;

	Vec2<float>  initCountDownPos, countDownPos;
	float countDownEndPos;
	Vec2<float> texSize;
	int countDownNum;
	
	bool countDownFlag;
	int countDownHandle[4], finishHandle;

	int finishTimer;

	bool counDownFinishFlag;

	std::vector<int> CountNumber(int TIME);
};

