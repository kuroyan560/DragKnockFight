#include "DrawMap.h"
#include"KuroEngine.h"
#include"LightManager.h"

std::shared_ptr<GraphicsPipeline>DrawMap::PIPELINE;
std::shared_ptr<ConstantBuffer>DrawMap::EXT_RATE_BUFF;
std::shared_ptr<TextureBuffer>DrawMap::DEFAULT_NORMAL_MAP;
std::shared_ptr<TextureBuffer>DrawMap::DEFAULT_EMISSIVE_MAP;

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
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"アクティブ中のライト数バッファ"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"ディレクションライト情報 (構造化バッファ)"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"ポイントライト情報 (構造化バッファ)"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"スポットライト情報 (構造化バッファ)"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"天球ライト情報 (構造化バッファ)"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"テクスチャリソース"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"ノーマルマップシェーダーリソース"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"エミッシブマップシェーダーリソース")
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
		PIPELINE = D3D12App::Instance()->GenerateGraphicsPipeline(PIPELINE_OPTION, SHADERS, INPUT_LAYOUT, ROOT_PARAMETER, RENDER_TARGET_INFO);

		EXT_RATE_BUFF = D3D12App::Instance()->GenerateConstantBuffer(sizeof(float), 1, nullptr, "DrawMap - ExtRate");

		// (0,0,-1) ノーマルマップ
		DEFAULT_NORMAL_MAP = D3D12App::Instance()->GenerateTextureBuffer(Color(0.5f, 0.5f, 1.0f, 1.0f));

		//黒テクスチャ
		DEFAULT_EMISSIVE_MAP = D3D12App::Instance()->GenerateTextureBuffer(Color(0.0f, 0.0f, 0.0f, 1.0f));
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

#include"ScrollMgr.h"
void DrawMap::Draw(LightManager& LigManager, const std::shared_ptr<TextureBuffer>& Tex, const std::shared_ptr<TextureBuffer>& NormalMap, const std::shared_ptr<TextureBuffer>& EmissiveMap)
{
	KuroEngine::Instance().Graphics().SetPipeline(PIPELINE);

	vertexBuff->sendVertexNum = chipNum;
	vertexBuff->Mapping(&chipDatas[0]);

	auto normalMap = NormalMap ? NormalMap : DEFAULT_NORMAL_MAP;
	auto emissiveMap = EmissiveMap ? EmissiveMap : DEFAULT_EMISSIVE_MAP;

	float zoom = ScrollMgr::Instance()->zoom * 1.6f;
	EXT_RATE_BUFF->Mapping(&zoom);

	KuroEngine::Instance().Graphics().ObjectRender(vertexBuff,
		{
			KuroEngine::Instance().GetParallelMatProjBuff(),
			EXT_RATE_BUFF,
			LigManager.GetLigNumInfo(),	//アクティブ中のライト数
			LigManager.GetLigInfo(Light::DIRECTION),	//ディレクションライト
			LigManager.GetLigInfo(Light::POINT),	//ポイントライト
			LigManager.GetLigInfo(Light::SPOT),	//スポットライト
			LigManager.GetLigInfo(Light::HEMISPHERE),	//天球ライト
			Tex,normalMap,emissiveMap
		}, { CBV,CBV,CBV,SRV,SRV,SRV,SRV,SRV,SRV,SRV }, 0.0f, true);

	chipNum = 0;
}
