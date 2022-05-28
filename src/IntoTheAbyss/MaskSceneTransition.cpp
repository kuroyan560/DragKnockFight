#include "MaskSceneTransition.h"
#include "TexHandleMgr.h"
#include "DrawFunc.h"
#include "DrawFunc_Mask.h"
#include "Vec.h"
#include "WinApp.h"
#include "D3D12App.h"
#include "KuroEngine.h"
#include "DrawMapChipForSceneChange.h"

MaskSceneTransition::MaskSceneTransition()
{
	maskHandle = TexHandleMgr::LoadGraph("resource/ChainCombat/scene_transition/scene_transition_mask.png");
	startFlag = false;
}

MaskSceneTransition::~MaskSceneTransition()
{
}

void MaskSceneTransition::OnStart()
{
	if (!startFlag || UsersInput::Instance()->KeyInput(DIK_K))
	{
		expRate = 0.0f;
		angle = 0;
		t = 0.0f;
		startFlag = true;
	}
}

bool MaskSceneTransition::OnUpdate()
{
	if (t <= 1.0f)
	{
		t += 1.0f / 120.0f;
	}
	if (1.0f <= t)
	{
		t = 1.0f;
	}

	// ‰æ‘œ‚ð0‚É‹ß‚Ã‚¯‚éB
	if (expRate < 30)
	{
		expRate = KuroMath::Ease(Out, Cubic, t, 0.0f, 1.0f) * 10.0f;
		//angle += 0.1f;
	}

	if (30 <= expRate)
	{
		expRate = 0;
		startFlag = false;
		return true;
	}

	return false;
}

void MaskSceneTransition::OnDraw()
{
	if (startFlag)
	{
		Vec2<float> windowCenter = Vec2<float>(static_cast<float>(WinApp::Instance()->GetWinCenter().x), static_cast<float>(WinApp::Instance()->GetWinCenter().y));
		Vec2<float> windowCenter2 = Vec2<float>(static_cast<float>(WinApp::Instance()->GetWinCenter().x), static_cast<float>(WinApp::Instance()->GetWinCenter().y));

		//DrawFunc::DrawRotaGraph2D(windowCenter, Vec2<float>(expRate, expRate), angle, TexHandleMgr::GetTexBuffer(maskHandle));
		DrawFunc_Mask::DrawGraphByMaskGraph(windowCenter, DrawMapChipForSceneChange::Instance()->mapBuffer, windowCenter2, TexHandleMgr::GetTexBuffer(maskHandle), Vec2<bool>(false, false));

	}
}
