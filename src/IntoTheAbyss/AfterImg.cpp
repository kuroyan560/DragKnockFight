#include "AfterImg.h"
#include"KuroEngine.h"
#include"TexHandleMgr.h"
#include"KuroMath.h"

std::shared_ptr<GraphicsPipeline>AfterImg::PIPELINE;
const float AfterImg::MAX_ALPHA = 0.1f;

AfterImg::AfterImg()
{
	//パイプライン未生成
	if (!PIPELINE)
	{
		//パイプライン設定
		static PipelineInitializeOption PIPELINE_OPTION(D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT, D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
		PIPELINE_OPTION.depthTest = false;

		//シェーダー情報
		static Shaders SHADERS;
		SHADERS.vs = D3D12App::Instance()->CompileShader("resource/HLSL/AfterImg.hlsl", "VSmain", "vs_5_0");
		SHADERS.gs = D3D12App::Instance()->CompileShader("resource/HLSL/AfterImg.hlsl", "GSmain", "gs_5_0");
		SHADERS.ps = D3D12App::Instance()->CompileShader("resource/HLSL/AfterImg.hlsl", "PSmain", "ps_5_0");

		//インプットレイアウト
		static std::vector<InputLayoutParam>INPUT_LAYOUT =
		{
			InputLayoutParam("POSITION_L_U",DXGI_FORMAT_R32G32_FLOAT),
			InputLayoutParam("POSITION_R_B",DXGI_FORMAT_R32G32_FLOAT),
			InputLayoutParam("MIROR",DXGI_FORMAT_R32G32_SINT),
			InputLayoutParam("SRV_IDX",DXGI_FORMAT_R32_SINT),
			InputLayoutParam("ALPHA",DXGI_FORMAT_R32_FLOAT),
			InputLayoutParam("SCALE",DXGI_FORMAT_R32_FLOAT)
		};

		//ルートパラメータ
		static std::vector<RootParam>ROOT_PARAMETER =
		{
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"平行投影行列定数バッファ"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"テクスチャリソース_1"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"テクスチャリソース_2"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"テクスチャリソース_3"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"テクスチャリソース_4"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"テクスチャリソース_5"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"テクスチャリソース_6"),
		};

		//レンダーターゲット描画先情報
		std::vector<RenderTargetInfo>RENDER_TARGET_INFO = { RenderTargetInfo(D3D12App::Instance()->GetBackBuffFormat(), AlphaBlendMode_Trans) };

		//パイプライン生成
		PIPELINE = D3D12App::Instance()->GenerateGraphicsPipeline(PIPELINE_OPTION, SHADERS, INPUT_LAYOUT, ROOT_PARAMETER, RENDER_TARGET_INFO);
	}

	vertexBuff = D3D12App::Instance()->GenerateVertexBuffer(sizeof(Vertex), MAX_NUM, nullptr, "AfterImg");

	descDatas.resize(SRV_MAX_IDX + 1);
	descDatas[0] = KuroEngine::Instance().GetParallelMatProjBuff();
	descTypes.resize(SRV_MAX_IDX + 1, SRV);
	descTypes[0] = CBV;
}

void AfterImg::EmitArray(const Vec2<float>& From, const Vec2<float>& To, const int& GraphHandle, const Vec2<float>& GraphSize, const Vec2<bool>& Miror)
{
	static const int LIFE_SPAN_MIN = 10;
	static const int LIFE_SPAN_MAX = 60;
	//static const float SPAN_DIST = 32.0f;
	static const float SPAN_DIST = 10.0f;
	const int num = From.Distance(To) / SPAN_DIST;
	const Vec2<float>span = (To - From) / num;

	descDatas[srvIdx] = TexHandleMgr::GetTexBuffer(GraphHandle);
	for (int i = 0; i < num; ++i)
	{
		imgs.emplace_front(From + span * i, KuroMath::Liner(LIFE_SPAN_MIN, LIFE_SPAN_MAX, (float)i / num), srvIdx - 1, GraphSize, Miror);
	}

	srvIdx++;
	if (SRV_MAX_IDX < srvIdx)srvIdx = SRV_MIN_IDX;
}

void AfterImg::Draw(const float& ExtRate, const Vec2<float>& Scroll)
{
	for (auto itr = imgs.begin(); itr != imgs.end(); ++itr)
	{
		itr->alpha = KuroMath::Ease(Out, Quad, itr->life, itr->lifeSpan, MAX_ALPHA, 0.0f);
		//itr->scale = KuroMath::Ease(Out, Quad, itr->life, itr->lifeSpan, 1.0f, 0.0f);
		itr->life++;
	}

	imgs.remove_if([](const Img& x)
		{
			return x.lifeSpan <= x.life;
		});

	if (imgs.empty())return;

	std::vector<Vertex>vertices;
	for (auto itr = imgs.begin(); itr != imgs.end(); ++itr)
	{
		const Vec2<float>offset = (itr->graphSize * ExtRate).Float();
		const Vec2<float>leftUp = (itr->pos * ExtRate) - offset - Scroll;
		const Vec2<float>rightBottom = (itr->pos * ExtRate) + offset - Scroll;
		vertices.emplace_back(leftUp, rightBottom, itr->miror, itr->handle, itr->alpha, itr->scale);
	}

	KuroEngine::Instance().Graphics().SetPipeline(PIPELINE);

	vertexBuff->sendVertexNum = std::distance(imgs.begin(), imgs.end());
	vertexBuff->Mapping(&vertices[0]);

	KuroEngine::Instance().Graphics().ObjectRender(vertexBuff, descDatas, descTypes, 0.0f, true);
}
