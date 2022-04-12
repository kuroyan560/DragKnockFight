#pragma once
#include"Vec.h"
//クラッシュ時の演出に関する機能
class CrashInterFace
{
	static const int CRASH_SPAN = 30;
	static const int SHAKE_SPAN = 1;
	static const int SHAKE_FULL_POWER = 15;
	Vec2<float>shake;
	Vec2<int>sign;
	float flashAlpha;
	Vec2<float>extRate;
	Vec2<float>addExtRate;
	int timer;
public:
	void Init();
	void Update();

	const Vec2<float>GetShake() { return shake; }
	const float& GetFlashAlpha() { return flashAlpha; }
	const Vec2<float>& GetExtRate() { return extRate; }

	void Crash(const Vec2<bool>& Ext);
};

