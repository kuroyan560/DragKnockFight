#include "Stamina.h"
#include "ScrollMgr.h"
#include "DrawFunc.h"

Stamina::Stamina()
{

	/*===== 初期化 =====*/

	nowGauge = 100;
	isActivate = true;

}

void Stamina::Init()
{

	/*===== 初期化 =====*/

	nowGauge = 100;
	isActivate = true;

}

void Stamina::Empty()
{

	/*===== 初期化 =====*/

	nowGauge = 0;
	isActivate = false;

}

void Stamina::Update()
{

	/*===== 更新処理 =====*/

	if (nowGauge < MAX_GAUGE) {

		isActivate = false;

	}
	else {

		isActivate = true;

	}

}

void Stamina::Draw(const Vec2<float>& DrawPos, const float& Width, const float& Height)
{

	/*===== 描画処理 =====*/

	// スタミナが溜まっている部分を描画
	float per = nowGauge / MAX_GAUGE;

	// スタミナがマックスだったら
	if (1.0f <= per) {
		
		// アウトライン
		Vec2<float> offset = {2,2};
		DrawFunc::DrawBox2D(ScrollMgr::Instance()->Affect(DrawPos - offset), ScrollMgr::Instance()->Affect(DrawPos + Vec2<float>(Width, Height) + offset), Color(0x29, 0xC9, 0xB4, 0xFF), true);

		// 内側に明るい四角形を描画
		DrawFunc::DrawBox2D(ScrollMgr::Instance()->Affect(DrawPos), ScrollMgr::Instance()->Affect(DrawPos + Vec2<float>(Width, Height)), Color(0x2F, 0xFF, 0x8B, 0xFF), true);

	}
	else {

		Vec2<float> nowGaugePos = DrawPos + Vec2<float>(Width * per, 0);
		DrawFunc::DrawBox2D(ScrollMgr::Instance()->Affect(DrawPos), ScrollMgr::Instance()->Affect(nowGaugePos + Vec2<float>(0, Height)), Color(0x2F, 0xFF, 0x8B, 0xFF), true);

		// スタミナが溜まっていない部分を描画
		DrawFunc::DrawBox2D(ScrollMgr::Instance()->Affect(nowGaugePos), ScrollMgr::Instance()->Affect(DrawPos + Vec2<float>(Width, Height)), Color(0x2F, 0xFF, 0x8B, 0x55), true, AlphaBlendMode_Trans);

	}

}

float Stamina::AddNowGauge(const float& Add)
{

	/*===== ゲージ量を足す =====*/

	nowGauge += Add;

	// 限界を超えていたら差分をリターンする。
	if (MAX_GAUGE < nowGauge) {

		float sub = nowGauge - MAX_GAUGE;

		nowGauge -= sub;

		isActivate = true;

		// 超過量を返す。このゲージの次のゲージに足すため。
		return sub;

	}

	return 0;

}
