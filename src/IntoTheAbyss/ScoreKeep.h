#pragma once
#include"../Common/Singleton.h"
#include<vector>

class ScoreKeep:public Singleton<ScoreKeep>
{
public:
	ScoreKeep();

	void Init(int MAX_RAP_NUM, int MAX_SCORE);
	void AddScore(int RAP_NUM, int SCORE_NUM);
	int GetMaxNum()
	{
		return allBlockNum;
	}
	int GetAddScore()
	{
		int count = 0;
		for (int i = 0; i < score.size(); ++i)
		{
			count += score[i];
		}
		return count;
	}

private:
	std::vector<int>score;
	int allBlockNum;

};

