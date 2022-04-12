#include "CrashMgr.h"
#include"ShakeMgr.h"
#include"StagingInterFace.h"
#include"AudioApp.h"
#include"TexHandleMgr.h"

CrashMgr::CrashMgr()
{
	se = AudioApp::Instance()->LoadAudio("resource/ChainCombat/sound/crash.wav");
	AudioApp::Instance()->ChangeVolume(se, 0.27f);

	crashGraph = TexHandleMgr::LoadGraph("resource/ChainCombat/UI/crash.png");
}

void CrashMgr::Crash(const Vec2<float>& Pos, StagingInterFace& CrashDevice, const Vec2<bool>& Ext)
{
	static const int CRASH_SPAN = 30;

	static const int SHAKE_SPAN = 1;
	static const int SHAKE_FULL_POWER = 15;
	CrashDevice.Shake(CRASH_SPAN, SHAKE_SPAN, SHAKE_FULL_POWER);

	static const float EXT_RATE_AMOUNT = 0.2f;
	CrashDevice.Extend(CRASH_SPAN, Ext, EXT_RATE_AMOUNT);

	static const float FLASH_RATE = 0.7f;
	CrashDevice.Flash(CRASH_SPAN, FLASH_RATE);

	ShakeMgr::Instance()->SetShake(15);
	AudioApp::Instance()->PlayWave(se);
}
