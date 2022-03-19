#include "Bubble.h"
#include "DrawFunc.h"
#include "ScrollMgr.h"
#include "ShakeMgr.h"

Bubble::Bubble()
{

	/*===== constructor =====*/

	pos = {};
	radius = 0;
	breakCoolTime = 0;
	isBreak = false;

}

void Bubble::Generate(const Vec2<float>& generatePos)
{

	/*===== generate =====*/

	pos = generatePos;
	radius = RADIUS;
	breakCoolTime = 0;
	isBreak = false;

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

}

void Bubble::Draw()
{

	/*===== draw =====*/

	Vec2<float> drawPos = pos;

	drawPos.x -= ScrollMgr::Instance()->scrollAmount.x - ShakeMgr::Instance()->shakeAmount.x;
	drawPos.y -= ScrollMgr::Instance()->scrollAmount.y - ShakeMgr::Instance()->shakeAmount.y;

	DrawFunc::DrawCircle2D(drawPos, radius, Color(100, 100, 100, 255), true);

}
