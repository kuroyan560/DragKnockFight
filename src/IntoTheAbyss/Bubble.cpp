#include "Bubble.h"
#include "DrawFunc.h"
#include "ScrollMgr.h"
#include "ShakeMgr.h"
#include "TexHandleMgr.h"
#include "KuroMath.h"
#include "SlowMgr.h"

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
	addEasingTimer = 0;
	isHitBullet = false;

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
	addEasingTimer = 0;
	easingScale = {};
	isHitBullet = false;

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

		radius = 0;
		easingTimer = EASING_TIMER / 2.0f;

	}

	// update radius
	radius += (RADIUS - radius) / 10.0f;

	// updateEasingTimer
	easingTimer += 1.0f * SlowMgr::Instance()->playerDeadSlow;
	easingTimer += addEasingTimer;
	if (easingTimer >= EASING_TIMER) {

		easingTimer = 0;
		easingFlag = easingFlag ? false : true;

	}

	float easingRadius = EASINIG_RADIUS;
	if (isHitBullet) {
		easingRadius += EASINIG_RADIUS / 2.0f;
	}
	else if (addEasingTimer > 0) {
		easingRadius += EASINIG_RADIUS;
	}

	Vec2<float> honraiEasingScale = {};

	// updateEasing
	if (easingFlag) {

		honraiEasingScale.x = KuroMath::Ease(InOut, Sine, (float)easingTimer / EASING_TIMER, 1.0f, 0.0f, 1.0f) * easingRadius;
		honraiEasingScale.y = KuroMath::Ease(InOut, Sine, (float)easingTimer / EASING_TIMER, 1.0f, 0.0f, 1.0f) * -easingRadius + easingRadius;

	}
	else {

		honraiEasingScale.x = KuroMath::Ease(InOut, Sine, (float)easingTimer / EASING_TIMER, 1.0f, 0.0f, 1.0f) * -easingRadius + easingRadius;
		honraiEasingScale.y = KuroMath::Ease(InOut, Sine, (float)easingTimer / EASING_TIMER, 1.0f, 0.0f, 1.0f) * easingRadius;

	}


	// update AddEasingTimer
	if (addEasingTimer > 0) {
		//addEasingTimer -= addEasingTimer / 10.0f;
		addEasingTimer -= 1.0f * SlowMgr::Instance()->playerDeadSlow;

		easingScale.x += (honraiEasingScale.x - easingScale.x) / 1.0f;
		easingScale.y += (honraiEasingScale.y - easingScale.y) / 1.0f;

	}
	else {

		easingScale.x += (honraiEasingScale.x - easingScale.x) / 10.0f;
		easingScale.y += (honraiEasingScale.y - easingScale.y) / 10.0f;

		isHitBullet = false;

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

	DrawFunc::DrawExtendGraph2D(leftUpPos, rightDownPos, TexHandleMgr::GetTexBuffer(graphHandle), AlphaBlendMode_Trans);

}
