#include "BehaviorPredection.h"
#include"../Engine/DrawFunc.h"
#include"TexHandleMgr.h"
#include"ScrollMgr.h"

BehaviorPredection::BehaviorPredection(const std::vector<BehaviorGraphData> &DATA)
{
	pos = { 0.0f,0.0f };
	size = { 1.0f,1.0f };
	gaugePos = { 0.0f,0.0f };
	gaugeSize = { 1.0f,1.0f };
	leftVarPos = { 0.0f,0.0f };
	rightVarPos = { 0.0f,0.0f };
	commandPos = { 0.0f,0.0f };
	commandSize = { 1.0f,1.0f };

	std::vector<Anim>data;
	for (int i = 0; i < DATA.size(); ++i)
	{
		data.push_back(DATA[i].handle);
		cautionFlags.push_back(DATA[i].cautionFlag);
	}
	handleData = std::make_unique<PlayerAnimation>(data);


	std::string pass = "resource/ChainCombat/boss/behaviorPrediction/";
	defaultFlameHandle = TexHandleMgr::LoadGraph(pass + "icon_frame.png");
	cautionFlameHandle = TexHandleMgr::LoadGraph(pass + "icon_frame_caution.png");
	gaugeHandle = TexHandleMgr::LoadGraph(pass + "gauge_frame.png");
	varHandle = TexHandleMgr::LoadGraph(pass + "gauge.png");
}

void BehaviorPredection::Init()
{
}

void BehaviorPredection::Update(const Vec2<float> &POS, int NOW_HANDLE, float RATE)
{
	Vec2<float>adjPos = { 0.0f,0.0f };
	{
		adjPos = { 0.0f,-50.0f };
		pos = POS + adjPos;
	}
	{
		adjPos = { 0.0f,30.0f };
		gaugePos = POS + adjPos;
	}
	{
		adjPos = { -97.5f,25.0f };
		//画像サイズを縮小しそこに割合を足してゲージの描画と合わせる
		Vec2<float> size = { RATE,1.0f };
		leftVarPos = POS + adjPos;
		rightVarPos = (leftVarPos + Vec2<float>(0.0f, 10.0f)) + (Vec2<float>(195.0f, 0.0f) * size);
	}
	{
		adjPos = { 0.0f,-60.0f };
		commandPos = POS + adjPos;
	}

	nowHandle = NOW_HANDLE;

	handleData->ChangeAnim(nowHandle);
	handleData->Update();
}

void BehaviorPredection::Draw(bool DEBUG)
{
	if (DEBUG)
	{
		float zoom = 1.0f;
		//フレームの描画
		if (cautionFlags[nowHandle])
		{
			DrawFunc::DrawRotaGraph2D(pos, size * zoom, 0.0f, TexHandleMgr::GetTexBuffer(cautionFlameHandle));
		}
		else
		{
			DrawFunc::DrawRotaGraph2D(pos, size * zoom, 0.0f, TexHandleMgr::GetTexBuffer(defaultFlameHandle));
		}

		//ゲージの描画
		DrawFunc::DrawExtendGraph2D(leftVarPos, rightVarPos, TexHandleMgr::GetTexBuffer(varHandle));
		DrawFunc::DrawRotaGraph2D(gaugePos, gaugeSize * zoom, 0.0f, TexHandleMgr::GetTexBuffer(gaugeHandle));

		//コマンドの描画
		DrawFunc::DrawRotaGraph2D(commandPos, commandSize * zoom, 0.0f, TexHandleMgr::GetTexBuffer(handleData->GetGraphHandle()));
	}
	else
	{
		float zoom = ScrollMgr::Instance()->zoom;
		//フレームの描画
		if (cautionFlags[nowHandle])
		{
			DrawFunc::DrawRotaGraph2D(ScrollMgr::Instance()->Affect(pos), size * zoom, 0.0f, TexHandleMgr::GetTexBuffer(cautionFlameHandle));
		}
		else
		{
			DrawFunc::DrawRotaGraph2D(ScrollMgr::Instance()->Affect(pos), size * zoom, 0.0f, TexHandleMgr::GetTexBuffer(defaultFlameHandle));
		}

		//ゲージの描画
		DrawFunc::DrawExtendGraph2D(ScrollMgr::Instance()->Affect(leftVarPos), ScrollMgr::Instance()->Affect(rightVarPos), TexHandleMgr::GetTexBuffer(varHandle));
		DrawFunc::DrawRotaGraph2D(ScrollMgr::Instance()->Affect(gaugePos), gaugeSize * zoom, 0.0f, TexHandleMgr::GetTexBuffer(gaugeHandle));

		//コマンドの描画
		DrawFunc::DrawRotaGraph2D(ScrollMgr::Instance()->Affect(commandPos), commandSize * zoom, 0.0f, TexHandleMgr::GetTexBuffer(handleData->GetGraphHandle()));
	}
}