#include "TimeStopTestBlock.h"
#include "ScrollMgr.h"
#include "ShakeMgr.h"
#include "MapChipCollider.h"

TimeStopTestBlock::TimeStopTestBlock()
{

	/*===== �R���X�g���N�^ =====*/

	pos = { 100,100 };
	isTimeStop = false;

}

void TimeStopTestBlock::Update(const Vec2<float>& playerPos)
{

	/*===== �X�V���� =====*/

	// �v���C���[�̈ʒu�ɋߕt����B
	Vec2<float> targetPos = playerPos;
	targetPos.y -= 10.0f;

	// ���Ԓ�~�̒Z�����h�����Ă��Ȃ�������ߕt����B
	if (isTimeStop) return;

	// �ߕt����B
	pos.x += (targetPos.x - pos.x) / 100.0f;
	pos.y += (targetPos.y - pos.y) / 100.0f;

}

#include"DrawFunc.h"
#include"TexHandleMgr.h"
void TimeStopTestBlock::Draw(const int& blockGraph)
{

	/*===== �`�揈�� =====*/

	// ���W���v�Z�B
	const float centerX = pos.x * ScrollMgr::Instance()->zoom - ScrollMgr::Instance()->scrollAmount.x * ScrollMgr::Instance()->zoom;
	const float centerY = pos.y * ScrollMgr::Instance()->zoom - ScrollMgr::Instance()->scrollAmount.y * ScrollMgr::Instance()->zoom;

	// �}�b�v��`��
	//DrawExtendGraph(centerX - MAP_CHIP_HALF_SIZE * ScrollMgr::Instance()->zoom + ShakeMgr::Instance()->shakeAmount.x * ScrollMgr::Instance()->zoom,
	//	centerY - MAP_CHIP_HALF_SIZE * ScrollMgr::Instance()->zoom + ShakeMgr::Instance()->shakeAmount.y * ScrollMgr::Instance()->zoom,
	//	centerX + MAP_CHIP_HALF_SIZE * ScrollMgr::Instance()->zoom + ShakeMgr::Instance()->shakeAmount.x * ScrollMgr::Instance()->zoom,
	//	centerY + MAP_CHIP_HALF_SIZE * ScrollMgr::Instance()->zoom + ShakeMgr::Instance()->shakeAmount.y * ScrollMgr::Instance()->zoom,
	//	blockGraph, FALSE);

	Vec2<float>leftUp = { centerX - MAP_CHIP_HALF_SIZE * ScrollMgr::Instance()->zoom + ShakeMgr::Instance()->shakeAmount.x * ScrollMgr::Instance()->zoom,
		centerY - MAP_CHIP_HALF_SIZE * ScrollMgr::Instance()->zoom + ShakeMgr::Instance()->shakeAmount.y * ScrollMgr::Instance()->zoom };
	Vec2<float>rightBottom = { centerX + MAP_CHIP_HALF_SIZE * ScrollMgr::Instance()->zoom + ShakeMgr::Instance()->shakeAmount.x * ScrollMgr::Instance()->zoom,
		centerY + MAP_CHIP_HALF_SIZE * ScrollMgr::Instance()->zoom + ShakeMgr::Instance()->shakeAmount.y * ScrollMgr::Instance()->zoom };
	
	// �}�b�v��`��
	DrawFunc::DrawExtendGraph2D(leftUp, rightBottom, TexHandleMgr::GetTexBuffer(blockGraph));

}
