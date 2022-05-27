#include "MaskSceneTransition.h"
#include "TexHandleMgr.h"
#include "DrawFunc.h"
#include "DrawFunc_Mask.h"
#include "Vec.h"
#include "WinApp.h"
#include "D3D12App.h"
#include "KuroEngine.h"

MaskSceneTransition::MaskSceneTransition()
{
	maskHandle = TexHandleMgr::LoadGraph("resource/ChainCombat/scene_transition/scene_transition_mask.png");
}

MaskSceneTransition::~MaskSceneTransition()
{
}

void MaskSceneTransition::OnStart()
{
	expRate = 30.0f;
	angle = 0;
}

bool MaskSceneTransition::OnUpdate()
{

	// âÊëúÇ0Ç…ãﬂÇ√ÇØÇÈÅB
	if (0 < expRate) {
		expRate -= expRate / 5.0f;
		//angle += 0.1f;
	}

	if (fabs(expRate) < 0.001f) {

		expRate = 0;

		return true;

	}

	return false;

}

void MaskSceneTransition::OnDraw()
{

	Vec2<float> windowCenter = Vec2<float>(static_cast<float>(WinApp::Instance()->GetWinCenter().x), static_cast<float>(WinApp::Instance()->GetWinCenter().y));

	DrawFunc::DrawRotaGraph2D(windowCenter, Vec2<float>(expRate, expRate), angle, TexHandleMgr::GetTexBuffer(maskHandle));
	//DrawFunc_Mask::DrawGraphByMaskGraph(windowCenter, D3D12App::Instance()->GetBackBuffRenderTarget(), windowCenter, TexHandleMgr::GetTexBuffer(maskHandle), Vec2<bool>(false, false));

}
