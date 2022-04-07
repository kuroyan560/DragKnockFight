#include "HomeBase.h"
#include"../Engine/ImguiApp.h"
#include"../Engine/DrawFunc.h"
#include"ScrollMgr.h"
#include"ShakeMgr.h"

int HomeBase::AREA_NUM = 0;

HomeBase::HomeBase()
{
	if (AREA_NUM == 0)
	{
		name = "PlayerHomeBase";
	}
	else
	{
		name = "EnemyHomeBase";
	}
	++AREA_NUM;
}

void HomeBase::Init(const Vec2<float> &LEFT_UP_POS, const Vec2<float> &RIGHT_DOWN_POS)
{
	leftUpPos = LEFT_UP_POS;
	rightDownPos = RIGHT_DOWN_POS;

	hitBox.size = rightDownPos - leftUpPos;
	centerPos = leftUpPos + (hitBox.size / 2.0f);
	hitBox.center = &centerPos;
}

bool HomeBase::Collision(const Square &OBJ_A)
{
	return AreaCollider::Instance()->CheckHitArea(OBJ_A, hitBox);
}

void HomeBase::Draw()
{
	Vec2<float> scrollShakeZoom = ScrollMgr::Instance()->scrollAmount + ShakeMgr::Instance()->shakeAmount;
	scrollShakeZoom.x *= ScrollMgr::Instance()->zoom;
	scrollShakeZoom.y *= ScrollMgr::Instance()->zoom;

	Vec2<float>drawLeftUpPos = leftUpPos - scrollShakeZoom;
	Vec2<float>drawRightDownPos = rightDownPos - scrollShakeZoom;
	DrawFunc::DrawBox2D(drawLeftUpPos, drawRightDownPos, Color(255, 255, 255, 255), DXGI_FORMAT_R8G8B8A8_UNORM);
}

void HomeBase::Debug()
{
	ImGui::Begin(name.c_str());
	ImGui::InputFloat("leftUpPosX", &leftUpPos.x);
	ImGui::InputFloat("leftUpPosY", &leftUpPos.y);
	ImGui::InputFloat("rightDownPosX", &rightDownPos.x);
	ImGui::InputFloat("rightDownPosY", &rightDownPos.y);
	ImGui::End();
}
