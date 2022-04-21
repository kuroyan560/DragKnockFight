#include "ScreenEdge.h"
#include"TexHandleMgr.h"
#include"../Common/Angle.h"
#include"../Common/KuroMath.h"
#include"../Engine/WinApp.h"

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
	lerpPos = basePos;
	pos = basePos;

	scrollChancePos[0] = {};
	if (angle == 90.0f)
	{
		vecType = X_VEC;
		speed = { 0.1f,0.0f };
		scrollChancePos[1] = { 0.0f,-1280.0f };
		adjTexPos.y = 640.0f;
	}
	else
	{
		vecType = Y_VEC;
		speed = { 0.1f,0.0f };
		scrollChancePos[1] = { -1280.0f,0.0f };
		adjTexPos.x = 640.0f;
		adjYPos.x = -14.0f;
	}
	scrollChancePos[1] = {};

	getAwayFlag = false;
	winFlag = false;


	initWinFlag = false;
	initGetAwayFlag = false;
	initTimer = 0;


	switch (vecType)
	{
	case ScreenEdge::X_VEC:
		if (lerpPos.x <= WinApp::Instance()->GetWinSize().x / 2.0f)
		{
			reversFlag = false;
			shakeRate = 0.0f;
		}
		//右側に居たら左側に消す
		else
		{
			reversFlag = true;
			shakeRate = 0.0f;
		}
		break;

	case ScreenEdge::Y_VEC:
		//左側にいたら右側に消す
		if (lerpPos.y <= WinApp::Instance()->GetWinSize().y / 2.0f)
		{
			reversFlag = false;
			shakeRate = 0.0f;
		}
		//右側に居たら左側に消す
		else
		{
			reversFlag = true;
			shakeRate = 0.0f;
		}
		break;
	default:
		break;
	}


	fadeOutMaxTimer = 120;

	sinCurve = 0.0f;
	chanceAlpha = 0.0f;

	adjPos.x = 16.0f;
	adjPos.y = -16.0f;
}

void ScreenEdge::StartWinEffect(int FADE_OUT_TIMER)
{
	winFlag = true;
	fadeOutMaxTimer = FADE_OUT_TIMER;
}

void ScreenEdge::StartFinishEffect()
{
	getAwayFlag = true;
}

void ScreenEdge::CaluPos(const float &RATE)
{

	//プレイヤー陣地
	if (RATE < 0.5f)
	{
		nowFlameHandle = playerFlamehandle;
		nowChanceHandle = playerChancehandle;

		rate = 1.0f - (RATE / 0.5f);
	}
	//敵陣地
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
	if (!winFlag && !getAwayFlag)
	{
		//進行方向に揺らす処理-----------------------
		if (1.0f <= shakeRate)
		{
			reversFlag = true;
		}
		if (shakeRate <= 0.0f)
		{
			reversFlag = false;
		}

		float timer = 2.0f + 20.0f * (1.0 - rate);
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
			shakeValue.x = KuroMath::Liner(0.0f, distance.x , shakeRate);
			break;
		case ScreenEdge::Y_VEC:
			shakeValue.y = KuroMath::Liner(0.0f, distance.y, shakeRate);
			break;
		default:
			break;
		}
		//進行方向に揺らす処理-----------------------

		//座標の初期化
		lerpPos = basePos + shakeValue;
	}
	//捌ける演出
	else if (getAwayFlag && !initGetAwayFlag)
	{
		switch (vecType)
		{
		case ScreenEdge::X_VEC:
			//左側にいたら右側に消す
			if (lerpPos.x <= WinApp::Instance()->GetWinSize().x / 2.0f)
			{
				lerpPos.x = WinApp::Instance()->GetWinSize().x + 20.0f;
			}
			//右側に居たら左側に消す
			else
			{
				lerpPos.x = -20.0f;
			}
			break;

		case ScreenEdge::Y_VEC:
			//左側にいたら右側に消す
			if (lerpPos.y <= WinApp::Instance()->GetWinSize().y / 2.0f)
			{
				lerpPos.y = WinApp::Instance()->GetWinSize().y + 20.0f;
			}
			//右側に居たら左側に消す
			else
			{
				lerpPos.y = -20.0f;
			}
			break;
		default:
			break;
		}

		initGetAwayFlag = true;
	}
	//勝ち演出
	else if (winFlag && !initWinFlag)
	{
		switch (vecType)
		{
		case ScreenEdge::X_VEC:
			//左側にいたら右側に動かす
			if (lerpPos.x <= WinApp::Instance()->GetWinSize().x / 2.0f)
			{
				lerpPos.x = WinApp::Instance()->GetWinSize().x - maxDistance;
			}
			//右側に居たら左側に動かす
			else
			{
				lerpPos.x = 0.0f + maxDistance;
			}
			break;

		case ScreenEdge::Y_VEC:
			//左側にいたら右側に動かす
			if (lerpPos.y <= WinApp::Instance()->GetWinSize().y / 2.0f)
			{
				lerpPos.y = WinApp::Instance()->GetWinSize().y - maxDistance;
			}
			//右側に居たら左側に消す
			else
			{
				lerpPos.y = 0.0f + maxDistance;
			}
			break;
		default:
			break;
		}
		initWinFlag = true;
	}

	if (initWinFlag || initGetAwayFlag)
	{
		++initTimer;
	}
	if (fadeOutMaxTimer <= initTimer && initWinFlag)
	{
		switch (vecType)
		{
		case ScreenEdge::X_VEC:
			//左側にいたら左の画面外側に動かす
			if (lerpPos.x <= WinApp::Instance()->GetWinSize().x / 2.0f)
			{
				lerpPos.x = 0.0f - maxDistance;
			}
			//右側に居たら左側に動かす
			else
			{
				lerpPos.x = WinApp::Instance()->GetWinSize().x + maxDistance;
			}
			break;

		case ScreenEdge::Y_VEC:
			//左側にいたら右側に動かす
			if (lerpPos.y <= WinApp::Instance()->GetWinSize().y / 2.0f)
			{
				lerpPos.y = 0.0f - maxDistance;;
			}
			//右側に居たら左側に消す
			else
			{
				lerpPos.y = WinApp::Instance()->GetWinSize().y + maxDistance;
			}
			break;
		default:
			break;
		}
	}


	vel = speed * rate;
	for (int i = 0; i < scrollChancePos.size(); ++i)
	{
		scrollChancePos[i] += vel;

		float texSize = 1280.0f;
		switch (vecType)
		{
		case ScreenEdge::X_VEC:
			if (texSize <= scrollChancePos[i].y)
			{
				scrollChancePos[i].y = -(1280.0f) * WinApp::Instance()->GetWinDifferRate().x;
			}
			break;

		case ScreenEdge::Y_VEC:
			if (texSize <= scrollChancePos[i].x)
			{
				scrollChancePos[i].x = -1280.0f;
			}
			break;
		default:
			break;
		}

	}


	Vec2<float> lerp = lerpPos - pos;
	pos += lerp * 0.2f;

	//チャンスフレームのサインカーブ
	float pi = 3.14f;
	float pi2 = pi / 2.0f;
	sinCurve += 3.0f;
	chanceAlpha = 55.0f + ((sinf(-pi2 + pi / 120.0f * sinCurve) + 1.0f) / 2) * 200.0f;

	uv += 0.001f;
	for (int i = 0; i < 2; ++i)
	{
		chanceRender[i].transform.SetPos(pos + adjPos + adjYPos);
		chanceRender[i].mesh.SetSize(Vec2<float>(1280.0f, 32.0f));
		Vec3<Angle>lAngle = { 0.0f,0.0f,Angle(static_cast<int>(angle)) };
		chanceRender[i].transform.SetRotate(lAngle);
		chanceRender[i].SetTexture(TexHandleMgr::GetTexBuffer(nowChanceHandle));
		chanceRender[i].SetColor(Color(255, 255, 255, static_cast<int>(chanceAlpha)));

		chanceRender[i].mesh.SetUv(0.0f, 1.0f, 0.0f + uv, 1.0f + uv);
	}
}

void ScreenEdge::Draw()
{
	DrawFunc::DrawRotaGraph2D(pos + adjTexPos, Vec2<float>(1.0f, 1.0f), Angle::ConvertToRadian(angle), TexHandleMgr::GetTexBuffer(nowFlameHandle));

	for (int i = 0; i < scrollChancePos.size(); ++i)
	{
		chanceRender[i].Draw(AlphaBlendMode_Trans);
	}
}
