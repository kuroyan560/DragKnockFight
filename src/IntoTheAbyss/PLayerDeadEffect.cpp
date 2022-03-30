#include "PLayerDeadEffect.h"
#include "SlowMgr.h"
#include "DrawFunc.h"
#include "WinApp.h"
#include "ShakeMgr.h"
#include "ScrollMgr.h"
#include "ParticleMgr.h"
#include "KuroMath.h"

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

	for (int index = 0; index < PARTICLE_COUNT; ++index) {

		deadParticle[index].Init();

	}

}

void PlayerDeadEffect::Init()
{

	// �������B
	playerTurnWhiteTimer = 0;
	playerExitDelayTimer = 0;
	particleStopTimer = 0;
	blackOutDelayTimer = 0;
	blackOutTimer = 0;
	slowTimer = 0;
	isActive = false;

	for (int index = 0; index < PARTICLE_COUNT; ++index) {

		deadParticle[index].Init();

	}

}

void PlayerDeadEffect::Activate(Vec2<float>& playerPos)
{

	// �������B
	this->playerPos = playerPos;
	playerTurnWhiteTimer = 0;
	playerExitDelayTimer = 0;
	particleStopTimer = 0;
	blackOutDelayTimer = 0;
	blackOutTimer = 0;
	slowTimer = 0;
	isActive = true;
	isExitPlayer = false;

	for (int index = 0; index < PARTICLE_COUNT; ++index) {

		deadParticle[index].Init();

	}

}

void PlayerDeadEffect::Update()
{

	/*===== �X�V���� =====*/

	// �L��������Ă��Ȃ������珈�����΂��B
	if (!isActive) {

		isInitStage = false;
		return;

	}


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


			deadParticle[0].Generate(playerPos, Vec2<float>(1, -1));
			deadParticle[1].Generate(playerPos, Vec2<float>(-1, -1));
			deadParticle[2].Generate(playerPos, Vec2<float>(1, 1));
			deadParticle[3].Generate(playerPos, Vec2<float>(-1, 1));

		}

	}


	/*-- �p�[�e�B�N�������S�Ɏ~�܂�܂ł̃^�C�}�[���X�V --*/
	{

		// �u�^�C�}�[���K��l�ɒB���Ă��Ȃ��v�A�u�����Ȃ����v���C���[��������܂ł̃^�C�}�[���K��l�ɒB���Ă���v
		if (particleStopTimer < PARTICLE_STOP_TIMER && PLAYER_EXIT_TIMER <= playerExitDelayTimer) {

			++particleStopTimer;

		}

		// �������v�Z
		particleSpeedPar = (float)particleStopTimer / PARTICLE_STOP_TIMER;

	}


	/*-- �p�[�e�B�N�����~�܂��Ă���Ó]����܂ł̒x���̃^�C�}�[ --*/
	{

		// �u�^�C�}�[���K��l�ɒB���Ă��Ȃ��v�A�u�p�[�e�B�N�����~�܂�܂ł̃^�C�}�[���K��l�ɒB���Ă���v
		if (blackOutDelayTimer < BLACK_OUT_DELAY_TIMER && PARTICLE_STOP_TIMER <= particleStopTimer) {

			++blackOutDelayTimer;

		}

	}


	/*-- �Ó]�^�C�}�[���X�V --*/
	{

		// �u�^�C�}�[���K��l�ɒB���Ă��Ȃ��v�A�u�p�[�e�B�N�����~�܂��Ă���Ó]����܂ł̒x���^�C�}�[���K��l�ɒB���Ă���v
		if (blackOutTimer < BLACK_OUT_TIMER && BLACK_OUT_DELAY_TIMER <= blackOutDelayTimer) {

			++blackOutTimer;

			// �^�C�}�[�������ȏゾ������
			if (BLACK_OUT_TIMER / 2.0f <= blackOutTimer) {

				// �p�[�e�B�N����������
				for (int index = 0; index < PARTICLE_COUNT; ++index) {

					deadParticle[index].Init();

				}

			}

		}

		// �Ó]�^�C�}�[���ő�l�ɒB������A���S���o�𖳌�������B
		if (BLACK_OUT_TIMER <= blackOutTimer) {

			Init();

			// SLOW��؂�B
			SlowMgr::Instance()->playerDeadSlow = 1.0f;

		}

	}


	/*-- �p�[�e�B�N���̍X�V���� --*/

	for (int index = 0; index < PARTICLE_COUNT; ++index) {

		// �������Ă��Ȃ������珈�����΂��B
		if (!deadParticle[index].isActive) continue;

		deadParticle[index].Update();

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

		for (int index = 0; index < PARTICLE_COUNT; ++index) {

			// �������Ă��Ȃ������珈�����΂��B
			if (!deadParticle[index].isActive) continue;

			deadParticle[index].Draw();

		}

	}

	// �Ó]�^�C�}�[���N�����Ă�����Ó]������B
	if (0 < blackOutTimer) {

		Color cal = {};

		// �^�C�}�[�������ȉ���������
		if (blackOutTimer < BLACK_OUT_TIMER / 3.0f) {

			float par = ((float)blackOutTimer / (BLACK_OUT_TIMER / 3.0f));

			cal.SetColor({ 0, 0, 0, (int)(par * 255.0f) });

			Vec2<float> windowSize = { 1280,740 };
			DrawFunc::DrawBox2D({ 0,0 }, windowSize, cal, DXGI_FORMAT_R8G8B8A8_UNORM, true, AlphaBlendMode_Trans);

		}
		// �^�C�}�[��3/2�ȏゾ������
		else if (blackOutTimer < BLACK_OUT_TIMER / 3.0f * 2.0f) {

			Vec2<float> windowSize = { 1280,740 };
			cal.SetColor({ 0, 0, 0, 255 });
			DrawFunc::DrawBox2D({ 0,0 }, windowSize, cal, DXGI_FORMAT_R8G8B8A8_UNORM, true, AlphaBlendMode_Trans);
			isInitStage = true;

		}
		// �^�C�}�[�������ȏゾ������
		else {

			float blackOutTimerBuff = blackOutTimer - BLACK_OUT_TIMER / 3.0f * 2.0f;

			float par = 1.0f - (blackOutTimerBuff / (BLACK_OUT_TIMER / 3.0f));

			cal.SetColor({ 0, 0, 0, (int)(par * 255.0f) });

			Vec2<float> windowSize = { 1280,740 };
			DrawFunc::DrawBox2D({ 0,0 }, windowSize, cal, DXGI_FORMAT_R8G8B8A8_UNORM, true, AlphaBlendMode_Trans);

			isInitStage = false;

		}

	}

}


/*========== �ȉ����S�p�[�e�B�N�� ==========*/


PlayerDeadParticle::PlayerDeadParticle()
{

	pos = {};
	this->fowardVec = {};
	radius = DEF_RADIUS;
	length = 0;
	isActive = false;

}

void PlayerDeadParticle::Init()
{

	/*===== ���������� =====*/

	pos = {};
	this->fowardVec = {};
	radius = DEF_RADIUS;
	length = 0;
	isActive = false;

}

void PlayerDeadParticle::Generate(const Vec2<float>& generatePos, const Vec2<float>& forwardVec)
{

	/*===== �������� =====*/

	pos = generatePos;
	this->fowardVec = forwardVec;
	radius = DEF_RADIUS;
	length = 0;
	angle = atan2(forwardVec.y, forwardVec.x);
	isActive = true;

}

void PlayerDeadParticle::Update()
{

	/*===== �X�V���� =====*/

	float par = PlayerDeadEffect::Instance()->particleSpeedPar;

	par = KuroMath::Ease(Out, Sine, par, 0.0f, 1.0f);

	// �ړ�������B
	length = MAX_SPEED * par;

	// ��]������B
	angle += 0.15f;

}

void PlayerDeadParticle::Draw()
{

	/*===== �`�揈�� =====*/

	Vec2<float> offsetPos = ScrollMgr::Instance()->scrollAmount;
	offsetPos += ShakeMgr::Instance()->shakeAmount;

	Vec2<float> anglePos = Vec2<float>(cosf(angle) * length, sinf(angle) * length);

	// �p�[�e�B�N���̕`��
	DrawFunc::DrawCircle2D(pos + anglePos - offsetPos, radius, Color(255, 255, 255, 255), true);

}
