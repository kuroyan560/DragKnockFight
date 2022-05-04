#include "WinCounter.h"
#include"ScrollMgr.h"
#include"TexHandleMgr.h"
#include"WinApp.h"
#include"KuroMath.h"
#include"DrawFunc.h"

WinCounter::WinCounter()
{
	winCountGraph_Left = TexHandleMgr::LoadGraph("resource/ChainCombat/UI/win_player.png");
	winCountGraph_Right = TexHandleMgr::LoadGraph("resource/ChainCombat/UI/win_enemy.png");
	knockOutGraph_Left = TexHandleMgr::LoadGraph("resource/ChainCombat/UI/knockOut_player.png");
	knockOutGraph_Right = TexHandleMgr::LoadGraph("resource/ChainCombat/UI/knockOut_enemy.png");
}

Vec2<float> WinCounter::GetWinCountPos(const bool& Left, const int& Num)
{
	static const Vec2<float>WIN_COUNT_POS = { 205,71 };
	static const float WIN_COUNT_OFFSET_X = 35;
	const float offsetX = WIN_COUNT_OFFSET_X * Num;

	static const int WIN_COUNT_WIDTH = TexHandleMgr::GetTexBuffer(winCountGraph_Right)->GetDesc().Width;
	static const float WIN_COUNT_RIGHT_X = WinApp::Instance()->GetExpandWinSize().x - WIN_COUNT_POS.x - WIN_COUNT_WIDTH * 4.5f;

	if (Left)return { WIN_COUNT_POS.x + offsetX, WIN_COUNT_POS.y };
	return { WIN_COUNT_RIGHT_X + offsetX, WIN_COUNT_POS.y };
}

void WinCounter::Update()
{
	if (!animation)return;

	//ノックアウトの文字アニメーション
	static const int APPEAR_TIME = 60;
	static const int WAIT_TIME = 30;
	static const int DISAPPEAR_TIME = 60;
	static const Vec2<float> APPEAR_POS = { (float)WinApp::Instance()->GetWinCenter().x, (float)WinApp::Instance()->GetWinCenter().y + 100.0f };
	const float toRad = Angle::ROUND() * 2.0f * knockOutSpinVec;

	if (knockOutTimer <= APPEAR_TIME)
	{
		/*knockOutPos = KuroMath::Ease(Out, Exp, knockOutTimer, APPEAR_TIME, knockOutAppearPos, WinApp::Instance()->GetExpandWinCenter());
		kncokOutScale = KuroMath::Ease(Out, Elastic, knockOutTimer, APPEAR_TIME, 0.0f, 1.0f);
		knockOutRadian = KuroMath::Ease(Out, Exp, knockOutTimer, APPEAR_TIME, 0.0f, toRad);*/

		knockOutPos = KuroMath::Ease(Out, Exp, knockOutTimer, APPEAR_TIME, { APPEAR_POS.x, -100.0f }, APPEAR_POS);
		kncokOutScale = 1.0f;
	}
	else if (knockOutTimer <= APPEAR_TIME + WAIT_TIME)
	{
	}
	else if (knockOutTimer <= APPEAR_TIME + WAIT_TIME + DISAPPEAR_TIME)
	{
		int timer = knockOutTimer - APPEAR_TIME - WAIT_TIME;

		knockOutPos = KuroMath::Ease(In, Cubic, timer, DISAPPEAR_TIME, APPEAR_POS, { APPEAR_POS.x, (float)WinApp::Instance()->GetWinSize().y + 100.0f });
		kncokOutScale = 1.0f;

		/*knockOutPos = KuroMath::Ease(In, Exp, timer, DISAPPEAR_TIME, WinApp::Instance()->GetExpandWinCenter(), knockOutDisappearPos);
		kncokOutScale = KuroMath::Ease(In, Back, timer, DISAPPEAR_TIME, 1.0f, 0.0f);
		knockOutRadian = KuroMath::Ease(In, Exp, timer, DISAPPEAR_TIME, toRad, 0.0f);*/
	}
	else
	{
		animation = false;

		if (knockOutGraph == knockOutGraph_Left)drawCountLeft++;
		else drawCountRight++;

		//試合終了かの判定はここでとる
		static const int WIN_MAX = 3;
		if (WIN_MAX <= drawCountLeft || WIN_MAX <= drawCountRight)
		{
			gameFinish = true;
		}
	}

	knockOutTimer++;
}

void WinCounter::Draw()
{
	//勝利数カウント(左)

	for (int i = 0; i < drawCountLeft; ++i)
	{
		DrawFunc::DrawGraph(GetWinCountPos(true, i), TexHandleMgr::GetTexBuffer(winCountGraph_Left));
	}

	//勝利数カウント(右)

	for (int i = 0; i < drawCountRight; ++i)
	{
		DrawFunc::DrawGraph(GetWinCountPos(false, i), TexHandleMgr::GetTexBuffer(winCountGraph_Right));
	}

	if (!animation)return;
	static const float KNOCK_OUT_SCALE = 0.8f;
	DrawFunc::DrawRotaGraph2D(knockOutPos, { kncokOutScale * KNOCK_OUT_SCALE,kncokOutScale * KNOCK_OUT_SCALE }, knockOutRadian, TexHandleMgr::GetTexBuffer(knockOutGraph));
}

#include"AudioApp.h"
void WinCounter::RoundFinish(const Vec2<float>& FinishPos, const bool& WinnerIsLeft, const Vec2<float>& WinnerPos)
{
	static int SE = -1;
	if (SE == -1)
	{
		SE = AudioApp::Instance()->LoadAudio("resource/ChainCombat/sound/knockout.wav");
		AudioApp::Instance()->ChangeVolume(SE, 0.25f);
	}
	AudioApp::Instance()->PlayWave(SE);

	countRound++;
	if (WinnerIsLeft)
	{
		countLeftWin++;
		knockOutDisappearPos = GetWinCountPos(true, drawCountLeft);
		knockOutGraph = knockOutGraph_Left;
		knockOutSpinVec = 1;
	}
	else
	{
		countRightWin++;
		knockOutDisappearPos = GetWinCountPos(false, drawCountRight);
		knockOutGraph = knockOutGraph_Right;
		knockOutSpinVec = -1;
	}

	const auto finishPosOnDraw = ScrollMgr::Instance()->Affect(FinishPos);
	knockOutAppearPos = finishPosOnDraw;
	knockOutPos = finishPosOnDraw;

	knockOutTimer = 0;
	animation = true;

	// ズームを付ける。
	Camera::Instance()->Focus(WinnerPos + Vec2<float>(0.0f, 50.0f), 1.8f);

}
