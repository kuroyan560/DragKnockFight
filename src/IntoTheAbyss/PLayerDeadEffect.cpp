#include "PLayerDeadEffect.h"
#include "SlowMgr.h"
#include "DrawFunc.h"
#include "WinApp.h"
#include "ShakeMgr.h"
#include "ParticleMgr.h"

PlayerDeadEffect::PlayerDeadEffect()
{

	// �������B
	playerTurnWhiteTimer = 0;
	playerExitDelayTimer = 0;
	particleStopTimer = 0;
	blackOutTimer = 0;
	slowTimer = 0;
	isActive = false;
	isExitPlayer = false;

}

void PlayerDeadEffect::Init()
{

	// �������B
	playerTurnWhiteTimer = 0;
	playerExitDelayTimer = 0;
	particleStopTimer = 0;
	blackOutTimer = 0;
	slowTimer = 0;
	isActive = false;

}

void PlayerDeadEffect::Activate(Vec2<float>& playerPos)
{

	// �������B
	this->playerPos = playerPos;
	playerTurnWhiteTimer = 0;
	playerExitDelayTimer = 0;
	particleStopTimer = 0;
	blackOutTimer = 0;
	slowTimer = 0;
	isActive = true;
	isExitPlayer = false;

}

void PlayerDeadEffect::Update()
{

	/*===== �X�V���� =====*/

	// �L��������Ă��Ȃ������珈�����΂��B
	if (!isActive) return;


	/*-- SLOW�^�C�}�[���X�V --*/

	int totalTimer = playerTurnWhiteTimer + playerExitDelayTimer + particleStopTimer;


	/*-- �v���C���[�������Ȃ�܂ł̃^�C�}�[���X�V --*/
	{

		// �^�C�}�[���K��l�ɒB���Ă��Ȃ�������B
		if (playerTurnWhiteTimer < PLAYER_TURN_WHITE_TIMER) {

			++playerTurnWhiteTimer;

		}

		// �v���C���[�̔������v�Z
		playerWhitePar = 1.0f - ((float)playerTurnWhiteTimer / PLAYER_TURN_WHITE_TIMER);

	}


	/*-- �����Ȃ����v���C���[��������܂ł̃^�C�}�[���X�V --*/
	{

		// �u�^�C�}�[���K��l�ɒB���Ă��Ȃ��v�A�u�����Ȃ�^�C�}�[���K��l�ɒB���Ă���v
		if (playerExitDelayTimer < PLAYER_EXIT_TIMER && PLAYER_TURN_WHITE_TIMER <= playerTurnWhiteTimer) {

			++playerExitDelayTimer;

		}

		// �^�C�}�[���ő�l�ɒB������v���C���[�̕`���؂��āA�p�[�e�B�N���𐶐�����B
		if (PLAYER_EXIT_TIMER <= playerExitDelayTimer && particleStopTimer == 0) {

			isExitPlayer = true;

			ShakeMgr::Instance()->maxShakeAmount = 20.0f;

		}

	}


	/*-- �p�[�e�B�N�������S�Ɏ~�܂�܂ł̃^�C�}�[���X�V --*/
	{

		// �u�^�C�}�[���K��l�ɒB���Ă��Ȃ��v�A�u�����Ȃ����v���C���[��������܂ł̃^�C�}�[���K��l�ɒB���Ă���v
		if (particleStopTimer < PARTICLE_STOP_TIMER && PLAYER_EXIT_TIMER <= playerExitDelayTimer) {

			++particleStopTimer;

		}

		// �������v�Z
		particleSpeedPar = 1.0f - ((float)particleStopTimer / PARTICLE_STOP_TIMER);

	}


	/*-- �Ó]�^�C�}�[���X�V --*/
	{

		// �u�^�C�}�[���K��l�ɒB���Ă��Ȃ��v�A�u�p�[�e�B�N�������S�Ɏ~�܂�܂ł̃^�C�}�[���K��l�ɒB���Ă���v
		if (blackOutTimer < BLACK_OUT_TIMER && PARTICLE_STOP_TIMER <= particleStopTimer) {

			++blackOutTimer;

		}

		// �Ó]�^�C�}�[���ő�l�ɒB������A���S���o�𖳌�������B
		if (BLACK_OUT_TIMER <= blackOutTimer) {

			Init();

			// SLOW��؂�B
			SlowMgr::Instance()->playerDeadSlow = 1.0f;

		}

	}

}

void PlayerDeadEffect::Draw()
{

	/*===== �`�揈�� =====*/

	// �L��������Ă��Ȃ������珈�����΂��B
	if (!isActive) return;

	// �v���C���[�������Ă�����p�[�e�B�N����`�悷��B
	if (PLAYER_EXIT_TIMER <= playerExitDelayTimer)
	{



	}

	// �Ó]�^�C�}�[���N�����Ă�����Ó]������B
	if (0 < blackOutTimer) {

		Color cal = {};

		// �^�C�}�[�������ȉ���������
		if (blackOutTimer < BLACK_OUT_TIMER / 2.0f) {

			float par = ((float)blackOutTimer / (BLACK_OUT_TIMER / 2.0f));

			cal.SetColor({ 0, 0, 0, (int)(par * 255.0f) });

			Vec2<float> windowSize = { 1280,740 };
			DrawFunc::DrawBox2D({ 0,0 }, windowSize, cal, DXGI_FORMAT_R8G8B8A8_UNORM, true, AlphaBlendMode_Trans);

		}
		// �^�C�}�[�������ȏゾ������
		else {

			float par = 1.0f - ((blackOutTimer / 2.0f) / (BLACK_OUT_TIMER / 2.0f));

			cal.SetColor({ 0, 0, 0, (int)(par * 255.0f) });

			Vec2<float> windowSize = { 1280,740 };
			DrawFunc::DrawBox2D({ 0,0 }, windowSize, cal, DXGI_FORMAT_R8G8B8A8_UNORM, true, AlphaBlendMode_Trans);

		}

	}

}


/*========== �ȉ����S�p�[�e�B�N�� ==========*/


PlayerDeadParticle::PlayerDeadParticle()
{

	pos = {};
	this->fowardVec = {};
	radius = DEF_RADIUS;
	speed = MAX_SPEED;
	isActive = false;

}

void PlayerDeadParticle::Generate(const Vec2<float>& generatePos, const Vec2<float>& forwardVec)
{

	/*===== �������� =====*/

	pos = generatePos;
	this->fowardVec = forwardVec;
	radius = DEF_RADIUS;
	speed = MAX_SPEED;
	isActive = true;

}

void PlayerDeadParticle::Update()
{

	/*===== �X�V���� =====*/

	// �ړ�������B
	

}

void PlayerDeadParticle::Draw()
{
}
