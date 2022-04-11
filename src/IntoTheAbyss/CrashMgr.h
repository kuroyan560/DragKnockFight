#pragma once
#include"Singleton.h"
#include"Vec.h"
class CrashInterFace;
class CrashMgr : public Singleton<CrashMgr>
{
	friend class Singleton<CrashMgr>;
	CrashMgr() {}
public:
	void Crash(const Vec2<float>& Pos, CrashInterFace& CrashDevice);
};

