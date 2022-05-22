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
	varPos = { 0.0f,0.0f };
	varSize = { 1.0f,1.0f };

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

void BehaviorPredection::Update(const Vec2<float> &POS, int NOW_HANDLE)
{
	Vec2<float>adjPos = { 0.0f,-50.0f };
	pos = POS + adjPos;
	nowHandle = NOW_HANDLE;

	handleData->ChangeAnim(nowHandle);
	handleData->Update();
}

void BehaviorPredection::Draw()
{
	//ƒtƒŒ[ƒ€‚Ì•`‰æ
	if (cautionFlags[nowHandle])
	{
		DrawFunc::DrawRotaGraph2D(pos, size, 0.0f, TexHandleMgr::GetTexBuffer(cautionFlameHandle));
	}
	else
	{
		DrawFunc::DrawRotaGraph2D(pos, size, 0.0f, TexHandleMgr::GetTexBuffer(defaultFlameHandle));
	}

	//ƒQ[ƒW‚Ì•`‰æ
	DrawFunc::DrawRotaGraph2D(varPos, varSize, 0.0f, TexHandleMgr::GetTexBuffer(varHandle));
	DrawFunc::DrawRotaGraph2D(gaugePos, gaugeSize, 0.0f, TexHandleMgr::GetTexBuffer(gaugeHandle));

	//ƒRƒ}ƒ“ƒh‚Ì•`‰æ
	DrawFunc::DrawRotaGraph2D(pos, size, 0.0f, TexHandleMgr::GetTexBuffer(handleData->GetGraphHandle()));
}
