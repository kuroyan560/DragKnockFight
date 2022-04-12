#pragma once
#include"Singleton.h"
#include"Vec.h"
class StagingInterFace;
class CrashMgr : public Singleton<CrashMgr>
{
	friend class Singleton<CrashMgr>;
	CrashMgr();

	int se;
	int crashGraph;

	struct CrashDrawInfo
	{
		Vec2<float>pos;
		float radian;
		float scale;
		float alpha;
		static const int LIFE_SPAN = 90;
		int life;
	};
	static const int CRASH_MAX_NUM = 10;
	CrashDrawInfo crashInfo[CRASH_MAX_NUM];

public:
	void Crash(const Vec2<float>& Pos, StagingInterFace& CrashDevice, const Vec2<bool>& Ext);
};

