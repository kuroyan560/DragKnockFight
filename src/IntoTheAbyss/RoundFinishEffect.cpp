#include "RoundFinishEffect.h"
#include "KuroFunc.h"
#include "UsersInput.h"
#include "StageMgr.h"
#include "ScoreManager.h"
#include "WinCounter.h"
#include "DrawFunc.h"
#include "ScrollMgr.h"
#include "WinApp.h"

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

}

void RoundFinishEffect::Init()
{

	/*===== ���������� =====*/

	status = EFFECT_STATUS::NUM1_ZOOMIN;
	timer = 0;
	isEnd = true;
	shakeAmount = Vec2<float>();

}

void RoundFinishEffect::Start(const bool& IsPerfect, const float& Rate)
{

	/*===== ���������� =====*/

	status = EFFECT_STATUS::NUM1_ZOOMIN;
	timer = 0;
	isEnd = false;
	shakeAmount = Vec2<float>();
	isPerfect = IsPerfect;
	perfectAnimTimer = 0;
	perfectAnimIndex = 0;

	static const float GOOD_PER = 0.5f;
	static const float GREAT_PER = 0.8f;

	// �����̊�������ǂ̉摜���g�p���邩���`�F�b�N����B
	if (Rate <= GOOD_PER) {

		useGraph = goodGraph;

	}
	else if (Rate <= GREAT_PER) {

		useGraph = greatGraph;

	}
	else {

		useGraph = excellentGraph;

	}

}

void RoundFinishEffect::Update(const Vec2<float>& LineCenterPos)
{

	/*===== �X�V���� =====*/

	if (isEnd) return;

	// �p�[�t�F�N�g�̉摜�̓����ʁB
	const float PERFECT_MOVE_POS_Y = -15.0f;

	const float SHAKE_AMOUNT = 10.0f;
	float shakeRate = 0;
	float nowShakeAmount = 0;
	Vec2<float> playerDefPos = {};
	Vec2<float> enemyDefPos = {};

	float mul = 0.0001f;

	switch (status)
	{
	case RoundFinishEffect::EFFECT_STATUS::NUM1_ZOOMIN:

		/*-- ���i�K --*/

		// �J�������l�̐^�񒆂Ƀt�H�[�J�X������B
		Camera::Instance()->Focus(LineCenterPos, 1.0f, 0.1f);

		// �^�C�}�[���X�V���Ď��ցB
		++timer;
		if (NUM1_ZOOMIN_TIMER <= timer) {

			timer = 0;
			status = EFFECT_STATUS::NUM2_ENEMY_SHAKE;

		}

		break;

	case RoundFinishEffect::EFFECT_STATUS::NUM2_ENEMY_SHAKE:

		/*-- ���i�K --*/

		// ���W����V�F�C�N�ʂ������B
		CharacterManager::Instance()->Right()->pos -= shakeAmount;

		// �G�Ƀt�H�[�J�X�����킹��B
		Camera::Instance()->Focus(CharacterManager::Instance()->Right()->pos, 1.5f, 0.01f);

		// �G���V�F�C�N������B
		shakeRate = static_cast<float>(timer) / static_cast<float>(NUM2_ENEMY_SHAKE_TIMER);
		nowShakeAmount = shakeRate * SHAKE_AMOUNT;
		shakeAmount = Vec2<float>(KuroFunc::GetRand(nowShakeAmount * 2.0f) - nowShakeAmount, KuroFunc::GetRand(nowShakeAmount * 2.0f) - nowShakeAmount);
		CharacterManager::Instance()->Right()->pos += shakeAmount;

		// �^�C�}�[���X�V���Ď��ցB
		++timer;
		if (NUM2_ENEMY_SHAKE_TIMER <= timer) {

			// ���W����V�F�C�N�ʂ������B
			CharacterManager::Instance()->Right()->pos -= shakeAmount;

			timer = 0;
			status = EFFECT_STATUS::NUM3_ENEMY_EXP;
			RoundFinishParticleMgr::Instance()->Init();
			RoundFinishParticleMgr::Instance()->Generate(CharacterManager::Instance()->Right()->pos);

			// �p�[�t�F�N�g�̉摜�𓮂����B
			perfectPos = WinApp::Instance()->GetWinDifferRate() * Vec2<float>(static_cast<float>(WinApp::Instance()->GetWinCenter().x), static_cast<float>(WinApp::Instance()->GetWinCenter().y));
			perfectExp = { 0.0f,0.0f };
			perfectMoveAmount = PERFECT_MOVE_POS_Y;

		}

		break;

	case RoundFinishEffect::EFFECT_STATUS::NUM3_ENEMY_EXP:

		/*-- ��O�i�K --*/

		// �J��������C�Ɉ����B
		Camera::Instance()->Focus(LineCenterPos, 0.5f, 0.3f);

		// �^�C�}�[���X�V���Ď��ցB
		++timer;
		if (NUM3_ENEMY_EXP_TIMER / 2.0f == timer) {

			RoundFinishParticleMgr::Instance()->SetReturn();

			// �J�������v���C���[�ɋ߂Â���B
			//Camera::Instance()->Focus(CharacterManager::Instance()->Left()->pos, 1.0f, 0.1f);

		}
		else if (timer == NUM3_ENEMY_EXP_TIMER * 0.75) {

			ScoreManager::Instance()->AddDestroyPoint();

		}
		else if (timer < NUM3_ENEMY_EXP_TIMER / 2.0f) {

			// �J��������C�Ɉ����B
			Camera::Instance()->Focus(LineCenterPos, 0.5f, 0.3f);

		}

		if (NUM3_ENEMY_EXP_TIMER <= timer) {

			status = EFFECT_STATUS::NUM1_ZOOMIN;
			timer = 0;
			status = EFFECT_STATUS::NUM4_RETURN_DEFPOS;

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

			perfectExp = { 1.0f,1.0f };

		}

		// ���W���K��l�ɖ߂��B
		playerDefPos = StageMgr::Instance()->GetPlayerResponePos();
		enemyDefPos = StageMgr::Instance()->GetBossResponePos();
		//playerDefPos = Vec2<float>(100, 700);
		//enemyDefPos = Vec2<float>(5000, 700);

		Camera::Instance()->Init();

		CharacterManager::Instance()->Left()->pos += (playerDefPos - CharacterManager::Instance()->Left()->pos) / 30.0f;
		CharacterManager::Instance()->Right()->pos += (enemyDefPos - CharacterManager::Instance()->Right()->pos) / 30.0f;

		++timer;
		if (NUM4_RETURN_DEFPOS_TIMER <= timer) {

			isEnd = true;
			timer = 0;
			status = EFFECT_STATUS::NUM1_ZOOMIN;

			++WinCounter::Instance()->countRound;

			perfectExp = {};

		}

		break;

	case RoundFinishEffect::EFFECT_STATUS::NUM5_RETURN_PLAYER_DEFPOS:

		/*-- ��ܒi�K --*/

		// �v���C���[�̍��W���K��l�ɖ߂��B

		++timer;

		playerDefPos = StageMgr::Instance()->GetPlayerResponePos();
		enemyDefPos = StageMgr::Instance()->GetBossResponePos();

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

		if (3 <= perfectAnimIndex) {

			perfectAnimIndex = 0;

		}

	}

}

#include <memory>

void RoundFinishEffect::Draw()
{

	/*===== �`�揈�� =====*/

	std::shared_ptr<TextureBuffer> drawGraph = 0;

	if (isPerfect) {

		drawGraph = TexHandleMgr::GetTexBuffer(perfectGraph[perfectAnimIndex]);

	}
	else {

		drawGraph = TexHandleMgr::GetTexBuffer(useGraph);

	}

	DrawFunc::DrawRotaGraph2D(perfectPos, perfectExp, 0, drawGraph);

}
