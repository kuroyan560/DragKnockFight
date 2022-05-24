#include"Gauge.h"
#include"../Engine/DrawFunc.h"
#include"../IntoTheAbyss/TexHandleMgr.h"
#include"../IntoTheAbyss/ScrollMgr.h"

Gauge::Gauge()
{
	std::string pass = "resource/ChainCombat/boss/behaviorPrediction/";
	gaugeHandle = TexHandleMgr::LoadGraph(pass + "gauge_frame.png");
	varHandle = TexHandleMgr::LoadGraph(pass + "gauge.png");

	gaugeSize = { 1.0f,1.0f };
}

void Gauge::Init(int RATE_MAX)
{
	rateMax = RATE_MAX;
	nowRate = 0;
}

void Gauge::Update(const Vec2<float> &POS)
{
	++nowRate;

	if (rateMax <= nowRate)
	{
		nowRate = rateMax;
	}

	float now = static_cast<float>(nowRate) / static_cast<float>(rateMax);


	Vec2<float>adjPos = { 0.0f,0.0f };

	adjPos = { 0.0f,-60.0f };
	gaugePos = POS + adjPos;

	adjPos = { -97.5f,-65.0f };
	//�摜�T�C�Y���k���������Ɋ����𑫂��ăQ�[�W�̕`��ƍ��킹��
	Vec2<float> size = { now,1.0f };
	leftVarPos = POS + adjPos;
	rightVarPos = (leftVarPos + Vec2<float>(0.0f, 10.0f)) + (Vec2<float>(195.0f, 0.0f) * size);

	adjPos = { 0.0f,-60.0f };
	gaugePos = POS + adjPos;
}

void Gauge::Draw()
{
	float zoom = ScrollMgr::Instance()->zoom;
	//�Q�[�W�̕`��
	DrawFunc::DrawExtendGraph2D(ScrollMgr::Instance()->Affect(leftVarPos), ScrollMgr::Instance()->Affect(rightVarPos), TexHandleMgr::GetTexBuffer(varHandle));
	DrawFunc::DrawRotaGraph2D(ScrollMgr::Instance()->Affect(gaugePos), gaugeSize * zoom, 0.0f, TexHandleMgr::GetTexBuffer(gaugeHandle));
}

bool Gauge::canUseGague()
{
	return rateMax <= nowRate;
}

void Gauge::Use()
{
	nowRate = 0;
}
