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
	blueNumberHandle.resize(12);
	TexHandleMgr::LoadDivGraph("resource/ChainCombat/UI/num.png", 12, { 12, 1 }, blueNumberHandle.data());
	goldNumberHandle.resize(12);
	TexHandleMgr::LoadDivGraph("resource/ChainCombat/UI/num_yellow.png", 12, { 12, 1 }, goldNumberHandle.data());

	resultUITimer = 0;
	breakEnemyUITimer = 0;
	breakPlayerUITimer = 0;
	delayTimer = 0;
	resultEasingAmount = 0;
	breakEnemyEasingAmount = 0;
	breakPlayerEasingAmount = 0;

	breakEnemyAmount = 0;
	breakPlayerAmount = 0;

}

void ResultScene::OnUpdate()
{

	Vec2<float> windowSize = { (float)WinApp::Instance()->GetWinSize().x, (float)WinApp::Instance()->GetWinSize().y };

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
		// [BREAK}�v���C���[�̉摜 �^�C�}�[���K��l�ȉ���������B
		if (breakPlayerUITimer < BREAK_PLAYER_UI_TIMER && BREAK_ENEMY_UI_TIMER <= breakEnemyUITimer) {
			++breakPlayerUITimer;
			// �^�C�}�[���K��l�ɒB������B
			if (BREAK_PLAYER_UI_TIMER <= breakPlayerUITimer) {
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

	if (UsersInput::Instance()->Input(DIK_R)) {

		OnInitialize();

	}

}

void ResultScene::OnDraw()
{

	KuroEngine::Instance().Graphics().SetRenderTargets({ D3D12App::Instance()->GetBackBuffRenderTarget() });

	Vec2<float> windowSize = { (float)WinApp::Instance()->GetWinSize().x, (float)WinApp::Instance()->GetWinSize().y };
	DrawFunc::DrawBox2D(Vec2<float>(0, 0), windowSize, Color(0, 0, 0, 255), DXGI_FORMAT_R8G8B8A8_UNORM);
	//DrawFunc::DrawGraph(Vec2<float>(0, 0), TexHandleMgr::GetTexBuffer(winnerFrameHandle), AlphaBlendMode_Trans);

	// [RESULT] �� [BREAK]�̕`�揈��
	{
		float easingPosX = resultEasingAmount * (RESULT_POS.x - windowSize.x);
		DrawFunc::DrawGraph(Vec2<float>(windowSize.x + easingPosX, RESULT_POS.y), TexHandleMgr::GetTexBuffer(resultHandle), AlphaBlendMode_Trans);
		DrawBREAK(windowSize, BREAK_ENEMY_POS, breakEnemyEasingAmount, true, 1);
		DrawBREAK(windowSize, BREAK_PLAYER_POS, breakPlayerEasingAmount, false, 10);
	}

}

void ResultScene::OnImguiDebug()
{
}

void ResultScene::OnFinalize()
{
}

void ResultScene::DrawBREAK(const Vec2<float>& windowSize, const Vec2<float>& targetPosm, const float& easingTimer, const bool& isBoss, const int& breakCount)
{

	// �g�p����摜�n���h�������߂�B
	int handle = breakEnemyHandle;
	if (!isBoss) handle = breakPlayerHandle;

	// �C�[�W���O���������ʒu�����߂�[BREAK]��`�������B
	float easingPosX = easingTimer * (targetPosm.x - windowSize.x);
	Vec2<float> drawPos = Vec2<float>(windowSize.x + easingPosX, targetPosm.y);
	DrawFunc::DrawGraph(drawPos, TexHandleMgr::GetTexBuffer(handle), AlphaBlendMode_Trans);

	// BREAK�̉摜�T�C�Y���E�ɓ��������ʒu��*��`�悷��B
	drawPos = drawPos + Vec2<float>(292.0f, 0.0f);
	DrawFunc::DrawGraph(drawPos, TexHandleMgr::GetTexBuffer(blueNumberHandle[blueNumberHandle.size() - 2]), AlphaBlendMode_Trans);

	// �����̃t�H���g���ړ�������B
	const int FONT_SIZE = 66.3f;
	drawPos.x += FONT_SIZE;

	// BREAK�̐���`�悷��B
	for (int index = 1; 0 <= index; --index) {

		// ���������߂ĕ`�悷��B
		int disit = GetDisit(breakCount, index);

		DrawFunc::DrawGraph(drawPos, TexHandleMgr::GetTexBuffer(blueNumberHandle[1]), AlphaBlendMode_Trans);

		// �t�H���g�T�C�Y���ړ�������B
		drawPos.x += FONT_SIZE;

	}
	Vec2<int> buff = TexHandleMgr::GetTexBuffer(blueNumberHandle[1])->GetGraphSize();
	for (int index = 0; index < 11; ++index) {

		DrawFunc::DrawGraph(Vec2<float>(0,0) + Vec2<float>(FONT_SIZE * index, FONT_SIZE * index), TexHandleMgr::GetTexBuffer(blueNumberHandle[index]), AlphaBlendMode_Trans);

	}

}
