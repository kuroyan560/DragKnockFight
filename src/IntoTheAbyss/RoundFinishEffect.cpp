#include "RoundFinishEffect.h"
#include "KuroFunc.h"
#include "UsersInput.h"

RoundFinishEffect::RoundFinishEffect()
{

	/*===== コンストラクタ =====*/

	status = EFFECT_STATUS::NUM1_ZOOMIN;
	timer = 0;
	isEnd = true;
	shakeAmount = Vec2<float>();

}

void RoundFinishEffect::Init()
{

	/*===== 初期化処理 =====*/

	status = EFFECT_STATUS::NUM1_ZOOMIN;
	timer = 0;
	isEnd = true;
	shakeAmount = Vec2<float>();

}

void RoundFinishEffect::Start()
{

	/*===== 初期化処理 =====*/

	status = EFFECT_STATUS::NUM1_ZOOMIN;
	timer = 0;
	isEnd = false;
	shakeAmount = Vec2<float>();

}

void RoundFinishEffect::Update(const Vec2<float>& LineCenterPos)
{

	/*===== 更新処理 =====*/


	const float SHAKE_AMOUNT = 10.0f;
	float shakeRate = 0;
	float nowShakeAmount = 0;

	switch (status)
	{
	case RoundFinishEffect::EFFECT_STATUS::NUM1_ZOOMIN:

		/*-- 第一段階 --*/

		// カメラを二人の真ん中にフォーカスさせる。
		Camera::Instance()->Focus(LineCenterPos, 1.0f);

		// タイマーを更新して次へ。
		++timer;
		if (NUM1_ZOOMIN_TIMER <= timer) {

			timer = 0;
			status = EFFECT_STATUS::NUM2_ENEMY_SHAKE;

		}

		break;

	case RoundFinishEffect::EFFECT_STATUS::NUM2_ENEMY_SHAKE:

		/*-- 第二段階 --*/

		// 座標からシェイク量を引く。
		CharacterManager::Instance()->Right()->pos -= shakeAmount;

		// 敵にフォーカスを合わせる。
		Camera::Instance()->Focus(CharacterManager::Instance()->Right()->pos, 1.5f, 0.01f);

		// 敵をシェイクさせる。
		shakeRate = static_cast<float>(timer) / static_cast<float>(NUM2_ENEMY_SHAKE_TIMER);
		nowShakeAmount = shakeRate * SHAKE_AMOUNT;
		shakeAmount = Vec2<float>(KuroFunc::GetRand(nowShakeAmount * 2.0f) - nowShakeAmount, KuroFunc::GetRand(nowShakeAmount * 2.0f) - nowShakeAmount);
		CharacterManager::Instance()->Right()->pos += shakeAmount;

		// タイマーを更新して次へ。
		++timer;
		if (NUM2_ENEMY_SHAKE_TIMER <= timer) {

			// 座標からシェイク量を引く。
			CharacterManager::Instance()->Right()->pos -= shakeAmount;

			timer = 0;
			status = EFFECT_STATUS::NUM3_ENEMY_EXP;
			RoundFinishParticleMgr::Instance()->Generate();

		}

		break;

	case RoundFinishEffect::EFFECT_STATUS::NUM3_ENEMY_EXP:

		/*-- 第三段階 --*/

		// カメラを一気に引く。
		Camera::Instance()->Focus(LineCenterPos, 0.5f, 0.3f);

		// タイマーを更新して次へ。
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

	// ラウンド終了時のパーティクルを更新。
	RoundFinishParticleMgr::Instance()->Update(CharacterManager::Instance()->Right()->pos, CharacterManager::Instance()->Left()->pos);

}

void RoundFinishEffect::Draw()
{

	/*===== 描画処理 =====*/



}
