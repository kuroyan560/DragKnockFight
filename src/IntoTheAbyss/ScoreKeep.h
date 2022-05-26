#pragma once
#include"../Common/Singleton.h"
#include<vector>

class ScoreKeep:public Singleton<ScoreKeep>
{
public:
	ScoreKeep();

	void Init(int MAX_RAP_NUM);
	void AddScore(int RAP_NUM, int SCORE_NUM);

private:
	std::vector<int>score;
};

