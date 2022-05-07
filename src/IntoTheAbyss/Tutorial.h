#pragma once
#include"CharacterInterFace.h"
#include<array>
#include"Vec.h"
class Tutorial
{
	bool active = true;

	struct IconGraphs
	{
		int stickHead;
		int stickBase_L;
		//int stickBase_R;
		int stickBaseSwing_R;
		int stickBasePilot_R;
		//int triggerOn_L;
		//int triggerOn_R;
		//int triggerOff_L;
		//int triggerOff_R;
		int buttonOn_L;
		int buttonOn_R;
		int buttonOff_L;
		int buttonOff_R;
	}iconGraphs;
	WHICH_TEAM team;

	//bool rightStickInput = false;
	bool pilotLeave = false;

	void DrawIcon(const Vec2<float>& Pos, const int& Handle);
	void DrawIconNonActive(const Vec2<float>& Pos, const int& Handle);
	void DrawIcon(const bool& IsActive, const Vec2<float>& Pos, const int& Handle);

public:
	Tutorial(const WHICH_TEAM& Team);
	//void Draw(const Vec2<float>& LStickVec, Vec2<float> RStickVec, const bool& LTrigger, bool RTrigger);
	void Draw(const Vec2<float>& LStickVec, Vec2<float> RStickVec, const bool& LButton, bool RButton);

	//void SetRstickInput(const bool& IsActive) { rightStickInput = IsActive; }
	void SetPilotLeave(const bool& IsLeave) { pilotLeave = IsLeave; }
	void TurnActive() { active = !active; }
};

