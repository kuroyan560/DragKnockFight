#include "ResultScene.h"
#include "IntoTheAbyss/TexHandleMgr.h"
#include "DrawFunc.h"
#include "WinApp.h"
#include "KuroMath.h"
#include "UsersInput.h"
#include "IntoTheAbyss/ResultTransfer.h"
#include "IntoTheAbyss/CharacterManager.h"
#include "IntoTheAbyss/CharacterInfo.h"
#include"IntoTheAbyss/ScoreKeep.h"
#include"IntoTheAbyss/EavaluationDataMgr.h"
#include"IntoTheAbyss/DebugKeyManager.h"
#include"IntoTheAbyss/ScoreManager.h"
#include"IntoTheAbyss/BackGroundParticle.h"
#include"IntoTheAbyss/KazHelper.h"

ResultScene::ResultScene()
{
	backGroundHandle = TexHandleMgr::LoadGraph("resource/ChainCombat/title_scene/star.png");
	winnerFrameHandle = TexHandleMgr::LoadGraph("resource/ChainCombat/result_scene/winnerFrame.png");
	lunaHandle = TexHandleMgr::LoadGraph("resource/ChainCombat/result_scene/luna.png");
	lacyHandle = TexHandleMgr::LoadGraph("resource/ChainCombat/result_scene/lacy.png");
	resultHandle = TexHandleMgr::LoadGraph("resource/ChainCombat/result_scene/result.png");
	breakEnemyHandle = TexHandleMgr::LoadGraph("resource/ChainCombat/UI/break_enemy.png");
	roundHandle = TexHandleMgr::LoadGraph("resource/ChainCombat/UI/round.png");
	TexHandleMgr::LoadDivGraph("resource/ChainCombat/UI/num.png", 12, { 12, 1 }, blueNumberHandle.data());
	TexHandleMgr::LoadDivGraph("resource/ChainCombat/player/lacy/pull_front.png", 3, { 3, 1 }, redCharaHandle.data());
	TexHandleMgr::LoadDivGraph("resource/ChainCombat/player/luna/pull_front.png", 3, { 3, 1 }, greenCharaHandle.data());
	TexHandleMgr::LoadDivGraph("resource/ChainCombat/ironBall/explosion_open.png", 3, { 3, 1 }, ironBallhandle.data());
	TexHandleMgr::LoadDivGraph("resource/ChainCombat/UI/newRecord.png", 3, { 3, 1 }, newRecordHandle.data());
	useHandle = ironBallhandle;
	slashHandle = blueNumberHandle[11];
	lissajousTimer = 0;

	failHandle = TexHandleMgr::LoadGraph("resource/ChainCombat/UI/good.png");
	goodHandle = TexHandleMgr::LoadGraph("resource/ChainCombat/UI/good.png");
	greatHandle = TexHandleMgr::LoadGraph("resource/ChainCombat/UI/great.png");
	excellentHandle = TexHandleMgr::LoadGraph("resource/ChainCombat/UI/excellent.png");
	TexHandleMgr::LoadDivGraph("resource/ChainCombat/UI/perfect.png", 3, Vec2<int>(3, 1), perfectHandle.data());


	changeScene = std::make_shared<SceneCange>();

	winnerGraph[PLAYABLE_LUNA] = TexHandleMgr::LoadGraph("resource/ChainCombat/result_scene/luna.png");
	winnerGraph[PLAYABLE_LACY] = TexHandleMgr::LoadGraph("resource/ChainCombat/result_scene/lacy.png");
	winnerGraph[PLAYABLE_BOSS_0] = TexHandleMgr::LoadGraph("resource/ChainCombat/result_scene/lacy.png");


	soundSe = EvaluationMgr::Instance()->soundData;
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
	isNewRecord = false;

	evaluationEasingTimer = 0;
	intervalTimer = 0;
	evaluationFlag = false;

	perfectInterval = 0;
	perfectIndex = 0;

	endFlg = false;

	lissajousTimer = 0;

	backGroundCharaAnimHandle = 0;
	backGroundCharaAnimTimer = 0;
	backGroundCharaPos = { -10000,-10000 };
	backGroundCharaVel = {};
	backGroundCharaAngle = 0;

	ssIntervalTimer = 0;

	initSoundFlag = false;

	newRecordExp = {};
	newRecordPos = {};
	newRecordTimer = 0;
	newRecordAlpha = 0;
	newRecordAnimTimer = 0;
	newRecordIndex = 0;
	isNewRecordActive = false;
	isNewRecord = false;

}

void ResultScene::OnUpdate()
{

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
				//AudioApp::Instance()->PlayWave(SE);
			}
			++resultUITimer;
			// �^�C�}�[���K��l�ɒB������B
			if (RESULT_UI_TIMER <= resultUITimer) {
				delayTimer = DELAY_TIMER;
				isSkip = true;
				//AudioApp::Instance()->PlayWave(SE);
			}
		}
		// [BREAK]�G�̉摜 �^�C�}�[���K��l�ȉ���������B
		if (!isSkip && breakEnemyUITimer < BREAK_COUNTUI_TIMER && RESULT_UI_TIMER <= resultUITimer) {
			++breakEnemyUITimer;
			// �^�C�}�[���K��l�ɒB������B
			if (BREAK_COUNTUI_TIMER <= breakEnemyUITimer) {
				baseBreakCount = ScoreManager::Instance()->GetScore();

				delayTimer = DELAY_TIMER;
				isSkip = true;
			}
		}

		// �X�R�A�̃^�C�}�[���K��l�ȉ���������B
		if (scoreEffectTimer < SCORE_EFFECT_TIMER && BREAK_COUNTUI_TIMER <= breakEnemyUITimer) {
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

	//�C�[�W���O�ʂ��X�V�B
	//�C�[�W���O�̓���
	resultEasingAmount = KuroMath::Ease(Out, Cubic, static_cast<float>(resultUITimer) / static_cast<float>(RESULT_UI_TIMER), 0.0f, 1.0f);
	breakCountEasingAmount = KuroMath::Ease(Out, Cubic, static_cast<float>(breakEnemyUITimer) / static_cast<float>(BREAK_COUNTUI_TIMER), 0.0f, 1.0f);


	if (UsersInput::Instance()->ControllerOnTrigger(0, XBOX_BUTTON::A)) {

		if (endFlg)
		{
			static int bgm = AudioApp::Instance()->LoadAudio("resource/ChainCombat/sound/bgm_1.wav");
			AudioApp::Instance()->StopWave(bgm);
			KuroEngine::Instance().ChangeScene(1, changeScene);
		}
		else
		{
			//���o�Ƃ΂�
			resultUITimer = RESULT_UI_TIMER;
			breakEnemyUITimer = BREAK_COUNTUI_TIMER;
			scoreEffectTimer = SCORE_EFFECT_TIMER;
			evaluationEasingTimer = EVALUATION_EFFECT_TIMER - 1;
			intervalTimer = 35;
			endFlg = true;
			bigFontFlag = true;
			isNewRecordActive = true;
			ssIntervalTimer = 0;
			baseBreakCount = ScoreManager::Instance()->GetScore();

			for (int index = 0; index < 10; ++index)
			{
				scoreSize[index] = defaultSize;
			}
		}
	}


	// �X�R�A�̃T�C�Y���f�t�H���g�ɋ߂Â���B
	for (int index = 0; index < 10; ++index) {
		scoreSize[index] += (defaultSize - scoreSize[index]) / 10.0f;
	}


	// �X�R�A�G�t�F�N�g�̃^�C�}�[�̊�����0.9�𒴂�����A��C�ɑS���ς���B
	if (ceil(breakCount) == baseBreakCount && !bigFontFlag)
	{
		scoreEffectTimer = SCORE_EFFECT_TIMER;

		for (int index = 0; index < 10; ++index) {
			scoreSize[index] = defaultSize + 1.7f;
		}
		bigFontFlag = true;

	}

	if (breakCount != prevBreakCount)
	{
		static const int SCORE_SE = AudioApp::Instance()->LoadAudio("resource/ChainCombat/sound/score.wav", 0.13f);
		AudioApp::Instance()->PlayWave(SCORE_SE);
	}
	prevBreakCount = breakCount;


	breakCount = KuroMath::Ease(In, Cubic, static_cast<float>(scoreEffectTimer) / static_cast<float>(SCORE_EFFECT_TIMER), 0.0f, 1.0f) * baseBreakCount;


	float rate = static_cast<float>(baseBreakCount) / static_cast<float>(ScoreKeep::Instance()->GetMaxNum());

	int stageNum = SelectStage::Instance()->GetStageNum();
	int roomNum = SelectStage::Instance()->GetRoomNum();

	StageEvaluationData data = EvaluationMgr::Instance()->GetData(stageNum, roomNum);
	const float FAIL_RATE = data.failRate;
	const float GOOD_RATE = data.goodRate;
	const float GREAT_RATE = data.greatRate;
	const float EXCELLENT_RATE = data.excellentRate;
	const float PERFECR_RATE = data.perfectRate;

	if (bigFontFlag)
	{
		++intervalTimer;
	}
	timeUpFlag = 35 <= intervalTimer;
	isNewRecordActive = 35 <= intervalTimer;


	Sound soundType = SOUND_GOOD;
	if (rate <= FAIL_RATE)
	{
		soundType = SOUND_FAIL;
		evaluationNowHandle = failHandle;
	}
	else if (rate <= GOOD_RATE)
	{
		soundType = SOUND_GOOD;
		evaluationNowHandle = goodHandle;
	}
	else if (rate <= GREAT_RATE)
	{
		soundType = SOUND_GREAT;
		evaluationNowHandle = greatHandle;
	}
	else if (rate <= EXCELLENT_RATE || rate < PERFECR_RATE)
	{
		soundType = SOUND_EXCELLENT;
		evaluationNowHandle = excellentHandle;
	}
	else if (PERFECR_RATE <= rate)
	{
		soundType = SOUND_PERFECT;
		evaluationFlag = true;
	}



	if (bigFontFlag && evaluationEasingTimer < EVALUATION_EFFECT_TIMER && timeUpFlag)
	{
		++evaluationEasingTimer;
		easeEvaluationPosY = -KuroMath::Ease(Out, Back, static_cast<float>(evaluationEasingTimer) / static_cast<float>(EVALUATION_EFFECT_TIMER), 0.0f, 1.0f) * 30.0f;

		if (!initSoundFlag)
		{
			AudioApp::Instance()->PlayWave(soundSe[soundType]);
			initSoundFlag = true;
		}
	}

	if (EVALUATION_EFFECT_TIMER <= evaluationEasingTimer)
	{
		endFlg = true;
	}

	if (evaluationFlag)
	{
		if (EvaluationMgr::Instance()->PERFECT_ANIMATION_INTERVAL <= perfectInterval && perfectIndex < 2)
		{
			++perfectIndex;
			perfectInterval = 0;
		}
		else if (EvaluationMgr::Instance()->PERFECT_ANIMATION_INTERVAL <= perfectInterval && 2 <= perfectIndex)
		{
			perfectIndex = 0;
			perfectInterval = 0;
		}
		else
		{
			++perfectInterval;
		}
	}

	lissajousTimer += 0.01f;

	// �w�i�̐���`��
	BackGroundParticleMgr::Instance()->Update();

	backGroundCharaAngle += 0.1f;

	// �w�i�̃L�����ɃA�j���[�V�������X�V
	++backGroundCharaAnimTimer;
	static const int BACKGROUND_CHARA_TIMER = 10;
	if (BACKGROUND_CHARA_TIMER < backGroundCharaAnimTimer) {

		++backGroundCharaAnimHandle;
		if (3 <= backGroundCharaAnimHandle) {

			backGroundCharaAnimHandle = 0;

		}

		backGroundCharaAnimTimer = 0;

	}

	// ���ɋH�ɃL�����N�^�[������̕��ɔ�΂��B
	int random = KuroFunc::GetRand(0, 1000);
	if (random == 50 && !((0 < backGroundCharaPos.x && backGroundCharaPos.x < windowSize.x) && (0 < backGroundCharaPos.y || backGroundCharaPos.y < windowSize.y))) {

		backGroundCharaPos = { 2000,-1000 };
		backGroundCharaVel = Vec2<float>(windowSize / 2.0f - backGroundCharaPos).GetNormal() * 10.0f;

		// �g�p����摜�������_���Ō��߂�B
		int randomhandle = KuroFunc::GetRand(0, 2);
		if (randomhandle == 0) {
			useHandle = redCharaHandle;
		}
		else if (randomhandle == 1) {
			useHandle = greenCharaHandle;
		}
		else {
			useHandle = ironBallhandle;
		}

	}

	// �w�i�̃L�����𓮂����B
	backGroundCharaPos += backGroundCharaVel;


	// �V�L�^��������
	if (isNewRecord && isNewRecordActive) {

		// �^�C�}�[���X�V���đ傫����ݒ�B
		++newRecordTimer;
		static const int EXP_TIMER = 15;
		if (EXP_TIMER < newRecordTimer) {

			newRecordTimer = 0;
			//newRecordAlpha = 0;
			//newRecordExp = { 0.6f,0.6f };

		}

		// �T�C�Y�𓮂����B
		newRecordExp.x += (1.0f - newRecordExp.x) / 5.0f;
		newRecordExp.y += (1.0f - newRecordExp.y) / 5.0f;

		// �A���t�@���K��l�ɋ߂Â���B
		newRecordAlpha += (255 - newRecordAlpha) / 5.0f;

		// ���W���Z�b�g����B
		const Vec2<float> POS = { 985,540 };
		newRecordPos = POS;

		// �A�j���[�V�����̃^�C�}�[���X�V����B
		++newRecordAnimTimer;
		const int ANIM_TIMER = 10;
		if (ANIM_TIMER < newRecordAnimTimer) {

			newRecordAnimTimer = 0;
			++newRecordIndex;
			if (3 <= newRecordIndex) {

				newRecordIndex = 0;

			}

		}

	}


}

#include"IntoTheAbyss/ClearInfoContainer.h"
void ResultScene::OnDraw()
{
	KuroEngine::Instance().Graphics().SetRenderTargets({ D3D12App::Instance()->GetBackBuffRenderTarget() });

	//ResultTransfer::Instance()->Draw();

	// �w�i�̐���`��
	BackGroundParticleMgr::Instance()->Draw();

	// �w�i�ɃL�����N�^�[��`��B
	DrawFunc::DrawRotaGraph2D(backGroundCharaPos, Vec2<float>(0.5f, 0.5f), backGroundCharaAngle, TexHandleMgr::GetTexBuffer(useHandle[backGroundCharaAnimHandle]));

	// ���T�[�W���Ȑ��̓������擾�B
	float lissajousMove = 10.0f;
	Vec2<float> lissajousCurve = Vec2<float>(cosf(1.0f * lissajousTimer) * lissajousMove, sinf(2.0f * lissajousTimer) * lissajousMove);

	// ���҂̃t���[�����擾����B
	DrawFunc::DrawRotaGraph2D(lissajousCurve + WinApp::Instance()->GetExpandWinCenter() + Vec2<float>(-280.0f, 0.0f), Vec2<float>(1.0f, 1.0f), 0, TexHandleMgr::GetTexBuffer(winnerFrameHandle));

	// ���҂�`�悷��B
	if (CharacterManager::Instance()->Left()->GetCharacterName() == PLAYABLE_CHARACTER_NAME::PLAYABLE_LACY) {

		DrawFunc::DrawRotaGraph2D(lissajousCurve + WinApp::Instance()->GetExpandWinCenter() + Vec2<float>(-280.0f, 0.0f), Vec2<float>(1.0f, 1.0f), 0, TexHandleMgr::GetTexBuffer(lacyHandle));

	}
	else if (CharacterManager::Instance()->Left()->GetCharacterName() == PLAYABLE_CHARACTER_NAME::PLAYABLE_LUNA) {

		DrawFunc::DrawRotaGraph2D(lissajousCurve + WinApp::Instance()->GetExpandWinCenter() + Vec2<float>(-280.0f, 0.0f), Vec2<float>(1.0f, 1.0f), 0, TexHandleMgr::GetTexBuffer(lunaHandle));

	}

	// �����ɃL������\������ɂ������đS�̓I�ɉE���ɕ`������炷�p�̒l�B
	static const float OFFSET_X = -30.0f;

	Vec2<float> windowSize = { static_cast<float>(WinApp::Instance()->GetWinSize().x), static_cast<float>(WinApp::Instance()->GetWinSize().y) };

	{
		float easingPosY = resultEasingAmount * (windowSize.y - RESULT_POS.y);
		DrawFunc::DrawGraph(lissajousCurve + Vec2<float>(580.0f + OFFSET_X, windowSize.y - easingPosY), TexHandleMgr::GetTexBuffer(resultHandle));

		DrawBreakCount(breakCountEasingAmount, ceil(breakCount), ScoreKeep::Instance()->GetMaxNum(), OFFSET_X, lissajousCurve);
	}

	if (bigFontFlag && timeUpFlag)
	{
		if (evaluationFlag)
		{
			DrawFunc::DrawRotaGraph2D(lissajousCurve + Vec2<float>(evaluationPosX + OFFSET_X, windowSize.y / 2.0f - 80.0f + easeEvaluationPosY), Vec2<float>(0.7f, 0.7f), 0.0f, TexHandleMgr::GetTexBuffer(perfectHandle[perfectIndex]));
		}
		else
		{
			DrawFunc::DrawRotaGraph2D(lissajousCurve + Vec2<float>(evaluationPosX - 30 + OFFSET_X, windowSize.y / 2.0f - 80.0f + easeEvaluationPosY), Vec2<float>(0.7f, 0.7f), 0.0f, TexHandleMgr::GetTexBuffer(evaluationNowHandle));
		}
	}

	// �V�L�^�̉摜��`�悷��B
	if (isNewRecord) {

		DrawFunc::DrawRotaGraph2D(newRecordPos + lissajousCurve, newRecordExp, 0, TexHandleMgr::GetTexBuffer(newRecordHandle[newRecordIndex]), Color(255, 255, 255, newRecordAlpha));

	}

	crt.Excute(D3D12App::Instance()->GetCmdList(), D3D12App::Instance()->GetBackBuffRenderTarget());
	KuroEngine::Instance().Graphics().SetRenderTargets({ D3D12App::Instance()->GetBackBuffRenderTarget() });
	crt.DrawResult(AlphaBlendMode_None);


	//�X�N���[���V���b�g�����܂ł̊Ԋu��������
	if (endFlg)
	{
		++ssIntervalTimer;
	}
	const int SCREEN_SHOT_TIME = 2;
	//�N���A���̃X�N�V��
	auto& nowContainer = ClearInfoContainerMgr::Instance()->GetContainer(SelectStage::Instance()->GetStageNum());

	if (nowContainer.maxBreakCount < breakCount) {

		isNewRecord = true;

	}

	if (endFlg && nowContainer.maxBreakCount < breakCount && SCREEN_SHOT_TIME <= ssIntervalTimer)
	{
		KuroEngine::Instance().Graphics().ClearRenderTarget(nowContainer.clearInfoRenderTarget);
		KuroEngine::Instance().Graphics().SetRenderTargets({ nowContainer.clearInfoRenderTarget });

		float easingPosY = resultEasingAmount * (windowSize.y - RESULT_POS.y);
		DrawFunc::DrawGraph(lissajousCurve + Vec2<float>(580.0f, windowSize.y - easingPosY), TexHandleMgr::GetTexBuffer(resultHandle));

		DrawBreakCount(breakCountEasingAmount, ceil(breakCount), ScoreKeep::Instance()->GetMaxNum(), OFFSET_X, lissajousCurve);

		if (bigFontFlag && timeUpFlag)
		{
			if (evaluationFlag)
			{
				DrawFunc::DrawRotaGraph2D(lissajousCurve + Vec2<float>(evaluationPosX, windowSize.y / 2.0f - 80.0f + easeEvaluationPosY), Vec2<float>(0.7f, 0.7f), 0.0f, TexHandleMgr::GetTexBuffer(perfectHandle[perfectIndex]));
			}
			else
			{
				DrawFunc::DrawRotaGraph2D(lissajousCurve + Vec2<float>(evaluationPosX - 30, windowSize.y / 2.0f - 80.0f + easeEvaluationPosY), Vec2<float>(0.7f, 0.7f), 0.0f, TexHandleMgr::GetTexBuffer(evaluationNowHandle));
			}
		}

		nowContainer.clear = true;
		nowContainer.maxBreakCount = breakCount;
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

void ResultScene::DrawBreakCount(float scoreEasingAmount, int BREAK_NOW_COUNT, int BREAK_MAX_COUNT, float OFFSET_X, Vec2<float> LISSAJOUS)
{
	Vec2<float> windowSize = { static_cast<float>(WinApp::Instance()->GetWinSize().x), static_cast<float>(WinApp::Instance()->GetWinSize().y) };

	const float baseX = 500.0f;
	float easingPosY = scoreEasingAmount * (windowSize.y / 2.0f + baseX);
	Vec2<float> drawPos = Vec2<float>(1020.0f, windowSize.y + baseX - easingPosY);


	const int FONT_SIZE = 66.3f;

	float leftX = 0.0f;
	//����
	{
		nowSize = breakSize;
		for (int i = 0; i < KuroFunc::GetDigit(BREAK_MAX_COUNT) - 1; ++i)
		{
			drawPos.x -= FONT_SIZE * nowSize.x;
		}
		leftX = drawPos.x;


		std::vector<int>number = KazHelper::CountNumber(BREAK_NOW_COUNT, KuroFunc::GetDigit(BREAK_MAX_COUNT));
		for (int i = 0; i < number.size(); ++i)
		{
			// ���������߂ĕ`�悷��B
			int disit = GetDisit(BREAK_NOW_COUNT, i);

			if (!bigFontFlag)
			{
				// �X�R�A���O�t���[���̒l�ƈ���Ă�����傫������B
				if (prevScore[i] != disit) scoreSize[i] = defaultSize + 0.5f;
			}

			DrawFunc::DrawRotaGraph2D(LISSAJOUS + drawPos + Vec2<float>(OFFSET_X, 0), Vec2<float>(scoreSize[i], scoreSize[i]), 0.0f, TexHandleMgr::GetTexBuffer(blueNumberHandle[number[i]]));
			// �t�H���g�T�C�Y���ړ�������B
			drawPos.x += FONT_SIZE * nowSize.x;

			// ���̃t���[���̃X�R�A��ۑ��B
			prevScore[i] = disit;
		}
	}

	evaluationPosX = leftX + (drawPos.x - leftX) / 2.0f;
	evaluationPosX -= 20.0f;

	//�X���b�V��
	DrawFunc::DrawRotaGraph2D(LISSAJOUS + drawPos + Vec2<float>(OFFSET_X, 0), nowSize, 0.0f, TexHandleMgr::GetTexBuffer(slashHandle));

	//�ő�
	{
		drawPos.x += FONT_SIZE * (nowSize.x / 2.0f);
		drawPos.y += 25.0f;
		nowSize = maxSize;
		std::vector<int>number = KazHelper::CountNumber(BREAK_MAX_COUNT);
		for (int i = 0; i < number.size(); ++i)
		{
			DrawFunc::DrawRotaGraph2D(LISSAJOUS + drawPos + Vec2<float>(OFFSET_X, 0), nowSize, 0.0f, TexHandleMgr::GetTexBuffer(blueNumberHandle[number[i]]));
			// �t�H���g�T�C�Y���ړ�������B
			drawPos.x += FONT_SIZE * nowSize.x;
		}
	}



	//Vec2<float> windowSize = {  static_cast<float>WinApp::Instance()->GetWinSize().x,  static_cast<float>WinApp::Instance()->GetWinSize().y };

	//float easingPosX = scoreEasingAmount * (SCORE_POS.x - windowSize.x);


	//SCORE_POS.y =  static_cast<float>WINDOW_CENTER.y + 180.0f;
	//Vec2<float> drawPos = Vec2<float>(windowSize.x + easingPosX - 150.0f, SCORE_POS.y);
	//// �����̃t�H���g�T�C�Y
	//const int FONT_SIZE = 66.3f * defaultSize;

	//// ���݂̐��������߂�B
	//int nowScore = scoreEffectEasingTimer * targetScore;

	//drawPos.y = 640.0f;

	//for (int index = 10; 0 < index; --index) {

	//	// ���������߂ĕ`�悷��B
	//	int disit = GetDisit(nowScore, index - 1);

	//	// �X�R�A���O�t���[���̒l�ƈ���Ă�����傫������B
	//	if (prevScore[index - 1] != disit) scoreSize[index - 1] = 1.5f;

	//	DrawFunc::DrawRotaGraph2D(drawPos, Vec2<float>(scoreSize[index - 1], scoreSize[index - 1]), 0, TexHandleMgr::GetTexBuffer(goldNumberHandle[disit]));

	//	// �t�H���g�T�C�Y���ړ�������B
	//	drawPos.x += FONT_SIZE;

	//	// ���̃t���[���̃X�R�A��ۑ��B
	//	prevScore[index - 1] = disit;

	//}

}
