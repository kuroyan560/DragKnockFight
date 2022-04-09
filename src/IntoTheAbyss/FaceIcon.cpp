#include "FaceIcon.h"
#include"TexHandleMgr.h"

FaceIcon::FaceIcon()
{
	const std::string dir = "resource/ChainCombat/UI/FaceIcon/";
	backGraph = TexHandleMgr::LoadGraph(dir + "back.png");
}

void FaceIcon::Init()
{
	left = DEFAULT;
	right = DEFAULT;
}

#include"D3D12App.h"
#include"DrawFunc_FillTex.h"
#include"WinApp.h"
void FaceIcon::Draw()
{
	//Šç”wŒi“h‚è‚Â‚Ô‚µƒeƒNƒXƒ`ƒƒ
	static const std::shared_ptr<TextureBuffer> BACK_COLOR[FACE_TYPE_NUM] =
	{
		D3D12App::Instance()->GenerateTextureBuffer(Color(60,84,119,255)),
		D3D12App::Instance()->GenerateTextureBuffer(Color(154,40,82,255)),
		D3D12App::Instance()->GenerateTextureBuffer(Color(56,10,90,255)),
		D3D12App::Instance()->GenerateTextureBuffer(Color(61,57,68,255))
	};

	static const Vec2<float>POS = { 30.0f,26.0f };
	auto backTex = TexHandleMgr::GetTexBuffer(backGraph);

	//¶
	DrawFunc_FillTex::DrawGraph(POS, backTex, BACK_COLOR[left], 1.0f);

	//‰E
	static const float RIGHT_X = WinApp::Instance()->GetExpandWinSize().x - POS.x - backTex->GetDesc().Width;
	DrawFunc_FillTex::DrawGraph({ RIGHT_X ,POS.y }, TexHandleMgr::GetTexBuffer(backGraph), BACK_COLOR[right], 1.0f, { true,false });
}

void FaceIcon::ChangeLeft(const FACE_TYPE& Status)
{
	left = Status;
}

void FaceIcon::ChangeRight(const FACE_TYPE& Status)
{
	right = Status;
}
