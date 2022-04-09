#include "ScoreManager.h"
#include"TexHandleMgr.h"
#include"../Engine/DrawFunc.h"
#include"../Common/KuroMath.h"

ScoreManager::ScoreManager()
{
	for (int i = 0; i < scorePos.size(); ++i)
	{
		scorePos[i] = { 200,200 };
	}
	texSize = { 44,44 };
	number.resize(10);
	TexHandleMgr::LoadDivGraph("resource/ChainCombat/UI/num.png", 10, { 10, 1 }, number.data());
}

void ScoreManager::Init()
{
	honraiScore = 1000;
	score = 1000;
	moveScoreCount = 0;
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
		if (moveInterval % 3 == 0)
		{
			for (int i = 0; i < moveFlag.size(); ++i)
			{
				rate[i] = 0.0f;
			}
			++moveScoreCount;

			if (moveScoreCount % 2 == 0)
			{
				//scorePos.y = 200;
			}
			else
			{
				//scorePos.y = 210;
			}
		}
	}
	else
	{
		moveScoreCount = 0;
		for (int i = 0; i < scorePos.size(); ++i)
		{
			scorePos[i].y = 200;
		}
	}

	//スコア計算
	numberHandle = CountNumber(score);


	for (int i = 0; i < oldNum.size(); ++i)
	{
		if (numberHandle[i] != oldNum[i])
		{
			moveFlag[i] = true;
		}
		else
		{
			moveFlag[i] = false;
		}
		oldNum[i] = numberHandle[i];
	}

	for (int i = 0; i < moveFlag.size(); ++i)
	{
		if (moveFlag[i])
		{
			if (rate[i] < 1.0f)
			{
				rate[i] += 0.1f;
			}
			if (1.0f <= rate[i])
			{
				rate[i] = 1.0f;
			}
		}
	}

	for (int i = 0; i < moveFlag.size(); ++i)
	{
		if (!moveFlag[i])
		{
			rate[i] = 0.0f;
		}
	}


	for (int i = 0; i < oldNum.size(); ++i)
	{
		scorePos[i].y = 200 + 15 * KuroMath::Ease(Out, Back, rate[i], 0.0f, 1.0f);
	}
}

void ScoreManager::Draw()
{
	for (int i = 0; i < numberHandle.size(); i++)
	{
		Vec2<float>centralPos(scorePos[i].x + i * texSize.x, scorePos[i].y);
		DrawFunc::DrawRotaGraph2D(centralPos, Vec2<float>(1.0f, 1.0f), 0.0f, TexHandleMgr::GetTexBuffer(number[numberHandle[i]]));
	}
}

void ScoreManager::Debug()
{
	ImGui::Begin("ScoreManager");
	ImGui::Text("Score:%f", score);
	for (int i = 0; i < numberHandle.size(); ++i)
	{
		ImGui::Text("%d", numberHandle[i]);
	}
	ImGui::End();
}
