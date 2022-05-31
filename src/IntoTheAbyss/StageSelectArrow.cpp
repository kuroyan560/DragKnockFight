#include "StageSelectArrow.h"
#include "DrawFunc.h"
#include "TexHandleMgr.h"
#include"IntoTheAbyss/StageSelectOffsetPosDebug.h"

StageSelectArrow::StageSelectArrow()
{

	//pos.Init(Vec2<float>(0, 0), Vec2<float>(0, 0));
	//expData.Init(Vec2<float>(0, 0), Vec2<float>(0, 0));
	angle = 0;

	arrowHandle = TexHandleMgr::LoadGraph("resource/ChainCombat/select_scene/arrow.png");

}

void StageSelectArrow::Init(const Vec2<float>& InitPos, const Vec2<float>& Pos, const float& Angle)
{

	defPos = Pos;
	pos.Init(InitPos, Vec2<float>(1, 1), Vec2<float>(1.0f, 1.0f), Vec2<float>(1.0f, 1.0f));
	pos.timer = 1.0f;
	expData.Init(Vec2<float>(0, 0), Vec2<float>(0, 0), Vec2<float>(0, 0), Vec2<float>(0, 0));
	angle = Angle;
	isZoomOut = false;
	timer = 0;

}

void StageSelectArrow::Update(const bool& isLeft)
{

	pos.EaseInOut();
	expData.lerpPos = Vec2<float>(0.0f, 0.0f);
	expData.lerpSize = Vec2<float>(0.0f, 0.0f);
	expData.Lerp();

	// リサージュ曲線に使用するタイマーを更新。
	if (isLeft)timer += 0.01f;
	if (!isLeft)timer += 0.01f;
	//if (1.0f < timer) timer = 0;

}

void StageSelectArrow::Draw()
{
	// 描画するリサージュ曲線のいちを求める。
	float lissajousMove = 10.0f;
	Vec2<float> lissajousCurve = Vec2<float>(cosf(1.0f * timer) * lissajousMove, sinf(2.0f * timer) * lissajousMove);

	Vec2<float> debugPos = StageSelectOffsetPosDebug::Instance()->pos;
	DrawFunc::DrawRotaGraph2D(pos.pos + expData.pos + debugPos + lissajousCurve, pos.size + expData.size, angle, TexHandleMgr::GetTexBuffer(arrowHandle));

}

void StageSelectArrow::SetZoomOut(const bool& Zoom)
{

	isZoomOut = Zoom;

	pos.timer = 0.0f;

}

void StageSelectArrow::SetDefPos()
{
	pos.startPos = pos.lerpPos;
	pos.startSize = pos.lerpSize;
	pos.lerpPos = defPos;
	pos.lerpSize = Vec2<float>(1.0f, 1.0f);
	pos.timer = 0;
}

void StageSelectArrow::SetExitPos(const Vec2<float>& ExitPos, const Vec2<float>& ExitSize)
{
	pos.startPos = pos.lerpPos;
	pos.startSize = pos.lerpSize;
	pos.lerpPos = ExitPos;
	pos.lerpSize = ExitSize;
	pos.timer = 0;
}

void StageSelectArrow::SetExpSize(const Vec2<float>& Size)
{
	expData.size = Size;
}
