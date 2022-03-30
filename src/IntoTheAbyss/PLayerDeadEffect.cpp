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

	// 初期化。
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

	// 初期化。
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

	// 初期化。
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

	/*===== 更新処理 =====*/

	// 有効化されていなかったら処理を飛ばす。
	if (!isActive) {

		isInitStage = false;
		return;

	}


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


			deadParticle[0].Generate(playerPos, Vec2<float>(1, -1));
			deadParticle[1].Generate(playerPos, Vec2<float>(-1, -1));
			deadParticle[2].Generate(playerPos, Vec2<float>(1, 1));
			deadParticle[3].Generate(playerPos, Vec2<float>(-1, 1));

		}

	}


	/*-- パーティクルが完全に止まるまでのタイマーを更新 --*/
	{

		// 「タイマーが規定値に達していない」、「白くなったプレイヤーが消えるまでのタイマーが規定値に達している」
		if (particleStopTimer < PARTICLE_STOP_TIMER && PLAYER_EXIT_TIMER <= playerExitDelayTimer) {

			++particleStopTimer;

		}

		// 割合を計算
		particleSpeedPar = (float)particleStopTimer / PARTICLE_STOP_TIMER;

	}


	/*-- パーティクルが止まってから暗転するまでの遅延のタイマー --*/
	{

		// 「タイマーが規定値に達していない」、「パーティクルが止まるまでのタイマーが規定値に達している」
		if (blackOutDelayTimer < BLACK_OUT_DELAY_TIMER && PARTICLE_STOP_TIMER <= particleStopTimer) {

			++blackOutDelayTimer;

		}

	}


	/*-- 暗転タイマーを更新 --*/
	{

		// 「タイマーが規定値に達していない」、「パーティクルが止まってから暗転するまでの遅延タイマーが規定値に達している」
		if (blackOutTimer < BLACK_OUT_TIMER && BLACK_OUT_DELAY_TIMER <= blackOutDelayTimer) {

			++blackOutTimer;

			// タイマーが半分以上だったら
			if (BLACK_OUT_TIMER / 2.0f <= blackOutTimer) {

				// パーティクルを初期化
				for (int index = 0; index < PARTICLE_COUNT; ++index) {

					deadParticle[index].Init();

				}

			}

		}

		// 暗転タイマーが最大値に達したら、死亡演出を無効化する。
		if (BLACK_OUT_TIMER <= blackOutTimer) {

			Init();

			// SLOWを切る。
			SlowMgr::Instance()->playerDeadSlow = 1.0f;

		}

	}


	/*-- パーティクルの更新処理 --*/

	for (int index = 0; index < PARTICLE_COUNT; ++index) {

		// 生成していなかったら処理を飛ばす。
		if (!deadParticle[index].isActive) continue;

		deadParticle[index].Update();

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

		for (int index = 0; index < PARTICLE_COUNT; ++index) {

			// 生成していなかったら処理を飛ばす。
			if (!deadParticle[index].isActive) continue;

			deadParticle[index].Draw();

		}

	}

	// 暗転タイマーが起動していたら暗転させる。
	if (0 < blackOutTimer) {

		Color cal = {};

		// タイマーが半分以下だったら
		if (blackOutTimer < BLACK_OUT_TIMER / 3.0f) {

			float par = ((float)blackOutTimer / (BLACK_OUT_TIMER / 3.0f));

			cal.SetColor({ 0, 0, 0, (int)(par * 255.0f) });

			Vec2<float> windowSize = { 1280,740 };
			DrawFunc::DrawBox2D({ 0,0 }, windowSize, cal, DXGI_FORMAT_R8G8B8A8_UNORM, true, AlphaBlendMode_Trans);

		}
		// タイマーが3/2以上だったら
		else if (blackOutTimer < BLACK_OUT_TIMER / 3.0f * 2.0f) {

			Vec2<float> windowSize = { 1280,740 };
			cal.SetColor({ 0, 0, 0, 255 });
			DrawFunc::DrawBox2D({ 0,0 }, windowSize, cal, DXGI_FORMAT_R8G8B8A8_UNORM, true, AlphaBlendMode_Trans);
			isInitStage = true;

		}
		// タイマーが半分以上だったら
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


/*========== 以下死亡パーティクル ==========*/


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

	/*===== 初期化処理 =====*/

	pos = {};
	this->fowardVec = {};
	radius = DEF_RADIUS;
	length = 0;
	isActive = false;

}

void PlayerDeadParticle::Generate(const Vec2<float>& generatePos, const Vec2<float>& forwardVec)
{

	/*===== 生成処理 =====*/

	pos = generatePos;
	this->fowardVec = forwardVec;
	radius = DEF_RADIUS;
	length = 0;
	angle = atan2(forwardVec.y, forwardVec.x);
	isActive = true;

}

void PlayerDeadParticle::Update()
{

	/*===== 更新処理 =====*/

	float par = PlayerDeadEffect::Instance()->particleSpeedPar;

	par = KuroMath::Ease(Out, Sine, par, 0.0f, 1.0f);

	// 移動させる。
	length = MAX_SPEED * par;

	// 回転させる。
	angle += 0.15f;

}

void PlayerDeadParticle::Draw()
{

	/*===== 描画処理 =====*/

	Vec2<float> offsetPos = ScrollMgr::Instance()->scrollAmount;
	offsetPos += ShakeMgr::Instance()->shakeAmount;

	Vec2<float> anglePos = Vec2<float>(cosf(angle) * length, sinf(angle) * length);

	// パーティクルの描画
	DrawFunc::DrawCircle2D(pos + anglePos - offsetPos, radius, Color(255, 255, 255, 255), true);

}
