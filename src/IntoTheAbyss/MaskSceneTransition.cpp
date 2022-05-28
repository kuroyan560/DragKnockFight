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
	if (!startFlag)
	{
		expRate = 0.0f;
		angle = 0;
		startFlag = true;
	}
}

bool MaskSceneTransition::OnUpdate()
{

	// âÊëúÇ0Ç…ãﬂÇ√ÇØÇÈÅB
	if (30 < expRate)
	{
		expRate += 30.0f / 5.0f;
		//angle += 0.1f;
	}

	if (30 <= fabs(expRate))
	{
		expRate = 0;
		startFlag = false;
		return true;
	}

	return true;

}

void MaskSceneTransition::OnDraw()
{
	if (startFlag)
	{
		Vec2<float> windowCenter = Vec2<float>(static_cast<float>(WinApp::Instance()->GetWinCenter().x), static_cast<float>(WinApp::Instance()->GetWinCenter().y));

		//DrawFunc::DrawRotaGraph2D(windowCenter, Vec2<float>(expRate, expRate), angle, TexHandleMgr::GetTexBuffer(maskHandle));
		//DrawFunc_Mask::DrawGraphByMaskGraph(windowCenter, DrawMapChipForSceneChange::Instance()->mapBuffer, windowCenter, TexHandleMgr::GetTexBuffer(maskHandle), Vec2<bool>(false, false));
	}
}
