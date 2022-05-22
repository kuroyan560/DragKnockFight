#pragma once
#include"Singleton.h"
#include"Vec.h"
#include<array>
class StagingInterFace;
class CrashMgr : public Singleton<CrashMgr>
{
	friend class Singleton<CrashMgr>;
	CrashMgr();

	static const int CRASH_APPEAR_TIME = 30;
	static const int CRASH_WAIT_TIME = 30;
	static const int CRASH_DISAPPEAR_TIME = 30;
	static const int CRASH_LIFE_SPAN = CRASH_APPEAR_TIME + CRASH_WAIT_TIME + CRASH_DISAPPEAR_TIME;
	int se;
	std::array<int, 3>crashGraph;

	struct CrashDrawInfo
	{
		Vec2<float>pos;
		float radian;
		float scale;
		float alpha;
		int life;
		float scaleOffsetRadian;
	};
	static const int CRASH_MAX_NUM = 10;
	CrashDrawInfo crashInfo[CRASH_MAX_NUM];

	static const int HIT_STOP = 30;
	int hitStopTimer;

public:
	void Init();
	void Update();
	void Draw();
	void Crash(const Vec2<float>& Pos, StagingInterFace& CrashDevice, const Vec2<bool>& Ext, const Vec2<float>& SmokeVec, const int& SmokeCol);
};

