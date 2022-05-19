#include "StageSelectArrow.h"
#include "DrawFunc.h"
#include "TexHandleMgr.h"

StageSelectArrow::StageSelectArrow()
{

	pos.Init(Vec2<float>(0, 0), Vec2<float>(0, 0));
	expData.Init(Vec2<float>(0, 0), Vec2<float>(0, 0));
	angle = 0;

	arrowHandle = TexHandleMgr::LoadGraph("resource/ChainCombat/select_scene/arrow.png");

}

void StageSelectArrow::Init(const Vec2<float>& Pos, const float& Angle)
{

	pos.Init(Pos, Vec2<float>(1, 1));
	expData.Init(Vec2<float>(0, 0), Vec2<float>(0, 0));
	angle = Angle;

}

void StageSelectArrow::Update()
{

	pos.Lerp();
	expData.Lerp();

}

void StageSelectArrow::Draw()
{

	DrawFunc::DrawRotaGraph2D(pos.pos + expData.pos, pos.size + expData.size, angle, TexHandleMgr::GetTexBuffer(arrowHandle));

}
