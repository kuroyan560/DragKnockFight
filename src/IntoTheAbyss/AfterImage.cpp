#include "AfterImage.h"
#include "DrawFunc_FillTex.h"
#include "TexHandleMgr.h"

void AfterImageMgr::Init()
{

	/*===== 初期化処理 =====*/

	for (int index = 0; index < AFTERIMAGE_COUNT; ++index) {

		afterImages[index].alpha = 0;
		afterImages[index].destHandle = 0;
		afterImages[index].srcHandle = 0;
		afterImages[index].isActive = false;
		afterImages[index].pos = {};

	}

	delay = 0;

}

void AfterImageMgr::Generate(const Vec2<float>& Pos, const Vec2<float>& ExtRate, const float& Radian, const int& SrcHandle, const int& DestHandle)
{

	/*===== 生成処理 =====*/

	// 遅延が発生していたら処理を飛ばす。
	--delay;
	if (0 < delay) return;

	// 未生成のオブジェクトを探して生成。
	for (int index = 0; index < AFTERIMAGE_COUNT; ++index) {

		// 生成済みだったら処理を飛ばす。
		if (afterImages[index].isActive) continue;

		// 生成する。
		afterImages[index].pos = Pos;
		afterImages[index].alpha = 0.5f;
		afterImages[index].srcHandle = SrcHandle;
		afterImages[index].destHandle = DestHandle;
		afterImages[index].extRate = ExtRate;
		afterImages[index].radian = Radian;
		afterImages[index].isActive = true;

		break;

	}

	// 遅延を生成。
	delay = DELAY;

}

void AfterImageMgr::Update()
{

	/*===== 更新処理 =====*/

	// アルファ値をだんだん下げる更新処理。
	for (int index = 0; index < AFTERIMAGE_COUNT; ++index) {

		// 未生成だったら処理を飛ばす。
		if (!afterImages[index].isActive) continue;

		// アルファ値を下げる。
		static const float SUB_ALPHA = 0.05f;
		afterImages[index].alpha -= SUB_ALPHA;

		// アルファ値が0未満になったら無効化する。
		if (afterImages[index].alpha < 0) {

			afterImages[index].isActive = false;

		}

	}

}
#include "DrawFunc.h"
#include "ScrollMgr.h"
void AfterImageMgr::Draw()
{

	/*===== 描画処理 =====*/

	for (int index = 0; index < AFTERIMAGE_COUNT; ++index) {

		// 未生成だったら処理を飛ばす。
		if (!afterImages[index].isActive) continue;

		// 描画。
		DrawFunc_FillTex::DrawRotaGraph2D(ScrollMgr::Instance()->Affect(afterImages[index].pos), afterImages[index].extRate, afterImages[index].radian, TexHandleMgr::GetTexBuffer(afterImages[index].destHandle), TexHandleMgr::GetTexBuffer(afterImages[index].srcHandle), afterImages[index].alpha);

	}

}
