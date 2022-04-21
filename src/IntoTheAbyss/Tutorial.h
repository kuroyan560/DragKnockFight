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
		int stickBase_R;
		int triggerOn_L;
		int triggerOn_R;
		int triggerOff_L;
		int triggerOff_R;
	}iconGraphs;
	WHICH_TEAM team;

	bool rightStickInput = false;

	void DrawIcon(const Vec2<float>& Pos, const int& Handle);
	void DrawIconNonActive(const Vec2<float>& Pos, const int& Handle);
	void DrawIcon(const bool& IsActive, const Vec2<float>& Pos, const int& Handle);

public:
	Tutorial(const WHICH_TEAM& Team);
	void Draw(const Vec2<float>& LStickVec, Vec2<float> RStickVec, const bool& LTrigger, bool RTrigger);

	void SetRstickInput(const bool& IsActive) { rightStickInput = IsActive; }
	void TurnActive() { active = !active; }
};

