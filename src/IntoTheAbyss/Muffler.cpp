#include "Muffler.h"
#include"KuroEngine.h"

//static const Vec2<float>MUFFLER_SIZE = { 48.0f,64.0f };
static const Vec2<float>MUFFLER_SIZE = { 32.0f,64.0f };
static const Vec2<float>OFFSET_FROM_PLAYER_POS = { 0.0f,-32.0f };

static const int UPDATE_PER_FRAME = 1;

unsigned int GetIndex(const int& x, const int& y, const int& numX)
{
	return y * numX + x;
}

Muffler::Muffler()
{
	//頂点情報
	const Vec2<float>uvOffset = { 1.0f / (VERTEX_NUM_X - 1),1.0f / (VERTEX_NUM_Y - 1) };
	for (int y = 0; y < VERTEX_NUM_Y; ++y)
	{
		for (int x = 0; x < VERTEX_NUM_X; ++x)
		{
			vertices[y][x].uv = { uvOffset.x * x,uvOffset.y * y };
		}
	}
	vertBuff = D3D12App::Instance()->GenerateVertexBuffer(sizeof(Vertex), VERTEX_NUM_X * VERTEX_NUM_Y, nullptr, "Muffler - Vertex", true);

	//インデックス情報
	std::vector<unsigned int>indices;
	for (int y = 0; y < VERTEX_NUM_Y - 1; ++y)
	{
		for (int x = 0; x < VERTEX_NUM_X; ++x)
		{
			//奇数
			if (y % 2 != 0)
			{
				indices.emplace_back(GetIndex(VERTEX_NUM_X - 1 - x, y, VERTEX_NUM_X));
				indices.emplace_back(GetIndex(VERTEX_NUM_X - 1 - x, y + 1, VERTEX_NUM_X));
			}
			//偶数
			else
			{
				indices.emplace_back(GetIndex(x, y, VERTEX_NUM_X));
				indices.emplace_back(GetIndex(x, y + 1, VERTEX_NUM_X));
			}
		}
	}
	idxBuff = D3D12App::Instance()->GenerateIndexBuffer(indices.size(), indices.data(), "Muffler - IndexBuff");

	//テクスチャ
	tex = D3D12App::Instance()->GenerateTextureBuffer(Color(217, 26, 96, 255));

	//コンピュートパイプライン
	{
		auto cs = D3D12App::Instance()->CompileShader("resource/HLSL/Muffler_Compute.hlsl", "CSmain", "cs_5_0");
		//ルートパラメータ
		std::vector<RootParam>rootParams =
		{
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_UAV,"頂点配列"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"マフラー基準点"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"マフラー性質"),
		};
		cPipeline = D3D12App::Instance()->GenerateComputePipeline(cs, rootParams);
	}
	playerPosBuff = D3D12App::Instance()->GenerateConstantBuffer(sizeof(Vec2<float>), 1, nullptr, "Muffler - PlayerPos");

	MufflerInfo info;

	info.vertexNum = { VERTEX_NUM_X,VERTEX_NUM_Y };
	info.mufflerSize = MUFFLER_SIZE;
	mufflerInfo = D3D12App::Instance()->GenerateConstantBuffer(sizeof(MufflerInfo), 1, &info, "Muffler - Info");

	//グラフィックスパイプライン
	{
		//パイプライン設定
		static PipelineInitializeOption PIPELINE_OPTION(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		PIPELINE_OPTION.depthTest = false;
		PIPELINE_OPTION.calling = false;

		//シェーダー情報
		static Shaders SHADERS;
		SHADERS.vs = D3D12App::Instance()->CompileShader("resource/HLSL/Muffler_Graphics.hlsl", "VSmain", "vs_5_0");
		SHADERS.ps = D3D12App::Instance()->CompileShader("resource/HLSL/Muffler_Graphics.hlsl", "PSmain", "ps_5_0");

		//インプットレイアウト
		static std::vector<InputLayoutParam>INPUT_LAYOUT =
		{
			InputLayoutParam("POSITION",DXGI_FORMAT_R32G32_FLOAT),
			InputLayoutParam("TEXCOORD",DXGI_FORMAT_R32G32_FLOAT),
			InputLayoutParam("VELOCITY",DXGI_FORMAT_R32G32_FLOAT),
			InputLayoutParam("FORCE",DXGI_FORMAT_R32G32_FLOAT),
		};

		//ルートパラメータ
		static std::vector<RootParam>ROOT_PARAMETER =
		{
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"平行投影行列定数バッファ"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"ズームとスクロール"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"アクティブ中のライト数バッファ"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"ディレクションライト情報 (構造化バッファ)"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"ポイントライト情報 (構造化バッファ)"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"スポットライト情報 (構造化バッファ)"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"天球ライト情報 (構造化バッファ)"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"テクスチャ情報"),
		};

		//レンダーターゲット描画先情報
		static std::vector<RenderTargetInfo>RENDER_TARGET_INFO =
		{
			//バックバッファのフォーマット、アルファブレンド
			RenderTargetInfo(D3D12App::Instance()->GetBackBuffFormat(),AlphaBlendMode_Trans),
			//エミッシブ
			RenderTargetInfo(DXGI_FORMAT_R32G32B32A32_FLOAT,AlphaBlendMode_Trans)
		};

		//パイプライン生成
		gPipeline = D3D12App::Instance()->GenerateGraphicsPipeline(PIPELINE_OPTION, SHADERS, INPUT_LAYOUT, ROOT_PARAMETER, RENDER_TARGET_INFO);
	}
	zoomAndScroll = D3D12App::Instance()->GenerateConstantBuffer(sizeof(ZoomAndScroll), 1, nullptr, "Muffler - ZoomAndScroll");
}

void Muffler::Init(const Vec2<float>& InitPos)
{
	Vec2<float>vertexOffset = { MUFFLER_SIZE.x / VERTEX_NUM_X,MUFFLER_SIZE.y / VERTEX_NUM_Y };
	auto initPos = InitPos;
	initPos.x -= MUFFLER_SIZE.x / 2.0f;

	for (int y = 0; y < VERTEX_NUM_Y; ++y)
	{
		for (int x = 0; x < VERTEX_NUM_X; ++x)
		{
			vertices[y][x].vel = { 0.0f,0.0f };
			vertices[y][x].force = { 0.0f,0.0f };
			vertices[y][x].pos = initPos + vertexOffset * Vec2<float>(x, y) + OFFSET_FROM_PLAYER_POS;
		}
	}
	for (int x = 0; x < VERTEX_NUM_X; ++x)vertices[0][x].pos = InitPos + OFFSET_FROM_PLAYER_POS;
	vertBuff->Mapping(&vertices[0][0]);
}

void Muffler::Update(const Vec2<float>& PlayerPos)
{
	// Muffler's starting point.
	static Vec2<float>mufflerBasePos = { 0.0f,0.0f };
	if (mufflerBasePos != PlayerPos + OFFSET_FROM_PLAYER_POS)
	{
		mufflerBasePos = PlayerPos + OFFSET_FROM_PLAYER_POS;
		playerPosBuff->Mapping(&PlayerPos);
	}

	D3D12App::Instance()->SetDescHeaps();
	auto cmdList = D3D12App::Instance()->GetCmdList();
	cPipeline->SetPipeline(cmdList);
	vertBuff->GetRWStructuredBuff().lock()->SetComputeDescriptorBuffer(cmdList, UAV, 0);
	playerPosBuff->SetComputeDescriptorBuffer(cmdList, CBV, 1);
	mufflerInfo->SetComputeDescriptorBuffer(cmdList, CBV, 2);

	for (int i = 0; i < UPDATE_PER_FRAME; ++i)
	{
		//static const int THREAD_NUM = 8;
		//cmdList->Dispatch(VERTEX_NUM_Y / THREAD_NUM, VERTEX_NUM_X / THREAD_NUM, 1);
		cmdList->Dispatch(VERTEX_NUM_Y, VERTEX_NUM_X, 1);
	}
}

#include"ScrollMgr.h"
#include"ShakeMgr.h"
#include"LightManager.h"
void Muffler::Draw(LightManager& LigManager)
{
	Vec2<float> scrollShakeZoom = ScrollMgr::Instance()->scrollAmount + ShakeMgr::Instance()->shakeAmount;
	scrollShakeZoom.x *= ScrollMgr::Instance()->zoom;
	scrollShakeZoom.y *= ScrollMgr::Instance()->zoom;

	static ZoomAndScroll ZOOM_AND_SCROLL;
	if (ZOOM_AND_SCROLL.zoom != ScrollMgr::Instance()->zoom || ZOOM_AND_SCROLL.scroll != scrollShakeZoom)
	{
		ZOOM_AND_SCROLL.zoom = ScrollMgr::Instance()->zoom;
		ZOOM_AND_SCROLL.scroll = scrollShakeZoom;
		zoomAndScroll->Mapping(&ZOOM_AND_SCROLL);
	}

	static std::vector<DESC_HANDLE_TYPE>descTypes = { CBV,CBV,CBV,SRV,SRV,SRV,SRV,SRV };
	std::vector<std::shared_ptr<DescriptorData>>descDatas =
	{
		KuroEngine::Instance().GetParallelMatProjBuff(),
		zoomAndScroll,
		LigManager.GetLigNumInfo(),
		LigManager.GetLigInfo(Light::DIRECTION),	//ディレクションライト
		LigManager.GetLigInfo(Light::POINT),	//ポイントライト
		LigManager.GetLigInfo(Light::SPOT),	//スポットライト
		LigManager.GetLigInfo(Light::HEMISPHERE),	//天球ライト
		tex
	};

	KuroEngine::Instance().Graphics().SetPipeline(gPipeline);
	KuroEngine::Instance().Graphics().ObjectRender(vertBuff, idxBuff, descDatas, descTypes, 0.0f, true);
}
