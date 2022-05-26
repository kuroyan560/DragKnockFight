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
	roundHandle = TexHandleMgr::LoadGraph("resource/ChainCombat/UI/round.png");;
	slashHandle = TexHandleMgr::LoadGraph("resource/ChainCombat/UI/score.png");
	TexHandleMgr::LoadDivGraph("resource/ChainCombat/UI/num.png", 12, { 12, 1 }, blueNumberHandle.data());

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
}

void ResultScene::OnUpdate()
{
	if (UsersInput::Instance()->KeyOnTrigger(DIK_J))
	{
		resultUITimer = 0;
		breakEnemyUITimer = 0;
		delayTimer = 0;
		resultEasingAmount = 0;
		breakCountEasingAmount = 0;
		winnerName = ResultTransfer::Instance()->winner;

		maxSize = { 0.5f,0.5f };
		breakSize = { 1.5f,1.5f };
	}



	static const int SE = AudioApp::Instance()->LoadAudio("resource/ChainCombat/sound/resultScore.wav", 0.13f);

	Vec2<float> windowSize = WinApp::Instance()->GetWinSize().Float();

	// �x���^�C�}�[������l�ȉ���������C���N�������g����B
	if (delayTimer < DELAY_TIMER) ++delayTimer;

	// �������΂��t���O�B
	bool isSkip = false;

	// �x�����������Ă��Ȃ�������B
	//�t�H���g�̉��ړ�
	if (DELAY_TIMER <= delayTimer) {

		// [RESULT]�̉摜 [RESULT]�̃^�C�}�[������l�ȉ���������B
		if (resultUITimer < RESULT_UI_TIMER) {
			// �^�C�}�[��0��������
			if (resultUITimer == 0) {
				AudioApp::Instance()->PlayWave(SE);
			}
			++resultUITimer;
			// �^�C�}�[���K��l�ɒB������B
			if (RESULT_UI_TIMER <= resultUITimer) {
				delayTimer = DELAY_TIMER;
				isSkip = true;
				AudioApp::Instance()->PlayWave(SE);
			}
		}
		// [BREAK]�G�̉摜 �^�C�}�[���K��l�ȉ���������B
		if (!isSkip && breakEnemyUITimer < BREAK_COUNTUI__TIMER && RESULT_UI_TIMER <= resultUITimer) {
			++breakEnemyUITimer;
			// �^�C�}�[���K��l�ɒB������B
			if (BREAK_COUNTUI__TIMER <= breakEnemyUITimer) {
				delayTimer = DELAY_TIMER;
				isSkip = true;
				AudioApp::Instance()->PlayWave(SE);
			}
		}
	}

	// �C�[�W���O�ʂ��X�V�B
	//�C�[�W���O�̓���
	resultEasingAmount = KuroMath::Ease(Out, Cubic, (float)resultUITimer / RESULT_UI_TIMER, 0.0f, 1.0f);
	breakCountEasingAmount = KuroMath::Ease(Out, Cubic, (float)breakEnemyUITimer / BREAK_COUNTUI__TIMER, 0.0f, 1.0f);


	// ���U���g��ʂ֔�΂�
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

	{
		float easingPosX = resultEasingAmount * (RESULT_POS.x - windowSize.x);
		DrawFunc::DrawGraph(Vec2<float>(windowSize.x + easingPosX, RESULT_POS.y), TexHandleMgr::GetTexBuffer(resultHandle));

		DrawBreakCount(breakCountEasingAmount, 1000, 15000);
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
	Vec2<float> windowSize = { (float)WinApp::Instance()->GetWinSize().x, (float)WinApp::Instance()->GetWinSize().y };

	const float baseX = 500.0f;
	float easingPosX = scoreEasingAmount * (windowSize.x / 2.0f+ baseX);
	Vec2<float> drawPos = Vec2<float>((windowSize.x+ baseX) - easingPosX, 430.0f);


	const int FONT_SIZE = 66.3f;
	//����
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
			// �t�H���g�T�C�Y���ړ�������B
			drawPos.x += FONT_SIZE * nowSize.x;
		}
	}

	//�X���b�V��
	//DrawFunc::DrawRotaGraph2D(drawPos, nowSize, 0.0f, TexHandleMgr::GetTexBuffer(slashHandle));

	//�ő�
	{
		drawPos.x += FONT_SIZE * (nowSize.x / 2.0f);
		drawPos.y += 25.0f;
		nowSize = maxSize;
		std::vector<int>number = CountNumber(BREAK_MAX_COUNT);
		for (int i = 0; i < number.size(); ++i)
		{
			DrawFunc::DrawRotaGraph2D(drawPos, nowSize, 0.0f, TexHandleMgr::GetTexBuffer(blueNumberHandle[number[i]]));
			// �t�H���g�T�C�Y���ړ�������B
			drawPos.x += FONT_SIZE * nowSize.x;
		}
	}
}
