#include "ResultScene.h"
#include "IntoTheAbyss/TexHandleMgr.h"
#include "DrawFunc.h"
#include "WinApp.h"
#include "KuroMath.h"
#include "UsersInput.h"
#include "IntoTheAbyss/ResultSceneBackGround.h"

ResultScene::ResultScene()
{
	backGroundHandle = TexHandleMgr::LoadGraph("resource/ChainCombat/title_scene/star.png");
	winnerFrameHandle = TexHandleMgr::LoadGraph("resource/ChainCombat/result_scene/winnerFrame.png");
	resultHandle = TexHandleMgr::LoadGraph("resource/ChainCombat/result_scene/result.png");
	breakEnemyHandle = TexHandleMgr::LoadGraph("resource/ChainCombat/UI/break_enemy.png");
	breakPlayerHandle = TexHandleMgr::LoadGraph("resource/ChainCombat/UI/break_player.png");
	scoreHandle = TexHandleMgr::LoadGraph("resource/ChainCombat/UI/score.png");
	TexHandleMgr::LoadDivGraph("resource/ChainCombat/UI/num.png", 12, { 12, 1 }, blueNumberHandle.data());
	TexHandleMgr::LoadDivGraph("resource/ChainCombat/UI/num_yellow.png", 12, { 12, 1 }, goldNumberHandle.data());

	changeScene = new SceneCange();

	lunaWinGraph = TexHandleMgr::LoadGraph("resource/ChainCombat/result_scene/luna.png");
	lacyWinGraph = TexHandleMgr::LoadGraph("resource/ChainCombat/result_scene/lacy.png");
}

void ResultScene::OnInitialize()
{
	resultUITimer = 0;
	breakEnemyUITimer = 0;
	breakPlayerUITimer = 0;
	scoreUITimer = 0;
	delayTimer = 0;
	resultEasingAmount = 0;
	breakEnemyEasingAmount = 0;
	breakPlayerEasingAmount = 0;
	scoreEffectTimer = 0;
	targetScore = 123456789;
	prevScore = { 1,2,3,4,5,6,7,8,9 };
	scoreSize = { 1 };
	scoreEffectEasingAmount = 0;

	breakEnemyAmount = 0;
	breakPlayerAmount = 0;

	winnerGraph = lunaWinGraph;
}

void ResultScene::OnUpdate()
{

	Vec2<float> windowSize = WinApp::Instance()->GetWinSize().Float();

	// �x���^�C�}�[������l�ȉ���������C���N�������g����B
	if (delayTimer < DELAY_TIMER) ++delayTimer;

	// �x�����������Ă��Ȃ�������B
	if (DELAY_TIMER <= delayTimer) {

		// [RESULT]�̉摜 [RESULT]�̃^�C�}�[������l�ȉ���������B
		if (resultUITimer < RESULT_UI_TIMER) {
			++resultUITimer;
			// �^�C�}�[���K��l�ɒB������B
			if (RESULT_UI_TIMER <= resultUITimer) {
				delayTimer = 0;
			}
			else {
				delayTimer = DELAY_TIMER;
			}
		}
		// [BREAK]�G�̉摜 �^�C�}�[���K��l�ȉ���������B
		if (breakEnemyUITimer < BREAK_ENEMY_UI_TIMER && RESULT_UI_TIMER <= resultUITimer) {
			++breakEnemyUITimer;
			// �^�C�}�[���K��l�ɒB������B
			if (BREAK_ENEMY_UI_TIMER <= breakEnemyUITimer) {
				delayTimer = 0;
			}
			else {
				delayTimer = DELAY_TIMER;
			}
		}
		// [BREAK]�v���C���[�̉摜 �^�C�}�[���K��l�ȉ���������B
		if (breakPlayerUITimer < BREAK_PLAYER_UI_TIMER && BREAK_ENEMY_UI_TIMER <= breakEnemyUITimer) {
			++breakPlayerUITimer;
			// �^�C�}�[���K��l�ɒB������B
			if (BREAK_PLAYER_UI_TIMER <= breakPlayerUITimer) {
				delayTimer = -DELAY_TIMER;
			}
			else {
				delayTimer = DELAY_TIMER;
			}
		}
		// [SCORE]�摜 �^�C�}�[���K��l�ȉ���������B
		if (scoreUITimer < SCORE_UI_TIMER && BREAK_PLAYER_UI_TIMER <= breakPlayerUITimer) {
			++scoreUITimer;
			// �^�C�}�[���K��l�ɒB������B
			if (SCORE_UI_TIMER <= scoreUITimer) {
				delayTimer = -180;
			}
			else {
				delayTimer = DELAY_TIMER;
			}
		}
		// �X�R�A�̃^�C�}�[���K��l�ȉ���������B
		if (scoreEffectTimer < SCORE_EFFECT_TIMER && SCORE_UI_TIMER <= scoreUITimer) {
			++scoreEffectTimer;
			// �^�C�}�[���K��l�ɒB������B
			if (SCORE_EFFECT_TIMER <= scoreEffectTimer) {
				delayTimer = 0;
			}
			else {
				delayTimer = DELAY_TIMER;
			}
		}

	}

	// �C�[�W���O�ʂ��X�V�B
	resultEasingAmount = KuroMath::Ease(Out, Cubic, (float)resultUITimer / RESULT_UI_TIMER, 0.0f, 1.0f);
	breakEnemyEasingAmount = KuroMath::Ease(Out, Cubic, (float)breakEnemyUITimer / BREAK_ENEMY_UI_TIMER, 0.0f, 1.0f);
	breakPlayerEasingAmount = KuroMath::Ease(Out, Cubic, (float)breakPlayerUITimer / BREAK_PLAYER_UI_TIMER, 0.0f, 1.0f);
	scoreEasingAmount = KuroMath::Ease(Out, Cubic, (float)scoreUITimer / SCORE_UI_TIMER, 0.0f, 1.0f);
	scoreEffectEasingAmount = KuroMath::Ease(Out, Cubic, (float)scoreEffectTimer / SCORE_EFFECT_TIMER, 0.0f, 1.0f);

	// �X�R�A�G�t�F�N�g�̃^�C�}�[�̊�����0.9�𒴂�����A��C�ɑS���ς���B
	if (0.97f <= (float)scoreEffectTimer / SCORE_EFFECT_TIMER && (float)scoreEffectTimer / SCORE_EFFECT_TIMER <= 0.99f) {

		scoreEffectTimer = SCORE_EFFECT_TIMER;

		for (int index = 0; index < 10; ++index) {
			scoreSize[index] = 1.7f;
		}

	}

	if (UsersInput::Instance()->Input(DIK_R)) {

		OnInitialize();

	}

	// ���U���g��ʂ֔�΂�
	if (UsersInput::Instance()->OnTrigger(DIK_0)) {
		KuroEngine::Instance().ChangeScene(0, changeScene);
	}

}

void ResultScene::OnDraw()
{

	KuroEngine::Instance().Graphics().SetRenderTargets({ D3D12App::Instance()->GetBackBuffRenderTarget() });

	ResultSceneBackGround::Instance()->Draw();

	Vec2<float> windowSize = { (float)WinApp::Instance()->GetWinSize().x, (float)WinApp::Instance()->GetWinSize().y };
	//DrawFunc::DrawBox2D(Vec2<float>(0, 0), windowSize, Color(0, 0, 0, 255), DXGI_FORMAT_R8G8B8A8_UNORM);
	DrawFunc::DrawGraph(Vec2<float>(0, 0), TexHandleMgr::GetTexBuffer(winnerFrameHandle), AlphaBlendMode_Trans);
	DrawFunc::DrawGraph({ 25.0f,30.0f }, TexHandleMgr::GetTexBuffer(winnerGraph), AlphaBlendMode_Trans);

	// [RESULT] �� [BREAK]�̕`�揈��
	{
		float easingPosX = resultEasingAmount * (RESULT_POS.x - windowSize.x);
		DrawFunc::DrawGraph(Vec2<float>(windowSize.x + easingPosX, RESULT_POS.y), TexHandleMgr::GetTexBuffer(resultHandle), AlphaBlendMode_Trans);
		DrawBREAK(BREAK_ENEMY_POS, breakEnemyEasingAmount, true, breakEnemyAmount);
		DrawBREAK(BREAK_PLAYER_POS, breakPlayerEasingAmount, false, breakPlayerAmount);
	}

	// [SCORE] �̕`�揈��
	DrawSCORE(scoreEasingAmount, scoreEffectEasingAmount);

}

void ResultScene::OnImguiDebug()
{
}

void ResultScene::OnFinalize()
{
}

void ResultScene::DrawBREAK(const Vec2<float>& targetPosm, const float& easingTimer, const bool& isBoss, const int& breakCount)
{

	Vec2<float> windowSize = { (float)WinApp::Instance()->GetWinSize().x, (float)WinApp::Instance()->GetWinSize().y };

	// �g�p����摜�n���h�������߂�B
	int handle = breakEnemyHandle;
	if (!isBoss) handle = breakPlayerHandle;

	// �C�[�W���O���������ʒu�����߂�[BREAK]��`�������B
	float easingPosX = easingTimer * (targetPosm.x - windowSize.x);
	Vec2<float> drawPos = Vec2<float>(windowSize.x + easingPosX, targetPosm.y);
	DrawFunc::DrawGraph(drawPos, TexHandleMgr::GetTexBuffer(handle), AlphaBlendMode_Trans);

	// BREAK�̉摜�T�C�Y���E�ɓ��������ʒu��*��`�悷��B
	drawPos = drawPos + Vec2<float>(292.0f, 0.0f);
	DrawFunc::DrawGraph(drawPos, TexHandleMgr::GetTexBuffer(blueNumberHandle[blueNumberHandle.size() - 1]), AlphaBlendMode_Trans);

	// �����̃t�H���g���ړ�������B
	const int FONT_SIZE = 66.3f;
	drawPos.x += FONT_SIZE;

	// BREAK�̐���`�悷��B
	for (int index = 1; 0 <= index; --index) {

		// ���������߂ĕ`�悷��B
		int disit = GetDisit(breakCount, index);

		DrawFunc::DrawGraph(drawPos, TexHandleMgr::GetTexBuffer(blueNumberHandle[disit]), AlphaBlendMode_Trans);

		// �t�H���g�T�C�Y���ړ�������B
		drawPos.x += FONT_SIZE;

	}

	// �X�R�A�̃T�C�Y���f�t�H���g�ɋ߂Â���B
	for (int index = 0; index < 10; ++index) {
		scoreSize[index] += (1.0f - scoreSize[index]) / 10.0f;
	}


}

void ResultScene::DrawSCORE(const float& easingTimer, const double& scoreEffectEasingTimer)
{

	Vec2<float> windowSize = { (float)WinApp::Instance()->GetWinSize().x, (float)WinApp::Instance()->GetWinSize().y };

	float easingPosX = scoreEasingAmount * (SCORE_POS.x - windowSize.x);
	Vec2<float> drawPos = Vec2<float>(windowSize.x + easingPosX, SCORE_POS.y);
	DrawFunc::DrawGraph(drawPos, TexHandleMgr::GetTexBuffer(scoreHandle), AlphaBlendMode_Trans);

	// �����̃t�H���g�T�C�Y
	const int FONT_SIZE = 66.3f;

	// ���݂̐��������߂�B
	int nowScore = scoreEffectEasingTimer * targetScore;

	drawPos.y += FONT_SIZE + FONT_SIZE / 2.0f;
	drawPos.x += FONT_SIZE / 2.0f;

	for (int index = 10; 0 < index; --index) {

		// ���������߂ĕ`�悷��B
		int disit = GetDisit(nowScore, index - 1);

		// �X�R�A���O�t���[���̒l�ƈ���Ă�����傫������B
		if (prevScore[index - 1] != disit) scoreSize[index - 1] = 1.5f;

		DrawFunc::DrawRotaGraph2D(drawPos, Vec2<float>(scoreSize[index - 1], scoreSize[index - 1]), 0, TexHandleMgr::GetTexBuffer(goldNumberHandle[disit]));

		// �t�H���g�T�C�Y���ړ�������B
		drawPos.x += FONT_SIZE;

		// ���̃t���[���̃X�R�A��ۑ��B
		prevScore[index - 1] = disit;

	}

}
