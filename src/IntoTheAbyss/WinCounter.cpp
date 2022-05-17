#include "WinCounter.h"
#include"ScrollMgr.h"
#include"TexHandleMgr.h"
#include"WinApp.h"
#include"KuroMath.h"
#include"DrawFunc.h"
#include"DrawFunc_Mask.h"

WinCounter::WinCounter()
{
	winCountGraph_Left = TexHandleMgr::LoadGraph("resource/ChainCombat/UI/win_player.png");
	winCountGraph_Right = TexHandleMgr::LoadGraph("resource/ChainCombat/UI/win_enemy.png");
	knockOutGraph_Left = TexHandleMgr::LoadGraph("resource/ChainCombat/UI/knockOut_player.png");
	knockOutGraph_Right = TexHandleMgr::LoadGraph("resource/ChainCombat/UI/knockOut_enemy.png");
	TexHandleMgr::LoadDivGraph("resource/ChainCombat/UI/knockout_back/luna.png", 3, Vec2<int>(3, 1), lunaHandle.data());
	TexHandleMgr::LoadDivGraph("resource/ChainCombat/UI/knockout_back/lacy.png", 3, Vec2<int>(3, 1), lacyHandle.data());
	TexHandleMgr::LoadDivGraph("resource/ChainCombat/UI/knockout_back/mask.png", 5, Vec2<int>(5, 1), maskHandle.data());
	TexHandleMgr::LoadDivGraph("resource/ChainCombat/UI/knockout_back/mask_frame.png", 5, Vec2<int>(5, 1), maskFrameHandle.data());
	maskAnimHandle = 0;
	lunaAnimHandle = 0;
	knockOutPhase = KNOCK_OUT_PHASE::START_PHASE;
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
	const int START_PHASE_TIME = 60;			// 開始フェーズの時間
	const int CHARA_APPEAR_PHASE_TIME = 5;		// キャラ出現フェーズの時間 このフェーズはなくなりそう
	const int STOP_PHASE_TIMER = 50;			// 一時停止フェーズの時間
	const int CHARA_EXIT_PHASE_TIME = 5;		// キャラ退出フェーズの時間 このフェーズも無くなりそう。
	const int END_PHASE_TIME = 60;				// 終了フェーズの時間
	const int CHARA_ANIM_FRAME = 5;				// キャラの1アニメーションにかかる時間
	const Vec2<float> KNOCK_OUT_APPEAR_POS = { static_cast<float>(WinApp::Instance()->GetWinCenter().x), static_cast<float>(WinApp::Instance()->GetWinCenter().y + 200.0f) };
	const Vec2<float> MASK_POS = { static_cast<float>(WinApp::Instance()->GetWinCenter().x), static_cast<float>(WinApp::Instance()->GetWinCenter().y) };
	const float toRad = Angle::ROUND() * 2.0f * knockOutSpinVec;

	// 現在のフェーズによってノックアウトの演出の更新処理を行う。
	switch (knockOutPhase)
	{
	case WinCounter::KNOCK_OUT_PHASE::START_PHASE:

		// 開始フェーズ

		// [KnockOut!!!!]の画像を動かす。
		knockOutPos = KuroMath::Ease(Out, Exp, knockOutTimer, START_PHASE_TIME, { KNOCK_OUT_APPEAR_POS.x, -100.0f }, KNOCK_OUT_APPEAR_POS);

		// [KnockOut!!!!]にかけるマスクの座標を動かす。
		maskPos = KuroMath::Ease(Out, Exp, knockOutTimer, START_PHASE_TIME, { -static_cast<float>(WinApp::Instance()->GetWinSize().x), static_cast<float>(-WinApp::Instance()->GetWinSize().y) }, MASK_POS);

		// マスクの内側のキャラクターを動かす。
		charaPos = KuroMath::Ease(Out, Exp, knockOutTimer, START_PHASE_TIME, { static_cast<float>(WinApp::Instance()->GetWinSize().x + WinApp::Instance()->GetWinCenter().x), static_cast<float>(WinApp::Instance()->GetWinSize().y + WinApp::Instance()->GetWinCenter().y) }, MASK_POS);

		// タイマーが10の倍数になったらアニメーションハンドルを加算する。
		if (knockOutTimer != 0 && knockOutTimer % 10 == 0 && maskAnimHandle < maskHandle.size() - 1)++maskAnimHandle;

		// キャラクターをアニメーションさせる。
		if (knockOutTimer % CHARA_ANIM_FRAME == 0) {

			++lunaAnimHandle;

			if (lunaHandle.size() <= lunaAnimHandle) {
				lunaAnimHandle = 0;
			}

		}

		// タイマーが規定値に達したら、次のフェーズへ移行する。
		if (START_PHASE_TIME <= knockOutTimer) {

			knockOutTimer = 0;
			knockOutPhase = KNOCK_OUT_PHASE::CHARA_APPEAR_PHASE;

		}

		break;
	case WinCounter::KNOCK_OUT_PHASE::CHARA_APPEAR_PHASE:

		// キャラ出現フェーズ

		// キャラクターをアニメーションさせる。
		if (knockOutTimer % CHARA_ANIM_FRAME == 0) {

			++lunaAnimHandle;

			if (lunaHandle.size() <= lunaAnimHandle) {
				lunaAnimHandle = 0;
			}

		}

		// タイマーが規定値に達したら、次のフェーズへ移行する。
		if (CHARA_APPEAR_PHASE_TIME <= knockOutTimer) {

			knockOutTimer = 0;
			knockOutPhase = KNOCK_OUT_PHASE::STOP_PHASE;

		}

		break;
	case WinCounter::KNOCK_OUT_PHASE::STOP_PHASE:

		// 一時停止フェーズ

		// キャラクターをアニメーションさせる。
		if (knockOutTimer % CHARA_ANIM_FRAME == 0) {

			++lunaAnimHandle;

			if (lunaHandle.size() <= lunaAnimHandle) {
				lunaAnimHandle = 0;
			}

		}

		// タイマーが規定値に達したら、次のフェーズへ移行する。
		if (STOP_PHASE_TIMER <= knockOutTimer) {

			knockOutTimer = 0;
			knockOutPhase = KNOCK_OUT_PHASE::CHARA_EXIT_PHASE;

		}

		break;
	case WinCounter::KNOCK_OUT_PHASE::CHARA_EXIT_PHASE:

		// キャラ退出フェーズ

		// キャラクターをアニメーションさせる。
		if (knockOutTimer % CHARA_ANIM_FRAME == 0) {

			++lunaAnimHandle;

			if (lunaHandle.size() <= lunaAnimHandle) {
				lunaAnimHandle = 0;
			}

		}

		// タイマーが規定値に達したら、次のフェーズへ移行する。
		if (CHARA_EXIT_PHASE_TIME <= knockOutTimer) {

			knockOutTimer = 0;
			knockOutPhase = KNOCK_OUT_PHASE::END_PHASE;

		}

		break;
	case WinCounter::KNOCK_OUT_PHASE::END_PHASE:

		// 最終フェーズ

		// キャラクターをアニメーションさせる。
		if (knockOutTimer % CHARA_ANIM_FRAME == 0) {

			++lunaAnimHandle;

			if (lunaHandle.size() <= lunaAnimHandle) {
				lunaAnimHandle = 0;
			}

		}

		// タイマーが10の倍数になったらアニメーションハンドルを減算する。
		if (knockOutTimer != 0 && knockOutTimer % 10 == 0 && 0 < maskAnimHandle)--maskAnimHandle;

		// [KnockOut!!!!]の画像を動かす。
		knockOutPos = KuroMath::Ease(In, Exp, knockOutTimer, END_PHASE_TIME / 2.0f, KNOCK_OUT_APPEAR_POS, { KNOCK_OUT_APPEAR_POS.x, static_cast<float>(WinApp::Instance()->GetWinSize().y) + 300.0f });

		// [KnockOut!!!!]にかけるマスクの座標を動かす。
		maskPos = KuroMath::Ease(In, Exp, knockOutTimer, END_PHASE_TIME, MASK_POS, { static_cast<float>(WinApp::Instance()->GetWinSize().x + WinApp::Instance()->GetWinCenter().x), static_cast<float>(WinApp::Instance()->GetWinSize().y + WinApp::Instance()->GetWinCenter().y) });

		// マスクの内側のキャラクターを動かす。
		charaPos = KuroMath::Ease(In, Exp, knockOutTimer, END_PHASE_TIME, MASK_POS, { -static_cast<float>(WinApp::Instance()->GetWinSize().x), static_cast<float>(-WinApp::Instance()->GetWinSize().y) });


		// タイマーが規定値に達したら、次のフェーズへ移行する。
		if (START_PHASE_TIME <= knockOutTimer) {

			knockOutTimer = 0;
			knockOutPhase = KNOCK_OUT_PHASE::START_PHASE;
			// 終わり
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


		break;
	default:
		break;
	}



	//if (knockOutTimer <= APPEAR_TIME)
	//{
	//	/*knockOutPos = KuroMath::Ease(Out, Exp, knockOutTimer, APPEAR_TIME, knockOutAppearPos, WinApp::Instance()->GetExpandWinCenter());
	//	kncokOutScale = KuroMath::Ease(Out, Elastic, knockOutTimer, APPEAR_TIME, 0.0f, 1.0f);
	//	knockOutRadian = KuroMath::Ease(Out, Exp, knockOutTimer, APPEAR_TIME, 0.0f, toRad);*/

	//	knockOutPos = KuroMath::Ease(Out, Exp, knockOutTimer, APPEAR_TIME, { APPEAR_POS.x, -100.0f }, APPEAR_POS);
	//	kncokOutScale = 1.0f;
	//}
	//else if (knockOutTimer <= APPEAR_TIME + WAIT_TIME)
	//{
	//}
	//else if (knockOutTimer <= APPEAR_TIME + WAIT_TIME + DISAPPEAR_TIME)
	//{
	//	int timer = knockOutTimer - APPEAR_TIME - WAIT_TIME;

	//	knockOutPos = KuroMath::Ease(In, Cubic, timer, DISAPPEAR_TIME, APPEAR_POS, { APPEAR_POS.x, (float)WinApp::Instance()->GetWinSize().y + 100.0f });
	//	kncokOutScale = 1.0f;

	//	/*knockOutPos = KuroMath::Ease(In, Exp, timer, DISAPPEAR_TIME, WinApp::Instance()->GetExpandWinCenter(), knockOutDisappearPos);
	//	kncokOutScale = KuroMath::Ease(In, Back, timer, DISAPPEAR_TIME, 1.0f, 0.0f);
	//	knockOutRadian = KuroMath::Ease(In, Exp, timer, DISAPPEAR_TIME, toRad, 0.0f);*/
	//}
	//else
	//{
	//	animation = false;

	//	if (knockOutGraph == knockOutGraph_Left)drawCountLeft++;
	//	else drawCountRight++;

	//	//試合終了かの判定はここでとる
	//	static const int WIN_MAX = 3;
	//	if (WIN_MAX <= drawCountLeft || WIN_MAX <= drawCountRight)
	//	{
	//		gameFinish = true;
	//	}
	//}

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

	// マスクを描画。
	//DrawFunc::DrawRotaGraph2D(maskPos, Vec2<float>(1.0f, 1.0f), 0.0f, TexHandleMgr::GetTexBuffer(maskHandle[maskAnimHandle]));

	// マスクの枠を描画。
	DrawFunc::DrawRotaGraph2D(maskPos, Vec2<float>(1.0f, 1.0f), 0.0f, TexHandleMgr::GetTexBuffer(maskFrameHandle[maskAnimHandle]));

	// マスクの内側のキャラクターを描画。
	//DrawFunc::DrawRotaGraph2D(maskPos, Vec2<float>(1.0f, 1.0f), 0.0f, TexHandleMgr::GetTexBuffer(maskHandle[maskAnimHandle]));
	DrawFunc_Mask::DrawGraphByMaskGraph(charaPos, TexHandleMgr::GetTexBuffer(lunaHandle[lunaAnimHandle]), maskPos, TexHandleMgr::GetTexBuffer(maskHandle[maskAnimHandle]));
	//DrawFunc_Mask::DrawRotaGraph2D(maskPos, Vec2<float>(5.0f, 5.0f), 0, TexHandleMgr::GetTexBuffer(lunaHandle[lunaAnimHandle]), maskPos, Vec2<float>(static_cast<float>(WinApp::Instance()->GetWinSize().x), static_cast<float>(WinApp::Instance()->GetWinSize().y)));
	//DrawFunc::DrawRotaGraph2D(charaPos, Vec2<float>(1.0f, 1.0f), 0.0f, TexHandleMgr::GetTexBuffer(lunaHandle[lunaAnimHandle]));

	// [KnockOut!!!!]の画像を描画。
	DrawFunc::DrawRotaGraph2D(knockOutPos, { 1.0f * KNOCK_OUT_SCALE,1.0f * KNOCK_OUT_SCALE }, 0.0f, TexHandleMgr::GetTexBuffer(knockOutGraph));

}

#include"AudioApp.h"
void WinCounter::RoundFinish(const Vec2<float>& FinishPos, const bool& WinnerIsLeft, const Vec2<float>& WinnerPos)
{
	static const int SE = AudioApp::Instance()->LoadAudio("resource/ChainCombat/sound/knockout.wav", 0.25f);
	AudioApp::Instance()->PlayWave(SE);

	countRound++;
	if (WinnerIsLeft)
	{
		countLeftWin++;
		//knockOutDisappearPos = GetWinCountPos(true, drawCountLeft);
		knockOutGraph = knockOutGraph_Left;
		knockOutSpinVec = 1;
	}
	else
	{
		countRightWin++;
		//knockOutDisappearPos = GetWinCountPos(false, drawCountRight);
		knockOutGraph = knockOutGraph_Right;
		knockOutSpinVec = -1;
	}

	const auto finishPosOnDraw = ScrollMgr::Instance()->Affect(FinishPos);
	//knockOutAppearPos = finishPosOnDraw;
	knockOutPos = finishPosOnDraw;

	knockOutTimer = 0;
	animation = true;

	// ズームを付ける。
	Camera::Instance()->Focus(WinnerPos + Vec2<float>(0.0f, 50.0f), 1.8f);

}
