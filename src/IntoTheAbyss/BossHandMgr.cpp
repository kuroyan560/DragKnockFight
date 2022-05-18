#include "BossHandMgr.h"
#include"TexHandleMgr.h"
#include"../Engine/ImguiApp.h"

BossHandMgr::BossHandMgr()
{
	leftHand = std::make_unique<BossHand>(TexHandleMgr::LoadGraph("resource/ChainCombat/boss/0/arm/default/L.png"));
	rightHand = std::make_unique<BossHand>(TexHandleMgr::LoadGraph("resource/ChainCombat/boss/0/arm/default/R.png"));
}

void BossHandMgr::Init()
{
	leftHand->Init();
	rightHand->Init();
}

void BossHandMgr::Update()
{
	leftHand->Update(leftHandData.pos, leftHandData.radius, leftHandData.angle);
	rightHand->Update(rightHandData.pos, rightHandData.radius, rightHandData.angle);
}

void BossHandMgr::Draw()
{
	leftHand->Draw();
	rightHand->Draw();
}

void BossHandMgr::ImGuiDraw()
{
	ImGui::Begin("Hand");
	ImGui::InputFloat("LeftHandPosX", &leftHandData.pos.x);
	ImGui::InputFloat("LeftHandPosY", &leftHandData.pos.y);
	ImGui::InputFloat("LeftRadius", &leftHandData.radius);
	ImGui::SliderFloat("LeftAngle", &leftHandData.angle, -360.0f, 360.0f);
	ImGui::InputFloat("RightHandPosX", &rightHandData.pos.x);
	ImGui::InputFloat("RightHandPosY", &rightHandData.pos.y);
	ImGui::InputFloat("RightRadius", &rightHandData.radius);
	ImGui::SliderFloat("RgihtAngle", &rightHandData.angle, -360.0f, 360.0f);
	ImGui::End();
}
