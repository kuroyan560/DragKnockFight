#pragma once
#include"Vec.h"
//�N���b�V�����̉��o�Ɋւ���@�\
class CrashInterFace
{
	static const int CRASH_SPAN = 30;
	static const int SHAKE_SPAN = 1;
	static const int SHAKE_FULL_POWER = 15;
	Vec2<float>shake;
	Vec2<int>sign;
	float flashAlpha;
	int timer;
public:
	void Init();
	void Update();

	const Vec2<float>GetShake() { return shake; }
	const float& GetFlashAlpha() { return flashAlpha; }

	void Crash();
};

