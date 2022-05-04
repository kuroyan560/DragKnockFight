#include "RunOutOfStaminaEffect.h"
#include"TexHandleMgr.h"
#include"../Engine/DrawFunc.h"
#include"../Common/Angle.h"
#include"../Common/KuroFunc.h"
#include"ScrollMgr.h"
#include"../Common/KuroMath.h"

RunOutOfStaminaEffect::RunOutOfStaminaEffect()
{
	emptyHande = TexHandleMgr::LoadGraph("resource/ChainCombat/empty.png");
	maxHande = TexHandleMgr::LoadGraph("resource/ChainCombat/full.png");
	lerpPlayerSize = { 1.0f,1.0f };
	playerSize = { 1.0f,1.0f };
}

void RunOutOfStaminaEffect::Init()
{
	drawEmptyFlag = false;
	drawMaxFlag = false;
}

void RunOutOfStaminaEffect::Update()
{
	if (!drawEmptyFlag && !drawMaxFlag)return;

	++timer;

	if (drawEmptyFlag)
	{

		//満タン演出
		if (finishTimer <= timer)
		{
			drawEmptyFlag = false;
			drawMaxFlag = true;
		}


		//空っぽのプレイヤー演出-----------------------
		//上に登場させる
		if (emptyRate <= 1.0f)
		{
			emptyRate += 0.1f;
		}
		if (1.0f <= emptyRate)
		{
			emptyRate = 1.0f;
		}


		lerpEmptyStringPos =
		{
			baseEmptyStringPos.x,
			baseEmptyStringPos.y + -55.0f * KuroMath::Ease(Out, Back, emptyRate, 0.6f, 1.0f)
		};
		emptyStringPos = lerpEmptyStringPos;


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
		//空っぽのプレイヤー演出-----------------------

		rate = 0.5f;
	}


	if (drawMaxFlag)
	{
		//満タンのプレイヤー演出-----------------------
		if (fullRate <= 1.0f)
		{
			fullRate += 0.1f;
		}
		if (1.0f <= fullRate)
		{
			fullRate = 1.0f;
		}
		lerpMaxStringPos =
		{
			baseMaxStringPos.x,
			baseMaxStringPos.y + -55.0f * KuroMath::Ease(Out, Back, fullRate, 0.6f, 1.0f)
		};
		maxStringPos = lerpMaxStringPos;



		if (0.5f < playerSize.x && !extendMaxFlag)
		{
			lerpPlayerSize.x = 1.5f;
			lerpPlayerSize.y = 0.7f;
			extendMaxFlag = true;
		}
		else if (1.5f <= playerSize.x && extendMaxFlag && !shrinkMaxFlag)
		{
			shrinkMaxFlag = true;
		}
		else if (shrinkMaxFlag && 1.0f < lerpPlayerSize.x)
		{
			lerpPlayerSize.x = 1.0f;
			lerpPlayerSize.y = 1.0f;
		}
		else if (playerSize.x <= 1.0f)
		{
			drawMaxFlag = false;
			playerSize = { 1.0f,1.0f };
			timer = 0;
		}
		//満タンのプレイヤー演出-----------------------

		rate = 0.7f;
	}


	Vec2<float> size = lerpPlayerSize - playerSize;
	playerSize += size * rate;

}

void RunOutOfStaminaEffect::Draw()
{
	//空っぽ表示
	if (drawEmptyFlag)
	{
		Vec2<float>drawPos = ScrollMgr::Instance()->Affect(emptyStringPos);
		DrawFunc::DrawRotaGraph2D(drawPos, Vec2<float>(1.0f, 1.0f), 0.0f, TexHandleMgr::GetTexBuffer(emptyHande));
	}
	//満タン表示
	else if (drawMaxFlag)
	{
		Vec2<float>drawPos = ScrollMgr::Instance()->Affect(maxStringPos);
		DrawFunc::DrawRotaGraph2D(drawPos, Vec2<float>(1.0f, 1.0f), 0.0f, TexHandleMgr::GetTexBuffer(maxHande));
	}
}

void RunOutOfStaminaEffect::Empty()
{
	//上の方向に飛ばす
	float rad = Angle::ConvertToRadian(90);
	ParticleMgr::Instance()->Generate(baseEmptyStringPos, Vec2<float>(cosf(rad), sinf(rad)), BULLET);
	lerpPlayerSize = { 1.0f,1.0f };
	playerSize = { 1.0f,1.0f };

	lerpEmptyStringPos = baseEmptyStringPos;
	emptyStringPos = baseEmptyStringPos;

	timer = 0;
	static const int EMPTY_DRAW_TIME = 90;
	finishTimer = EMPTY_DRAW_TIME;
	drawEmptyFlag = true;
	shrinkFlag = false;
	extendFlag = false;

	drawMaxFlag = false;

	fullRate = 0.0f;
	emptyRate = 0.0f;
}

void RunOutOfStaminaEffect::Full()
{
	//上の方向に飛ばす
	lerpPlayerSize = { 1.0f,1.0f };
	playerSize = { 1.0f,1.0f };

	lerpMaxStringPos = baseMaxStringPos;
	maxStringPos = baseMaxStringPos;


	timer = 0;
	static const int FULL_DRAW_TIME = 90;
	finishTimer = FULL_DRAW_TIME;
	drawEmptyFlag = false;
	drawMaxFlag = true;
	extendMaxFlag = false;
	shrinkMaxFlag = false;

	fullRate = 0.0f;
	emptyRate = 0.0f;
}

/*
void RunOutOfStaminaEffect::Start(const Vec2<float> &POS, int MAX_TIMER)
{
	if (!startFlag)
	{
		//上の方向に飛ばす
		float rad = Angle::ConvertToRadian(90);
		ParticleMgr::Instance()->Generate(POS, Vec2<float>(cosf(rad), sinf(rad)), BULLET);
		lerpPlayerSize = { 1.0f,1.0f };
		playerSize = { 1.0f,1.0f };

		lerpEmptyStringPos = POS;
		emptyStringPos = POS;

		lerpMaxStringPos = POS;
		maxStringPos = POS;


		timer = 0;
		finishTimer = MAX_TIMER;
		drawEmptyFlag = true;
		startFlag = true;
		shrinkFlag = false;
		extendFlag = false;

		drawMaxFlag = false;
		extendMaxFlag = false;
		shrinkMaxFlag = false;

		fullRate = 0.0f;
		emptyRate = 0.0f;
	}

	//baseEmptyStringPos = POS;
	//baseMaxStringPos = POS;
}
*/

