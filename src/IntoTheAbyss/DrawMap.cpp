#include "DrawMap.h"
#include"KuroEngine.h"

std::shared_ptr<GraphicsPipeline>DrawMap::PIPELINE;
std::shared_ptr<ConstantBuffer>DrawMap::EXT_RATE_BUFF;

DrawMap::DrawMap()
{
	if (!PIPELINE)
	{
		//パイプライン設定
		static PipelineInitializeOption PIPELINE_OPTION(D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT, D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
		PIPELINE_OPTION.depthTest = false;

		//シェーダー情報
		static Shaders SHADERS;
		SHADERS.vs = D3D12App::Instance()->CompileShader("resource/HLSL/DrawMap.hlsl", "VSmain", "vs_5_0");
		SHADERS.gs = D3D12App::Instance()->CompileShader("resource/HLSL/DrawMap.hlsl", "GSmain", "gs_5_0");
		SHADERS.ps = D3D12App::Instance()->CompileShader("resource/HLSL/DrawMap.hlsl", "PSmain", "ps_5_0");

		//インプットレイアウト
		static std::vector<InputLayoutParam>INPUT_LAYOUT =
		{
			InputLayoutParam("POSITION",DXGI_FORMAT_R32G32_FLOAT),
			InputLayoutParam("RADIAN",DXGI_FORMAT_R32_FLOAT)
		};

		//ルートパラメータ
		static std::vector<RootParam>ROOT_PARAMETER =
		{
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"平行投影行列定数バッファ"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"拡大率"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"テクスチャリソース"),
		};

		//レンダーターゲット描画先情報
		std::vector<RenderTargetInfo>RENDER_TARGET_INFO = { RenderTargetInfo(D3D12App::Instance()->GetBackBuffFormat(), AlphaBlendMode_Trans) };

		//パイプライン生成
		PIPELINE = D3D12App::Instance()->GenerateGraphicsPipeline(PIPELINE_OPTION, SHADERS, INPUT_LAYOUT, ROOT_PARAMETER, RENDER_TARGET_INFO);

		float initExtRate = 1.6f;
		EXT_RATE_BUFF = D3D12App::Instance()->GenerateConstantBuffer(sizeof(float), 1, &initExtRate, "DrawMap - ExtRate");
	}
	vertexBuff = D3D12App::Instance()->GenerateVertexBuffer(sizeof(ChipData), MAX_CHIP_NUM, nullptr, "DrawMap - ChipNum");
}

void DrawMap::AddChip(const Vec2<float>& Pos, const float& Radian)
{
	if (MAX_CHIP_NUM <= chipNum + 1)
	{
		assert(0);
	}
	chipDatas[chipNum].pos = Pos;
	chipDatas[chipNum].radian = Radian;
	chipNum++;
}

void DrawMap::Draw(const std::shared_ptr<TextureBuffer>& Tex)
{
	KuroEngine::Instance().Graphics().SetPipeline(PIPELINE);

	vertexBuff->sendVertexNum = chipNum;
	vertexBuff->Mapping(&chipDatas[0]);

	KuroEngine::Instance().Graphics().ObjectRender(vertexBuff, { KuroEngine::Instance().GetParallelMatProjBuff(),EXT_RATE_BUFF,Tex }, { CBV,CBV,SRV }, 0.0f, true);

	chipNum = 0;
}
