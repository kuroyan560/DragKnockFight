#include "ResultScene.h"
#include "IntoTheAbyss/TexHandleMgr.h"
#include "DrawFunc.h"
#include "WinApp.h"
#include "KuroMath.h"
#include "UsersInput.h"
#include "IntoTheAbyss/ResultTransfer.h"
#include"IntoTheAbyss/ScoreKeep.h"
#include"IntoTheAbyss/EavaluationDataMgr.h"
#include"IntoTheAbyss/DebugKeyManager.h"

ResultScene::ResultScene()
{
	backGroundHandle = TexHandleMgr::LoadGraph("resource/ChainCombat/title_scene/star.png");
	winnerFrameHandle = TexHandleMgr::LoadGraph("resource/ChainCombat/result_scene/winnerFrame.png");
	resultHandle = TexHandleMgr::LoadGraph("resource/ChainCombat/result_scene/result.png");
	breakEnemyHandle = TexHandleMgr::LoadGraph("resource/ChainCombat/UI/break_enemy.png");
	roundHandle = TexHandleMgr::LoadGraph("resource/ChainCombat/UI/round.png");
	TexHandleMgr::LoadDivGraph("resource/ChainCombat/UI/num.png", 12, { 12, 1 }, blueNumberHandle.data());
	slashHandle = blueNumberHandle[11];

	goodHandle = TexHandleMgr::LoadGraph("resource/ChainCombat/UI/good.png");
	greatHandle = TexHandleMgr::LoadGraph("resource/ChainCombat/UI/great.png");
	excellentHandle = TexHandleMgr::LoadGraph("resource/ChainCombat/UI/excellent.png");
	TexHandleMgr::LoadDivGraph("resource/ChainCombat/UI/perfect.png", 3, Vec2<int>(3, 1), perfectHandle.data());


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
	delayTimer = 0;
	resultEasingAmount = 0;
	breakCountEasingAmount = 0;
	winnerName = ResultTransfer::Instance()->winner;

	maxSize = { 0.5f,0.5f };
	breakSize = { 1.5f,1.5f };

	breakCount = 0;
	baseBreakCount = -1;

	scoreEffectTimer = 0;

	for (int i = 0; i < scoreSize.size(); ++i)
	{
		scoreSize[i] = 1.5f;
	}
	defaultSize = 1.5f;
	bigFontFlag = false;

	evaluationEasingTimer = 0;
	intervalTimer = 0;
	evaluationFlag = false;

	perfectInterval = 0;
	perfectIndex = 0;
}

void ResultScene::OnUpdate()
{
	if (DebugKeyManager::Instance()->DebugKeyTrigger(DIK_J, "ResetResult", TO_STRING(DIK_J)))
	{
		resultUITimer = 0;
		breakEnemyUITimer = 0;
		delayTimer = 0;
		resultEasingAmount = 0;
		breakCountEasingAmount = 0;
		winnerName = ResultTransfer::Instance()->winner;

		maxSize = { 0.5f,0.5f };
		breakSize = { 1.5f,1.5f };

		breakCount = 0;
		baseBreakCount = -1;

		scoreEffectTimer = 0;

		for (int i = 0; i < scoreSize.size(); ++i)
		{
			scoreSize[i] = 1.5f;
		}
		defaultSize = 1.5f;
		bigFontFlag = false;

		evaluationEasingTimer = 0;
		intervalTimer = 0;
		evaluationFlag = false;

		perfectInterval = 0;
		perfectIndex = 0;
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
		if (!isSkip && breakEnemyUITimer < BREAK_COUNTUI_TIMER && RESULT_UI_TIMER <= resultUITimer) {
			++breakEnemyUITimer;
			// タイマーが規定値に達したら。
			if (BREAK_COUNTUI_TIMER <= breakEnemyUITimer) {
				baseBreakCount = ScoreKeep::Instance()->GetAddScore();

				delayTimer = DELAY_TIMER;
				isSkip = true;
				AudioApp::Instance()->PlayWave(SE);
			}
		}

		// スコアのタイマーが規定値以下だったら。
		if (scoreEffectTimer < SCORE_EFFECT_TIMER && BREAK_COUNTUI_TIMER <= breakEnemyUITimer) {
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
	resultEasingAmount = KuroMath::Ease(Out, Cubic,  static_cast<float>(resultUITimer) / static_cast<float>( RESULT_UI_TIMER), 0.0f, 1.0f);
	breakCountEasingAmount = KuroMath::Ease(Out, Cubic,  static_cast<float>(breakEnemyUITimer) / static_cast<float>(BREAK_COUNTUI_TIMER), 0.0f, 1.0f);




	// リザルト画面へ飛ばす
	if (UsersInput::Instance()->ControllerOnTrigger(0, XBOX_BUTTON::A)) {
		static int bgm = AudioApp::Instance()->LoadAudio("resource/ChainCombat/sound/bgm_1.wav");
		AudioApp::Instance()->StopWave(bgm);
		KuroEngine::Instance().ChangeScene(1, changeScene);
	}


	// スコアのサイズをデフォルトに近づける。
	for (int index = 0; index < 10; ++index) {
		scoreSize[index] += (defaultSize - scoreSize[index]) / 10.0f;
	}


	// スコアエフェクトのタイマーの割合が0.9を超えたら、一気に全部変える。
	if (ceil(breakCount) == baseBreakCount && !bigFontFlag)
	{
		scoreEffectTimer = SCORE_EFFECT_TIMER;

		for (int index = 0; index < 10; ++index) {
			scoreSize[index] = defaultSize + 1.7f;
		}
		bigFontFlag = true;
	}

	breakCount = KuroMath::Ease(In, Cubic, static_cast<float>(scoreEffectTimer) / static_cast<float>(SCORE_EFFECT_TIMER), 0.0f, 1.0f) * baseBreakCount;


	float rate = static_cast<float>(baseBreakCount) / static_cast<float>(ScoreKeep::Instance()->GetMaxNum());
	const float GOOD_RATE = EavaluationDataMgr::Instance()->GOOD_RATE;
	const float GREAT_RATE = EavaluationDataMgr::Instance()->GREAT_RATE;

	if (bigFontFlag)
	{
		++intervalTimer;
	}
	timeUpFlag = 35 <= intervalTimer;

	if (bigFontFlag && evaluationEasingTimer < EVALUATION_EFFECT_TIMER && timeUpFlag)
	{
		++evaluationEasingTimer;
		easeEvaluationPosY = -KuroMath::Ease(Out, Back, static_cast<float>(evaluationEasingTimer) / static_cast<float>(EVALUATION_EFFECT_TIMER), 0.0f, 1.0f) * 30.0f;
	}

	if (rate <= GOOD_RATE)
	{
		evaluationNowHandle = goodHandle;
	}
	else if (rate <= GREAT_RATE)
	{
		evaluationNowHandle = greatHandle;
	}
	else
	{
		evaluationFlag = true;
	}

	if (evaluationFlag)
	{
		if (EavaluationDataMgr::Instance()->PERFECT_ANIMATION_INTERVAL <= perfectInterval && perfectIndex < 2)
		{
			++perfectIndex;
			perfectInterval = 0;
		}
		else if (EavaluationDataMgr::Instance()->PERFECT_ANIMATION_INTERVAL <= perfectInterval && 2 <= perfectIndex)
		{
			perfectIndex = 0;
			perfectInterval = 0;
		}
		else
		{
			++perfectInterval;
		}
	}


}

void ResultScene::OnDraw()
{

	KuroEngine::Instance().Graphics().SetRenderTargets({ D3D12App::Instance()->GetBackBuffRenderTarget() });

	ResultTransfer::Instance()->Draw();

	Vec2<float> windowSize = { static_cast<float>(WinApp::Instance()->GetWinSize().x), static_cast<float>(WinApp::Instance()->GetWinSize().y) };

	{
		float easingPosY = resultEasingAmount * (windowSize.y - RESULT_POS.y);
		DrawFunc::DrawGraph(Vec2<float>(440.0f, windowSize.y - easingPosY), TexHandleMgr::GetTexBuffer(resultHandle));

		DrawBreakCount(breakCountEasingAmount, ceil(breakCount), ScoreKeep::Instance()->GetMaxNum());
	}

	if (bigFontFlag && timeUpFlag)
	{
		if (evaluationFlag)
		{
			DrawFunc::DrawRotaGraph2D(Vec2<float>(evaluationPosX, windowSize.y / 2.0f - 80.0f + easeEvaluationPosY), Vec2<float>(0.7f, 0.7f), 0.0f, TexHandleMgr::GetTexBuffer(perfectHandle[perfectIndex]));
		}
		else
		{
			DrawFunc::DrawRotaGraph2D(Vec2<float>(evaluationPosX-30, windowSize.y / 2.0f - 80.0f + easeEvaluationPosY), Vec2<float>(0.7f, 0.7f), 0.0f, TexHandleMgr::GetTexBuffer(evaluationNowHandle));
		}
	}

}

void ResultScene::OnImguiDebug()
{
}

void ResultScene::OnFinalize()
{
	static const int BGM = AudioApp::Instance()->LoadAudio("resource/ChainCombat/sound/bgm_1.wav");
	AudioApp::Instance()->StopWave(BGM);
}

void ResultScene::DrawBreakCount(float scoreEasingAmount, int BREAK_NOW_COUNT, int BREAK_MAX_COUNT)
{
	Vec2<float> windowSize = { static_cast<float>(WinApp::Instance()->GetWinSize().x), static_cast<float>(WinApp::Instance()->GetWinSize().y) };

	const float baseX = 500.0f;
	float easingPosY = scoreEasingAmount * (windowSize.y / 2.0f + baseX);
	Vec2<float> drawPos = Vec2<float>(840.0f, windowSize.y + baseX - easingPosY);


	const int FONT_SIZE = 66.3f;

	float leftX = 0.0f;
	//現在
	{
		nowSize = breakSize;
		for (int i = 0; i < KuroFunc::GetDigit(BREAK_NOW_COUNT) - 1; ++i)
		{
			drawPos.x -= FONT_SIZE * nowSize.x;
		}
		leftX = drawPos.x;


		std::vector<int>number = CountNumber(BREAK_NOW_COUNT);
		for (int i = 0; i < number.size(); ++i)
		{
			// 数字を求めて描画する。
			int disit = GetDisit(BREAK_NOW_COUNT, i);

			if (!bigFontFlag)
			{
				// スコアが前フレームの値と違っていたら大きくする。
				if (prevScore[i] != disit) scoreSize[i] = defaultSize + 0.5f;
			}

			DrawFunc::DrawRotaGraph2D(drawPos, Vec2<float>(scoreSize[i], scoreSize[i]), 0.0f, TexHandleMgr::GetTexBuffer(blueNumberHandle[number[i]]));
			// フォントサイズ分移動させる。
			drawPos.x += FONT_SIZE * nowSize.x;

			// このフレームのスコアを保存。
			prevScore[i] = disit;
		}
	}

	evaluationPosX = leftX + (drawPos.x - leftX) / 2.0f;
	evaluationPosX -= 20.0f;

	//スラッシュ
	DrawFunc::DrawRotaGraph2D(drawPos, nowSize, 0.0f, TexHandleMgr::GetTexBuffer(slashHandle));

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



	//Vec2<float> windowSize = {  static_cast<float>WinApp::Instance()->GetWinSize().x,  static_cast<float>WinApp::Instance()->GetWinSize().y };

	//float easingPosX = scoreEasingAmount * (SCORE_POS.x - windowSize.x);


	//SCORE_POS.y =  static_cast<float>WINDOW_CENTER.y + 180.0f;
	//Vec2<float> drawPos = Vec2<float>(windowSize.x + easingPosX - 150.0f, SCORE_POS.y);
	//// 数字のフォントサイズ
	//const int FONT_SIZE = 66.3f * defaultSize;

	//// 現在の数字を求める。
	//int nowScore = scoreEffectEasingTimer * targetScore;

	//drawPos.y = 640.0f;

	//for (int index = 10; 0 < index; --index) {

	//	// 数字を求めて描画する。
	//	int disit = GetDisit(nowScore, index - 1);

	//	// スコアが前フレームの値と違っていたら大きくする。
	//	if (prevScore[index - 1] != disit) scoreSize[index - 1] = 1.5f;

	//	DrawFunc::DrawRotaGraph2D(drawPos, Vec2<float>(scoreSize[index - 1], scoreSize[index - 1]), 0, TexHandleMgr::GetTexBuffer(goldNumberHandle[disit]));

	//	// フォントサイズ分移動させる。
	//	drawPos.x += FONT_SIZE;

	//	// このフレームのスコアを保存。
	//	prevScore[index - 1] = disit;

	//}

}
