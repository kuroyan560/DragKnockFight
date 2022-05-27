#include "ScrollMgr.h"
#include"../Engine/WinApp.h"

const float ScrollMgr::INIT_SCROLL = 0.4f;

void ScrollMgr::Init(const Vec2<float> POS, const Vec2<float>& MAP_MAX_SIZE, const Vec2<float>& ADJ)
{
	mapSize = MAP_MAX_SIZE;
	adjLine = ADJ;


	windowSize = WinApp::Instance()->GetWinSize();
	windowHalfSize = Vec2<float>(windowSize.x / 2.0f, windowSize.y / 2.0f);



	Vec2<float>startPos = CaluStartScrollLine(windowHalfSize - adjLine);
	Vec2<float>endPos = CaluEndScrollLine(windowHalfSize + adjLine);
	honraiScrollAmount = (POS + adjLine) - startPos;
	scrollAmount = honraiScrollAmount;

	resetAmount = scrollAmount;
	warpFlag = false;
}

void ScrollMgr::Update(const Vec2<float>& LineCenterPos)
{
	if (Camera::Instance()->active) {
		/*scrollAmount.x = honraiScrollAmount.x - Camera::Instance()->scrollAffect.x;
		scrollAmount.y = honraiScrollAmount.y - Camera::Instance()->scrollAffect.y;*/
	}
	else {
		scrollAmount.x += (honraiScrollAmount.x - scrollAmount.x) / 5.0f;
		scrollAmount.y += (honraiScrollAmount.y - scrollAmount.y) / 5.0f;
	}

	// ���̒��S�ɃX�N���[���ʂȂǂ�������B
	Vec2<float> scrollShakeZoom = scrollAmount + ShakeMgr::Instance()->shakeAmount;
	scrollShakeZoom.x *= zoom;
	scrollShakeZoom.y *= zoom;
	Vec2<float> lineCenterPos = LineCenterPos * zoom - scrollShakeZoom;

	// ��ʂ̒��S���擾�B
	Vec2<int> windowCenter = WinApp::Instance()->GetWinCenter();

	// ���̒��S�Ɖ�ʂ̒��S�Ƃ̍��������߂�B
	Vec2<float> lineCenterOffsetBuff = Vec2<float>((float)windowCenter.x, (float)windowCenter.y) - lineCenterPos;

	// �⊮��������B
	if (Camera::Instance()->active) {
		lineCenterOffset = lineCenterOffsetBuff;
	}
	else {
		lineCenterOffset += (lineCenterOffsetBuff - lineCenterOffset) / 5.0f;
	}

}

void ScrollMgr::CalucurateScroll(const Vec2<float>& VEL, const Vec2<float>& PLAYER_POS)
{
	//���[�v�����ۂ͑O�t���[���v�Z�̉e�����o�Ȃ��悤�ɏ������΂�

	//�X�N���[���̐���
	Vec2<float>startPos = CaluStartScrollLine(windowHalfSize - adjLine);
	Vec2<float>endPos = CaluEndScrollLine(windowHalfSize + adjLine);

	//�v���C���[���W��胉�C���O�ɏo����X�N���[���ʂ͕ω����Ȃ�

	//if (!warpFlag)
	//{
	//	honraiScrollAmount -= VEL;

	//	const float adj = 50.0f / 2.0f;
	//	const Vec2<float>scrollMaxValue = (endPos - startPos) - Vec2<float>(adj, adj);
	//	const Vec2<float>scrollMinValue = Vec2<float>(-adj, -adj);

	//	if (PLAYER_POS.x <= startPos.x - adjLine.x)
	//	{
	//		honraiScrollAmount.x = scrollMinValue.x;
	//	}
	//	if (endPos.x - adjLine.x - adj <= PLAYER_POS.x)
	//	{
	//		honraiScrollAmount.x = scrollMaxValue.x;
	//	}

	//	const bool getMinVelFlag = PLAYER_POS.y <= startPos.y - adjLine.y;
	//	if (getMinVelFlag)
	//	{
	//		honraiScrollAmount.y = scrollMinValue.y;
	//	}

	//	const bool getMaxVelFlag = endPos.y - adjLine.y - adj <= PLAYER_POS.y;
	//	if (getMaxVelFlag)
	//	{
	//		honraiScrollAmount.y = scrollMaxValue.y;
	//	}
	//}
	warpFlag = false;
}

Vec2<float> ScrollMgr::Affect(const Vec2<float>& Pos)
{
	Vec2<float> scrollShakeZoom = scrollAmount + ShakeMgr::Instance()->shakeAmount;
	scrollShakeZoom.x *= zoom;
	scrollShakeZoom.y *= zoom;
	return Pos * zoom - scrollShakeZoom + lineCenterOffset;
}

void ScrollMgr::Warp(const Vec2<float> POS)
{
	//warpFlag = true;
	//honraiScrollAmount = { 0.0f,0.0f };
	//Vec2<float>startPos = CaluStartScrollLine(windowHalfSize - adjLine);
	//honraiScrollAmount = (POS + adjLine) - startPos;
}

void ScrollMgr::Reset()
{
	honraiScrollAmount = resetAmount;
	scrollAmount = resetAmount;
}
