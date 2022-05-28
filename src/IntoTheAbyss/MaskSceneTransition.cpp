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
	Vec2<float>initPos = Vec2<float>(static_cast<float>(WinApp::Instance()->GetWinCenter().x - 35.0f), static_cast<float>(WinApp::Instance()->GetWinCenter().y-2.0f));


	if (t <= 1.0f)
	{
		t += 1.0f / 120.0f;
	}
	if (1.0f <= t)
	{
		t = 1.0f;
	}

	// 画像を0に近づける。
	expRate = KuroMath::Ease(In, Cubic, t, 0.0f, 1.0f) * 30.0f;
	pos.x = initPos.x + KuroMath::Ease(In, Cubic, t, 0.0f, 1.0f) * 1050.0f;
	pos.y = initPos.y + KuroMath::Ease(In, Cubic, t, 0.0f, 1.0f) * 50.0f;

	backGroundPos.x = initPos.x;
	backGroundPos.y = initPos.y;


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
		DrawFunc_Mask::DrawGraphByMaskGraph(pos, DrawMapChipForSceneChange::Instance()->mapBuffer, backGroundPos, TexHandleMgr::GetTexBuffer(maskHandle), Vec2<float>(expRate, expRate));
	}
}
