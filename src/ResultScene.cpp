#include "ResultScene.h"
#include "IntoTheAbyss/TexHandleMgr.h"
#include "DrawFunc.h"
#include "WinApp.h"
#include "KuroMath.h"
#include "UsersInput.h"
#include "IntoTheAbyss/ResultTransfer.h"
#include"IntoTheAbyss/ScoreKeep.h"

ResultScene::ResultScene()
{
	backGroundHandle = TexHandleMgr::LoadGraph("resource/ChainCombat/title_scene/star.png");
	winnerFrameHandle = TexHandleMgr::LoadGraph("resource/ChainCombat/result_scene/winnerFrame.png");
	resultHandle = TexHandleMgr::LoadGraph("resource/ChainCombat/result_scene/result.png");
	breakEnemyHandle = TexHandleMgr::LoadGraph("resource/ChainCombat/UI/break_enemy.png");
	breakPlayerHandle = TexHandleMgr::LoadGraph("resource/ChainCombat/UI/break_player.png");
	crashEnemyHandle = TexHandleMgr::LoadGraph("resource/ChainCombat/result_scene/crash_red.png");;
	crashPlayerHandle = TexHandleMgr::LoadGraph("resource/ChainCombat/result_scene/crash_green.png");;
	roundHandle = TexHandleMgr::LoadGraph("resource/ChainCombat/UI/round.png");;
	scoreHandle = TexHandleMgr::LoadGraph("resource/ChainCombat/UI/score.png");
	slashHandle = TexHandleMgr::LoadGraph("resource/ChainCombat/UI/score.png");
	TexHandleMgr::LoadDivGraph("resource/ChainCombat/UI/num.png", 12, { 12, 1 }, blueNumberHandle.data());
	TexHandleMgr::LoadDivGraph("resource/ChainCombat/UI/num_yellow.png", 12, { 12, 1 }, goldNumberHandle.data());

	changeScene = std::make_shared<SceneCange>();

	winnerGraph[PLAYABLE_LUNA] = TexHandleMgr::LoadGraph("resource/ChainCombat/result_scene/luna.png");
	winnerGraph[PLAYABLE_LACY] = TexHandleMgr::LoadGraph("resource/ChainCombat/result_scene/lacy.png");
	winnerGraph[PLAYABLE_BOSS_0] = TexHandleMgr::LoadGraph("resource/ChainCombat/result_scene/lacy.png");
}

void ResultScene::OnInitialize()
{
	static const int BGM = AudioApp::Instance()->LoadAudio("resource/ChainCombat/sound/bgm_1.wav");
	AudioApp::Instance()->ChangeVolume(BGM, 0.05f);

	resultUITimer = 0;
	breakEnemyUITimer = 0;
	breakPlayerUITimer = 0;
	crashEnemyUITimer = 0;
	crashPlayerUITimer = 0;
	scoreUITimer = 0;
	delayTimer = 0;
	resultEasingAmount = 0;
	breakEnemyEasingAmount = 0;
	breakPlayerEasingAmount = 0;
	crashEnemyEasingAmount = 0;
	crashPlayerEasingAmount = 0;
	scoreEffectTimer = 0;
	targetScore = ResultTransfer::Instance()->resultScore;
	prevScore = { 1,2,3,4,5,6,7,8,9 };
	scoreSize = { 1 };
	defaultSize = 3.0f;
	scoreEffectEasingAmount = 0;

	breakEnemyAmount = ResultTransfer::Instance()->rightBreakCount;
	breakPlayerAmount = ResultTransfer::Instance()->leftBreakCount;
	crashEnemyAmount = ResultTransfer::Instance()->rightCrashCount;
	crashPlayerAmount = ResultTransfer::Instance()->leftCrashCount;
	winnerName = ResultTransfer::Instance()->winner;
}

void ResultScene::OnUpdate()
{
	if (UsersInput::Instance()->KeyOnTrigger(DIK_J))
	{
		resultUITimer = 0;
		breakEnemyUITimer = 0;
		breakPlayerUITimer = 0;
		crashEnemyUITimer = 0;
		crashPlayerUITimer = 0;
		scoreUITimer = 0;
		delayTimer = 0;
		resultEasingAmount = 0;
		breakEnemyEasingAmount = 0;
		breakPlayerEasingAmount = 0;
		crashEnemyEasingAmount = 0;
		crashPlayerEasingAmount = 0;
		scoreEffectTimer = 0;
		targetScore = ResultTransfer::Instance()->resultScore;
		prevScore = { 1,2,3,4,5,6,7,8,9 };
		scoreSize = { 1.0f };
		scoreEffectEasingAmount = 0;
		defaultSize = 1.4f;
		breakEnemyAmount = ResultTransfer::Instance()->rightBreakCount;
		breakPlayerAmount = ResultTransfer::Instance()->leftBreakCount;
		crashEnemyAmount = ResultTransfer::Instance()->rightCrashCount;
		crashPlayerAmount = ResultTransfer::Instance()->leftCrashCount;
		winnerName = ResultTransfer::Instance()->winner;
		rapNumber = 3;

		maxSize = { 0.5f,0.5f };
		breakSize = { 1.5f,1.5f };
	}



	static const int SE = AudioApp::Instance()->LoadAudio("resource/ChainCombat/sound/resultScore.wav", 0.13f);

	Vec2<float> windowSize = WinApp::Instance()->GetWinSize().Float();

	// 遅延タイマーが既定値以下だったらインクリメントする。
	if (delayTimer < DELAY_TIMER) ++delayTimer;

	// 処理を飛ばすフラグ。
	bool isSkip = false;

	// 遅延が発生していなかったら。
	//フォントの横移動
	if (DELAY_TIMER <= delayTimer) {

		// [RESULT]の画像 [RESULT]のタイマーが既定値以下だったら。
		if (resultUITimer < RESULT_UI_TIMER) {
			// タイマーが0だったら
			if (resultUITimer == 0) {
				AudioApp::Instance()->PlayWave(SE);
			}
			++resultUITimer;
			// タイマーが規定値に達したら。
			if (RESULT_UI_TIMER <= resultUITimer) {
				delayTimer = DELAY_TIMER;
				isSkip = true;
				AudioApp::Instance()->PlayWave(SE);
			}
		}
		// [BREAK]敵の画像 タイマーが規定値以下だったら。
		if (!isSkip && breakEnemyUITimer < BREAK_ENEMY_UI_TIMER && RESULT_UI_TIMER <= resultUITimer) {
			++breakEnemyUITimer;
			// タイマーが規定値に達したら。
			if (BREAK_ENEMY_UI_TIMER <= breakEnemyUITimer) {
				delayTimer = DELAY_TIMER;
				isSkip = true;
				AudioApp::Instance()->PlayWave(SE);
			}
		}
		// [CRASH]敵の画像 タイマーが規定値以下だったら。
		if (!isSkip && crashEnemyUITimer < CRASH_ENEMY_UI_TIMER && BREAK_ENEMY_UI_TIMER <= breakEnemyUITimer) {
			++crashEnemyUITimer;
			// タイマーが規定値に達したら。
			if (CRASH_ENEMY_UI_TIMER <= crashEnemyUITimer) {
				delayTimer = DELAY_TIMER;
				isSkip = true;
				AudioApp::Instance()->PlayWave(SE);
			}
		}
		// [BREAK]プレイヤーの画像 タイマーが規定値以下だったら。
		if (!isSkip && breakPlayerUITimer < BREAK_PLAYER_UI_TIMER && CRASH_ENEMY_UI_TIMER <= crashEnemyUITimer) {
			++breakPlayerUITimer;
			// タイマーが規定値に達したら。
			if (BREAK_PLAYER_UI_TIMER <= breakPlayerUITimer) {
				delayTimer = DELAY_TIMER;
				isSkip = true;
				AudioApp::Instance()->PlayWave(SE);
			}
		}
		// [CRASH]プレイヤーの画像 タイマーが規定値以下だったら。
		if (!isSkip && crashPlayerUITimer < CRASH_PLAYER_UI_TIMER && BREAK_PLAYER_UI_TIMER <= breakPlayerUITimer) {
			++crashPlayerUITimer;
			// タイマーが規定値に達したら。
			if (CRASH_PLAYER_UI_TIMER <= crashPlayerUITimer) {
				delayTimer = DELAY_TIMER;
				isSkip = true;
			}
		}
		// [SCORE]画像 タイマーが規定値以下だったら。
		if (scoreUITimer < SCORE_UI_TIMER && CRASH_PLAYER_UI_TIMER <= crashPlayerUITimer) {
			++scoreUITimer;
			// タイマーが規定値に達したら。
			if (SCORE_UI_TIMER <= scoreUITimer) {
				delayTimer = -180;
			}
			else {
				delayTimer = DELAY_TIMER;
			}
		}
		// スコアのタイマーが規定値以下だったら。
		if (scoreEffectTimer < SCORE_EFFECT_TIMER && SCORE_UI_TIMER <= scoreUITimer) {
			++scoreEffectTimer;
			// タイマーが規定値に達したら。
			if (SCORE_EFFECT_TIMER <= scoreEffectTimer) {
				delayTimer = 0;
			}
			else {
				delayTimer = DELAY_TIMER;
			}
		}

	}

	// イージング量を更新。
	//イージングの動き
	resultEasingAmount = KuroMath::Ease(Out, Cubic, (float)resultUITimer / RESULT_UI_TIMER, 0.0f, 1.0f);
	breakEnemyEasingAmount = KuroMath::Ease(Out, Cubic, (float)breakEnemyUITimer / BREAK_ENEMY_UI_TIMER, 0.0f, 1.0f);
	breakPlayerEasingAmount = KuroMath::Ease(Out, Cubic, (float)breakPlayerUITimer / BREAK_PLAYER_UI_TIMER, 0.0f, 1.0f);
	crashEnemyEasingAmount = KuroMath::Ease(Out, Cubic, (float)crashEnemyUITimer / CRASH_ENEMY_UI_TIMER, 0.0f, 1.0f);
	crashPlayerEasingAmount = KuroMath::Ease(Out, Cubic, (float)crashPlayerUITimer / CRASH_PLAYER_UI_TIMER, 0.0f, 1.0f);
	scoreEasingAmount = KuroMath::Ease(Out, Cubic, (float)scoreUITimer / SCORE_UI_TIMER, 0.0f, 1.0f);
	scoreEffectEasingAmount = KuroMath::Ease(Out, Cubic, (float)scoreEffectTimer / SCORE_EFFECT_TIMER, 0.0f, 1.0f);

	// スコアエフェクトのタイマーの割合が0.9を超えたら、一気に全部変える。
	if (0.97f <= (float)scoreEffectTimer / SCORE_EFFECT_TIMER && (float)scoreEffectTimer / SCORE_EFFECT_TIMER <= 0.99f) {

		scoreEffectTimer = SCORE_EFFECT_TIMER;

		for (int index = 0; index < 10; ++index) {
			scoreSize[index] = defaultSize + 1.7f;
		}

	}

	/*switch (rapNumber)
	{
	case 1:
		BREAK_ENEMY_UI_TIMER = 20;
		BREAK_PLAYER_UI_TIMER = 2;
		CRASH_ENEMY_UI_TIMER = 2;
		CRASH_PLAYER_UI_TIMER = 2;

		BREAK_ENEMY_POS = { (float)WINDOW_CENTER.x - 110.0f, 350.0f };
		break;
	case 2:
		BREAK_ENEMY_UI_TIMER = 20;
		BREAK_PLAYER_UI_TIMER = 2;

		CRASH_PLAYER_UI_TIMER = 2;

		BREAK_ENEMY_POS = { (float)WINDOW_CENTER.x - 110.0f, 300.0f };
		CRASH_ENEMY_POS = { (float)WINDOW_CENTER.x - 110.0f, 400.0f };
		break;
	case 3:

		BREAK_PLAYER_UI_TIMER = 20;
		CRASH_ENEMY_UI_TIMER = 20;
		CRASH_PLAYER_UI_TIMER = 2;


		BREAK_PLAYER_POS = { (float)WINDOW_CENTER.x - 110.0f, 460.0f };
		break;
	default:
		break;
	}*/

	BREAK_ENEMY_UI_TIMER = 20;
	CRASH_ENEMY_UI_TIMER = 20;
	BREAK_ENEMY_POS = { (float)WINDOW_CENTER.x - 200.0f , 360.0f };
	CRASH_ENEMY_POS = { (float)WINDOW_CENTER.x - 150.0f, 380.0f };
	breakEnemyAmount = 0;
	// リザルト画面へ飛ばす
	if (UsersInput::Instance()->ControllerOnTrigger(0, XBOX_BUTTON::A)) {
		static int bgm = AudioApp::Instance()->LoadAudio("resource/ChainCombat/sound/bgm_1.wav");
		AudioApp::Instance()->StopWave(bgm);
		KuroEngine::Instance().ChangeScene(0, changeScene);
	}

}

void ResultScene::OnDraw()
{

	KuroEngine::Instance().Graphics().SetRenderTargets({ D3D12App::Instance()->GetBackBuffRenderTarget() });

	ResultTransfer::Instance()->Draw();

	Vec2<float> windowSize = { (float)WinApp::Instance()->GetWinSize().x, (float)WinApp::Instance()->GetWinSize().y };
	//DrawFunc::DrawBox2D(Vec2<float>(0, 0), windowSize, Color(0, 0, 0, 255), DXGI_FORMAT_R8G8B8A8_UNORM);
	//DrawFunc::DrawGraph(Vec2<float>(0, 0), TexHandleMgr::GetTexBuffer(winnerFrameHandle));
	//DrawFunc::DrawGraph({ 25.0f,30.0f }, TexHandleMgr::GetTexBuffer(winnerGraph[winnerName]));

	// [RESULT] と [BREAK]の描画処理



	{
		float easingPosX = resultEasingAmount * (RESULT_POS.x - windowSize.x);
		DrawFunc::DrawGraph(Vec2<float>(windowSize.x + easingPosX, RESULT_POS.y), TexHandleMgr::GetTexBuffer(resultHandle));

		//DrawFunc::DrawRotaGraph2D(Vec2<float>(windowSize.x + easingPosX - 20.0f, ROUND_POS.y) + Vec2<float>(220.0f, 0.0f), Vec2<float>(0.5f, 0.5f), 0.0f, TexHandleMgr::GetTexBuffer(roundHandle));
		//DrawFunc::DrawGraph(Vec2<float>(windowSize.x + easingPosX + 90.0f, 155.0f) + Vec2<float>(300.0f, 0.0f), TexHandleMgr::GetTexBuffer(scoreHandle));


		DrawBreakCount(breakEnemyEasingAmount, 1000, 15000);

		//DrawBREAK(BREAK_ENEMY_POS, breakEnemyEasingAmount, blueNumberHandle[0], breakEnemyAmount, false);
		//DrawBREAK(CRASH_ENEMY_POS, breakEnemyEasingAmount, blueNumberHandle[0], breakEnemyAmount, true);

		//DrawBREAK(BREAK_ENEMY_POS, breakEnemyEasingAmount, breakEnemyHandle, breakEnemyAmount);
		//DrawBREAK(CRASH_ENEMY_POS, crashEnemyEasingAmount, crashEnemyHandle, crashEnemyAmount);
		//DrawBREAK(BREAK_PLAYER_POS, breakPlayerEasingAmount, breakPlayerHandle, breakPlayerAmount);
		//DrawBREAK(CRASH_PLAYER_POS, crashPlayerEasingAmount, crashPlayerHandle, crashPlayerAmount);
	}

	// [SCORE] の描画処理
	//DrawSCORE(scoreEasingAmount, scoreEffectEasingAmount);

}

void ResultScene::OnImguiDebug()
{
}

void ResultScene::OnFinalize()
{
	static const int BGM = AudioApp::Instance()->LoadAudio("resource/ChainCombat/sound/bgm_1.wav");
	AudioApp::Instance()->StopWave(BGM);
}

void ResultScene::DrawBREAK(const Vec2<float> &targetPosm, const float &easingTimer, const int &graphHandle, const int &breakCount, bool SIZE_FLAG)
{

	Vec2<float> windowSize = { (float)WinApp::Instance()->GetWinSize().x, (float)WinApp::Instance()->GetWinSize().y };

	// イージングをかけた位置を求めて[BREAK]を描画をする。
	float easingPosX = easingTimer * (targetPosm.x - windowSize.x);
	Vec2<float> drawPos = Vec2<float>(windowSize.x + easingPosX, targetPosm.y);
	//DrawFunc::DrawGraph(drawPos, TexHandleMgr::GetTexBuffer(graphHandle));

	// BREAKの画像サイズ分右に動かした位置に*を描画する。
	//drawPos = drawPos + Vec2<float>(160.0f, 0.0f);
	//DrawFunc::DrawGraph(drawPos, TexHandleMgr::GetTexBuffer(blueNumberHandle[0]));

	// 数字のフォント分移動させる。

	int FONT_SIZE = 66.3f;
	if (SIZE_FLAG)
	{
		nowSize = maxSize;
		FONT_SIZE *= nowSize.x;

		for (int i = 0; i < KuroFunc::GetDigit(breakCount) - 1; ++i)
		{
			drawPos.x += FONT_SIZE;
		}
	}
	else
	{
		nowSize = breakSize;
		FONT_SIZE *= nowSize.x;

		for (int i = 0; i < KuroFunc::GetDigit(breakCount) - 1; ++i)
		{
			drawPos.x -= FONT_SIZE;
		}
	}



	std::vector<int>number = CountNumber(breakCount);
	for (int i = 0; i < number.size(); ++i)
	{
		DrawFunc::DrawRotaGraph2D(drawPos, nowSize, 0.0f, TexHandleMgr::GetTexBuffer(blueNumberHandle[number[i]]));
		// フォントサイズ分移動させる。
		drawPos.x += FONT_SIZE;
	}

	//// BREAKの数を描画する。
	//for (int index = 1; 0 <= index; --index) {

	//	// 数字を求めて描画する。
	//	int disit = GetDisit(breakCount, index);

	//	DrawFunc::DrawGraph(drawPos, TexHandleMgr::GetTexBuffer(blueNumberHandle[disit]));

	//	// フォントサイズ分移動させる。
	//	drawPos.x += FONT_SIZE;

	//}

	// スコアのサイズをデフォルトに近づける。
	for (int index = 0; index < 10; ++index) {
		scoreSize[index] += (defaultSize - scoreSize[index]) / 10.0f;
	}


}

void ResultScene::DrawSCORE(const float &easingTimer, const double &scoreEffectEasingTimer)
{

	Vec2<float> windowSize = { (float)WinApp::Instance()->GetWinSize().x, (float)WinApp::Instance()->GetWinSize().y };

	float easingPosX = scoreEasingAmount * (SCORE_POS.x - windowSize.x);


	SCORE_POS.y = (float)WINDOW_CENTER.y + 180.0f;
	Vec2<float> drawPos = Vec2<float>(windowSize.x + easingPosX - 150.0f, SCORE_POS.y);
	// 数字のフォントサイズ
	const int FONT_SIZE = 66.3f * defaultSize;

	// 現在の数字を求める。
	int nowScore = scoreEffectEasingTimer * targetScore;

	drawPos.y = 640.0f;

	for (int index = 10; 0 < index; --index) {

		// 数字を求めて描画する。
		int disit = GetDisit(nowScore, index - 1);

		// スコアが前フレームの値と違っていたら大きくする。
		if (prevScore[index - 1] != disit) scoreSize[index - 1] = 1.5f;

		DrawFunc::DrawRotaGraph2D(drawPos, Vec2<float>(scoreSize[index - 1], scoreSize[index - 1]), 0, TexHandleMgr::GetTexBuffer(goldNumberHandle[disit]));

		// フォントサイズ分移動させる。
		drawPos.x += FONT_SIZE;

		// このフレームのスコアを保存。
		prevScore[index - 1] = disit;

	}

}

void ResultScene::DrawBreakCount(float scoreEasingAmount, int BREAK_NOW_COUNT, int BREAK_MAX_COUNT)
{
	Vec2<float> windowSize = { (float)WinApp::Instance()->GetWinSize().x, (float)WinApp::Instance()->GetWinSize().y };

	const float baseX = 500.0f;
	float easingPosX = scoreEasingAmount * (windowSize.x / 2.0f+ baseX);
	Vec2<float> drawPos = Vec2<float>((windowSize.x+ baseX) - easingPosX, 430.0f);


	const int FONT_SIZE = 66.3f;
	//現在
	{
		nowSize = breakSize;
		for (int i = 0; i < KuroFunc::GetDigit(BREAK_NOW_COUNT) - 1; ++i)
		{
			drawPos.x -= FONT_SIZE * nowSize.x;
		}

		std::vector<int>number = CountNumber(BREAK_NOW_COUNT);
		for (int i = 0; i < number.size(); ++i)
		{
			DrawFunc::DrawRotaGraph2D(drawPos, nowSize, 0.0f, TexHandleMgr::GetTexBuffer(blueNumberHandle[number[i]]));
			// フォントサイズ分移動させる。
			drawPos.x += FONT_SIZE * nowSize.x;
		}
	}

	//スラッシュ
	//DrawFunc::DrawRotaGraph2D(drawPos, nowSize, 0.0f, TexHandleMgr::GetTexBuffer(slashHandle));

	//最大
	{
		drawPos.x += FONT_SIZE * (nowSize.x / 2.0f);
		drawPos.y += 25.0f;
		nowSize = maxSize;
		std::vector<int>number = CountNumber(BREAK_MAX_COUNT);
		for (int i = 0; i < number.size(); ++i)
		{
			DrawFunc::DrawRotaGraph2D(drawPos, nowSize, 0.0f, TexHandleMgr::GetTexBuffer(blueNumberHandle[number[i]]));
			// フォントサイズ分移動させる。
			drawPos.x += FONT_SIZE * nowSize.x;
		}
	}
}
