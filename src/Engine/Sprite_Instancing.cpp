#include "Sprite_Instancing.h"
#include"KuroEngine.h"

std::shared_ptr<GraphicsPipeline>Sprite_Instancing::PIPELINE[AlphaBlendModeNum];
std::shared_ptr<TextureBuffer>Sprite_Instancing::DEFAULT_TEX;

Sprite_Instancing::Sprite_Instancing(const std::shared_ptr<TextureBuffer>& Texture, const char* Name)
{
	if (!PIPELINE[0])
	{
		//�p�C�v���C���ݒ�
		static PipelineInitializeOption PIPELINE_OPTION(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		PIPELINE_OPTION.depthTest = false;

		//�V�F�[�_�[���
		static Shaders SHADERS;
		SHADERS.vs = D3D12App::Instance()->CompileShader("resource/HLSL/Engine/Sprite_Instancing.hlsl", "VSmain", "vs_5_0");
		SHADERS.ps = D3D12App::Instance()->CompileShader("resource/HLSL/Engine/Sprite_Instancing.hlsl", "PSmain", "ps_5_0");

		//�C���v�b�g���C�A�E�g
		static std::vector<InputLayoutParam>INPUT_LAYOUT =
		{
			InputLayoutParam("POSITION",DXGI_FORMAT_R32G32_FLOAT),
			InputLayoutParam("TEXCOORD",DXGI_FORMAT_R32G32_FLOAT)
		};

		//���[�g�p�����[�^
		static std::vector<RootParam>ROOT_PARAMETER =
		{
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"���s���e�s��萔�o�b�t�@"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"�e�N�X�`���V�F�[�_�[���\�[�X"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"�C���X�^���V���O���z��̍\�����o�b�t�@",INTANCING_NUM_MAX)
		};

		//�����_�[�^�[�Q�b�g�`�����
		for (int i = 0; i < AlphaBlendModeNum; ++i)
		{
			std::vector<RenderTargetInfo>RENDER_TARGET_INFO = { RenderTargetInfo(D3D12App::Instance()->GetBackBuffFormat(), (AlphaBlendMode)i) };
			//�p�C�v���C������
			PIPELINE[i] = D3D12App::Instance()->GenerateGraphicsPipeline(PIPELINE_OPTION, SHADERS, INPUT_LAYOUT, ROOT_PARAMETER, RENDER_TARGET_INFO);
		}

		//���e�N�X�`��
		DEFAULT_TEX = D3D12App::Instance()->GenerateTextureBuffer(Color(1.0f, 1.0f, 1.0f, 1.0f));
	}

	//�f�t�H���g�̃e�N�X�`���o�b�t�@
	texBuff = DEFAULT_TEX;

	//�e�N�X�`���Z�b�g
	SetTexture(Texture);

	//�\�����o�b�t�@����
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
