#include "RunOutOfStaminaEffect.h"
#include"TexHandleMgr.h"
#include"../Engine/DrawFunc.h"
#include"../Common/Angle.h"
#include"../Common/KuroFunc.h"
#include"ScrollMgr.h"

RunOutOfStaminaEffect::RunOutOfStaminaEffect()
{
	emptyHande = TexHandleMgr::LoadGraph("resource/ChainCombat/empty.png");
}

void RunOutOfStaminaEffect::Init()
{
	startFlag = false;
}

void RunOutOfStaminaEffect::Update()
{
	if (startFlag)
	{
		++timer;

		//è„Ç…ìoèÍÇ≥ÇπÇÈ
		lerpEmptyStringPos =
		{
			baseEmptyStringPos.x,
			baseEmptyStringPos.y + -50.0f
		};
		Vec2<float> pos = lerpEmptyStringPos - emptyStringPos;
		emptyStringPos += pos * 0.5f;


		if (40 <= timer)
		{
			//ì_ñ≈ï\åª
			if (timer % 2 == 0)
			{
				drawFlag = !drawFlag;
			}
		}
		//èIóπ
		if (60 <= timer)
		{
			startFlag = false;
			timer = 0;
		}
	}
}

void RunOutOfStaminaEffect::Draw()
{
	if (startFlag && drawFlag)
	{
		Vec2<float>drawPos = ScrollMgr::Instance()->Affect(emptyStringPos);
		DrawFunc::DrawRotaGraph2D(drawPos, Vec2<float>(1.0f, 1.0f), 0.0f, TexHandleMgr::GetTexBuffer(emptyHande));
	}
}

void RunOutOfStaminaEffect::Start(const Vec2<float> &POS)
{
	if (!startFlag)
	{
		for (int i = 0; i < 3; ++i)
		{
			float max = 180 / 3;
			//è„ÇÃï˚å¸Ç…îÚÇŒÇ∑
			float rad = Angle::ConvertToRadian(180 - (max * i));
			ParticleMgr::Instance()->Generate(POS, Vec2<float>(cosf(rad), sinf(rad)), BULLET);
		}

		lerpEmptyStringPos = POS;
		emptyStringPos = POS;
		timer = 0;
		drawFlag = true;
		startFlag = true;
	}

	baseEmptyStringPos = POS;
}

bool RunOutOfStaminaEffect::isDraw()
{
	return startFlag;
}
