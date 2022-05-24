#include "SwingDestroyCounter.h"
#include "KuroMath.h"

SwingDestroyCounter::SwingDestroyCounter()
{

	/*===== コンストラクタ =====*/

	for (int index = 0; index < DESTROY_COUNTER; ++index) {



	}

	nowCounter = 0;

}

void SwingDestroyCounter::Init()
{

	/*===== 初期化処理 =====*/

	for (int index = 0; index < DESTROY_COUNTER; ++index) {



	}

	nowCounter = 0;

}

void SwingDestroyCounter::Update()
{

	/*===== 更新処理 =====*/

	for (int index = 0; index < DESTROY_COUNTER; ++index) {



	}

}

void SwingDestroyCounter::Draw()
{

	/*===== 描画処理 =====*/

	for (int index = 0; index < DESTROY_COUNTER; ++index) {



	}

}

void SwingDestroyCounter::Increment()
{

	/*===== カウントを更新する =====*/

	++nowCounter;

}

DestroyCounter::DestroyCounter()
{

	/*===== コンストラクタ =====*/

	disit = 0;
	easingTimer = 0;
	isActive = false;
	status = STATUS::EXIT;

}

void DestroyCounter::Init()
{

	/*===== 初期化処理 =====*/

	disit = 0;
	easingTimer = 0;
	isActive = false;
	status = STATUS::EXIT;

}

void DestroyCounter::SetExtry(const int& Disit)
{

	/*===== 開始する =====*/

	disit = Disit;
	easingTimer = 0;
	isActive = true;
	status = STATUS::ENTRY;

}

void DestroyCounter::SetExit()
{

	/*===== 終了する =====*/

	easingTimer = 0;
	isActive = true;
	status = STATUS::EXIT;

}

void DestroyCounter::Update(const Vec2<float>& CharaPos)
{

	/*===== 更新処理 =====*/

	switch (status)
	{
	case DestroyCounter::STATUS::ENTRY:

		/*----- 開始段階だったら -----*/

		easingTimer += ADD_EASING_TIMER;

		if (1.0f <= easingTimer) {

			easingTimer = 1.0f;

		}

		// イージングを元に座標を計算する。
		float easingAmount = KuroMath::Ease(Out, Exp, easingTimer, 0.0f, 1.0f);
		pos = CharaPos;
		pos.y -= easingAmount * EASING_CHANGE;

		// イージングタイマーが1.0fを超えたらステータスを更新する。
		if (1.0f <= easingTimer) {

			status = STATUS::STAY;

		}

		break;
	case DestroyCounter::STATUS::STAY:

		/*----- 停止段階だったら -----*/

		// 今のところは何も行わない。

		break;
	case DestroyCounter::STATUS::EXIT:

		/*----- 終了段階だったら -----*/

		easingTimer += ADD_EASING_TIMER;

		if (1.0f <= easingTimer) {

			easingTimer = 1.0f;

		}

		// イージングを元に座標を計算する。
		float easingAmount = KuroMath::Ease(Out, Exp, easingTimer, 0.0f, 1.0f);
		pos = CharaPos;
		pos.y += easingAmount * EASING_CHANGE;

		// イージングタイマーが1.0fを超えたらステータスを更新する。
		if (1.0f <= easingTimer) {

			Init();

		}


		break;
	default:
		break;
	}

}

void DestroyCounter::Draw()
{
}
