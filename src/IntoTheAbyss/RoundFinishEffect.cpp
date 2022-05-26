#include "RoundFinishEffect.h"
#include "KuroFunc.h"
#include "UsersInput.h"
#include "StageMgr.h"
#include "ScoreManager.h"
#include "WinCounter.h"

RoundFinishEffect::RoundFinishEffect()
{

	/*===== �R���X�g���N�^ =====*/

	status = EFFECT_STATUS::NUM1_ZOOMIN;
	timer = 0;
	isEnd = true;
	shakeAmount = Vec2<float>();

}

void RoundFinishEffect::Init()
{

	/*===== ���������� =====*/

	status = EFFECT_STATUS::NUM1_ZOOMIN;
	timer = 0;
	isEnd = true;
	shakeAmount = Vec2<float>();

}

void RoundFinishEffect::Start()
{

	/*===== ���������� =====*/

	status = EFFECT_STATUS::NUM1_ZOOMIN;
	timer = 0;
	isEnd = false;
	shakeAmount = Vec2<float>();

}

void RoundFinishEffect::Update(const Vec2<float>& LineCenterPos)
{

	/*===== �X�V���� =====*/


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

		}

		break;

	case RoundFinishEffect::EFFECT_STATUS::NUM3_ENEMY_EXP:

		/*-- ��O�i�K --*/

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

			timer = 0;
			status = EFFECT_STATUS::NUM4_RETURN_DEFPOS;

		}


		break;

	case RoundFinishEffect::EFFECT_STATUS::NUM4_RETURN_DEFPOS:

		/*-- ��l�i�K --*/

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
			status = EFFECT_STATUS::NUM5_RETURN_PLAYER_DEFPOS;

			++WinCounter::Instance()->countRound;

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

			//++WinCounter::Instance()->countRound;

		}


		break;

	default:

		break;

	}

	// ���E���h�I�����̃p�[�e�B�N�����X�V�B
	RoundFinishParticleMgr::Instance()->Update(CharacterManager::Instance()->Left()->pos);

}

void RoundFinishEffect::Draw()
{

	/*===== �`�揈�� =====*/



}
