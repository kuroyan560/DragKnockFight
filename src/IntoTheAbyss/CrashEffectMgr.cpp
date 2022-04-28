#include "CrashEffectMgr.h"
#include "KuroMath.h"
#include "DrawFunc.h"
#include "ScrollMgr.h"

void ClassEffect::Init()
{

	/*===== 生成処理 =====*/

	isActive = false;

}

void ClassEffect::Generate(const Vec2<float>& Pos)
{

	/*===== 生成処理 =====*/

	pos = Pos;
	size = { 1.0f, 0.0f };
	isExpY = true;
	isActive = true;

}

void ClassEffect::Update()
{

	/*===== 更新処理 =====*/

	// タイマーを更新。
	++timer;

	// Y軸拡張段階だったら
	if (isExpY) {

		// タイマーが既定値を超えたら次の段階へ進む。
		if (EXP_Y_TIMER < timer) {

			timer = 0;
			isExpY = false;

		}
		else {

			// イージング量を求める。
			float easingAmount = KuroMath::Ease(Out, Sine, (float)timer / EXP_Y_TIMER, 0.0f, 1.0f);

			// Y軸を拡張する。
			size.y = easingAmount * EXP_Y_AMOUNT;

		}

	}
	// X軸拡張段階だったら
	else {

		// タイマーが既定値を超えたら無効化する。
		if (EXP_X_TIMER < timer) {

			isActive = false;

		}
		else {

			// イージング量を求める。
			float easingAmount = KuroMath::Ease(Out, Sine, (float)timer / EXP_X_TIMER, 0.0f, 1.0f);

			// Y軸を縮小する。
			size.y = EXP_Y_AMOUNT - (easingAmount * EXP_Y_AMOUNT);

			// X軸を拡張する。
			size.x = easingAmount * EXP_X_AMOUNT + 1.0f;

		}

	}

}

void ClassEffect::Draw()
{

	/*===== 描画処理 =====*/

	Color col = Color();
	int rand = KuroFunc::GetRand(0, 2);
	if (rand == 0) {
		col = { 239, 1, 144,255 };
	}
	else if (rand == 1) {
		col = { 47, 255, 139,255 };
	}

	DrawFunc::DrawBox2D(ScrollMgr::Instance()->Affect(pos - size), ScrollMgr::Instance()->Affect(pos + size), col, true);

}

void CrashEffectMgr::Init()
{

	/*===== 初期化処理 =====*/

	for (int index = 0; index < CRASH_EFFECT_AMOUNT; ++index) {

		crashEffects[index].Init();

	}

}

void CrashEffectMgr::Generate(const Vec2<float>& Pos)
{

	/*===== 生成処理 =====*/

	static const int GENERATE_COUNT = 3;
	for (int generateCount = 0; generateCount < GENERATE_COUNT; ++generateCount) {

		for (int index = 0; index < CRASH_EFFECT_AMOUNT; ++index) {

			// 生成済みだったら処理を飛ばす。
			if (crashEffects[index].GetIsActive()) continue;

			// 生成地点からランダムで位置をずらす。
			const float RANDOM = 200.0f;
			Vec2<float> randomPos = { KuroFunc::GetRand(-RANDOM,RANDOM),KuroFunc::GetRand(-RANDOM,RANDOM) };
			crashEffects[index].Generate(Pos + randomPos);

			break;

		}

	}

}

void CrashEffectMgr::Update()
{

	/*===== 更新処理 =====*/

	for (int index = 0; index < CRASH_EFFECT_AMOUNT; ++index) {

		if (!crashEffects[index].GetIsActive()) continue;

		crashEffects[index].Update();

	}

}

void CrashEffectMgr::Draw()
{

	/*=====	描画処理 =====*/

	for (int index = 0; index < CRASH_EFFECT_AMOUNT; ++index) {

		if (!crashEffects[index].GetIsActive()) continue;

		crashEffects[index].Draw();

	}

}
