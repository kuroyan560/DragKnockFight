#include "ResultScene.h"
#include "IntoTheAbyss/TexHandleMgr.h"
#include "DrawFunc.h"
#include "WinApp.h"
#include "KuroMath.h"
#include "UsersInput.h"

ResultScene::ResultScene()
{
}

void ResultScene::OnInitialize()
{
	backGroundHandle = TexHandleMgr::LoadGraph("resource/ChainCombat/title_scene/star.png");
	winnerFrameHandle = TexHandleMgr::LoadGraph("resource/ChainCombat/result_scene/winnerFrame.png");
	resultHandle = TexHandleMgr::LoadGraph("resource/ChainCombat/result_scene/result.png");
	breakEnemyHandle = TexHandleMgr::LoadGraph("resource/ChainCombat/UI/break_enemy.png");
	breakPlayerHandle = TexHandleMgr::LoadGraph("resource/ChainCombat/UI/break_player.png");
	scoreHandle = TexHandleMgr::LoadGraph("resource/ChainCombat/UI/score.png");

	resultUITimer = 0;
	breakEnemyUITimer = 0;
	breakPlayerUITimer = 0;
	delayTimer = 0;
	resultEasingAmount = 0;
	breakEnemyEasingAmount = 0;
	breakPlayerEasingAmount = 0;

}

void ResultScene::OnUpdate()
{

	Vec2<float> windowSize = { (float)WinApp::Instance()->GetWinSize().x, (float)WinApp::Instance()->GetWinSize().y };

	// 遅延タイマーが既定値以下だったらインクリメントする。
	if (delayTimer < DELAY_TIMER) ++delayTimer;

	// 遅延が発生していなかったら。
	if (DELAY_TIMER <= delayTimer) {

		// [RESULT]の画像 [RESULT]のタイマーが既定値以下だったら。
		if (resultUITimer < RESULT_UI_TIMER) {
			++resultUITimer;
			// タイマーが規定値に達したら。
			if (RESULT_UI_TIMER <= resultUITimer) {
				delayTimer = 0;
			}
			else {
				delayTimer = DELAY_TIMER;
			}
		}
		// [BREAK]敵の画像 タイマーが規定値以下だったら。
		if (breakEnemyUITimer < BREAK_ENEMY_UI_TIMER && RESULT_UI_TIMER <= resultUITimer) {
			++breakEnemyUITimer;
			// タイマーが規定値に達したら。
			if (BREAK_ENEMY_UI_TIMER <= breakEnemyUITimer) {
				delayTimer = 0;
			}
			else {
				delayTimer = DELAY_TIMER;
			}
		}
		// [BREAK}プレイヤーの画像 タイマーが規定値以下だったら。
		if (breakPlayerUITimer < BREAK_PLAYER_UI_TIMER && BREAK_ENEMY_UI_TIMER <= breakEnemyUITimer) {
			++breakPlayerUITimer;
			// タイマーが規定値に達したら。
			if (BREAK_PLAYER_UI_TIMER <= breakPlayerUITimer) {
				delayTimer = 0;
			}
			else {
				delayTimer = DELAY_TIMER;
			}
		}
	}

	// イージング量を更新。
	resultEasingAmount = KuroMath::Ease(Out, Cubic, (float)resultUITimer / RESULT_UI_TIMER, 0.0f, 1.0f);
	breakEnemyEasingAmount = KuroMath::Ease(Out, Cubic, (float)breakEnemyUITimer / BREAK_ENEMY_UI_TIMER, 0.0f, 1.0f);
	breakPlayerEasingAmount = KuroMath::Ease(Out, Cubic, (float)breakPlayerUITimer / BREAK_PLAYER_UI_TIMER, 0.0f, 1.0f);

	if (UsersInput::Instance()->Input(DIK_R)) {

		OnInitialize();

	}

}

void ResultScene::OnDraw()
{

	KuroEngine::Instance().Graphics().SetRenderTargets({ D3D12App::Instance()->GetBackBuffRenderTarget() });

	Vec2<float> windowSize = { (float)WinApp::Instance()->GetWinSize().x, (float)WinApp::Instance()->GetWinSize().y };
	DrawFunc::DrawBox2D(Vec2<float>(0, 0), windowSize, Color(0, 0, 0, 255), DXGI_FORMAT_R8G8B8A8_UNORM);
	DrawFunc::DrawGraph(Vec2<float>(0, 0), TexHandleMgr::GetTexBuffer(winnerFrameHandle), AlphaBlendMode_Trans);
	float easingPosX = resultEasingAmount * (RESULT_POS.x - windowSize.x);
	DrawFunc::DrawGraph(Vec2<float>(windowSize.x + easingPosX, RESULT_POS.y), TexHandleMgr::GetTexBuffer(resultHandle), AlphaBlendMode_Trans);
	easingPosX = breakEnemyEasingAmount * (BREAK_ENEMY_POS.x - windowSize.x);
	DrawFunc::DrawGraph(Vec2<float>(windowSize.x + easingPosX, BREAK_ENEMY_POS.y), TexHandleMgr::GetTexBuffer(breakEnemyHandle), AlphaBlendMode_Trans);
	easingPosX = breakPlayerEasingAmount * (BREAK_PLAYER_POS.x - windowSize.x);
	DrawFunc::DrawGraph(Vec2<float>(windowSize.x + easingPosX, BREAK_PLAYER_POS.y), TexHandleMgr::GetTexBuffer(breakPlayerHandle), AlphaBlendMode_Trans);

}

void ResultScene::OnImguiDebug()
{
}

void ResultScene::OnFinalize()
{
}
