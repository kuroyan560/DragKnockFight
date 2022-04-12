#include "ResultSceneBackGround.h"

ResultSceneBackGround::ResultSceneBackGround()
{

	gaussianBlur = std::make_shared<GaussianBlur>(WinApp::Instance()->GetWinSize(), DXGI_FORMAT_R32G32B32A32_FLOAT);

	backGround = D3D12App::Instance()->GenerateRenderTarget(DXGI_FORMAT_R32G32B32A32_FLOAT, Color(0.0f, 0.0f, 0.0f, 1.0f),
		WinApp::Instance()->GetWinSize(), L"ResultSceneBackGround");

}

void ResultSceneBackGround::Init()
{

}
