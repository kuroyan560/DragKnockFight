#include "Sprite_Shadow.h"
#include"KuroEngine.h"
#include"LightManager.h"

std::shared_ptr<GraphicsPipeline>Sprite_Shadow::PIPELINE_TRANS;
std::shared_ptr<ConstantBuffer>Sprite_Shadow::EYE_POS_BUFF;
std::shared_ptr<TextureBuffer>Sprite_Shadow::DEFAULT_TEX;
std::shared_ptr<TextureBuffer>Sprite_Shadow::DEFAULT_NORMAL_MAP;
std::shared_ptr<TextureBuffer>Sprite_Shadow::DEFAULT_EMISSIVE_MAP;

void Sprite_Shadow::SetEyePos(Vec3<float> EyePos)
{
	EYE_POS_BUFF->Mapping(&EyePos);
}

Sprite_Shadow::Sprite_Shadow(const std::shared_ptr<TextureBuffer>& Texture, const std::shared_ptr<TextureBuffer>& NormalMap, const std::shared_ptr<TextureBuffer>& EmissiveMap, const char* Name)
{
	//�ÓI�����o�̏�����
	if (!PIPELINE_TRANS)
	{
		//�p�C�v���C���ݒ�
		static PipelineInitializeOption PIPELINE_OPTION(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		PIPELINE_OPTION.depthTest = false;

		//�C���v�b�g���C�A�E�g
		static std::vector<InputLayoutParam>INPUT_LAYOUT =
		{
			InputLayoutParam("POSITION",DXGI_FORMAT_R32G32_FLOAT),
			InputLayoutParam("TEXCOORD",DXGI_FORMAT_R32G32_FLOAT)
		};

		//�����_�[�^�[�Q�b�g�`�����
		static std::vector<RenderTargetInfo>RENDER_TARGET_INFO =
		{
			//�o�b�N�o�b�t�@�̃t�H�[�}�b�g�A�A���t�@�u�����h
			RenderTargetInfo(D3D12App::Instance()->GetBackBuffFormat(),AlphaBlendMode_Trans),
			//�G�~�b�V�u
			RenderTargetInfo(DXGI_FORMAT_R32G32B32A32_FLOAT,AlphaBlendMode_Trans)
		};

		//�V�F�[�_�[���
		static Shaders SHADERS;
		SHADERS.vs = D3D12App::Instance()->CompileShader("resource/HLSL/Engine/Sprite_Shadow.hlsl", "VSmain", "vs_5_0");
		SHADERS.ps = D3D12App::Instance()->CompileShader("resource/HLSL/Engine/Sprite_Shadow.hlsl", "PSmain", "ps_5_0");

		//���[�g�p�����[�^
		static std::vector<RootParam>ROOT_PARAMETER =
		{
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"���s���e�s��萔�o�b�t�@"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"�J���[ & �s��萔 & �X�v���C�g��Z�ݒ�l�o�b�t�@"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"���_���W���"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"�A�N�e�B�u���̃��C�g���o�b�t�@"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"�f�B���N�V�������C�g��� (�\�����o�b�t�@)"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"�|�C���g���C�g��� (�\�����o�b�t�@)"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"�X�|�b�g���C�g��� (�\�����o�b�t�@)"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"�V�����C�g��� (�\�����o�b�t�@)"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"�e�N�X�`���V�F�[�_�[���\�[�X"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"�m�[�}���}�b�v�V�F�[�_�[���\�[�X"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"�G�~�b�V�u�}�b�v�V�F�[�_�[���\�[�X"),
		};

		//�p�C�v���C������
		PIPELINE_TRANS = D3D12App::Instance()->GenerateGraphicsPipeline(PIPELINE_OPTION, SHADERS, INPUT_LAYOUT, ROOT_PARAMETER, RENDER_TARGET_INFO, WrappedSampler(false, true));

		//���_���W
		Vec3<float>INIT_EYE_POS = { WinApp::Instance()->GetExpandWinCenter().x,WinApp::Instance()->GetExpandWinCenter().y,-5.0f };
		EYE_POS_BUFF = D3D12App::Instance()->GenerateConstantBuffer(sizeof(Vec3<float>), 1, &INIT_EYE_POS, "Sprite_Shadow - EYE_POS");

		//���e�N�X�`��
		DEFAULT_TEX = D3D12App::Instance()->GenerateTextureBuffer(Color(1.0f, 1.0f, 1.0f, 1.0f));

		// (0,0,-1) �m�[�}���}�b�v
		DEFAULT_NORMAL_MAP = D3D12App::Instance()->GenerateTextureBuffer(Color(0.5f, 0.5f, 1.0f, 1.0f));

		//���e�N�X�`��
		DEFAULT_EMISSIVE_MAP = D3D12App::Instance()->GenerateTextureBuffer(Color(0.0f, 0.0f, 0.0f, 1.0f));
	}

	//�f�t�H���g�̃e�N�X�`���o�b�t�@
	texBuff = DEFAULT_TEX;
	normalMap = DEFAULT_NORMAL_MAP;
	emissiveMap = DEFAULT_EMISSIVE_MAP;
	
	//�e�N�X�`���Z�b�g
	SetTexture(Texture, NormalMap, EmissiveMap);

	//�s�񏉊���
	constData.mat = *transform.GetMat();

	//�萔�o�b�t�@����
	constBuff = D3D12App::Instance()->GenerateConstantBuffer(sizeof(constData), 1, &constData, Name);
}

void Sprite_Shadow::SetTexture(const std::shared_ptr<TextureBuffer>& Texture, const std::shared_ptr<TextureBuffer>& NormalMap, const std::shared_ptr<TextureBuffer>& EmissiveMap)
{
	if (Texture != nullptr)
	{
		texBuff = Texture;
	}
	if (NormalMap != nullptr)
	{
		normalMap = NormalMap;
	}
	if (EmissiveMap != nullptr)
	{
		emissiveMap = EmissiveMap;
	}
}

void Sprite_Shadow::SetColor(const Color& Color)
{
	if (constData.color == Color)return;
	constData.color = Color;
	constBuff->Mapping(&constData);
}

void Sprite_Shadow::SetDepth(const float& Depth)
{
	if (constData.z == Depth)return;
	constData.z = Depth;
	constBuff->Mapping(&constData);
}

void Sprite_Shadow::SetDiffuseAffect(const float& Diffuse)
{
	if (constData.diffuse == Diffuse)return;
	constData.diffuse = Diffuse;
	constBuff->Mapping(&constData);
}

void Sprite_Shadow::SetSpecularAffect(const float& Specular)
{
	if (constData.specular == Specular)return;
	constData.specular = Specular;
	constBuff->Mapping(&constData);
}

void Sprite_Shadow::SetLimAffect(const float& Lim)
{
	if (constData.lim == Lim)return;
	constData.lim = Lim;
	constBuff->Mapping(&constData);
}

void Sprite_Shadow::Draw(LightManager& LigManager)
{
	KuroEngine::Instance().Graphics().SetPipeline(PIPELINE_TRANS);

	if (transform.GetDirty())
	{
		constData.mat = *transform.GetMat();
		constBuff->Mapping(&constData);
	}

	mesh.Render({
		KuroEngine::Instance().GetParallelMatProjBuff(),	//���s���e�s��
		constBuff,	//�J���[ & ���[���h�s��
		EYE_POS_BUFF,	//���_���W
		LigManager.GetLigNumInfo(),	//�A�N�e�B�u���̃��C�g��
		LigManager.GetLigInfo(Light::DIRECTION),	//�f�B���N�V�������C�g
		LigManager.GetLigInfo(Light::POINT),	//�|�C���g���C�g
		LigManager.GetLigInfo(Light::SPOT),	//�X�|�b�g���C�g
		LigManager.GetLigInfo(Light::HEMISPHERE),	//�V�����C�g
		texBuff,			//�e�N�X�`��
		normalMap, 	//�m�[�}���}�b�v
		emissiveMap,	//�G�~�b�V�u�}�b�v
		},
		{ CBV,CBV,CBV,CBV,SRV,SRV,SRV,SRV,SRV,SRV,SRV });
}
