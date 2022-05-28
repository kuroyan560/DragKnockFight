#include "GameTimer.h"
#include"../IntoTheAbyss/TexHandleMgr.h"
#include"../Engine/DrawFunc.h"
#include"../Engine/ImguiApp.h"
#include"../Common/KuroMath.h"
#include"WinApp.h"
#include"AudioApp.h"

GameTimer::GameTimer()
{
	startFlag = false;
	timer = -1;
	flame = -1;
	timerSize = OFFSET_SIZE;
	timerAlpha = 255;
	isLessThan5SecondsLeft = false;
	centerCountDownSize = 0;
	centerCoundDownAlpha = 0;

	number.resize(12);
	texSize = { 64,44 };
	TexHandleMgr::LoadDivGraph("resource/ChainCombat/UI/num.png", 12, { 12, 1 }, number.data());

	//スコア無効、タイマーを中心に描画
	timerPos.x = texSize.x * 2.0f - 42;
	timerPos.y = 46.0f;

	countDownSE[0] = AudioApp::Instance()->LoadAudio("resource/ChainCombat/sound/voice/Voice_1.wav", 0.2f);
	countDownSE[1] = AudioApp::Instance()->LoadAudio("resource/ChainCombat/sound/voice/Voice_2.wav", 0.2f);
	countDownSE[2] = AudioApp::Instance()->LoadAudio("resource/ChainCombat/sound/voice/Voice_3.wav", 0.2f);
	countDownSE[3] = AudioApp::Instance()->LoadAudio("resource/ChainCombat/sound/voice/Voice_4.wav", 0.2f);
	countDownSE[4] = AudioApp::Instance()->LoadAudio("resource/ChainCombat/sound/voice/Voice_5.wav", 0.2f);

}

void GameTimer::Init(int TIME)
{
	startFlag = false;
	timeUpFlag = false;
	isLessThan5SecondsLeft = false;
	timer = TIME;
	flame = 0;
	timerSize = OFFSET_SIZE;
	timerAlpha = 255;
	centerCountDownSize = 0;
	centerCoundDownAlpha = 0;


	countDownNum = 3;

	countDownFlag = false;
	int minite = 0;
	int second = 0;
	int t = 0;

	if (60 <= timer)
	{
		minite = timer;
		second = 0;
		t = 0;
		for (; 60 <= minite;)
		{
			minite -= 60;
			if (minite <= 60)
			{
				second = minite;
			}
			++t;
		}
	}
	else
	{
		minite = 0;
		second = timer;
		t = 0;
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

	//登場演出のため最初は動かない
	interruput = true;
}

void GameTimer::Finalize()
{
}

void GameTimer::Update()
{
	if (interruput)return;

	startFlag = countDownFlag;
	if (startFlag && !timeUpFlag)
	{

		//時間切れ
		if (timer <= 1 && flame <= 0)
		{
			timeUpFlag = true;
		}

		//時間のカウント
		if (0 < flame)
		{
			flame--;
		}
		else
		{
			timer--;
			flame = 59;

			// タイマーが5以下だったらサウンドを鳴らす。
			if (timer <= 5 && timer != 0) {

				AudioApp::Instance()->PlayWave(countDownSE[timer - 1]);
				isLessThan5SecondsLeft = true;

				centerCoundDownAlpha = 0;
				centerCountDownSize = 10.0f;


			}

			// タイマーが10以下だったら強調表示をする。
			if (timer <= 10 && 5 < timer) {

				// 10 ~ 5になるにあたってだんだん演出を派手にする。
				float rate = (timer - 5.0f) / 5.0f;
				rate = 1.0f - rate + 0.5f;

				timerSize = 5.0f * rate;
				timerAlpha = 0;

			}
			else {

				timerSize = 1.8f;

			}

		}

		int minite = timer;
		int t = 0;
		for (; 60 <= minite;)
		{
			minite -= 60;
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

	// 残り五秒以下だったら
	if (isLessThan5SecondsLeft) {

		// UIのサイズを0に近づける。
		timerSize += (-timerSize) / 5.0f;

		// UIのアルファ値を0に近づける。
		timerAlpha += (-timerAlpha) / 5.0f;

		// 残りの時間に応じてサイズに加算する量を足す。
		float rate = 1.0f - (timer / 5.0f);

		rate *= 1.0f;

		// タイマーが0だったらサイズなどなどを0に近づける
		if (timer == 0) {

			centerCountDownSize -= centerCountDownSize / 5.0f;

		}
		else {

			centerCountDownSize += (1.3f + rate - centerCountDownSize) / 5.0f;
			centerCoundDownAlpha += (255 - centerCoundDownAlpha) / 5.0f;

		}

	}
	else {

		// UIのサイズをデフォルトに近づける。
		timerSize += (OFFSET_SIZE - timerSize) / 5.0f;

		// UIのアルファ値を255に近づける。
		timerAlpha += (255 - timerAlpha) / 5.0f;

	}

}

void GameTimer::Draw()
{

	// 五秒以下だったら
	if (isLessThan5SecondsLeft) {

		int num = timer;

		if (num == 0) num = 1;

		DrawFunc::DrawRotaGraph2D(WinApp::Instance()->GetExpandWinCenter(), Vec2<float>(centerCountDownSize, centerCountDownSize), 0, TexHandleMgr::GetTexBuffer(number[num]), Color(255, 255, 255, centerCoundDownAlpha));

	}
	else {

		Vec2<float>centralPos;
		int offset = 0;
		int offsetY = 15;
		//分
		for (int i = 0; i < minitueHandle.size(); i++)
		{
			offset = i;
			centralPos = { timerPos.x + i * texSize.x, timerPos.y + offsetY };
			DrawFunc::DrawRotaGraph2D(centralPos, Vec2<float>(timerSize, timerSize), 0.0f, TexHandleMgr::GetTexBuffer(number[minitueHandle[i]]), Color(255, 255, 255, timerAlpha));
		}

		++offset;
		centralPos = { timerPos.x + offset * texSize.x,timerPos.y + offsetY };
		DrawFunc::DrawRotaGraph2D(centralPos, Vec2<float>(timerSize, timerSize), 0.0f, TexHandleMgr::GetTexBuffer(number[10]), Color(255, 255, 255, timerAlpha));
		++offset;

		//秒
		for (int i = 0; i < timeHandle.size(); i++)
		{
			centralPos = { timerPos.x + (offset + i) * texSize.x, timerPos.y + offsetY };
			DrawFunc::DrawRotaGraph2D(centralPos, Vec2<float>(timerSize, timerSize), 0.0f, TexHandleMgr::GetTexBuffer(number[timeHandle[i]]), Color(255, 255, 255, timerAlpha));
		}

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
