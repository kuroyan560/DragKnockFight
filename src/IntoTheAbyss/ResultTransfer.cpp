#include "ResultTransfer.h"
#include "GaussianBlur.h"

ResultTransfer::ResultTransfer()
{
	gaussianBlur = std::make_shared<GaussianBlur>(WinApp::Instance()->GetWinSize(), D3D12App::Instance()->GetBackBuffFormat());

	auto backBuff = D3D12App::Instance()->GetBackBuffRenderTarget();
	backGround = D3D12App::Instance()->GenerateRenderTarget(backBuff->GetDesc().Format, Color(0.0f, 0.0f, 0.0f, 1.0f), backBuff->GetGraphSize(), L"ResultSceneBackGround");
}

void ResultTransfer::Init()
{
	leftBreakCount = 0;
	rightBreakCount = 0;
	winner = PLAYABLE_LUNA;
}

void ResultTransfer::Draw()
{
	gaussianBlur->Register(backGround);
	gaussianBlur->DrawResult(AlphaBlendMode_None);
}
