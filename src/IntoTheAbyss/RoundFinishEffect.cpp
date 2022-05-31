#include "RoundFinishEffect.h"
#include "KuroFunc.h"
#include "UsersInput.h"
#include "StageMgr.h"
#include "ScoreManager.h"
#include "WinCounter.h"
#include "DrawFunc.h"
#include "ScrollMgr.h"
#include "WinApp.h"
#include"SelectStage.h"
#include <IntoTheAbyss/Boss.h>
#include "RoundCountMgr.h"

RoundFinishEffect::RoundFinishEffect()
{

	/*===== �R���X�g���N�^ =====*/

	status = EFFECT_STATUS::NUM1_ZOOMIN;
	timer = 0;
	isEnd = true;
	shakeAmount = Vec2<float>();

	TexHandleMgr::LoadDivGraph("resource/ChainCombat/UI/perfect.png", 3, Vec2<int>(3, 1), perfectGraph.data());
	goodGraph = TexHandleMgr::LoadGraph("resource/ChainCombat/UI/good.png");
	greatGraph = TexHandleMgr::LoadGraph("resource/ChainCombat/UI/great.png");
	excellentGraph = TexHandleMgr::LoadGraph("resource/ChainCombat/UI/excellent.png");


	evaluationSoundHandle = EvaluationMgr::Instance()->soundData;
}

void RoundFinishEffect::Init()
{

	/*===== ���������� =====*/

	status = EFFECT_STATUS::NUM1_ZOOMIN;
	timer = 0;
	isEnd = true;
	shakeAmount = Vec2<float>();
	changeMap = false;
	perfectExp = {};
}

void RoundFinishEffect::Start(const bool& IsPerfect, const float& Rate, const float& CameraZoom)
{

	/*===== ���������� =====*/

	status = EFFECT_STATUS::NUM1_ZOOMIN;
	timer = 0;
	isEnd = false;
	shakeAmount = Vec2<float>();
	isPerfect = IsPerfect;
	perfectAnimTimer = 0;
	perfectAnimIndex = 0;
	finishLap = false;
	cameraZoom = CameraZoom;
	isEndResultScene = false;

	int stageNum = SelectStage::Instance()->GetStageNum();
	int roomNum = SelectStage::Instance()->GetRoomNum();
	static const float FAIL_PER = EvaluationMgr::Instance()->GetData(stageNum, roomNum).failRate;
	static const float GOOD_PER = EvaluationMgr::Instance()->GetData(stageNum, roomNum).goodRate;
	static const float GREAT_PER = EvaluationMgr::Instance()->GetData(stageNum, roomNum).greatRate;

	// �����̊�������ǂ̉摜���g�p���邩���`�F�b�N����B
	if (Rate <= FAIL_PER)
	{

		soundType = SOUND_FAIL;
		useGraph = goodGraph;

	}
	else if (Rate <= GOOD_PER) {

		soundType = SOUND_GOOD;
		useGraph = goodGraph;

	}
	else if (Rate <= GREAT_PER) {

		soundType = SOUND_GREAT;
		useGraph = greatGraph;

	}
	else {

		soundType = SOUND_EXCELLENT;
		useGraph = excellentGraph;

	}
	addScoreFlag = false;
}

void RoundFinishEffect::Update(const Vec2<float>& LineCenterPos)
{

	static const int EXPLOSION_SE = AudioApp::Instance()->LoadAudio("resource/ChainCombat/sound/break.wav");
	static const int READY_EXPLOSION_SE = AudioApp::Instance()->LoadAudio("resource/ChainCombat/sound/readyExplosion.wav", 0.5f);

	/*===== �X�V���� =====*/

	if (isEnd) return;

	// �p�[�t�F�N�g�̉摜�̓����ʁB
	const float PERFECT_MOVE_POS_Y = -15.0f;

	const float SHAKE_AMOUNT = 10.0f;
	float shakeRate = 0;
	float nowShakeAmount = 0;
	Vec2<float> playerDefPos = {};
	Vec2<float> enemyDefPos = {};
	float lerpRate = 0;
	float lerpRateBuff = 0;

	float mul = 0.0001f;

	switch (status)
	{
	case RoundFinishEffect::EFFECT_STATUS::NUM1_ZOOMIN:

		/*-- ���i�K --*/

		// �J�������l�̐^�񒆂Ƀt�H�[�J�X������B
		Camera::Instance()->Focus(LineCenterPos, cameraZoom + 0.02f, 0.1f);

		// �^�C�}�[���X�V���Ď��ցB
		++timer;
		if (NUM1_ZOOMIN_TIMER <= timer) {

			timer = 0;
			status = EFFECT_STATUS::NUM2_ENEMY_SHAKE;
			AudioApp::Instance()->PlayWave(READY_EXPLOSION_SE);
		}

		break;

	case RoundFinishEffect::EFFECT_STATUS::NUM2_ENEMY_SHAKE:

		/*-- ���i�K --*/

		// ���W����V�F�C�N�ʂ������B
		CharacterManager::Instance()->Right()->pos -= shakeAmount;

		// �G�Ƀt�H�[�J�X�����킹��B
		Camera::Instance()->Focus(CharacterManager::Instance()->Right()->pos, 2.0f, 0.1f);

		// �G���V�F�C�N������B
		shakeRate = static_cast<float>(timer) / static_cast<float>(NUM2_ENEMY_SHAKE_TIMER);
		nowShakeAmount = shakeRate * SHAKE_AMOUNT;
		shakeAmount = Vec2<float>(KuroFunc::GetRand(nowShakeAmount * 2.0f) - nowShakeAmount, KuroFunc::GetRand(nowShakeAmount * 2.0f) - nowShakeAmount);
		CharacterManager::Instance()->Right()->pos += shakeAmount;

		UsersInput::Instance()->ShakeController(0, 0.1f, 6);

		// �^�C�}�[���X�V���Ď��ցB
		++timer;
		if (NUM2_ENEMY_SHAKE_TIMER <= timer) {

			// ���W����V�F�C�N�ʂ������B
			CharacterManager::Instance()->Right()->pos -= shakeAmount;
			std::dynamic_pointer_cast<Boss>(CharacterManager::Instance()->Right())->anim->ChangeAnim(Boss::EXPLOSION_OPEN);

			timer = 0;
			status = EFFECT_STATUS::NUM3_ENEMY_EXP;
			RoundFinishParticleMgr::Instance()->Init();
			RoundFinishParticleMgr::Instance()->Generate(CharacterManager::Instance()->Right()->pos);

			// �p�[�t�F�N�g�̉摜�𓮂����B
			perfectPos = WinApp::Instance()->GetWinDifferRate() * Vec2<float>(static_cast<float>(WinApp::Instance()->GetWinCenter().x), static_cast<float>(WinApp::Instance()->GetWinCenter().y));
			perfectExp = { 0.0f,0.0f };
			perfectMoveAmount = PERFECT_MOVE_POS_Y;
			
			AudioApp::Instance()->StopWave(READY_EXPLOSION_SE);
			AudioApp::Instance()->PlayWave(EXPLOSION_SE);
			AudioApp::Instance()->PlayWave(evaluationSoundHandle[soundType]);
		}

		break;

	case RoundFinishEffect::EFFECT_STATUS::NUM3_ENEMY_EXP:

		/*-- ��O�i�K --*/

		// �J��������C�Ɉ����B
		Camera::Instance()->Focus(LineCenterPos, cameraZoom, 0.3f);

		// �^�C�}�[���X�V���Ď��ցB
		++timer;
		if (NUM3_ENEMY_EXP_TIMER / 2.0f == timer) {

			RoundFinishParticleMgr::Instance()->SetReturn();

			// �J�������v���C���[�ɋ߂Â���B
			//Camera::Instance()->Focus(CharacterManager::Instance()->Left()->pos, 1.0f, 0.1f);

		}
		else if (timer == NUM3_ENEMY_EXP_TIMER * 0.75) {

			//ScoreManager::Instance()->AddDestroyPoint();
			addScoreFlag = true;

			UsersInput::Instance()->ShakeController(0, 0.5f, 12);

			// ���E���h����UI���X�V�B
			RoundCountMgr::Instance()->RoundIncrement();

		}
		else if (timer < NUM3_ENEMY_EXP_TIMER / 2.0f) {

			if (SelectStage::Instance()->HaveNextLap()) {

				// �J��������C�Ɉ����B
				Camera::Instance()->Focus(LineCenterPos, cameraZoom, 0.3f);
				UsersInput::Instance()->ShakeController(0, 1.0f, 5);

			}

		}

		if (NUM3_ENEMY_EXP_TIMER <= timer) {

			timer = 0;
			status = EFFECT_STATUS::NUM4_RETURN_DEFPOS;
			changeMap = true;
			std::dynamic_pointer_cast<Boss>(CharacterManager::Instance()->Right())->anim->ChangeAnim(Boss::EXPLOSION_CLOSE);
		}

		// �p�[�t�F�N�g�̉摜�𓮂����B
		perfectMoveAmount -= perfectMoveAmount / 10.0f;
		perfectPos.y += perfectMoveAmount;
		perfectExp.x += (1.0f - perfectExp.x) / 10.0f;
		perfectExp.y += (1.0f - perfectExp.y) / 10.0f;


		break;

	case RoundFinishEffect::EFFECT_STATUS::NUM4_RETURN_DEFPOS:

		/*-- ��l�i�K --*/

		// �p�[�t�F�N�g�̉摜������������B
		perfectExp.x -= perfectExp.x / 5.0f;
		perfectExp.y -= perfectExp.y / 5.0f;
		if (perfectExp.x <= 0.05f) {

			perfectExp = { 0.0f,0.0f };

		}

		// ���W���K��l�ɖ߂��B
		if (SelectStage::Instance()->HaveNextLap())
		{
			playerDefPos = StageMgr::Instance()->GetPlayerResponePos(SelectStage::Instance()->GetStageNum(), SelectStage::Instance()->GetRoomNum() + 1);
			enemyDefPos = StageMgr::Instance()->GetBossResponePos(SelectStage::Instance()->GetStageNum(), SelectStage::Instance()->GetRoomNum() + 1);


			Camera::Instance()->Init();

			CharacterManager::Instance()->Left()->pos += (playerDefPos - CharacterManager::Instance()->Left()->pos) / 30.0f;
			CharacterManager::Instance()->Right()->pos += (enemyDefPos - CharacterManager::Instance()->Right()->pos) / 30.0f;
		}
		//playerDefPos = Vec2<float>(100, 700);
		//enemyDefPos = Vec2<float>(5000, 700);

		++timer;
		if (NUM4_RETURN_DEFPOS_TIMER <= timer) {

			isEnd = true;
			//timer = 0;
			status = EFFECT_STATUS::NUM1_ZOOMIN;

			++WinCounter::Instance()->countRound;

			perfectExp = {};

			ScoreManager::Instance()->destroyPoint = 0;

			isEndResultScene = true;

		}

		break;

	case RoundFinishEffect::EFFECT_STATUS::NUM5_RETURN_PLAYER_DEFPOS:

		/*-- ��ܒi�K --*/

		// �v���C���[�̍��W���K��l�ɖ߂��B

		++timer;

		// ���W���K��l�ɖ߂��B
		if (SelectStage::Instance()->HaveNextLap())
		{
			playerDefPos = StageMgr::Instance()->GetPlayerResponePos(SelectStage::Instance()->GetStageNum(), SelectStage::Instance()->GetRoomNum() + 1);
			enemyDefPos = StageMgr::Instance()->GetBossResponePos(SelectStage::Instance()->GetStageNum(), SelectStage::Instance()->GetRoomNum() + 1);
		}

		CharacterManager::Instance()->Left()->pos += (playerDefPos - CharacterManager::Instance()->Left()->pos) / 30.0f;


		if (NUM5_RETURN_PLAYER_DEF_POS <= timer) {

			isEnd = true;

			timer = 0;
			status = EFFECT_STATUS::NUM1_ZOOMIN;

			++WinCounter::Instance()->countRound;

		}


		break;

	default:

		break;

	}

	// ���E���h�I�����̃p�[�e�B�N�����X�V�B
	RoundFinishParticleMgr::Instance()->Update(CharacterManager::Instance()->Left()->pos);

	// �p�[�t�F�N�g�摜���A�j���[�V����������B
	++perfectAnimTimer;
	if (PERFECT_ANIM_TIMER <= perfectAnimTimer) {

		perfectAnimTimer = 0;

		++perfectAnimIndex;

		if (EvaluationMgr::Instance()->PERFECT_ANIMATION_INTERVAL <= perfectAnimIndex) {

			perfectAnimIndex = 0;

		}

	}

}

#include <memory>
#include "GameSceneCamerMove.h"

void RoundFinishEffect::Draw()
{
	if (isEnd)return;
	/*===== �`�揈�� =====*/

	std::shared_ptr<TextureBuffer> drawGraph = 0;

	if (isPerfect) {

		drawGraph = TexHandleMgr::GetTexBuffer(perfectGraph[perfectAnimIndex]);

	}
	else {

		drawGraph = TexHandleMgr::GetTexBuffer(useGraph);

	}

	DrawFunc::DrawRotaGraph2D(perfectPos + GameSceneCameraMove::Instance()->move, perfectExp, 0, drawGraph);

}
