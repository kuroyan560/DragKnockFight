#include "DrawFunc_Shadow.h"
#include"KuroEngine.h"
#include"LightManager.h"

std::shared_ptr<ConstantBuffer>DrawFunc_Shadow::EYE_POS_BUFF;
std::shared_ptr<TextureBuffer>DrawFunc_Shadow::DEFAULT_TEX;
std::shared_ptr<TextureBuffer>DrawFunc_Shadow::DEFAULT_NORMAL_MAP;
std::shared_ptr<TextureBuffer>DrawFunc_Shadow::DEFAULT_EMISSIVE_MAP;

//DrawExtendGraph
std::shared_ptr<GraphicsPipeline>DrawFunc_Shadow::EXTEND_GRAPH_PIPELINE;
int DrawFunc_Shadow::DRAW_EXTEND_GRAPH_COUNT = 0;
std::vector<std::shared_ptr<VertexBuffer>>DrawFunc_Shadow::EXTEND_GRAPH_VERTEX_BUFF;

//DrawRotaGraph
std::shared_ptr<GraphicsPipeline>DrawFunc_Shadow::ROTA_GRAPH_PIPELINE;
int DrawFunc_Shadow::DRAW_ROTA_GRAPH_COUNT = 0;
std::vector<std::shared_ptr<VertexBuffer>>DrawFunc_Shadow::ROTA_GRAPH_VERTEX_BUFF;

static std::vector<RootParam>ROOT_PARAMETER =
{
	RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, "���s���e�s��萔�o�b�t�@"),
		RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, "���_���W���"),
		RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, "�A�N�e�B�u���̃��C�g���o�b�t�@"),
		RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, "�f�B���N�V�������C�g��� (�\�����o�b�t�@)"),
		RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, "�|�C���g���C�g��� (�\�����o�b�t�@)"),
		RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, "�X�|�b�g���C�g��� (�\�����o�b�t�@)"),
		RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, "�V�����C�g��� (�\�����o�b�t�@)"),
		RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, "�e�N�X�`�����\�[�X"),
		RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, "�@���}�b�v"),
		RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, "���˃}�b�v")
};

void DrawFunc_Shadow::StaticInit()
{
	if (EYE_POS_BUFF)return;

	//���_���W
	Vec3<float>INIT_EYE_POS = { WinApp::Instance()->GetExpandWinCenter().x,WinApp::Instance()->GetExpandWinCenter().y,-5.0f };
	EYE_POS_BUFF = D3D12App::Instance()->GenerateConstantBuffer(sizeof(Vec3<float>), 1, &INIT_EYE_POS, "DrawFunc_Shadow - EYE_POS");

	//���e�N�X�`��
	DEFAULT_TEX = D3D12App::Instance()->GenerateTextureBuffer(Color(1.0f, 1.0f, 1.0f, 1.0f));

	// (0,0,-1) �m�[�}���}�b�v
	DEFAULT_NORMAL_MAP = D3D12App::Instance()->GenerateTextureBuffer(Color(0.5f, 0.5f, 1.0f, 1.0f));

	//���e�N�X�`��
	DEFAULT_EMISSIVE_MAP = D3D12App::Instance()->GenerateTextureBuffer(Color(0.0f, 0.0f, 0.0f, 1.0f));
}

void DrawFunc_Shadow::SetEyePos(Vec3<float> EyePos)
{
	EYE_POS_BUFF->Mapping(&EyePos);
}

void DrawFunc_Shadow::DrawExtendGraph2D(LightManager& LigManager, const Vec2<float>& LeftUpPos, const Vec2<float>& RightBottomPos, const std::shared_ptr<TextureBuffer>& Tex, const std::shared_ptr<TextureBuffer>& NormalMap, const std::shared_ptr<TextureBuffer>& EmissiveMap, const float& SpriteDepth, const Vec2<bool>& Miror, const float& Diffuse, const float& Specular, const float& Lim)
{
	//DrawExtendGraph��p���_
	class ExtendGraphVertex
	{
	public:
		Vec2<float>leftUpPos;
		Vec2<float>rightBottomPos;
		Vec2<int> miror;
		float depth;
		float diffuse;
		float specular;
		float lim;
		ExtendGraphVertex(const Vec2<float>& LeftUpPos, const Vec2<float>& RightBottomPos, const Vec2<bool>& Miror,
			const float& Depth, const float& Diffuse, const float& Specular, const float& Lim)
			:leftUpPos(LeftUpPos), rightBottomPos(RightBottomPos), miror({ Miror.x ? 1 : 0 ,Miror.y ? 1 : 0 }),
			depth(Depth), diffuse(Diffuse), specular(Specular), lim(Lim) {}
	};

	//�p�C�v���C��������
	if (!EXTEND_GRAPH_PIPELINE)
	{
		//�p�C�v���C���ݒ�
		static PipelineInitializeOption PIPELINE_OPTION(D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT, D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
		PIPELINE_OPTION.depthTest = false;

		//�V�F�[�_�[���
		static Shaders SHADERS;
		SHADERS.vs = D3D12App::Instance()->CompileShader("resource/HLSL/Engine/DrawExtendGraph_Shadow.hlsl", "VSmain", "vs_5_0");
		SHADERS.gs = D3D12App::Instance()->CompileShader("resource/HLSL/Engine/DrawExtendGraph_Shadow.hlsl", "GSmain", "gs_5_0");
		SHADERS.ps = D3D12App::Instance()->CompileShader("resource/HLSL/Engine/DrawExtendGraph_Shadow.hlsl", "PSmain", "ps_5_0");

		//�C���v�b�g���C�A�E�g
		static std::vector<InputLayoutParam>INPUT_LAYOUT =
		{
			InputLayoutParam("POSITION_L_U",DXGI_FORMAT_R32G32_FLOAT),
			InputLayoutParam("POSITION_R_B",DXGI_FORMAT_R32G32_FLOAT),
			InputLayoutParam("MIROR",DXGI_FORMAT_R32G32_SINT),
			InputLayoutParam("DEPTH",DXGI_FORMAT_R32_FLOAT),
			InputLayoutParam("DIFFUSE",DXGI_FORMAT_R32_FLOAT),
			InputLayoutParam("SPECULAR",DXGI_FORMAT_R32_FLOAT),
			InputLayoutParam("LIM",DXGI_FORMAT_R32_FLOAT)
		};

		//�����_�[�^�[�Q�b�g�`�����
		std::vector<RenderTargetInfo>RENDER_TARGET_INFO =
		{
			//�o�b�N�o�b�t�@�̃t�H�[�}�b�g�A�A���t�@�u�����h
			RenderTargetInfo(D3D12App::Instance()->GetBackBuffFormat(),AlphaBlendMode_Trans),
			//�G�~�b�V�u
			RenderTargetInfo(DXGI_FORMAT_R32G32B32A32_FLOAT,AlphaBlendMode_Trans)
		};

		//�p�C�v���C������
		EXTEND_GRAPH_PIPELINE = D3D12App::Instance()->GenerateGraphicsPipeline(PIPELINE_OPTION, SHADERS, INPUT_LAYOUT, ROOT_PARAMETER, RENDER_TARGET_INFO);
	}

	StaticInit();

	KuroEngine::Instance().Graphics().SetPipeline(EXTEND_GRAPH_PIPELINE);

	if (EXTEND_GRAPH_VERTEX_BUFF.size() < (DRAW_EXTEND_GRAPH_COUNT + 1))
	{
		EXTEND_GRAPH_VERTEX_BUFF.emplace_back(D3D12App::Instance()->GenerateVertexBuffer(sizeof(ExtendGraphVertex), 1, nullptr, ("DrawExtendGraph_Shadow -" + std::to_string(DRAW_EXTEND_GRAPH_COUNT)).c_str()));
	}

	ExtendGraphVertex vertex(LeftUpPos, RightBottomPos, Miror, SpriteDepth, Diffuse, Specular, Lim);
	EXTEND_GRAPH_VERTEX_BUFF[DRAW_EXTEND_GRAPH_COUNT]->Mapping(&vertex);

	auto normalMap = NormalMap ? NormalMap : DEFAULT_NORMAL_MAP;
	auto emissiveMap = EmissiveMap ? EmissiveMap : DEFAULT_EMISSIVE_MAP;

	KuroEngine::Instance().Graphics().ObjectRender(EXTEND_GRAPH_VERTEX_BUFF[DRAW_EXTEND_GRAPH_COUNT],
		{
			KuroEngine::Instance().GetParallelMatProjBuff(),
			EYE_POS_BUFF,	//���_���W
			LigManager.GetLigNumInfo(),	//�A�N�e�B�u���̃��C�g��
			LigManager.GetLigInfo(Light::DIRECTION),	//�f�B���N�V�������C�g
			LigManager.GetLigInfo(Light::POINT),	//�|�C���g���C�g
			LigManager.GetLigInfo(Light::SPOT),	//�X�|�b�g���C�g
			LigManager.GetLigInfo(Light::HEMISPHERE),	//�V�����C�g
			Tex,normalMap,emissiveMap
		},
		{ CBV,CBV,CBV,SRV,SRV,SRV,SRV,SRV,SRV,SRV }, 0.0f, true);

	DRAW_EXTEND_GRAPH_COUNT++;
}

void DrawFunc_Shadow::DrawRotaGraph2D(LightManager& LigManager, const Vec2<float>& Center, const Vec2<float>& ExtRate, const float& Radian, const std::shared_ptr<TextureBuffer>& Tex, const std::shared_ptr<TextureBuffer>& NormalMap, const std::shared_ptr<TextureBuffer>& EmissiveMap, const float& SpriteDepth, const Vec2<float>& RotaCenterUV, const Vec2<bool>& Miror, const float& Diffuse, const float& Specular, const float& Lim)
{
	//DrawRotaGraph��p���_
	class RotaGraphVertex
	{
	public:
		Vec2<float>center;
		Vec2<float> extRate;
		float radian;
		Vec2<float>rotaCenterUV;
		Vec2<int> miror;
		float depth;
		float diffuse;
		float specular;
		float lim;
		RotaGraphVertex(const Vec2<float>& Center, const Vec2<float>& ExtRate, const float& Radian,
			const Vec2<float>& RotaCenterUV, const Vec2<bool>& Miror,
			const float& Depth, const float& Diffuse, const float& Specular, const float& Lim)
			:center(Center), extRate(ExtRate), radian(Radian), rotaCenterUV(RotaCenterUV), miror({ Miror.x ? 1 : 0,Miror.y ? 1 : 0 }),
			depth(Depth), diffuse(Diffuse), specular(Specular), lim(Lim) {}
	};

	//�p�C�v���C��������
	if (!ROTA_GRAPH_PIPELINE)
	{
		//�p�C�v���C���ݒ�
		static PipelineInitializeOption PIPELINE_OPTION(D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT, D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
		PIPELINE_OPTION.depthTest = false;

		//�V�F�[�_�[���
		static Shaders SHADERS;
		SHADERS.vs = D3D12App::Instance()->CompileShader("resource/HLSL/Engine/DrawRotaGraph_Shadow.hlsl", "VSmain", "vs_5_0");
		SHADERS.gs = D3D12App::Instance()->CompileShader("resource/HLSL/Engine/DrawRotaGraph_Shadow.hlsl", "GSmain", "gs_5_0");
		SHADERS.ps = D3D12App::Instance()->CompileShader("resource/HLSL/Engine/DrawRotaGraph_Shadow.hlsl", "PSmain", "ps_5_0");

		//�C���v�b�g���C�A�E�g
		static std::vector<InputLayoutParam>INPUT_LAYOUT =
		{
			InputLayoutParam("CENTER",DXGI_FORMAT_R32G32_FLOAT),
			InputLayoutParam("EXT_RATE",DXGI_FORMAT_R32G32_FLOAT),
			InputLayoutParam("RADIAN",DXGI_FORMAT_R32_FLOAT),
			InputLayoutParam("ROTA_CENTER_UV",DXGI_FORMAT_R32G32_FLOAT),
			InputLayoutParam("MIROR",DXGI_FORMAT_R32G32_SINT),
			InputLayoutParam("DEPTH",DXGI_FORMAT_R32_FLOAT),
			InputLayoutParam("DIFFUSE",DXGI_FORMAT_R32_FLOAT),
			InputLayoutParam("SPECULAR",DXGI_FORMAT_R32_FLOAT),
			InputLayoutParam("LIM",DXGI_FORMAT_R32_FLOAT)
		};

		//�����_�[�^�[�Q�b�g�`�����
		std::vector<RenderTargetInfo>RENDER_TARGET_INFO =
		{
			//�o�b�N�o�b�t�@�̃t�H�[�}�b�g�A�A���t�@�u�����h
			RenderTargetInfo(D3D12App::Instance()->GetBackBuffFormat(),AlphaBlendMode_Trans),
			//�G�~�b�V�u
			RenderTargetInfo(DXGI_FORMAT_R32G32B32A32_FLOAT,AlphaBlendMode_Trans)
		};

		//�p�C�v���C������
		ROTA_GRAPH_PIPELINE = D3D12App::Instance()->GenerateGraphicsPipeline(PIPELINE_OPTION, SHADERS, INPUT_LAYOUT, ROOT_PARAMETER, RENDER_TARGET_INFO);
	}

	StaticInit();

	KuroEngine::Instance().Graphics().SetPipeline(ROTA_GRAPH_PIPELINE);

	if (ROTA_GRAPH_VERTEX_BUFF.size() < (DRAW_ROTA_GRAPH_COUNT + 1))
	{
		ROTA_GRAPH_VERTEX_BUFF.emplace_back(D3D12App::Instance()->GenerateVertexBuffer(sizeof(RotaGraphVertex), 1, nullptr, ("DrawRotaGraph_Shadow -" + std::to_string(DRAW_ROTA_GRAPH_COUNT)).c_str()));
	}

	RotaGraphVertex vertex(Center, ExtRate, Radian, RotaCenterUV, Miror, SpriteDepth, Diffuse, Specular, Lim);
	ROTA_GRAPH_VERTEX_BUFF[DRAW_ROTA_GRAPH_COUNT]->Mapping(&vertex);

	auto normalMap = NormalMap ? NormalMap : DEFAULT_NORMAL_MAP;
	auto emissiveMap = EmissiveMap ? EmissiveMap : DEFAULT_EMISSIVE_MAP;

	KuroEngine::Instance().Graphics().ObjectRender(ROTA_GRAPH_VERTEX_BUFF[DRAW_ROTA_GRAPH_COUNT],
		{
			KuroEngine::Instance().GetParallelMatProjBuff(),
			EYE_POS_BUFF,	//���_���W
			LigManager.GetLigNumInfo(),	//�A�N�e�B�u���̃��C�g��
			LigManager.GetLigInfo(Light::DIRECTION),	//�f�B���N�V�������C�g
			LigManager.GetLigInfo(Light::POINT),	//�|�C���g���C�g
			LigManager.GetLigInfo(Light::SPOT),	//�X�|�b�g���C�g
			LigManager.GetLigInfo(Light::HEMISPHERE),	//�V�����C�g
			Tex,normalMap,emissiveMap
		},
		{ CBV,CBV,CBV,SRV,SRV,SRV,SRV,SRV,SRV,SRV }, 0.0f, true);

	DRAW_ROTA_GRAPH_COUNT++;
}