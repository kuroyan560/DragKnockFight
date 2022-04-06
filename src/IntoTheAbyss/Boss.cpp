#include "Boss.h"

Boss::Boss()
{

	pos = {};
	scale = {};
	forwardVec = {};
	vel = {};

}

void Boss::Init()
{

	pos = {};
	scale = {};
	forwardVec = {};
	vel = {};

}

void Boss::Generate(const Vec2<float>& generatePos)
{

	/*===== �������� =====*/

	pos = generatePos;
	scale = SCALE;
	forwardVec = { 1,0 };
	vel = 0;

}

void Boss::Update()
{

	/*===== �X�V���� =====*/

	vel = OFFSET_VEL;

}
