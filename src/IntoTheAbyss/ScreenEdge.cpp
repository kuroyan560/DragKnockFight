#include "ScreenEdge.h"
#include"../Engine/DrawFunc.h"
#include"TexHandleMgr.h"
#include"../Common/Angle.h"
#include"../Common/KuroMath.h"

ScreenEdge::ScreenEdge()
{
	enemyFlamehandle = TexHandleMgr::LoadGraph("resource/ChainCombat/frame_line_enemy.png");
	playerFlamehandle = TexHandleMgr::LoadGraph("resource/ChainCombat/frame_line_player.png");
	playerChancehandle = TexHandleMgr::LoadGraph("resource/ChainCombat/frame_chance_player.png");
	enemyChancehandle = TexHandleMgr::LoadGraph("resource/ChainCombat/frame_chance_enemy.png");
}

void ScreenEdge::Init(const float &DISTANCE, const Vec2<float> INIT_POS, const float ANGLE)
{
	maxDistance = DISTANCE;
	basePos = INIT_POS;
	angle = ANGLE;

	if (angle == 90.0f)
	{
		vecType = X_VEC;
	}
	else
	{
		vecType = Y_VEC;
	}
}

void ScreenEdge::StartWinEffect()
{
}

void ScreenEdge::StartFinishEffect()
{
}

void ScreenEdge::CaluPos(const float &RATE)
{

	//ÉvÉåÉCÉÑÅ[êwín
	if (RATE < 0.5f)
	{
		nowFlameHandle = playerFlamehandle;
		nowChanceHandle = playerChancehandle;

		rate = 1.0f - (RATE / 0.5f);
	}
	//ìGêwín
	else
	{
		nowFlameHandle = enemyFlamehandle;
		nowChanceHandle = enemyChancehandle;

		rate = ((RATE - 0.5f) / 0.5f);
	}

	bool minusFlag = false;
	switch (vecType)
	{
	case ScreenEdge::X_VEC:
		minusFlag = signbit(maxDistance - basePos.x);
		if (minusFlag)
		{
			distance.x = -maxDistance * rate;
		}
		else
		{
			distance.x = maxDistance * rate;
		}
		break;

	case ScreenEdge::Y_VEC:
		minusFlag = signbit(maxDistance - basePos.y);
		if (minusFlag)
		{
			distance.y = -maxDistance * rate;
		}
		else
		{
			distance.y = maxDistance * rate;
		}
		break;
	default:
		break;
	}

}

void ScreenEdge::Update()
{
	//êiçsï˚å¸Ç…óhÇÁÇ∑èàóù-----------------------
	if (1.0f <= shakeRate)
	{
		reversFlag = true;
	}
	if (shakeRate <= 0.0f)
	{
		reversFlag = false;
	}

	float timer = 30.0f + 30.0f * (1.0 - rate);
	if (reversFlag)
	{
		MRate(&shakeRate, timer);
	}
	else
	{
		Rate(&shakeRate, timer);
	}
	switch (vecType)
	{
	case ScreenEdge::X_VEC:
		shakeValue.x = KuroMath::Lerp(0.0f, 10.0f, shakeRate);
		break;
	case ScreenEdge::Y_VEC:
		shakeValue.y = KuroMath::Lerp(0.0f, 10.0f, shakeRate);
		break;
	default:
		break;
	}
	//êiçsï˚å¸Ç…óhÇÁÇ∑èàóù-----------------------

	//ç¿ïWÇÃèâä˙âª
	pos = basePos + distance + shakeValue;

}

void ScreenEdge::Draw()
{
	DrawFunc::DrawRotaGraph2D(pos, Vec2<float>(1.0f, 1.0f), Angle::ConvertToRadian(angle), TexHandleMgr::GetTexBuffer(nowFlameHandle));
	DrawFunc::DrawRotaGraph2D(pos, Vec2<float>(1.0f, 1.0f), Angle::ConvertToRadian(angle), TexHandleMgr::GetTexBuffer(nowChanceHandle));
}
