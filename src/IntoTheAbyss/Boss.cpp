#include "Boss.h"
#include "ShakeMgr.h"
#include "ScrollMgr.h"
#include "DrawFunc.h"
#include"UsersInput.h"

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

	if (UsersInput::Instance()->Input(DIK_0)) {

		vel = OFFSET_VEL * 5.0f;

	}

}

void Boss::Draw()
{

	/*===== 描画処理 =====*/

	Vec2<float> scrollShakeAmount = ScrollMgr::Instance()->scrollAmount + ShakeMgr::Instance()->shakeAmount;

	DrawFunc::DrawBox2D(pos - scale / 2.0f - scrollShakeAmount, pos + scale / 2.0f - scrollShakeAmount, Color(230, 38, 113, 255), DXGI_FORMAT_R8G8B8A8_UNORM, true);

}
