#include "FaceIcon.h"
#include"TexHandleMgr.h"

FaceIcon::FaceIcon()
{
	const std::string dir = "resource/ChainCombat/UI/FaceIcon/";
	backGraph = TexHandleMgr::LoadGraph(dir + "back.png");

	const std::string lunaDir = dir + "luna/";
	animasions[PLAYABLE_LUNA][DEFAULT].graph.emplace_back(TexHandleMgr::LoadGraph(lunaDir + "default.png"));
	animasions[PLAYABLE_LUNA][DAMAGE].graph.emplace_back(TexHandleMgr::LoadGraph(lunaDir + "damage.png"));
	animasions[PLAYABLE_LUNA][BREAK].graph.emplace_back(TexHandleMgr::LoadGraph(lunaDir + "break.png"));
	animasions[PLAYABLE_LUNA][DEAD].graph.emplace_back(TexHandleMgr::LoadGraph(lunaDir + "dead.png"));
}

void FaceIcon::Init(const PLAYABLE_CHARACTER_NAME& Left, const PLAYABLE_CHARACTER_NAME& Right)
{
	character[LEFT_TEAM] = Left;
	character[RIGHT_TEAM] = Right;

	for (int i = 0; i < TEAM_NUM; ++i)
	{
		status[i] = DEFAULT;
		timer[i] = 0;
		graphIdx[i] = 0;
	}
}

void FaceIcon::Update()
{
	for (int i = 0; i < TEAM_NUM; ++i)
	{
		auto& anim = animasions[character[i]][status[i]];
		timer[i]++;
		if (anim.interval <= timer[i])
		{
			graphIdx[i]++;
			if (anim.graph.size() <= graphIdx[i])
			{
				if (status[i] == DEFAULT)graphIdx[i] = 0;	//デフォルトのときはループ
				else graphIdx[i]--;	//最後の画像を表示し続ける
			}
			timer[i] = 0;
		}
	}
}

#include"D3D12App.h"
#include"DrawFunc_FillTex.h"
#include"WinApp.h"

#include"DrawFunc.h"

void FaceIcon::Draw()
{
	//顔背景塗りつぶしテクスチャ
	static const std::shared_ptr<TextureBuffer> BACK_COLOR[FACE_STATUS_NUM] =
	{
		D3D12App::Instance()->GenerateTextureBuffer(Color(60,84,119,255)),
		D3D12App::Instance()->GenerateTextureBuffer(Color(154,40,82,255)),
		D3D12App::Instance()->GenerateTextureBuffer(Color(56,10,90,255)),
		D3D12App::Instance()->GenerateTextureBuffer(Color(61,57,68,255))
	};

	static const Vec2<float>POS = { 30.0f,26.0f };
	static const auto BACK_WIDTH = TexHandleMgr::GetTexBuffer(backGraph)->GetDesc().Width;
	static const int FACE_WIDTH = 150;
	static const float OFFSET_X = (BACK_WIDTH - FACE_WIDTH) / 2.0f;
	static const int FACE_ICON_OFFSET = 9;

	//左
	DrawFunc_FillTex::DrawGraph(POS, TexHandleMgr::GetTexBuffer(backGraph), BACK_COLOR[status[LEFT_TEAM]], 1.0f);
	DrawFunc::DrawGraph({ POS.x + OFFSET_X - FACE_ICON_OFFSET,POS.y }, TexHandleMgr::GetTexBuffer(animasions[character[LEFT_TEAM]][status[LEFT_TEAM]].graph[graphIdx[LEFT_TEAM]]), AlphaBlendMode_Trans);

	//右
	static const float RIGHT_X = WinApp::Instance()->GetExpandWinSize().x - POS.x - BACK_WIDTH;
	DrawFunc_FillTex::DrawGraph({ RIGHT_X ,POS.y }, TexHandleMgr::GetTexBuffer(backGraph), BACK_COLOR[status[RIGHT_TEAM]], 1.0f, { true,false });
	DrawFunc::DrawGraph({ RIGHT_X + OFFSET_X + FACE_ICON_OFFSET,POS.y }, TexHandleMgr::GetTexBuffer(animasions[character[RIGHT_TEAM]][status[RIGHT_TEAM]].graph[graphIdx[RIGHT_TEAM]]), AlphaBlendMode_Trans, { true,false });
}

void FaceIcon::Change(const WHICH_TEAM& WhichTeam, const FACE_STATUS& Status)
{
	status[WhichTeam] = Status;
	timer[WhichTeam] = 0;
	graphIdx[WhichTeam] = 0;
}