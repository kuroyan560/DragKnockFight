#include "AuraBlock.h"
#include "ScrollMgr.h"
#include"ShakeMgr.h"

#include"DrawFunc.h"

AuraBlock::AuraBlock()
{
}

void AuraBlock::Init(Vec2<float> POS, Vec2<float> SIZE, AuraDir UPDOWN_OR_SIDE_FLAG)
{
	leftUpPos = POS;
	rightDownPos = SIZE;



	eDir = UPDOWN_OR_SIDE_FLAG;


	switch (eDir)
	{
	case AURA_DIR_UPORDOWN:
		//color = GetColor(255, 255, 255);
		color = Color(1.0f, 1.0f, 1.0f, 1.0f);
		break;
	case AURA_DIR_LEFTORRIGHT:
		//color = GetColor(255, 0, 0);
		color = Color(1.0f, 0.0f, 0.0f, 1.0f);
		break;
	default:
		break;
	}
}

void AuraBlock::Finalize()
{
}

void AuraBlock::Update()
{
}

#include"D3D12App.h"
void AuraBlock::Draw()
{
	//DrawBox(
	//	(leftUpPos.x - ScrollMgr::Instance()->scrollAmount.x - ShakeMgr::Instance()->shakeAmount.x) * ScrollMgr::Instance()->zoom,
	//	(leftUpPos.y - ScrollMgr::Instance()->scrollAmount.y - ShakeMgr::Instance()->shakeAmount.y) * ScrollMgr::Instance()->zoom,
	//	(rightDownPos.x - ScrollMgr::Instance()->scrollAmount.x - ShakeMgr::Instance()->shakeAmount.x) * ScrollMgr::Instance()->zoom,
	//	(rightDownPos.y - ScrollMgr::Instance()->scrollAmount.y - ShakeMgr::Instance()->shakeAmount.y) * ScrollMgr::Instance()->zoom,
	//	color,
	//	false);

	Vec2<float>leftUp = { (leftUpPos.x - ScrollMgr::Instance()->scrollAmount.x - ShakeMgr::Instance()->shakeAmount.x) * ScrollMgr::Instance()->zoom,
		(leftUpPos.y - ScrollMgr::Instance()->scrollAmount.y - ShakeMgr::Instance()->shakeAmount.y) * ScrollMgr::Instance()->zoom };

	Vec2<float>rightBottom = { (rightDownPos.x - ScrollMgr::Instance()->scrollAmount.x - ShakeMgr::Instance()->shakeAmount.x) * ScrollMgr::Instance()->zoom,
		(rightDownPos.y - ScrollMgr::Instance()->scrollAmount.y - ShakeMgr::Instance()->shakeAmount.y) * ScrollMgr::Instance()->zoom };

	DrawFunc::DrawBox2D(leftUp, rightBottom, color, D3D12App::Instance()->GetBackBuffFormat());
}
