#include "ClearInfoContainer.h"
#include"KuroEngine.h"

ClearInfoContainer::ClearInfoContainer()
{
	auto backBuff = D3D12App::Instance()->GetBackBuffRenderTarget();
	clearInfoRenderTarget = D3D12App::Instance()->GenerateRenderTarget(backBuff->GetDesc().Format, Color(0, 0, 0, 0),
		backBuff->GetGraphSize(), L"ClearInfoRenderTarget");
}
