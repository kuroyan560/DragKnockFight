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

void BossHandMgr::Init(bool DEBUG)
{
	leftHand->Init();
	rightHand->Init();

	centralPos = { 400.0f,400.0f };
	leftHandData.radius = -100.0f;
	leftHandData.angle = 0.0f;
	rightHandData.pos = { 400.0f,400.0f };
	rightHandData.radius = 100.0f;
	rightHandData.angle = 0.0f;

	debugFlag = DEBUG;
	initFlag = false;
	//adjPos.y = 35.0f;
}

void BossHandMgr::Update(const Vec2<float> &POS)
{
	if (lockOnFlag)
	{
		//デバック用
		if (debugFlag)
		{
			//目標地点のベクトルの保存----------------------------------------------
			Vec2<float> vec = (targetPos - (centralPos + adjPos)) * -1.0f;
			vec.Normalize();
			float radian = atan2f(vec.y, vec.x);
			//目標地点のベクトルの保存----------------------------------------------
			float space = 10.0f;
			endLeftAngleLerp = radian - Angle::ConvertToRadian(-space);
			endRightAngleLerp = radian + Angle::ConvertToRadian(180.0f - space);

			if (!initFlag)
			{
				leftHandData.startAngle = leftHandData.angle;
				rightHandData.startAngle = rightHandData.angle;
				initFlag = true;
			}
		}
		//ゲーム用
		else
		{
			float space = 10.0f;
			endLeftAngleLerp = holdRadian - Angle::ConvertToRadian(-space);
			endRightAngleLerp = holdRadian + Angle::ConvertToRadian(180.0f - space);

			if (!initFlag)
			{
				leftHandData.startAngle = leftHandData.angle;
				rightHandData.startAngle = rightHandData.angle;
				initFlag = true;
			}
		}
	}
	//通常時
	else
	{
		//デバック用
		if (debugFlag)
		{
			leftHandData.angle = Angle::ConvertToRadian(leftAngle);
			rightHandData.angle = Angle::ConvertToRadian(rightAngle);
			endLeftAngleLerp = leftAngle;
			endRightAngleLerp = rightAngle;
		}
		//ゲーム用
		else
		{
			endLeftAngleLerp = Angle::ConvertToRadian(0.0f);
			endRightAngleLerp = Angle::ConvertToRadian(0.0f);
			leftAngle = endLeftAngleLerp;
			rightAngle = endRightAngleLerp;
			leftHandData.angle = leftAngle;
			rightHandData.angle = rightAngle;
		}
		initFlag = false;
	}


	float timeOver = 10.0f;
	if (lockOnFlag && timeOver <= timer)
	{
		rightHandData.angle = endRightAngleLerp;
		leftHandData.angle = endLeftAngleLerp;
		holdFlag = true;
	}
	else if (!lockOnFlag)
	{
		float space = 10.0f;
		timer = 0;
		//leftHandData.angle = 0.0f - Angle::ConvertToRadian(-space);
		//rightHandData.angle = 0.0f + Angle::ConvertToRadian(180.0f - space);
		holdFlag = false;
	}
	else
	{
		++timer;
		//float sub = endLeftAngleLerp - leftHandData.startAngle;
		//leftHandData.angle += sub / timeOver;
		//float sub2 = endRightAngleLerp - rightHandData.startAngle;
		//rightHandData.angle += sub2 / timeOver;

		//補間
		float mul = 0.3f;
		{
			float distance = endLeftAngleLerp - leftHandData.angle;
			leftHandData.angle += distance * mul;
		}
		{
			float distance = endRightAngleLerp - rightHandData.angle;
			rightHandData.angle += distance * mul;
		}
	}


	if (debugFlag)
	{
		leftHandData.pos = centralPos + adjPos;
		rightHandData.pos = centralPos + adjPos;
	}
	else
	{
		centralPos = POS;
		leftHandData.pos = centralPos + adjPos;
		rightHandData.pos = centralPos + adjPos;
	}

	leftHand->Update(leftHandData.pos, leftHandData.radius, leftHandData.angle, holdFlag);
	rightHand->Update(rightHandData.pos, rightHandData.radius, rightHandData.angle, holdFlag);

}

void BossHandMgr::Draw()
{
	if (debugFlag)
	{
		leftHand->Draw(false);
		rightHand->Draw(false);
	}
	else
	{
		leftHand->Draw();
		rightHand->Draw();
	}
	if (debugFlag)
	{
		DrawFunc::DrawCircle2D(centralPos, 1.0f, Color(255, 255, 255, 255));
		DrawFunc::DrawCircle2D(targetPos, 5.0f, Color(255, 0, 0, 255));
		DrawFunc::DrawLine2D(centralPos, targetPos, Color(0, 255, 0, 255));
	}
}

void BossHandMgr::Hold(const Vec2<float> &DIR, bool HOLD)
{
	if (HOLD)
	{
		holdRadian = atan2f(DIR.y, DIR.x);
		lockOnFlag = true;
	}
	else
	{
		lockOnFlag = false;
		endLeftAngleLerp = 0.0f;
		endRightAngleLerp = 0.0f;
	}
}

void BossHandMgr::ImGuiDraw()
{
	ImGui::Begin("Hand");
	ImGui::Checkbox("Hold", &holdFlag);
	ImGui::Checkbox("LockOn", &lockOnFlag);
	ImGui::SliderFloat("TargetY", &targetPos.y, 0.0f, 720.0f);
	ImGui::SliderFloat("TargetX", &targetPos.x, 0.0f, 1280.0f);
	ImGui::InputFloat("CentralPosX", &centralPos.x);
	ImGui::InputFloat("CentralPosY", &centralPos.y);
	ImGui::InputFloat("LeftRadius", &leftHandData.radius);
	ImGui::SliderFloat("LeftAngle", &leftAngle, -360.0f, 360.0f);
	ImGui::InputFloat("RightRadius", &rightHandData.radius);
	ImGui::SliderFloat("RgihtAngle", &rightAngle, -360.0f, 360.0f);
	ImGui::InputFloat("AdjPosX", &adjPos.x, -360.0f, 360.0f);
	ImGui::InputFloat("AdhPosY", &adjPos.y, -360.0f, 360.0f);
	ImGui::End();
}
