#include "StageSelectArrow.h"
#include "DrawFunc.h"
#include "TexHandleMgr.h"
#include"IntoTheAbyss/StageSelectOffsetPosDebug.h"

StageSelectArrow::StageSelectArrow()
{

	pos.Init(Vec2<float>(0, 0), Vec2<float>(0, 0));
	expData.Init(Vec2<float>(0, 0), Vec2<float>(0, 0));
	angle = 0;

	arrowHandle = TexHandleMgr::LoadGraph("resource/ChainCombat/select_scene/arrow.png");

}

void StageSelectArrow::Init(const Vec2<float>& Pos, const float& Angle)
{

	defPos = Pos;
	pos.Init(Pos, Vec2<float>(1, 1));
	expData.Init(Vec2<float>(0, 0), Vec2<float>(0, 0));
	angle = Angle;
	isZoomOut = false;

}

void StageSelectArrow::Update()
{

	pos.Lerp();
	expData.lerpPos = Vec2<float>(0.0f, 0.0f);
	expData.lerpSize = Vec2<float>(0.0f, 0.0f);
	expData.Lerp();

}

void StageSelectArrow::Draw()
{
	
	Vec2<float> debugPos = StageSelectOffsetPosDebug::Instance()->pos;
	DrawFunc::DrawRotaGraph2D(pos.pos + expData.pos + debugPos, pos.size + expData.size, angle, TexHandleMgr::GetTexBuffer(arrowHandle));

}

void StageSelectArrow::SetDefPos()
{
	pos.lerpPos = defPos;
	pos.lerpSize = Vec2<float>(1.0f, 1.0f);
}

void StageSelectArrow::SetExitPos(const Vec2<float>& ExitPos, const Vec2<float>& ExitSize)
{
	pos.lerpPos = ExitPos;
	pos.lerpSize = ExitSize;
}

void StageSelectArrow::SetExpSize(const Vec2<float>& Size)
{
	expData.size = Size;
}
