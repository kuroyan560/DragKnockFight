#include "CrashMgr.h"
#include"ShakeMgr.h"
#include"StagingInterFace.h"
#include"AudioApp.h"
#include"TexHandleMgr.h"

CrashMgr::CrashMgr()
{
	se = AudioApp::Instance()->LoadAudio("resource/ChainCombat/sound/crash.wav", 0.27f);

	TexHandleMgr::LoadDivGraph("resource/ChainCombat/UI/crash.png", 3, { 3,1 }, crashGraph.data());
}

void CrashMgr::Init()
{
	for (auto& info : crashInfo)
	{
		info.life = CRASH_LIFE_SPAN + 1;
	}
}

#include"KuroMath.h"
void CrashMgr::Update()
{
	for (auto& info : crashInfo)
	{
		//Ž€–S‚µ‚Ä‚¢‚é
		if (CRASH_LIFE_SPAN < info.life)continue;

		if (info.life == 0)Sleep(30);

		int timer = info.life;
		int totalTime = CRASH_APPEAR_TIME;

		if (info.life < CRASH_APPEAR_TIME)
		{
			info.radian = KuroMath::Ease(Out, Exp, timer, totalTime, 0.0f, Angle::ConvertToRadian(360 * 2));
			info.scale = KuroMath::Ease(Out, Elastic, timer, totalTime, 0.0f, 1.0f);
		}
		else if (info.life < CRASH_APPEAR_TIME + CRASH_WAIT_TIME)
		{
			//timer = info.life - CRASH_APPEAR_TIME;
			//totalTime = CRASH_APPEAR_TIME;
		}
		else if (info.life < CRASH_LIFE_SPAN)
		{
			timer = info.life - CRASH_APPEAR_TIME - CRASH_WAIT_TIME;
			totalTime = CRASH_DISAPPEAR_TIME;

			info.alpha = KuroMath::Ease(In, Circ, timer, totalTime, 1.0f, 0.0f);
			info.scale = KuroMath::Ease(In, Back, timer, totalTime, 1.0f, 0.0f);
		}

		info.scaleOffsetRadian += Angle::ConvertToRadian(20.0f);

		info.life++;
	}
}

#include"DrawFunc.h"
#include"ScrollMgr.h"
void CrashMgr::Draw()
{
	for (auto& info : crashInfo)
	{
		//Ž€–S‚µ‚Ä‚¢‚é
		if (CRASH_LIFE_SPAN < info.life)continue;

		auto drawPos = ScrollMgr::Instance()->Affect(info.pos);
		float drawScale = info.scale * ScrollMgr::Instance()->zoom + cos(info.scaleOffsetRadian) * 0.05f;
		DrawFunc::DrawRotaGraph2D(drawPos, { drawScale,drawScale }, info.radian, TexHandleMgr::GetTexBuffer(crashGraph[KuroFunc::GetRand(2)]));
	}
}

#include"ParticleMgr.h"
void CrashMgr::Crash(const Vec2<float>& Pos, StagingInterFace& CrashDevice, const Vec2<bool>& Ext, const Vec2<float>& SmokeVec, const int& SmokeCol)
{
	PARTICLE_TYPE particleType = CRASH_W;
	if (SmokeCol == 1)particleType = CRASH_G;
	if (SmokeCol == 2)particleType = CRASH_R;
	ParticleMgr::Instance()->Generate(Pos, SmokeVec, particleType);

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

	for (auto& info : crashInfo)
	{
		if (CRASH_LIFE_SPAN < info.life)
		{
			info.life = 0;
			info.pos = Pos;
			info.radian = 0.0f;
			info.scale = 0.0f;
			info.alpha = 1.0f;
			info.scaleOffsetRadian = 0.0f;
			break;
		}
	}
}
