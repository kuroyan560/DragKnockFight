#pragma once
#include"../Common/Singleton.h"
#include<vector>
#include"../Engine/ImguiApp.h"
#include"../Common/Vec.h"
#include<array>

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
private:
	float score;
	float honraiScore;

	std::vector<int>numberHandle;
	std::array<int, 10> number;

	Vec2<float>scorePos;
	Vec2<float>basePos;
	Vec2<float>texSize;

	int moveInterval;
	float rate;

	std::vector<int> CountNumber(int TIME)
	{
		float score = TIME;
		std::vector<int> Number(9);
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

