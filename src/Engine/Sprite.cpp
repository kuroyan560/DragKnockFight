#include "Sprite.h"
#include"KuroEngine.h"

std::shared_ptr<GraphicsPipeline>Sprite::PIPELINE[AlphaBlendModeNum];
std::shared_ptr<TextureBuffer>Sprite::DEFAULT_TEX;

Sprite::Sprite(const std::shared_ptr<TextureBuffer>& Texture, const char* Name) : mesh(Name)
{
	if (!PIPELINE[0])
	{
		//�p�C�v���C���ݒ�
		static PipelineInitializeOption PIPELINE_OPTION(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		PIPELINE_OPTION.depthTest = false;

		//�V�F�[�_�[���
		static Shaders SHADERS;
		SHADERS.vs = D3D12App::Instance()->CompileShader("resource/HLSL/Engine/Sprite.hlsl", "VSmain", "vs_5_0");
		SHADERS.ps = D3D12App::Instance()->CompileShader("resource/HLSL/Engine/Sprite.hlsl", "PSmain", "ps_5_0");

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
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"�J���[ & �s��萔�o�b�t�@")
		};

		//�����_�[�^�[�Q�b�g�`�����
		for (int i = 0; i < AlphaBlendModeNum; ++i)
		{
			std::vector<RenderTargetInfo>RENDER_TARGET_INFO = { RenderTargetInfo(D3D12App::Instance()->GetBackBuffFormat(), (AlphaBlendMode)i) };
			//�p�C�v���C������
			PIPELINE[i] = D3D12App::Instance()->GenerateGraphicsPipeline(PIPELINE_OPTION, SHADERS, INPUT_LAYOUT, ROOT_PARAMETER, RENDER_TARGET_INFO, WrappedSampler(true, false));
		}

		//���e�N�X�`��
		DEFAULT_TEX = D3D12App::Instance()->GenerateTextureBuffer(Color(1.0f, 1.0f, 1.0f, 1.0f));
	}

	//�f�t�H���g�̃e�N�X�`���o�b�t�@
	texBuff = DEFAULT_TEX;

	//�e�N�X�`���Z�b�g
	SetTexture(Texture);

	//�s�񏉊���
	constData.mat = *transform.GetMat();

	//�萔�o�b�t�@����
	constBuff = D3D12App::Instance()->GenerateConstantBuffer(sizeof(constData), 1, &constData, Name);
}

void Sprite::SetTexture(const std::shared_ptr<TextureBuffer>& Texture)
{
	if (Texture == nullptr)return;
	texBuff = Texture;
}

void Sprite::SetColor(const Color& Color)
{
	if (constData.color == Color)return;
	constData.color = Color;
	constBuff->Mapping(&constData);
}

void Sprite::Draw(const AlphaBlendMode& BlendMode)
{
	KuroEngine::Instance().Graphics().SetPipeline(PIPELINE[(int)BlendMode]);

	if (transform.GetDirty())
	{
		constData.mat = *transform.GetMat();
		constBuff->Mapping(&constData);
	}

	mesh.Render({
		KuroEngine::Instance().GetParallelMatProjBuff(),	//���s���e�s��
		texBuff,			//�e�N�X�`�����\�[�X
		constBuff },//�J���[ & ���[���h�s��
		{ CBV,SRV,CBV });
}