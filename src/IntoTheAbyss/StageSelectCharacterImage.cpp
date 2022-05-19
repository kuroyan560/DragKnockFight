#include "StageSelectCharacterImage.h"
#include "DrawFunc.h"
#include"IntoTheAbyss/StageSelectOffsetPosDebug.h"

StageSelectCharacterImage::StageSelectCharacterImage()
{

}

void StageSelectCharacterImage::Init(const Vec2<float>& ZoomInPos, const Vec2<float>& ZoomOutPos, const int& GraphHandle)
{

	zoomInPos.Init(ZoomInPos, Vec2<float>(2.8f, 2.5f));
	zoomOutPos.Init(ZoomOutPos, Vec2<float>(1.0f, 1.0f));
	pos.Init(zoomInPos.pos, zoomInPos.size);
	isZoomOut = false;
	graphHandle = GraphHandle;

}

void StageSelectCharacterImage::Update()
{

	// �Y�[���A�E�g�̏󋵂ɉ����āA��Ԑ�̍��W��؂�ւ���B
	if (isZoomOut) {

		pos.lerpPos = zoomOutPos.lerpPos;
		pos.lerpSize = zoomOutPos.lerpSize;

	}
	else {

		pos.lerpPos = zoomInPos.lerpPos;
		pos.lerpSize = zoomInPos.lerpSize;

	}


	// �ړ���ۊǂ�����B
	pos.Lerp();

}

void StageSelectCharacterImage::Draw()
{
	
	Vec2<float> debugPos = StageSelectOffsetPosDebug::Instance()->pos;
	DrawFunc::DrawRotaGraph2D(pos.pos + debugPos, pos.size, 0, TexHandleMgr::GetTexBuffer(graphHandle));

}
