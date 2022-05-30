#include "RoundCountMgr.h"
#include "DrawFunc.h"
#include "TexHandleMgr.h"

RoundCount::RoundCount()
{

	/*===== �R���X�g���N�^ =====*/

	pos = {};
	isActive = false;
	isFill = false;

}

void RoundCount::Init()
{

	/*===== ���������� =====*/

	pos = {};
	isActive = false;
	isFill = false;

}

void RoundCount::Generate(const Vec2<float>& Pos)
{

	/*===== ���������� =====*/

	pos = Pos;
	isActive = true;
	isFill = false;

}

void RoundCount::Draw(const int& FrameGraph, const int& InnerGraph)
{

	/*===== �`�揈�� =====*/

	// �O����`��
	DrawFunc::DrawRotaGraph2D(pos, Vec2<float>(1.0f,1.0f), 0, TexHandle)

}
