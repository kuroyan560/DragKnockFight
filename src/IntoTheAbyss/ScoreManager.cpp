#include "ScoreManager.h"
#include"TexHandleMgr.h"
#include"../Engine/DrawFunc.h"
#include"../Common/KuroMath.h"

ScoreManager::ScoreManager()
{
	scorePos = { 200,200 };
	texSize = { 44,44 };
	TexHandleMgr::LoadDivGraph("resource/ChainCombat/UI/num.png", 10, { 10, 1 }, number.data());
}

ScoreManager::~ScoreManager()
{
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

	if (honraiScore - score <= 1 && -1 <= honraiScore - score)
	{
		score = honraiScore;
	}

	if (score != honraiScore)
	{
		++moveInterval;
		if (moveInterval % 5 == 0)
		{
			rate = 0.0f;
		}
	}
	else
	{
		scorePos.y = 200;
	}


	if (rate < 1.0f)
	{
		rate += 0.1f;
	}
	if (1.0f <= rate)
	{
		rate = 1.0f;
	}

	scorePos.y = 210 + 10 * KuroMath::Ease(Out, Back, rate, 0.0f, 1.0f);


	//スコア計算
	numberHandle = CountNumber(score);
}

void ScoreManager::Draw()
{
	float size = 0.6f;
	for (int i = 0; i < numberHandle.size(); i++)
	{
		Vec2<float>centralPos(scorePos.x + i * texSize.x, scorePos.y);
		DrawFunc::DrawRotaGraph2D(centralPos, Vec2<float>(size, size), 0.0f, TexHandleMgr::GetTexBuffer(number[numberHandle[i]]));
	}
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
