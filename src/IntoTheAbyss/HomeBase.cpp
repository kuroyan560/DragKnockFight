#include "HomeBase.h"
#include"../Engine/ImguiApp.h"
#include"../Engine/DrawFunc.h"

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
}

bool HomeBase::Collision(const Sphere &OBJ_A)
{
	return false;
}

void HomeBase::Draw()
{
	DrawFunc::DrawBox2D(leftUpPos, rightDownPos, Color(255, 255, 255, 255), DXGI_FORMAT_R8G8B8A8_UNORM);
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
