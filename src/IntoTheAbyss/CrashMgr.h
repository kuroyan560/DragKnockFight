#pragma once
#include"Singleton.h"
#include"Vec.h"
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
	int crashGraph;

	struct CrashDrawInfo
	{
		Vec2<float>pos;
		float radian;
		float scale;
		float alpha;
		int life;
	};
	static const int CRASH_MAX_NUM = 10;
	CrashDrawInfo crashInfo[CRASH_MAX_NUM];

public:
	void Init();
	void Update();
	void Draw();
	void Crash(const Vec2<float>& Pos, StagingInterFace& CrashDevice, const Vec2<bool>& Ext, const Vec2<float>& SmokeVec);
};

