#include "Bubble.h"
#include"DrawFunc.h"

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
	radius = CHECK_HIT_RADIUS;
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
	radius += (CHECK_HIT_RADIUS - radius) / 10.0f;

}

void Bubble::Draw()
{

	/*===== draw =====*/

	DrawFunc::DrawCircle2D(pos, radius, Color(100, 100, 100, 255), true);

}
