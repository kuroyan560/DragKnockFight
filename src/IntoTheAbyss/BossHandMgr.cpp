#include "BossHandMgr.h"
#include"TexHandleMgr.h"
#include"../Engine/ImguiApp.h"
#include"../Engine/DrawFunc.h"
#include"../Common/KuroMath.h"

BossHandMgr::BossHandMgr()
{
	leftHand = std::make_unique<BossHand>(TexHandleMgr::LoadGraph("resource/ChainCombat/boss/0/arm/default/L.png"), TexHandleMgr::LoadGraph("resource/ChainCombat/boss/0/arm/hold/L.png"));
	rightHand = std::make_unique<BossHand>(TexHandleMgr::LoadGraph("resource/ChainCombat/boss/0/arm/default/R.png"), TexHandleMgr::LoadGraph("resource/ChainCombat/boss/0/arm/hold/R.png"));
}

void BossHandMgr::Init()
{
	leftHand->Init();
	rightHand->Init();

	centralPos = { 400.0f,400.0f };
	leftHandData.radius = -100.0f;
	rightHandData.pos = { 400.0f,400.0f };
	rightHandData.radius = 100.0f;
}

void BossHandMgr::Update()
{
	if (lockOnFlag)
	{
		//目標地点のベクトルの保存----------------------------------------------
		Vec2<float> vec = (targetPos - centralPos) * -1.0f;
		vec.Normalize();
		float radian = atan2f(vec.y, vec.x);
		//目標地点のベクトルの保存----------------------------------------------



		leftHandData.angle = radian;
		rightHandData.angle = radian + Angle::ConvertToRadian(180.0f);
	}
	else
	{
		leftHandData.angle = Angle::ConvertToRadian(leftAngle);
		rightHandData.angle = Angle::ConvertToRadian(rightAngle);
	}

	leftHandData.pos = centralPos;
	rightHandData.pos = centralPos;

	leftHand->Update(leftHandData.pos, leftHandData.radius, leftHandData.angle, holdFlag);
	rightHand->Update(rightHandData.pos, rightHandData.radius, rightHandData.angle, holdFlag);

}

void BossHandMgr::Draw()
{
	leftHand->Draw();
	rightHand->Draw();
	DrawFunc::DrawCircle2D(centralPos, 1.0f, Color(255, 255, 255, 255));
	DrawFunc::DrawCircle2D(targetPos, 5.0f, Color(255, 0, 0, 255));
	DrawFunc::DrawLine2D(centralPos, targetPos, Color(0, 255, 0, 255));
}

void BossHandMgr::ImGuiDraw()
{
	ImGui::Begin("Hand");
	ImGui::Checkbox("Hold", &holdFlag);
	ImGui::Checkbox("LockOn", &lockOnFlag);
	ImGui::InputFloat("CentralPosX", &centralPos.x);
	ImGui::InputFloat("CentralPosY", &centralPos.y);
	ImGui::InputFloat("LeftRadius", &leftHandData.radius);
	ImGui::SliderFloat("LeftAngle", &leftAngle, -360.0f, 360.0f);
	ImGui::InputFloat("RightRadius", &rightHandData.radius);
	ImGui::SliderFloat("RgihtAngle", &rightAngle, -360.0f, 360.0f);
	ImGui::SliderFloat("TargetX", &targetPos.x, 0.0f, 1280.0f);
	ImGui::SliderFloat("TargetY", &targetPos.y, 0.0f, 720.0f);
	ImGui::End();
}
