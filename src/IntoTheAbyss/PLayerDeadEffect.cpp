#include "PLayerDeadEffect.h"
#include "SlowMgr.h"
#include "DrawFunc.h"
#include "WinApp.h"
#include "ShakeMgr.h"
#include "ParticleMgr.h"

PlayerDeadEffect::PlayerDeadEffect()
{

	// 初期化。
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

	// 初期化。
	playerTurnWhiteTimer = 0;
	playerExitDelayTimer = 0;
	particleStopTimer = 0;
	blackOutTimer = 0;
	slowTimer = 0;
	isActive = false;

}

void PlayerDeadEffect::Activate(Vec2<float>& playerPos)
{

	// 初期化。
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

	/*===== 更新処理 =====*/

	// 有効化されていなかったら処理を飛ばす。
	if (!isActive) return;


	/*-- SLOWタイマーを更新 --*/

	int totalTimer = playerTurnWhiteTimer + playerExitDelayTimer + particleStopTimer;


	/*-- プレイヤーが白くなるまでのタイマーを更新 --*/
	{

		// タイマーが規定値に達していなかったら。
		if (playerTurnWhiteTimer < PLAYER_TURN_WHITE_TIMER) {

			++playerTurnWhiteTimer;

		}

		// プレイヤーの白さを計算
		playerWhitePar = 1.0f - ((float)playerTurnWhiteTimer / PLAYER_TURN_WHITE_TIMER);

	}


	/*-- 白くなったプレイヤーが消えるまでのタイマーを更新 --*/
	{

		// 「タイマーが規定値に達していない」、「白くなるタイマーが規定値に達している」
		if (playerExitDelayTimer < PLAYER_EXIT_TIMER && PLAYER_TURN_WHITE_TIMER <= playerTurnWhiteTimer) {

			++playerExitDelayTimer;

		}

		// タイマーが最大値に達したらプレイヤーの描画を切って、パーティクルを生成する。
		if (PLAYER_EXIT_TIMER <= playerExitDelayTimer && particleStopTimer == 0) {

			isExitPlayer = true;

			ShakeMgr::Instance()->maxShakeAmount = 20.0f;

		}

	}


	/*-- パーティクルが完全に止まるまでのタイマーを更新 --*/
	{

		// 「タイマーが規定値に達していない」、「白くなったプレイヤーが消えるまでのタイマーが規定値に達している」
		if (particleStopTimer < PARTICLE_STOP_TIMER && PLAYER_EXIT_TIMER <= playerExitDelayTimer) {

			++particleStopTimer;

		}

		// 割合を計算
		particleSpeedPar = 1.0f - ((float)particleStopTimer / PARTICLE_STOP_TIMER);

	}


	/*-- 暗転タイマーを更新 --*/
	{

		// 「タイマーが規定値に達していない」、「パーティクルが完全に止まるまでのタイマーが規定値に達している」
		if (blackOutTimer < BLACK_OUT_TIMER && PARTICLE_STOP_TIMER <= particleStopTimer) {

			++blackOutTimer;

		}

		// 暗転タイマーが最大値に達したら、死亡演出を無効化する。
		if (BLACK_OUT_TIMER <= blackOutTimer) {

			Init();

			// SLOWを切る。
			SlowMgr::Instance()->playerDeadSlow = 1.0f;

		}

	}

}

void PlayerDeadEffect::Draw()
{

	/*===== 描画処理 =====*/

	// 有効化されていなかったら処理を飛ばす。
	if (!isActive) return;

	// プレイヤーが消えていたらパーティクルを描画する。
	if (PLAYER_EXIT_TIMER <= playerExitDelayTimer)
	{



	}

	// 暗転タイマーが起動していたら暗転させる。
	if (0 < blackOutTimer) {

		Color cal = {};

		// タイマーが半分以下だったら
		if (blackOutTimer < BLACK_OUT_TIMER / 2.0f) {

			float par = ((float)blackOutTimer / (BLACK_OUT_TIMER / 2.0f));

			cal.SetColor({ 0, 0, 0, (int)(par * 255.0f) });

			Vec2<float> windowSize = { 1280,740 };
			DrawFunc::DrawBox2D({ 0,0 }, windowSize, cal, DXGI_FORMAT_R8G8B8A8_UNORM, true, AlphaBlendMode_Trans);

		}
		// タイマーが半分以上だったら
		else {

			float par = 1.0f - ((blackOutTimer / 2.0f) / (BLACK_OUT_TIMER / 2.0f));

			cal.SetColor({ 0, 0, 0, (int)(par * 255.0f) });

			Vec2<float> windowSize = { 1280,740 };
			DrawFunc::DrawBox2D({ 0,0 }, windowSize, cal, DXGI_FORMAT_R8G8B8A8_UNORM, true, AlphaBlendMode_Trans);

		}

	}

}


/*========== 以下死亡パーティクル ==========*/


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

	/*===== 生成処理 =====*/

	pos = generatePos;
	this->fowardVec = forwardVec;
	radius = DEF_RADIUS;
	speed = MAX_SPEED;
	isActive = true;

}

void PlayerDeadParticle::Update()
{

	/*===== 更新処理 =====*/

	// 移動させる。
	

}

void PlayerDeadParticle::Draw()
{
}
