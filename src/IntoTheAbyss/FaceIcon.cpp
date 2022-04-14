#include "FaceIcon.h"
#include"TexHandleMgr.h"
#include"SuperiorityGauge.h"

FaceIcon::FaceIcon()
{
	const std::string dir = "resource/ChainCombat/UI/FaceIcon/";
	backGraph = TexHandleMgr::LoadGraph(dir + "back.png");

	const std::string lunaDir = dir + "luna/";
	animasions[LUNA][DEFAULT].graph.emplace_back(TexHandleMgr::LoadGraph(lunaDir + "default.png"));
	animasions[LUNA][DAMAGE].graph.emplace_back(TexHandleMgr::LoadGraph(lunaDir + "damage.png"));
	animasions[LUNA][BREAK].graph.emplace_back(TexHandleMgr::LoadGraph(lunaDir + "break.png"));
	animasions[LUNA][DEAD].graph.emplace_back(TexHandleMgr::LoadGraph(lunaDir + "dead.png"));
}

void FaceIcon::Init(const CHARACTER& Left, const CHARACTER& Right)
{
	character[LEFT_FACE] = Left;
	character[RIGHT_FACE] = Right;

	for (int i = 0; i < FACE_NUM; ++i)
	{
		status[i] = DEFAULT;
		timer[i] = 0;
		graphIdx[i] = 0;
	}
}

void FaceIcon::Update()
{
	for (int i = 0; i < FACE_NUM; ++i)
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
		}

		//ダメージ（クラッシュ受けたとき）の表示時間
		static const int DAMAGE_SPAN = 90;
		//一定時間ダメージ表示したら自動的にデフォルト状態に戻る
		if (status[i] == DAMAGE && DAMAGE_SPAN <= timer[i])
		{
			Change((WHICH_FACE)i, DEFAULT);
		}

		//スタン（ブレイク）の表示時間
		static const int BREAK_SPAN = 90;
		//一定時間ダメージ表示したら自動的にデフォルト状態に戻る
		if (status[i] == BREAK && BREAK_SPAN <= timer[i])
		{
			Change((WHICH_FACE)i, DEFAULT);
			SuperiorityGauge::Instance()->Init();
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
	DrawFunc_FillTex::DrawGraph(POS, TexHandleMgr::GetTexBuffer(backGraph), BACK_COLOR[status[LEFT_FACE]], 1.0f);
	DrawFunc::DrawGraph({ POS.x + OFFSET_X - FACE_ICON_OFFSET,POS.y }, TexHandleMgr::GetTexBuffer(animasions[character[LEFT_FACE]][status[LEFT_FACE]].graph[graphIdx[LEFT_FACE]]), AlphaBlendMode_Trans);

	//右
	static const float RIGHT_X = WinApp::Instance()->GetExpandWinSize().x - POS.x - BACK_WIDTH;
	DrawFunc_FillTex::DrawGraph({ RIGHT_X ,POS.y }, TexHandleMgr::GetTexBuffer(backGraph), BACK_COLOR[status[RIGHT_FACE]], 1.0f, { true,false });
	DrawFunc::DrawGraph({ RIGHT_X + OFFSET_X + FACE_ICON_OFFSET,POS.y }, TexHandleMgr::GetTexBuffer(animasions[character[RIGHT_FACE]][status[RIGHT_FACE]].graph[graphIdx[RIGHT_FACE]]), AlphaBlendMode_Trans, { true,false });
}

void FaceIcon::Change(const WHICH_FACE& Which, const FACE_STATUS& Status)
{
	status[Which] = Status;
	timer[Which] = 0;
	graphIdx[Which] = 0;
}