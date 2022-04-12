#include "ResultSceneBackGround.h"
#include "GaussianBlur.h"


void ResultSceneBackGround::Init()
{
	gaussianBlur = std::make_shared<GaussianBlur>(WinApp::Instance()->GetWinSize(), D3D12App::Instance()->GetBackBuffFormat());

	//backGround = D3D12App::Instance()->GenerateRenderTarget(DXGI_FORMAT_R32G32B32A32_FLOAT, Color(0.0f, 0.0f, 0.0f, 1.0f),
	//	WinApp::Instance()->GetWinSize(), L"ResultSceneBackGround");

	auto backBuff = D3D12App::Instance()->GetBackBuffRenderTarget();
	backGround = D3D12App::Instance()->GenerateRenderTarget(backBuff->GetDesc().Format, Color(0.0f, 0.0f, 0.0f, 1.0f), backBuff->GetGraphSize(), L"ScreenShot");
}

void ResultSceneBackGround::Draw()
{
	gaussianBlur->Register(backGround);
	gaussianBlur->DrawResult(AlphaBlendMode_None);
}
