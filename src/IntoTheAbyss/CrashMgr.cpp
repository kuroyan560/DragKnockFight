#include "CrashMgr.h"
#include"ShakeMgr.h"
#include"CrashInterFace.h"
#include"AudioApp.h"

CrashMgr::CrashMgr()
{
	se = AudioApp::Instance()->LoadAudio("resource/ChainCombat/sound/crash.wav");
	AudioApp::Instance()->ChangeVolume(se, 0.27f);
}

void CrashMgr::Crash(const Vec2<float>& Pos, CrashInterFace& CrashDevice)
{
	ShakeMgr::Instance()->SetShake(15);
	CrashDevice.Crash();
	AudioApp::Instance()->PlayWave(se);
}
