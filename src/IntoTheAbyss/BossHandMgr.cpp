#include "BossHandMgr.h"
#include"TexHandleMgr.h"
#include"../Engine/ImguiApp.h"
#include"../Engine/DrawFunc.h"
#include"../Common/KuroMath.h"

BossHandMgr::BossHandMgr(int L_DEFAULT_HANDLE, int R_DEFAULT_HANDLE, int L_HOLD_HANDLE, int R_HOLD_HANDLE)
{
	leftHand = std::make_unique<BossHand>(L_DEFAULT_HANDLE, L_HOLD_HANDLE);
	rightHand = std::make_unique<BossHand>(R_DEFAULT_HANDLE, R_HOLD_HANDLE);
}

void BossHandMgr::Init(bool DEBUG)
{
	leftHand->Init();
	rightHand->Init();

	normalLeftRadian = Angle::ConvertToRadian(-40.0f);
	normalRightRadian = Angle::ConvertToRadian(40.0f);

	centralPos = { 400.0f,400.0f };
	leftHandData.radius = -500.0f;
	leftHandData.angle = normalLeftRadian;
	rightHandData.pos = { 400.0f,400.0f };
	rightHandData.radius = 500.0f;
	rightHandData.angle = normalRightRadian;

	debugFlag = DEBUG;
	initFlag = false;
	initStartFlag = false;
	startEffectFlag = false;
	initStartEffectFlag = false;
	holdFlag = false;
}

void BossHandMgr::InitRadius()
{
	leftHandData.radius = -500.0f;
	rightHandData.radius = 500.0f;
}

void BossHandMgr::Update(const Vec2<float> &POS)
{

	Vec2<float> vec = { 0.0f,0.0f };

	bool startFlag = StartEffect();
	if (startFlag && !initStartEffectFlag)
	{
		startEffectFlag = true;
		initStartEffectFlag = true;
	}



	if (startEffectFlag)
	{
		//掴み時
		if (lockOnFlag)
		{
			//デバック用
			if (debugFlag)
			{
				//目標地点のベクトルの保存----------------------------------------------
				vec = (targetPos - centralPos) * -1.0f;
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
				leftHandData.angle = normalLeftRadian;
				rightHandData.angle = normalRightRadian;
				endLeftAngleLerp = leftAngle;
				endRightAngleLerp = rightAngle;
			}
			//ゲーム用
			else
			{
				endLeftAngleLerp = normalLeftRadian;
				endRightAngleLerp = normalRightRadian;
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
			holdFlag = false;
		}
		else
		{
			++timer;

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
	}

	if (debugFlag)
	{
		leftHandData.pos = centralPos;
		rightHandData.pos = centralPos;
	}
	else
	{
		centralPos = POS;
		leftHandData.pos = centralPos;
		rightHandData.pos = centralPos;
	}

	if (debugFlag)
	{
		leftHand->Update(leftHandData.pos, leftHandData.radius, leftHandData.angle, holdFlag, vec);
		rightHand->Update(rightHandData.pos, rightHandData.radius, rightHandData.angle, holdFlag, vec);
	}
	else
	{
		leftHand->Update(leftHandData.pos, leftHandData.radius, leftHandData.angle, holdFlag, holdDir);
		rightHand->Update(rightHandData.pos, rightHandData.radius, rightHandData.angle, holdFlag, holdDir);
	}
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
	holdDir = DIR;
	if (HOLD)
	{
		holdRadian = atan2f(DIR.y, DIR.x);
		lockOnFlag = true;
	}
	else if (!startEffectFlag)
	{
		lockOnFlag = false;
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
	ImGui::End();
}

bool BossHandMgr::StartEffect()
{


	//回す----------------------------------------------
	float speed = 0.3f;
	endLeftAngleLerp += speed;
	endRightAngleLerp += speed;

	float minRadius = 10.0f;
	//一定の大きさ&&特定のアングルになったら終了
	bool sameRadiusFlag = leftHandData.radius == -minRadius && rightHandData.radius == minRadius;

	if (!initStartFlag && sameRadiusFlag)
	{
		initStartFlag = true;
	}

	if (initStartFlag)
	{
		endLeftAngleLerp = normalLeftRadian;
		endRightAngleLerp = normalRightRadian;

		float radiusSpeed = 3.0f;
		if (leftHandData.radius <= -100)
		{
			leftHandData.radius = -100;
		}
		else
		{
			leftHandData.radius -= radiusSpeed;
		}
		if (100 <= rightHandData.radius)
		{
			rightHandData.radius = 100.0f;
		}
		else
		{
			rightHandData.radius += radiusSpeed;
		}


		bool startRadiusFlag = leftHandData.radius == -100 && rightHandData.radius == 100;
		bool sameRadianFlag = endLeftAngleLerp == normalLeftRadian &&
			endRightAngleLerp == normalRightRadian;
		if (startRadiusFlag && sameRadianFlag)
		{
			return true;
		}
	}
	else
	{
		//半径を小さくする----------------------------------------------
		float smallSpeed = 8.0f;
		if (-minRadius <= leftHandData.radius)
		{
			leftHandData.radius = -minRadius;
		}
		else
		{
			leftHandData.radius += smallSpeed;
		}
		if (rightHandData.radius <= minRadius)
		{
			rightHandData.radius = minRadius;
		}
		else
		{
			rightHandData.radius -= smallSpeed;
		}
	}


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

	return false;
}
