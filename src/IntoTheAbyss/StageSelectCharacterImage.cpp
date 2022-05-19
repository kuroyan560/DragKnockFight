#include "StageSelectCharacterImage.h"
#include "DrawFunc.h"
#include"IntoTheAbyss/StageSelectOffsetPosDebug.h"

StageSelectCharacterImage::StageSelectCharacterImage()
{

}

void StageSelectCharacterImage::Init(const Vec2<float>& ZoomInPos, const Vec2<float>& ZoomOutPos, const int& GraphHandle)
{

	zoomInPos.Init(ZoomInPos, Vec2<float>(2.8f, 2.5f), Vec2<float>(0.0f, 0.0f), Vec2<float>(0.0f, 0.0f));
	zoomOutPos.Init(ZoomOutPos, Vec2<float>(1.0f, 1.0f), Vec2<float>(0.0f, 0.0f), Vec2<float>(0.0f, 0.0f));
	pos.Init(zoomInPos.pos, zoomInPos.size, Vec2<float>(0.0f, 0.0f), Vec2<float>(0.0f, 0.0f));
	pos.timer = 1.0f;
	isZoomOut = false;
	graphHandle = GraphHandle;

}

void StageSelectCharacterImage::Update()
{

	// ズームアウトの状況に応じて、補間先の座標を切り替える。
	if (isZoomOut) {

		pos.lerpPos = zoomOutPos.lerpPos;
		pos.lerpSize = zoomOutPos.lerpSize;
		pos.startPos = zoomInPos.lerpPos;
		pos.startSize = zoomInPos.lerpSize;

	}
	else {

		pos.lerpPos = zoomInPos.lerpPos;
		pos.lerpSize = zoomInPos.lerpSize;
		pos.startPos = zoomOutPos.lerpPos;
		pos.startSize = zoomOutPos.lerpSize;

	}


	// 移動を保管させる。
	pos.EaseInOut();

}

void StageSelectCharacterImage::Draw()
{

	Vec2<float> debugPos = StageSelectOffsetPosDebug::Instance()->pos;
	DrawFunc::DrawRotaGraph2D(pos.pos + debugPos, pos.size, 0, TexHandleMgr::GetTexBuffer(graphHandle));

}
