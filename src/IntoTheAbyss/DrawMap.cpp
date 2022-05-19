#include "DrawMap.h"
#include"KuroEngine.h"
#include"LightManager.h"

std::shared_ptr<GraphicsPipeline>DrawMap::PIPELINE_BASE;
std::shared_ptr<GraphicsPipeline>DrawMap::PIPELINE_SHOCKED;
std::shared_ptr<ConstantBuffer>DrawMap::EXT_RATE_BUFF;

DrawMap::DrawMap()
{
	if (!PIPELINE_BASE)
	{
		//�p�C�v���C���ݒ�
		static PipelineInitializeOption PIPELINE_OPTION(D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT, D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
		PIPELINE_OPTION.depthTest = false;

		//�V�F�[�_�[���
		static Shaders SHADERS;
		SHADERS.vs = D3D12App::Instance()->CompileShader("resource/HLSL/DrawMap.hlsl", "VSmain_Base", "vs_5_0");
		SHADERS.gs = D3D12App::Instance()->CompileShader("resource/HLSL/DrawMap.hlsl", "GSmain_Base", "gs_5_0");
		SHADERS.ps = D3D12App::Instance()->CompileShader("resource/HLSL/DrawMap.hlsl", "PSmain_Base", "ps_5_0");

		//�C���v�b�g���C�A�E�g
		static std::vector<InputLayoutParam>INPUT_LAYOUT =
		{
			InputLayoutParam("POSITION",DXGI_FORMAT_R32G32_FLOAT),
			InputLayoutParam("RADIAN",DXGI_FORMAT_R32_FLOAT),
			InputLayoutParam("SHOCKED",DXGI_FORMAT_R32_FLOAT),
		};

		//���[�g�p�����[�^
		static std::vector<RootParam>ROOT_PARAMETER =
		{
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"���s���e�s��萔�o�b�t�@"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"�g�嗦"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"�e�N�X�`�����\�[�X"),
		};

		//�����_�[�^�[�Q�b�g�`�����
		static std::vector<RenderTargetInfo>RENDER_TARGET_INFO =
		{
			//�o�b�N�o�b�t�@�̃t�H�[�}�b�g�A�A���t�@�u�����h
			RenderTargetInfo(D3D12App::Instance()->GetBackBuffFormat(),AlphaBlendMode_Trans)
		};

		//�p�C�v���C������
		PIPELINE_BASE = D3D12App::Instance()->GenerateGraphicsPipeline(PIPELINE_OPTION, SHADERS, INPUT_LAYOUT, ROOT_PARAMETER, RENDER_TARGET_INFO, WrappedSampler(false, false));
		
		SHADERS.gs = D3D12App::Instance()->CompileShader("resource/HLSL/DrawMap.hlsl", "GSmain_Shocked", "gs_5_0");
		SHADERS.ps = D3D12App::Instance()->CompileShader("resource/HLSL/DrawMap.hlsl", "PSmain_Shocked", "ps_5_0");
		RENDER_TARGET_INFO[0] = RenderTargetInfo(D3D12App::Instance()->GetBackBuffFormat(), AlphaBlendMode_Add);
		PIPELINE_SHOCKED = D3D12App::Instance()->GenerateGraphicsPipeline(PIPELINE_OPTION, SHADERS, INPUT_LAYOUT, ROOT_PARAMETER, RENDER_TARGET_INFO, WrappedSampler(false, false));

		EXT_RATE_BUFF = D3D12App::Instance()->GenerateConstantBuffer(sizeof(float), 1, nullptr, "DrawMap - ExtRate");
	}
	vertexBuff = D3D12App::Instance()->GenerateVertexBuffer(sizeof(ChipData), MAX_CHIP_NUM, nullptr, "DrawMap - ChipNum");
}

void DrawMap::AddChip(const ChipData& Data)
{
	if (MAX_CHIP_NUM <= chipNum + 1)
	{
		assert(0);
	}
	chipDatas[chipNum] = Data;
	chipNum++;
}

#include"ScrollMgr.h"
void DrawMap::Draw(const std::shared_ptr<TextureBuffer>& Tex)
{

	vertexBuff->sendVertexNum = chipNum;
	vertexBuff->Mapping(&chipDatas[0]);

	float zoom = ScrollMgr::Instance()->zoom * 1.6f;
	EXT_RATE_BUFF->Mapping(&zoom);

	KuroEngine::Instance().Graphics().SetPipeline(PIPELINE_BASE);
	KuroEngine::Instance().Graphics().ObjectRender(vertexBuff,
		{
			KuroEngine::Instance().GetParallelMatProjBuff(),
			EXT_RATE_BUFF,
			Tex
		}, { CBV,CBV,SRV }, 0.0f, true);

	KuroEngine::Instance().Graphics().SetPipeline(PIPELINE_SHOCKED);
	KuroEngine::Instance().Graphics().ObjectRender(vertexBuff,
		{
			KuroEngine::Instance().GetParallelMatProjBuff(),
			EXT_RATE_BUFF,
			Tex
		}, { CBV,CBV,SRV }, 0.0f, true);

	chipNum = 0;
}
