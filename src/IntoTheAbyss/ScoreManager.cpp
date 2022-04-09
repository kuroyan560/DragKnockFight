#include "ScoreManager.h"

ScoreManager::ScoreManager()
{
	//TexHandleMgr::LoadDivGraph("", 10, {}, number.data());
}

void ScoreManager::Init()
{
	honraiScore = 1000;
	score = 1000;
}

void ScoreManager::Add(const int &ADD_POINT)
{
	honraiScore += ADD_POINT;
}

void ScoreManager::Sub(const int &SUB_POINT)
{
	honraiScore -= SUB_POINT;
}

void ScoreManager::Update()
{
	float lerp = honraiScore - score;
	score += lerp * 0.1f;

	if (honraiScore - score <= 1)
	{
		score = honraiScore;
	}

	//スコア計算
	numberHandle = CountNumber(score);
}

void ScoreManager::Draw()
{
}

void ScoreManager::Debug()
{
	ImGui::Begin("ScoreManager");
	ImGui::Text("Score:%f",score);
	for (int i = 0; i < numberHandle.size(); ++i)
	{
		ImGui::Text("%d", numberHandle[i]);
	}
	ImGui::End();
}
