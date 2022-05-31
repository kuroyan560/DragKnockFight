#include "AfterImage.h"
#include "DrawFunc_Color.h"
#include "TexHandleMgr.h"

void AfterImageMgr::Init()
{

	/*===== 初期化処理 =====*/

	for (int index = 0; index < AFTERIMAGE_COUNT; ++index) {

		afterImages[index].pos = {};
		afterImages[index].alpha = 0.8f;
		afterImages[index].handle = 0;
		afterImages[index].extRate = {};
		afterImages[index].radian = 0;
		afterImages[index].srcColor = Color();
		afterImages[index].isActive = false;

	}

	delay = 0;

}

void AfterImageMgr::Generate(const Vec2<float>& Pos, const Vec2<float>& ExtRate, const float& Radian, const int& Handle, const Color& SrcColor, const bool& IsExtendGraph, const Vec2<float>& Size)
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
		afterImages[index].handle = Handle;
		afterImages[index].extRate = ExtRate;
		afterImages[index].radian = Radian;
		afterImages[index].srcColor = SrcColor;
		afterImages[index].isActive = true;
		afterImages[index].size = Size;
		afterImages[index].isExtendGraph = IsExtendGraph;

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
		static const float SUB_ALPHA = 0.04f;
		afterImages[index].alpha -= SUB_ALPHA;

		// アルファ値が0未満になったら無効化する。
		if (afterImages[index].alpha < 0) {

			afterImages[index].isActive = false;

		}

	}

}
#include "DrawFunc.h"
#include "ScrollMgr.h"
#include "GameSceneCamerMove.h"
void AfterImageMgr::Draw()
{

	/*===== 描画処理 =====*/

	for (int index = 0; index < AFTERIMAGE_COUNT; ++index) {

		// 未生成だったら処理を飛ばす。
		if (!afterImages[index].isActive) continue;

		// 描画。
		Color texColor = afterImages[index].srcColor;
		texColor.a = afterImages[index].alpha;

		// ExtendGraphフラグが立っていたら。
		if (afterImages[index].isExtendGraph) {

			DrawFunc_Color::DrawExtendGraph2D(ScrollMgr::Instance()->Affect(afterImages[index].pos - afterImages[index].size) + GameSceneCameraMove::Instance()->move, ScrollMgr::Instance()->Affect(afterImages[index].pos + afterImages[index].size), TexHandleMgr::GetTexBuffer(afterImages[index].handle), texColor, AlphaBlendMode_None);

		}
		else {

			DrawFunc_Color::DrawRotaGraph2D(ScrollMgr::Instance()->Affect(afterImages[index].pos) + GameSceneCameraMove::Instance()->move, afterImages[index].extRate, afterImages[index].radian, TexHandleMgr::GetTexBuffer(afterImages[index].handle), texColor);

		}
	}

}
