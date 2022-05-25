#pragma once
#include"../Common/Singleton.h"
#include<vector>
#include"../Engine/ImguiApp.h"
#include"../Common/Vec.h"
#include<array>
#include"KuroFunc.h"

class ScoreManager :public Singleton<ScoreManager>
{
	friend class Singleton<ScoreManager>;
	ScoreManager();
public:
	~ScoreManager();

	void Init();
	void Add(const int &ADD_POINT);
	void Sub(const int &SUB_POINT);
	void Update();
	void Draw();

	void Debug();
	const float& GetScore() { return honraiScore; }
private:
	enum MODE { APPEAR, STAY, EXIT, MODE_NUM, NONE = MODE_NUM }mode = NONE;
	const std::array<int, MODE_NUM> totalTime = { 30,120,30 };
	int timer;

	float score;
	float honraiScore;

	std::vector<int>numberHandle;
	std::array<int, 12> number;
	int scoreGraph;

	float scoreOffsetY;

	int moveInterval;
	float rate;

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
};

