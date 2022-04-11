#include "GameTimer.h"
#include"../IntoTheAbyss/TexHandleMgr.h"
#include"../Engine/DrawFunc.h"
#include"../Engine/ImguiApp.h"
#include"../Common/KuroMath.h"

GameTimer::GameTimer()
{
	startFlag = false;
	timer = -1;
	flame = -1;
	timerPos = { 714,62 };

	number.resize(11);
	texSize = { 48,44 };
	TexHandleMgr::LoadDivGraph("resource/ChainCombat/UI/num.png", 12, { 12, 1 }, number.data());

}

void GameTimer::Init(const Vec2<float> &POS, int TIME, const Vec2<float> &COUNT_DOWN_START_POS, const Vec2<float> &COUNT_DOWN_END_POS)
{
	startFlag = false;
	timeUpFlag = false;
	timer = TIME;
	flame = 0;

	initCountDownPos = COUNT_DOWN_START_POS;
	initCountDownPos.y -= (texSize.y + 10);
	countDownEndPos = COUNT_DOWN_END_POS.x - COUNT_DOWN_START_POS.x;

	countDownNum = 3;

	countDownFlag = false;

	int minite = timer;
	int second = 0;
	int t = 0;
	for (; 60 <= minite;)
	{
		minite /= 60;
		if (minite <= 60)
		{
			second = minite;
		}
		++t;
	}
	//分
	minitueHandle = CountNumber(t);
	timeHandle = CountNumber(second);
	miriHandle = CountNumber(flame);

	finishTimer = 0;

	const Vec2<float>size = { 600,200 };
	Vec2<float> p;
	p.x = timerPos.x + (size.x / 2);
	p.y = timerPos.y + (size.y / 2) + 50;

	//timerPos = p;

	counDownFinishFlag = false;

}

void GameTimer::Finalize()
{
}

void GameTimer::Update()
{
	startFlag = countDownFlag;
	if (startFlag && !timeUpFlag)
	{
		//時間のカウント
		if (0 < flame)
		{
			flame--;
		}
		else
		{
			timer--;
			flame = 59;
		}

		//時間切れ
		if (timer <= 0 && flame <= 0)
		{
			timeUpFlag = true;
		}

		int minite = timer;
		int t = 0;
		for (; 60 <= minite;)
		{
			minite /= 60;
			++t;
		}
		//分
		minitueHandle = CountNumber(t);

		//秒
		timeHandle = CountNumber(timer - 60 * t);

		//ミリ秒
		int tmp = (flame / 60) * 100;
		miriHandle = CountNumber(tmp);
	}

	if (timeUpFlag)
	{
		finishTimer++;
		if (180 <= finishTimer)
		{

		}
	}
}

void GameTimer::Draw()
{
	Vec2<float>centralPos;
	int offset = 0;
	float size = 0.8f;
	//分
	for (int i = 0; i < minitueHandle.size(); i++)
	{
		offset = i;
		centralPos = { timerPos.x + i * texSize.x, timerPos.y };
		DrawFunc::DrawRotaGraph2D(centralPos, Vec2<float>(size, size), 0.0f, TexHandleMgr::GetTexBuffer(number[minitueHandle[i]]));
	}

	++offset;
	centralPos = { timerPos.x + offset * texSize.x,timerPos.y };
	DrawFunc::DrawRotaGraph2D(centralPos, Vec2<float>(size, size), 0.0f, TexHandleMgr::GetTexBuffer(number[10]));
	++offset;

	//秒
	for (int i = 0; i < timeHandle.size(); i++)
	{
		centralPos = { timerPos.x + (offset + i) * texSize.x, timerPos.y };
		DrawFunc::DrawRotaGraph2D(centralPos, Vec2<float>(size, size), 0.0f, TexHandleMgr::GetTexBuffer(number[timeHandle[i]]));
	}

	//float colonPos = timerPos.x + 2 * tex.GetNumTexSize().x;
	//DrawGraph(colonPos, timerPos.y-5, tex.GetColonHandle(), true);

	//ミリ秒
	//for (int i = 0; i < miriHandle.size(); i++)
	//{
	//	float miriPos = timerPos.x + 2 * tex.GetNumTexSize().x;
	//	miriPos += tex.GetColonTexSize().x;
	//	DrawGraph(miriPos + i * tex.GetNumTexSize().x, timerPos.y, number[miriHandle[i]], true);
	//}

	//カウントダウン
	/*if (countDownFlag)
	{
		DrawGraph(countDownPos.x, countDownPos.y, countDownHandle[countDownNum], true);
	}*/

}

void GameTimer::Start()
{
	countDownFlag = true;
}

int GameTimer::GetFlame()
{
	return timer;
}

int GameTimer::GetSeceond()
{
	float second = timer / 60;
	return second;
}

bool GameTimer::TimeUpFlag()
{
	return timeUpFlag;
}

bool GameTimer::StartGame()
{
	return counDownFinishFlag;
}

bool GameTimer::FinishAllEffect()
{
	return false;
}

void GameTimer::Debug()
{
	ImGui::Begin("GameTimer");
	ImGui::Text("Time:%d%d", timeHandle[0], timeHandle[1]);
	ImGui::End();
}

std::vector<int> GameTimer::CountNumber(int TIME)
{
	float score = TIME;
	std::vector<int> Number(2);
	Number[0] = -1;
	Number[1] = -1;

	int tmp = score;
	//スコア計算
	for (int i = 0; tmp > 0; i++)
	{
		float result = tmp % 10;
		//Number.push_back(result);
		Number[i] = result;
		tmp /= 10.0f;
	}
	//0埋め
	for (int i = 0; i < Number.size(); i++)
	{
		if (Number[i] == -1)
		{
			Number[i] = 0;
		}
	}
	std::reverse(Number.begin(), Number.end());
	return Number;
}
