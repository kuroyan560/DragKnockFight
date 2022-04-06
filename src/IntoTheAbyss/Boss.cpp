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

	/*===== 生成処理 =====*/

	pos = generatePos;
	scale = SCALE;
	forwardVec = { 1,0 };
	vel = 0;

}

void Boss::Update()
{

	/*===== 更新処理 =====*/

	vel = OFFSET_VEL;

}
