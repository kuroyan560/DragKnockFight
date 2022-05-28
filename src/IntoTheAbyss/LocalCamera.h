#pragma once
#include"Vec.h"
#include"ScrollMgr.h"

/// <summary>
/// ï°êªóp
/// </summary>
class LocalCamera
{
public:
	float initZoom;
	LocalCamera()
	{
		initZoom = 0.2f;
	}
	// If camera focuses on something, it will be active.
	int active;
	float lerpAmount;
	// Info to focus.
	Vec2<float>target;
	float zoom;

	// These parameters affect scroll and scale on "ScrollMgr".
	Vec2<float>scrollAffect;
	//float zoomAffect;

	void Init();
	void Update();
	// This "TargetPos" must not be affected by scroll.
	void Focus(const Vec2<float> &TargetPos, const float &Zoom, const float &LerpAmount = 0.1f);
	void Release() { active = 0; zoom = initZoom; }

	void Zoom(const Vec2<float> &PLAYER_POS, const Vec2<float> &BOSS_POS, float MIN_ZOOM_SIZE = 0.2f);

	const int &Active() { return active; }
};