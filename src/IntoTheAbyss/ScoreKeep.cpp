#include "ScoreKeep.h"

ScoreKeep::ScoreKeep()
{
	allBlockNum = 0;

}

void ScoreKeep::Init(int MAX_RAP_NUM, int MAX_SCORE)
{
	score.clear();

	score.reserve(MAX_RAP_NUM);
	score.resize(MAX_RAP_NUM);
	allBlockNum = MAX_SCORE;
}

void ScoreKeep::AddScore(int RAP_NUM, int SCORE_NUM)
{
	if (RAP_NUM < score.size()) {
		score[RAP_NUM] = SCORE_NUM;
	}
}
