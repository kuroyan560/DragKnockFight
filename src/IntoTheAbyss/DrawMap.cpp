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
		//�p�C�v���C���ݒ�
		static PipelineInitializeOption PIPELINE_OPTION(D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT, D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
		PIPELINE_OPTION.depthTest = false;

		//�V�F�[�_�[���
		static Shaders SHADERS;
		SHADERS.vs = D3D12App::Instance()->CompileShader("resource/HLSL/DrawMap.hlsl", "VSmain", "vs_5_0");
		SHADERS.gs = D3D12App::Instance()->CompileShader("resource/HLSL/DrawMap.hlsl", "GSmain", "gs_5_0");
		SHADERS.ps = D3D12App::Instance()->CompileShader("resource/HLSL/DrawMap.hlsl", "PSmain", "ps_5_0");

		//�C���v�b�g���C�A�E�g
		static std::vector<InputLayoutParam>INPUT_LAYOUT =
		{
			InputLayoutParam("POSITION",DXGI_FORMAT_R32G32_FLOAT),
			InputLayoutParam("RADIAN",DXGI_FORMAT_R32_FLOAT)
		};

		//���[�g�p�����[�^
		static std::vector<RootParam>ROOT_PARAMETER =
		{
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"���s���e�s��萔�o�b�t�@"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"�g�嗦"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"�A�N�e�B�u���̃��C�g���o�b�t�@"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"�f�B���N�V�������C�g��� (�\�����o�b�t�@)"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"�|�C���g���C�g��� (�\�����o�b�t�@)"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"�X�|�b�g���C�g��� (�\�����o�b�t�@)"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"�V�����C�g��� (�\�����o�b�t�@)"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"�e�N�X�`�����\�[�X"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"�m�[�}���}�b�v�V�F�[�_�[���\�[�X"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"�G�~�b�V�u�}�b�v�V�F�[�_�[���\�[�X")
		};

		//�����_�[�^�[�Q�b�g�`�����
		static std::vector<RenderTargetInfo>RENDER_TARGET_INFO =
		{
			//�o�b�N�o�b�t�@�̃t�H�[�}�b�g�A�A���t�@�u�����h
			RenderTargetInfo(D3D12App::Instance()->GetBackBuffFormat(),AlphaBlendMode_Trans),
			//�G�~�b�V�u
			RenderTargetInfo(DXGI_FORMAT_R32G32B32A32_FLOAT,AlphaBlendMode_Trans)
		};

		//�p�C�v���C������
		PIPELINE = D3D12App::Instance()->GenerateGraphicsPipeline(PIPELINE_OPTION, SHADERS, INPUT_LAYOUT, ROOT_PARAMETER, RENDER_TARGET_INFO);

		EXT_RATE_BUFF = D3D12App::Instance()->GenerateConstantBuffer(sizeof(float), 1, nullptr, "DrawMap - ExtRate");

		// (0,0,-1) �m�[�}���}�b�v
		DEFAULT_NORMAL_MAP = D3D12App::Instance()->GenerateTextureBuffer(Color(0.5f, 0.5f, 1.0f, 1.0f));

		//���e�N�X�`��
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
			LigManager.GetLigNumInfo(),	//�A�N�e�B�u���̃��C�g��
			LigManager.GetLigInfo(Light::DIRECTION),	//�f�B���N�V�������C�g
			LigManager.GetLigInfo(Light::POINT),	//�|�C���g���C�g
			LigManager.GetLigInfo(Light::SPOT),	//�X�|�b�g���C�g
			LigManager.GetLigInfo(Light::HEMISPHERE),	//�V�����C�g
			Tex,normalMap,emissiveMap
		}, { CBV,CBV,CBV,SRV,SRV,SRV,SRV,SRV,SRV,SRV }, 0.0f, true);

	chipNum = 0;
}
