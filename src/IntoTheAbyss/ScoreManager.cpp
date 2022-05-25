#include "ScoreManager.h"
#include"TexHandleMgr.h"
#include"../Engine/DrawFunc.h"
#include"../Common/KuroMath.h"

ScoreManager::ScoreManager()
{
	TexHandleMgr::LoadDivGraph("resource/ChainCombat/UI/num_yellow.png", 12, { 12, 1 }, number.data());
	scoreGraph = TexHandleMgr::LoadGraph("resource/ChainCombat/UI/score.png");
}

ScoreManager::~ScoreManager()
{
}

void ScoreManager::Init()
{
	scoreOffsetY = 0.0f;
	honraiScore = 0;
	score = 0;
}

void ScoreManager::Add(const int &ADD_POINT)
{
	honraiScore += ADD_POINT;

	timer = 0;
	if (mode == EXIT)mode = STAY;
	if (mode != NONE)return;
	mode = APPEAR;
}

void ScoreManager::Sub(const int &SUB_POINT)
{
	honraiScore -= SUB_POINT;
}

#include"AudioApp.h"
void ScoreManager::Update()
{
	static int SCORE_UP_SE = AudioApp::Instance()->LoadAudio("resource/ChainCombat/sound/itemGet_0.wav");

	if (mode == NONE)return;

	timer++;
	if (totalTime[mode] <= timer)
	{
		mode = MODE(mode + 1);
		timer = 0;
	}


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
			AudioApp::Instance()->PlayWave(SCORE_UP_SE);
		}
	}
	else
	{
		scoreOffsetY = 0;
	}


	if (rate < 1.0f)
	{
		rate += 0.1f;
	}
	if (1.0f <= rate)
	{
		rate = 1.0f;
	}

	scoreOffsetY = 0 + 10 * KuroMath::Ease(Out, Back, rate, 0.0f, 1.0f);

	//スコア計算
	numberHandle = CountNumber(score);
}

#include"WinApp.h"
void ScoreManager::Draw()
{
	if (mode == NONE)return;

	Vec2<float> offset = { 0.0f,0.0f };
	float alpha = 1.0f;

	if (mode == APPEAR)
	{
		offset = KuroMath::Ease(Out, Quart, timer, totalTime[mode], { -1300.0f,0.0f }, { 0.0f,0.0f });
		alpha = KuroMath::Ease(Out, Quart, timer, totalTime[mode], 0.0f, 1.0f);
	}
	else if (mode == EXIT)
	{
		offset = KuroMath::Ease(In, Back, timer, totalTime[mode], { 0.0f,0.0f }, { 64.0f,0.0f });
		alpha = KuroMath::Ease(In, Quart, timer, totalTime[mode], 1.0f, 0.0f);
	}

	const auto winSize = WinApp::Instance()->GetExpandWinSize();
	//スコア制度なし
	const float size = 2.0f;
	const auto texSize = TexHandleMgr::GetTexBuffer(number[0])->GetGraphSize();

	const auto drawSize = texSize * size;

	DrawFunc::DrawGraph({ 1050,500 }, TexHandleMgr::GetTexBuffer(scoreGraph), Color(1.0f, 1.0f, 1.0f, alpha));
	for (int i = 0; i < numberHandle.size(); i++)
	{
		Vec2<float>centralPos(winSize.x - ((numberHandle.size() - 1 - i) + 0.5f) * drawSize.x, winSize.y - drawSize.y * 0.5f - scoreOffsetY);
		centralPos += offset;
		DrawFunc::DrawRotaGraph2D(centralPos, Vec2<float>(size, size), 0.0f, TexHandleMgr::GetTexBuffer(number[numberHandle[i]]), Color(1.0f, 1.0f, 1.0f, alpha));
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
