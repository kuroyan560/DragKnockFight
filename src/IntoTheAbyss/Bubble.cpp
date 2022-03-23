#include "Bubble.h"
#include "DrawFunc.h"
#include "ScrollMgr.h"
#include "ShakeMgr.h"
#include "TexHandleMgr.h"
#include "KuroMath.h"

Bubble::Bubble()
{

	/*===== constructor =====*/

	pos = {};
	radius = 0;
	breakCoolTime = 0;
	isBreak = false;
	graphHandle = TexHandleMgr::LoadGraph("resource/IntoTheAbyss/Bubble.png");
	scale = {};
	easingTimer = 30;
	easingFlag = false;
	easingScale = {};

}

void Bubble::Generate(const Vec2<float>& generatePos)
{

	/*===== generate =====*/

	pos = generatePos;
	radius = RADIUS;
	breakCoolTime = 0;
	isBreak = false;
	scale = {};
	easingTimer = 30;
	easingFlag = false;
	easingScale = {};

}

void Bubble::Init()
{

	/*===== initialize =====*/

	pos = {};
	radius = 0;
	breakCoolTime = 0;
	isBreak = false;

}

void Bubble::Update()
{

	/*===== update =====*/

	// If it's broken
	if (isBreak) {

		// update cooltime
		if (breakCoolTime > 0) --breakCoolTime;

		// cooltime is over
		if (breakCoolTime <= 0) isBreak = false;

	}

	// update radius
	radius += (RADIUS - radius) / 10.0f;

	// updateEasingTimer
	++easingTimer;
	if (easingTimer >= EASING_TIMER) {

		easingTimer = 0;
		easingFlag = easingFlag ? false : true;

	}

	// updateEasing
	if (easingFlag) {

		//easingScale.x = KuroMath::Ease(Cubic, Out, (float)easingTimer / EASING_TIMER, 0, 1.0f);

	}

}

void Bubble::Draw()
{

	/*===== draw =====*/

	Vec2<float> centerPos = pos;

	centerPos.x -= ScrollMgr::Instance()->scrollAmount.x - ShakeMgr::Instance()->shakeAmount.x;
	centerPos.y -= ScrollMgr::Instance()->scrollAmount.y - ShakeMgr::Instance()->shakeAmount.y;

	Vec2<float> leftUpPos = centerPos - Vec2<float>(radius, radius) - easingScale;
	Vec2<float> rightDownPos = centerPos + Vec2<float>(radius, radius) + easingScale;

	DrawFunc::DrawExtendGraph2D(leftUpPos, rightDownPos, TexHandleMgr::GetTexBuffer(graphHandle));

}
