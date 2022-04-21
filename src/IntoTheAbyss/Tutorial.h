#pragma once
#include"CharacterInterFace.h"
#include<array>
#include"Vec.h"
class Tutorial
{
	bool active = true;

	struct IconGraphs
	{
		int stickBase_L;
		int stickBase_R;
		int triggerOn_L;
		int triggerOn_R;
		int triggerOff_L;
		int triggerOff_R;
	}iconGraphs;
	WHICH_TEAM team;

	bool rightStickInput = false;
	bool rightTriggerInput = false;

	void DrawIcon(const Vec2<float>& Pos, const int& Handle);

public:
	Tutorial(const WHICH_TEAM& Team);
	void Draw(const Vec2<float>& LStickVec, const Vec2<float>& RStickVec, const bool& LTrigger, const bool& RTrigger);

	void SetRstickInput(const bool& IsActive) { rightStickInput = IsActive; }
	void SetRtriggerInput(const bool& IsActive) { rightTriggerInput = IsActive; }
	void TurnActive() { active = !active; }
};

