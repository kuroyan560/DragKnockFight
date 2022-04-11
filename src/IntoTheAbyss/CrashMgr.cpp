#include "CrashMgr.h"
#include"ShakeMgr.h"
#include"CrashInterFace.h"
void CrashMgr::Crash(const Vec2<float>& Pos, CrashInterFace& CrashDevice)
{
	//ShakeMgr::Instance()->SetShake(15);
	CrashDevice.Crash();
}
