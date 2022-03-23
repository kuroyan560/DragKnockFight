#include "DrawFunc_Color.h"
#include"KuroEngine.h"

//DrawExtendGraph
std::shared_ptr<GraphicsPipeline>DrawFunc_Color::EXTEND_GRAPH_PIPELINE;
int DrawFunc_Color::DRAW_EXTEND_GRAPH_COUNT = 0;
std::vector<std::shared_ptr<VertexBuffer>>DrawFunc_Color::EXTEND_GRAPH_VERTEX_BUFF;

//DrawRotaGraph
std::shared_ptr<GraphicsPipeline>DrawFunc_Color::ROTA_GRAPH_PIPELINE;
int DrawFunc_Color::DRAW_ROTA_GRAPH_COUNT = 0;
std::vector<std::shared_ptr<VertexBuffer>>DrawFunc_Color::ROTA_GRAPH_VERTEX_BUFF;

static std::vector<RootParam>ROOT_PARAMETER =
{
	RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, "平行投影行列定数バッファ"),
	RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, "テクスチャリソース"),
};

void DrawFunc_Color::DrawExtendGraph2D(const Vec2<float>& LeftUpPos, const Vec2<float>& RightBottomPos, const std::shared_ptr<TextureBuffer>& Tex, const Color& Paint)
{
	//DrawExtendGraph専用頂点
	class ExtendGraphVertex
	{
	public:
		Vec2<float>leftUpPos;
		Vec2<float>rightBottomPos;
		Color paintColor;
		ExtendGraphVertex(const Vec2<float>& LeftUpPos, const Vec2<float>& RightBottomPos, const Color& Paint)
			:leftUpPos(LeftUpPos), rightBottomPos(RightBottomPos), paintColor(Paint) {}
	};

	//パイプライン未生成
	if (!EXTEND_GRAPH_PIPELINE)
	{
		//パイプライン設定
		static PipelineInitializeOption PIPELINE_OPTION(D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT, D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
		PIPELINE_OPTION.depthTest = false;

		//シェーダー情報
		static Shaders SHADERS;
		SHADERS.vs = D3D12App::Instance()->CompileShader("resource/HLSL/Engine/DrawExtendGraph_Color.hlsl", "VSmain", "vs_5_0");
		SHADERS.gs = D3D12App::Instance()->CompileShader("resource/HLSL/Engine/DrawExtendGraph_Color.hlsl", "GSmain", "gs_5_0");
		SHADERS.ps = D3D12App::Instance()->CompileShader("resource/HLSL/Engine/DrawExtendGraph_Color.hlsl", "PSmain", "ps_5_0");

		//インプットレイアウト
		static std::vector<InputLayoutParam>INPUT_LAYOUT =
		{
			InputLayoutParam("POSITION_L_U",DXGI_FORMAT_R32G32_FLOAT),
			InputLayoutParam("POSITION_R_B",DXGI_FORMAT_R32G32_FLOAT),
			InputLayoutParam("PAINT_COLOR",DXGI_FORMAT_R32G32B32A32_FLOAT)
		};

		//レンダーターゲット描画先情報
		std::vector<RenderTargetInfo>RENDER_TARGET_INFO = { RenderTargetInfo(D3D12App::Instance()->GetBackBuffFormat(), AlphaBlendMode_Trans) };
		//パイプライン生成
		EXTEND_GRAPH_PIPELINE = D3D12App::Instance()->GenerateGraphicsPipeline(PIPELINE_OPTION, SHADERS, INPUT_LAYOUT, ROOT_PARAMETER, RENDER_TARGET_INFO);
	}

	KuroEngine::Instance().Graphics().SetPipeline(EXTEND_GRAPH_PIPELINE);

	if (EXTEND_GRAPH_VERTEX_BUFF.size() < (DRAW_EXTEND_GRAPH_COUNT + 1))
	{
		EXTEND_GRAPH_VERTEX_BUFF.emplace_back(D3D12App::Instance()->GenerateVertexBuffer(sizeof(ExtendGraphVertex), 1, nullptr, ("DrawExtendGraph_Color -" + std::to_string(DRAW_EXTEND_GRAPH_COUNT)).c_str()));
	}

	ExtendGraphVertex vertex(LeftUpPos, RightBottomPos, Paint);
	EXTEND_GRAPH_VERTEX_BUFF[DRAW_EXTEND_GRAPH_COUNT]->Mapping(&vertex);

	KuroEngine::Instance().Graphics().ObjectRender(EXTEND_GRAPH_VERTEX_BUFF[DRAW_EXTEND_GRAPH_COUNT], { KuroEngine::Instance().GetParallelMatProjBuff(),Tex }, { CBV,SRV }, 0.0f, true);

	DRAW_EXTEND_GRAPH_COUNT++;
}

void DrawFunc_Color::DrawRotaGraph2D(const Vec2<float>& Center, const Vec2<float>& ExtRate, const float& Radian, const std::shared_ptr<TextureBuffer>& Tex, const Color& Paint, const Vec2<float>& RotaCenterUV)
{
	//DrawRotaGraph専用頂点
	class RotaGraphVertex
	{
	public:
		Vec2<float>center;
		Vec2<float> extRate;
		float radian;
		Vec2<float>rotaCenterUV;
		Color paintColor;
		RotaGraphVertex(const Vec2<float>& Center, const Vec2<float>& ExtRate, const float& Radian,
			const Vec2<float>& RotaCenterUV, const Color& Paint)
			:center(Center), extRate(ExtRate), radian(Radian), rotaCenterUV(RotaCenterUV), paintColor(Paint) {}
	};

	//パイプライン未生成
	if (!ROTA_GRAPH_PIPELINE)
	{
		//パイプライン設定
		static PipelineInitializeOption PIPELINE_OPTION(D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT, D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
		PIPELINE_OPTION.depthTest = false;

		//シェーダー情報
		static Shaders SHADERS;
		SHADERS.vs = D3D12App::Instance()->CompileShader("resource/HLSL/Engine/DrawRotaGraph_Color.hlsl", "VSmain", "vs_5_0");
		SHADERS.gs = D3D12App::Instance()->CompileShader("resource/HLSL/Engine/DrawRotaGraph_Color.hlsl", "GSmain", "gs_5_0");
		SHADERS.ps = D3D12App::Instance()->CompileShader("resource/HLSL/Engine/DrawRotaGraph_Color.hlsl", "PSmain", "ps_5_0");

		//インプットレイアウト
		static std::vector<InputLayoutParam>INPUT_LAYOUT =
		{
			InputLayoutParam("CENTER",DXGI_FORMAT_R32G32_FLOAT),
			InputLayoutParam("EXT_RATE",DXGI_FORMAT_R32G32_FLOAT),
			InputLayoutParam("RADIAN",DXGI_FORMAT_R32_FLOAT),
			InputLayoutParam("ROTA_CENTER_UV",DXGI_FORMAT_R32G32_FLOAT),
			InputLayoutParam("PAINT_COLOR",DXGI_FORMAT_R32G32B32A32_FLOAT)
		};

		//レンダーターゲット描画先情報
		std::vector<RenderTargetInfo>RENDER_TARGET_INFO = { RenderTargetInfo(D3D12App::Instance()->GetBackBuffFormat(), AlphaBlendMode_Trans) };
		//パイプライン生成
		ROTA_GRAPH_PIPELINE = D3D12App::Instance()->GenerateGraphicsPipeline(PIPELINE_OPTION, SHADERS, INPUT_LAYOUT, ROOT_PARAMETER, RENDER_TARGET_INFO);
	}

	KuroEngine::Instance().Graphics().SetPipeline(ROTA_GRAPH_PIPELINE);

	if (ROTA_GRAPH_VERTEX_BUFF.size() < (DRAW_ROTA_GRAPH_COUNT + 1))
	{
		ROTA_GRAPH_VERTEX_BUFF.emplace_back(D3D12App::Instance()->GenerateVertexBuffer(sizeof(RotaGraphVertex), 1, nullptr, ("DrawRotaGraph_Color -" + std::to_string(DRAW_ROTA_GRAPH_COUNT)).c_str()));
	}

	RotaGraphVertex vertex(Center, ExtRate, Radian, RotaCenterUV, Paint);
	ROTA_GRAPH_VERTEX_BUFF[DRAW_ROTA_GRAPH_COUNT]->Mapping(&vertex);

	KuroEngine::Instance().Graphics().ObjectRender(ROTA_GRAPH_VERTEX_BUFF[DRAW_ROTA_GRAPH_COUNT], { KuroEngine::Instance().GetParallelMatProjBuff(),Tex }, { CBV,SRV }, 0.0f, true);

	DRAW_ROTA_GRAPH_COUNT++;
}
