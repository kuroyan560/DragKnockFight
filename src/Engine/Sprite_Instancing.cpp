#include "Sprite_Instancing.h"
#include"KuroEngine.h"

std::shared_ptr<GraphicsPipeline>Sprite_Instancing::PIPELINE[AlphaBlendModeNum];
std::shared_ptr<TextureBuffer>Sprite_Instancing::DEFAULT_TEX;

Sprite_Instancing::Sprite_Instancing(const std::shared_ptr<TextureBuffer>& Texture, const char* Name)
{
	if (!PIPELINE[0])
	{
		//パイプライン設定
		static PipelineInitializeOption PIPELINE_OPTION(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		PIPELINE_OPTION.depthTest = false;

		//シェーダー情報
		static Shaders SHADERS;
		SHADERS.vs = D3D12App::Instance()->CompileShader("resource/HLSL/Engine/Sprite_Instancing.hlsl", "VSmain", "vs_5_0");
		SHADERS.ps = D3D12App::Instance()->CompileShader("resource/HLSL/Engine/Sprite_Instancing.hlsl", "PSmain", "ps_5_0");

		//インプットレイアウト
		static std::vector<InputLayoutParam>INPUT_LAYOUT =
		{
			InputLayoutParam("POSITION",DXGI_FORMAT_R32G32_FLOAT),
			InputLayoutParam("TEXCOORD",DXGI_FORMAT_R32G32_FLOAT)
		};

		//ルートパラメータ
		static std::vector<RootParam>ROOT_PARAMETER =
		{
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"平行投影行列定数バッファ"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"テクスチャシェーダーリソース"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"インスタンシング情報配列の構造化バッファ",INTANCING_NUM_MAX)
		};

		//レンダーターゲット描画先情報
		for (int i = 0; i < AlphaBlendModeNum; ++i)
		{
			std::vector<RenderTargetInfo>RENDER_TARGET_INFO = { RenderTargetInfo(D3D12App::Instance()->GetBackBuffFormat(), (AlphaBlendMode)i) };
			//パイプライン生成
			PIPELINE[i] = D3D12App::Instance()->GenerateGraphicsPipeline(PIPELINE_OPTION, SHADERS, INPUT_LAYOUT, ROOT_PARAMETER, RENDER_TARGET_INFO);
		}

		//白テクスチャ
		DEFAULT_TEX = D3D12App::Instance()->GenerateTextureBuffer(Color(1.0f, 1.0f, 1.0f, 1.0f));
	}

	//デフォルトのテクスチャバッファ
	texBuff = DEFAULT_TEX;

	//テクスチャセット
	SetTexture(Texture);

	//構造化バッファ生成
	structuredBuff = D3D12App::Instance()->GenerateStructuredBuffer(sizeof(StructuredData), INTANCING_NUM_MAX, nullptr, Name);
}

void Sprite_Instancing::SetTexture(const std::shared_ptr<TextureBuffer>& Texture)
{
	if (Texture == nullptr)return;
	texBuff = Texture;
}

void Sprite_Instancing::Draw(const AlphaBlendMode& BlendMode)
{
	KuroEngine::Instance().Graphics().SetPipeline(PIPELINE[(int)BlendMode]);

	StructuredData* ptr = (StructuredData*)structuredBuff->GetBuffOnCPU();
	for (int i = 0; i < instanceDatas.size(); ++i)
	{
		if (instanceDatas[i].colorDirty)
		{
			ptr[i].color = instanceDatas[i].color;
			instanceDatas[i].colorDirty = false;
		}
		if (instanceDatas[i].transform.GetDirty())
		{
			ptr[i].mat = *instanceDatas[i].transform.GetMat();
		}
	}

	mesh.Render({
		KuroEngine::Instance().GetParallelMatProjBuff(),
		texBuff,
		structuredBuff },
		{ CBV,SRV,SRV },
		instanceDatas.size());
}
