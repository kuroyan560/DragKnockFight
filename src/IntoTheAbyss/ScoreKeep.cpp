#include "ScoreKeep.h"

ScoreKeep::ScoreKeep()
{
}

void ScoreKeep::Init(int MAX_RAP_NUM)
{
	score.reserve(MAX_RAP_NUM);
	score.resize(MAX_RAP_NUM);
}

void ScoreKeep::AddScore(int RAP_NUM, int SCORE_NUM)
{
	score[RAP_NUM] = SCORE_NUM;
}
