#include "RunOutOfStaminaEffect.h"
#include"TexHandleMgr.h"
#include"../Engine/DrawFunc.h"
#include"../Common/Angle.h"
#include"../Common/KuroFunc.h"
#include"ScrollMgr.h"

RunOutOfStaminaEffect::RunOutOfStaminaEffect()
{
	emptyHande = TexHandleMgr::LoadGraph("resource/ChainCombat/empty.png");
	lerpPlayerSize = { 1.0f,1.0f };
	playerSize = { 1.0f,1.0f };
	startFlag = false;
	drawFlag = false;
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

		//上に登場させる
		lerpEmptyStringPos =
		{
			baseEmptyStringPos.x,
			baseEmptyStringPos.y + -50.0f
		};
		emptyStringPos.x = baseEmptyStringPos.x;
		Vec2<float> pos = lerpEmptyStringPos - emptyStringPos;
		emptyStringPos.y += pos.y * 0.3f;



	
		if (0.5f < playerSize.x && !shrinkFlag)
		{
			lerpPlayerSize.x = 0.5f;
			shrinkFlag = true;
		}
		else if (playerSize.x <= 0.5f && !extendFlag)
		{
			extendFlag = true;
		}
		else if (extendFlag)
		{
			lerpPlayerSize.x = 1.0f;
		}

		Vec2<float> size = lerpPlayerSize - playerSize;
		float rate = 0.5f;
		playerSize += size * rate;

		//少量時間より少し前に点滅を開始する
		if (finishTimer - 40 <= timer)
		{
			//点滅表現
			if (timer % 2 == 0)
			{
				drawFlag = !drawFlag;
			}
		}
		//終了
		if (finishTimer < timer)
		{
			startFlag = false;
			playerSize = { 1.0f,1.0f };
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

void RunOutOfStaminaEffect::Start(const Vec2<float> &POS, int MAX_TIMER)
{
	if (!startFlag)
	{
		for (int i = 0; i < 3; ++i)
		{
			float max = 180 / 3;
			//上の方向に飛ばす
			float rad = Angle::ConvertToRadian(180 - (max * i));
			ParticleMgr::Instance()->Generate(POS, Vec2<float>(cosf(rad), sinf(rad)), BULLET);
		}
		lerpPlayerSize = { 1.0f,1.0f };
		playerSize = { 1.0f,1.0f };

		lerpEmptyStringPos = POS;
		emptyStringPos = POS;
		timer = 0;
		finishTimer = MAX_TIMER;
		drawFlag = true;
		startFlag = true;
		shrinkFlag = false;
		extendFlag = false;
	}

	baseEmptyStringPos = POS;
}

bool RunOutOfStaminaEffect::isDraw()
{
	return startFlag;
}
