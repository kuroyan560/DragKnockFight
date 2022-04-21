#include "RunOutOfStaminaEffect.h"
#include"TexHandleMgr.h"
#include"../Engine/DrawFunc.h"
#include"../Common/Angle.h"
#include"../Common/KuroFunc.h"
#include"ScrollMgr.h"

RunOutOfStaminaEffect::RunOutOfStaminaEffect()
{
	emptyHande = TexHandleMgr::LoadGraph("resource/ChainCombat/empty.png");
	maxHande = TexHandleMgr::LoadGraph("resource/ChainCombat/empty.png");
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

		if(drawFlag)
		{
			//空っぽのプレイヤー演出-----------------------
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
			//空っぽのプレイヤー演出-----------------------
		}


		if(drawMaxFlag)
		{
			//満タンのプレイヤー演出-----------------------
			lerpMaxStringPos =
			{
				baseMaxStringPos.x,
				baseMaxStringPos.y + -50.0f
			};
			maxStringPos.x = baseMaxStringPos.x;
			Vec2<float> pos = lerpMaxStringPos - maxStringPos;
			maxStringPos.y += pos.y * 0.3f;


			if (0.5f < playerSize.x && !extendMaxFlag)
			{
				lerpPlayerSize.x = 1.5f;
				extendMaxFlag = true;
			}
			else if (1.5f <= playerSize.x && extendMaxFlag && !shrinkMaxFlag)
			{
				shrinkMaxFlag = true;
			}
			else if (shrinkMaxFlag && 1.0f < lerpPlayerSize.x)
			{
				lerpPlayerSize.x = 1.0f;
			}
			else if (playerSize.x <= 1.0f)
			{
				startFlag = false;
				playerSize = { 1.0f,1.0f };
				timer = 0;
			}
			//満タンのプレイヤー演出-----------------------
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
		//満タン演出
		if (finishTimer < timer)
		{
			drawFlag = false;
			drawMaxFlag = true;
		}


	}
}

void RunOutOfStaminaEffect::Draw()
{
	//空っぽ表示
	if (startFlag && drawFlag)
	{
		Vec2<float>drawPos = ScrollMgr::Instance()->Affect(emptyStringPos);
		DrawFunc::DrawRotaGraph2D(drawPos, Vec2<float>(1.0f, 1.0f), 0.0f, TexHandleMgr::GetTexBuffer(emptyHande));
	}
	//満タン表示
	else if (startFlag && drawMaxFlag)
	{
		Vec2<float>drawPos = ScrollMgr::Instance()->Affect(maxStringPos);
		DrawFunc::DrawRotaGraph2D(drawPos, Vec2<float>(1.0f, 1.0f), 0.0f, TexHandleMgr::GetTexBuffer(maxHande));
	}
}

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
		drawFlag = true;
		startFlag = true;
		shrinkFlag = false;
		extendFlag = false;

		drawMaxFlag = false;
		extendMaxFlag = false;
		shrinkMaxFlag = false;
	}

	baseEmptyStringPos = POS;
	baseMaxStringPos = POS;
}

bool RunOutOfStaminaEffect::isDraw()
{
	return startFlag;
}
