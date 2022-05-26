#include "RoundFinishEffect.h"
#include "KuroFunc.h"
#include "UsersInput.h"

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

	switch (status)
	{
	case RoundFinishEffect::EFFECT_STATUS::NUM1_ZOOMIN:

		/*-- ���i�K --*/

		// �J�������l�̐^�񒆂Ƀt�H�[�J�X������B
		Camera::Instance()->Focus(LineCenterPos, 1.0f);

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
			RoundFinishParticleMgr::Instance()->Generate();

		}

		break;

	case RoundFinishEffect::EFFECT_STATUS::NUM3_ENEMY_EXP:

		/*-- ��O�i�K --*/

		// �J��������C�Ɉ����B
		Camera::Instance()->Focus(LineCenterPos, 0.5f, 0.3f);

		// �^�C�}�[���X�V���Ď��ցB
		++timer;
		if (NUM3_ENEMY_EXP_TIMER / 3 < timer) {

			RoundFinishParticleMgr::Instance()->SetReturn();

		}

		if (UsersInput::Instance()->KeyInput(DIK_V)) {

			status = EFFECT_STATUS::NUM1_ZOOMIN;
			timer = 0;

		}


		break;

	case RoundFinishEffect::EFFECT_STATUS::NUM4_EXP_END:

		break;

	case RoundFinishEffect::EFFECT_STATUS::NUM5_RETURN_DEFPOS:

		break;

	default:

		break;

	}

	// ���E���h�I�����̃p�[�e�B�N�����X�V�B
	RoundFinishParticleMgr::Instance()->Update(CharacterManager::Instance()->Right()->pos, CharacterManager::Instance()->Left()->pos);

}

void RoundFinishEffect::Draw()
{

	/*===== �`�揈�� =====*/



}
