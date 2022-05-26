#include "SwingDestroyCounter.h"
#include "KuroMath.h"
#include "KuroFunc.h"
#include "TexHandleMgr.h"
#include "ScrollMgr.h"
#include "DrawFunc.h"

SwingDestroyCounter::SwingDestroyCounter()
{

	/*===== コンストラクタ =====*/

	for (int index = 0; index < DESTROY_COUNTER; ++index) {

		destroyCounter[index].Init();

	}

	nowCounter = 0;

	TexHandleMgr::LoadDivGraph("resource/ChainCombat/UI/num.png", 12, Vec2<int>(12, 1), numberHandle.data());
	TexHandleMgr::LoadDivGraph("resource/ChainCombat/UI/num_yellow.png", 12, Vec2<int>(12, 1), numberHandle_yellow.data());

}

void SwingDestroyCounter::Init()
{

	/*===== 初期化処理 =====*/

	for (int index = 0; index < DESTROY_COUNTER; ++index) {

		destroyCounter[index].Init();

	}

	nowCounter = 0;

}

#include"DebugParameter.h"
#include"ScoreManager.h"
void SwingDestroyCounter::AllExit()
{
	/*===== すべてをExit状態にする =====*/

	for (int index = 0; index < DESTROY_COUNTER; ++index) {

		if (!destroyCounter[index].isActive) continue;

		destroyCounter[index].isExitReservation = true;

	}

	DebugParameter::Instance()->totalCombo += nowCounter;
	//ScoreManager::Instance()->Add(nowCounter);
	ScoreManager::Instance()->destroyPoint = nowCounter;
	nowCounter = 0;

}

void SwingDestroyCounter::Update(const Vec2<float>& CharaPos)
{

	/*===== 更新処理 =====*/

	for (int index = 0; index < DESTROY_COUNTER; ++index) {

		if (!destroyCounter[index].isActive) continue;

		destroyCounter[index].Update(CharaPos);

	}

}

void SwingDestroyCounter::Draw()
{

	/*===== 描画処理 =====*/

	for (int index = 0; index < DESTROY_COUNTER; ++index) {

		if (!destroyCounter[index].isActive) continue;

		if (nowCounter < 100)
		{
			destroyCounter[index].Draw(numberHandle);
		}
		else
		{
			destroyCounter[index].Draw(numberHandle_yellow);
		}
	}

}

void SwingDestroyCounter::Increment(int INCRE_NUM)
{

	/*===== カウントを更新する =====*/

	// 加算する前の値を保存。
	int prevCounter = nowCounter;

	// 加算。
	nowCounter += INCRE_NUM;

	// どこが前フレームと変わっているかを調べる。
	const int NOW_DISIT = KuroFunc::GetDigit(nowCounter);
	for (int index = 0; index < NOW_DISIT; ++index) {

		// 前フレームと変わっていたら。
		if (KuroFunc::GetSpecifiedDigitNum(prevCounter, index) == KuroFunc::GetSpecifiedDigitNum(nowCounter, index)) continue;

		// その場所にあったやつをExit状態にする。
		CheckDisitStayID(static_cast<DestroyCounter::DISIT_ID>(index + 1));

		// 新しくEntry状態にする。
		for (auto& counterIndex : destroyCounter) {

			if (counterIndex.isActive || counterIndex.status != DestroyCounter::STATUS::NONE) continue;

			counterIndex.SetEntry(KuroFunc::GetSpecifiedDigitNum(nowCounter, index), static_cast<DestroyCounter::DISIT_ID>(index + 1));

			break;

		}

	}

}

void SwingDestroyCounter::CheckDisitStayID(const DestroyCounter::DISIT_ID DisitID)
{

	/*===== 指定のDisitIDのIndexを取得 =====*/

	for (auto& index : destroyCounter) {

		if (!index.isActive) continue;
		if (!(index.disitID == DisitID)) continue;

		index.isExitReservation = true;

	}
}

DestroyCounter::DestroyCounter()
{

	/*===== コンストラクタ =====*/

	disit = 0;
	easingTimer = 0;
	isActive = false;
	status = STATUS::NONE;

}

void DestroyCounter::Init()
{

	/*===== 初期化処理 =====*/

	disitID = DISIT_ID::DISIT_NONE;
	disit = 0;
	easingTimer = 0;
	isActive = false;
	status = STATUS::NONE;
	isExitReservation = false;
	alpha = 0;

}

void DestroyCounter::SetEntry(const int& Disit, const DISIT_ID DisitID)
{

	/*===== 開始する =====*/

	disit = Disit;
	disitID = DisitID;
	easingTimer = 0;
	isActive = true;
	exitTimer = EXIT_TIMER;
	status = STATUS::ENTRY;
	alpha = 0;
	exitReservationTimer = 1;

}

void DestroyCounter::SetExit()
{

	/*===== 終了する =====*/

	easingTimer = 0;
	isActive = true;
	status = STATUS::EXIT;
	alpha = 255;

}

void DestroyCounter::Update(const Vec2<float>& CharaPos)
{

	/*===== 更新処理 =====*/

	float easingAmount = 0;

	switch (status)
	{
	case DestroyCounter::STATUS::ENTRY:

		/*----- 開始段階だったら -----*/

		easingTimer += ADD_EASING_TIMER;

		if (1.0f <= easingTimer) {

			easingTimer = 1.0f;

		}

		// イージングを元に座標を計算する。
		easingAmount = KuroMath::Ease(Out, Exp, easingTimer, 0.0f, 1.0f);
		pos = CharaPos;
		pos.y += EASING_CHANGE - (easingAmount * EASING_CHANGE);

		alpha = 255 * easingAmount;

		// イージングタイマーが1.0fを超えたらステータスを更新する。
		if (1.0f <= easingTimer) {

			status = STATUS::STAY;

		}

		break;
	case DestroyCounter::STATUS::STAY:

		/*----- 停止段階だったら -----*/

		// 座標を代入。
		pos = CharaPos;

		// 即Exitするフラグが立っていたら。
		if (isExitReservation) {

			--exitReservationTimer;
			if (exitReservationTimer <= 0) {

				//status = STATUS::EXIT;
				SetExit();
				Init();

			}

		}

		// 自動的に消えるタイマーを更新する。
		//--exitTimer;
		if (exitTimer <= 0) {

			SetExit();

		}


		break;
	case DestroyCounter::STATUS::EXIT:

		/*----- 終了段階だったら -----*/

		easingTimer += ADD_EASING_TIMER;

		if (1.0f <= easingTimer) {

			easingTimer = 1.0f;

		}

		// イージングを元に座標を計算する。
		easingAmount = KuroMath::Ease(Out, Exp, easingTimer, 0.0f, 1.0f);
		pos = CharaPos;
		pos.y -= easingAmount * EASING_CHANGE;

		alpha = 255 - (255 * easingAmount);

		// イージングタイマーが1.0fを超えたらステータスを更新する。
		if (1.0f <= easingTimer) {

			Init();

		}


		break;
	default:
		break;
	}

}

void DestroyCounter::Draw(std::array<int, 12> NumberHandle)
{

	/*===== 描画処理 =====*/

	float zoom = ScrollMgr::Instance()->zoom;
	DrawFunc::DrawRotaGraph2D(ScrollMgr::Instance()->Affect(pos - Vec2<float>(FONT_SIZE * static_cast<int>(disitID), 0)), Vec2<float>(1.4f * zoom, 1.4f * zoom), 0, TexHandleMgr::GetTexBuffer(NumberHandle[disit]), Color(255, 255, 255, alpha));

}
