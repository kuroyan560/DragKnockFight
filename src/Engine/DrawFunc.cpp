#include "DrawFunc.h"
#include"KuroEngine.h"

//DrawLine
std::shared_ptr<GraphicsPipeline>DrawFunc::LINE_PIPELINE[AlphaBlendModeNum];
int DrawFunc::DRAW_LINE_COUNT = 0;
std::vector<std::shared_ptr<VertexBuffer>>DrawFunc::LINE_VERTEX_BUFF;

//DrawBox
std::map<DXGI_FORMAT, std::map<AlphaBlendMode, std::shared_ptr<GraphicsPipeline>>>DrawFunc::BOX_PIPELINE;
int DrawFunc::DRAW_BOX_COUNT = 0;
std::vector<std::shared_ptr<VertexBuffer>>DrawFunc::BOX_VERTEX_BUFF;

//DrawCircle
std::shared_ptr<GraphicsPipeline>DrawFunc::CIRCLE_PIPELINE[AlphaBlendModeNum];
int DrawFunc::DRAW_CIRCLE_COUNT = 0;
std::vector<std::shared_ptr<VertexBuffer>>DrawFunc::CIRCLE_VERTEX_BUFF;

//DrawExtendGraph
std::shared_ptr<GraphicsPipeline>DrawFunc::EXTEND_GRAPH_PIPELINE[AlphaBlendModeNum];
int DrawFunc::DRAW_EXTEND_GRAPH_COUNT = 0;
std::vector<std::shared_ptr<VertexBuffer>>DrawFunc::EXTEND_GRAPH_VERTEX_BUFF;

//DrawRotaGraph
std::shared_ptr<GraphicsPipeline>DrawFunc::ROTA_GRAPH_PIPELINE[AlphaBlendModeNum];
int DrawFunc::DRAW_ROTA_GRAPH_COUNT = 0;
std::vector<std::shared_ptr<VertexBuffer>>DrawFunc::ROTA_GRAPH_VERTEX_BUFF;

void DrawFunc::DrawLine2D(const Vec2<float>& FromPos, const Vec2<float>& ToPos, const Color& LineColor, const AlphaBlendMode& BlendMode)
{
	//DrawLine��p���_
	class LineVertex
	{
	public:
		Vec2<float>pos;
		Color color;
		LineVertex(const Vec2<float>& Pos, const Color& Color) :pos(Pos), color(Color) {}
	};

	//�p�C�v���C��������
	if(!LINE_PIPELINE[0])
	{
		//�p�C�v���C���ݒ�
		static PipelineInitializeOption PIPELINE_OPTION(D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE, D3D_PRIMITIVE_TOPOLOGY_LINESTRIP);
		PIPELINE_OPTION.depthTest = false;

		//�V�F�[�_�[���
		static Shaders SHADERS;
		SHADERS.vs = D3D12App::Instance()->CompileShader("resource/HLSL/Engine/DrawLine.hlsl", "VSmain", "vs_5_0");
		SHADERS.ps = D3D12App::Instance()->CompileShader("resource/HLSL/Engine/DrawLine.hlsl", "PSmain", "ps_5_0");

		//�C���v�b�g���C�A�E�g
		static std::vector<InputLayoutParam>INPUT_LAYOUT =
		{
			InputLayoutParam("POSITION",DXGI_FORMAT_R32G32_FLOAT),
			InputLayoutParam("COLOR",DXGI_FORMAT_R32G32B32A32_FLOAT)
		};

		//���[�g�p�����[�^
		static std::vector<RootParam>ROOT_PARAMETER =
		{
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"���s���e�s��萔�o�b�t�@")
		};

		//�����_�[�^�[�Q�b�g�`�����
		for (int i = 0; i < AlphaBlendModeNum; ++i)
		{
			std::vector<RenderTargetInfo>RENDER_TARGET_INFO = { RenderTargetInfo(D3D12App::Instance()->GetBackBuffFormat(), (AlphaBlendMode)i) };
			//�p�C�v���C������
			LINE_PIPELINE[i] = D3D12App::Instance()->GenerateGraphicsPipeline(PIPELINE_OPTION, SHADERS, INPUT_LAYOUT, ROOT_PARAMETER, RENDER_TARGET_INFO, WrappedSampler(false, false));
		}
	}

	KuroEngine::Instance().Graphics().SetPipeline(LINE_PIPELINE[BlendMode]);

	if (LINE_VERTEX_BUFF.size() < (DRAW_LINE_COUNT + 1))
	{
		LINE_VERTEX_BUFF.emplace_back(D3D12App::Instance()->GenerateVertexBuffer(sizeof(LineVertex), 2, nullptr, ("DrawLine -" + std::to_string(DRAW_LINE_COUNT)).c_str()));
	}

	LineVertex vertex[2] =
	{
		{FromPos,LineColor },
		{ToPos,LineColor}
	};
	LINE_VERTEX_BUFF[DRAW_LINE_COUNT]->Mapping(&vertex[0]);

	KuroEngine::Instance().Graphics().ObjectRender(LINE_VERTEX_BUFF[DRAW_LINE_COUNT], { KuroEngine::Instance().GetParallelMatProjBuff() }, { CBV }, 0.0f, true);

	DRAW_LINE_COUNT++;
}

void DrawFunc::DrawLine2DGraph(const Vec2<float>& FromPos, const Vec2<float>& ToPos, const std::shared_ptr<TextureBuffer>& Tex, const int& Thickness, const AlphaBlendMode& BlendMode, const Vec2<bool>& Mirror)
{
	float distance = FromPos.Distance(ToPos);
	Vec2<float> vec = (ToPos - FromPos).GetNormal();

	auto graphSize = Tex->GetGraphSize().Float();
	Vec2<float>expRate = { distance / graphSize.x,Thickness / graphSize.y };
	Vec2<float>centerPos = FromPos + vec * distance / 2;

	DrawRotaGraph2D(centerPos, expRate, KuroFunc::GetAngle(vec), Tex, { 0.5f,0.5f }, BlendMode, Mirror);
}


void DrawFunc::DrawBox2D(const Vec2<float>& LeftUpPos, const Vec2<float>& RightBottomPos, const Color& BoxColor, const DXGI_FORMAT& Format, const bool& FillFlg, const AlphaBlendMode& BlendMode)
{
	if (FillFlg)
	{
		//DrawBox��p���_
		class BoxVertex
		{
		public:
			Vec2<float>leftUpPos;
			Vec2<float>rightBottomPos;
			Color color;
			BoxVertex(const Vec2<float>& LeftUpPos, const Vec2<float>& RightBottomPos, const Color& Color)
				:leftUpPos(LeftUpPos), rightBottomPos(RightBottomPos), color(Color) {}
		};

		//�p�C�v���C��������
		if (!BOX_PIPELINE[Format][BlendMode])
		{
			//�p�C�v���C���ݒ�
			static PipelineInitializeOption PIPELINE_OPTION(D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT, D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
			PIPELINE_OPTION.depthTest = false;

			//�V�F�[�_�[���
			static Shaders SHADERS;
			SHADERS.vs = D3D12App::Instance()->CompileShader("resource/HLSL/Engine/DrawBox.hlsl", "VSmain", "vs_5_0");
			SHADERS.gs = D3D12App::Instance()->CompileShader("resource/HLSL/Engine/DrawBox.hlsl", "GSmain", "gs_5_0");
			SHADERS.ps = D3D12App::Instance()->CompileShader("resource/HLSL/Engine/DrawBox.hlsl", "PSmain", "ps_5_0");

			//�C���v�b�g���C�A�E�g
			static std::vector<InputLayoutParam>INPUT_LAYOUT =
			{
				InputLayoutParam("POSITION_L_U",DXGI_FORMAT_R32G32_FLOAT),
				InputLayoutParam("POSITION_R_B",DXGI_FORMAT_R32G32_FLOAT),
				InputLayoutParam("COLOR",DXGI_FORMAT_R32G32B32A32_FLOAT)
			};

			//���[�g�p�����[�^
			static std::vector<RootParam>ROOT_PARAMETER =
			{
				RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"���s���e�s��萔�o�b�t�@")
			};

			//�����_�[�^�[�Q�b�g�`�����
			std::vector<RenderTargetInfo>RENDER_TARGET_INFO = { RenderTargetInfo(Format, BlendMode) };
			//�p�C�v���C������
			BOX_PIPELINE[Format][BlendMode] = D3D12App::Instance()->GenerateGraphicsPipeline(PIPELINE_OPTION, SHADERS, INPUT_LAYOUT, ROOT_PARAMETER, RENDER_TARGET_INFO, WrappedSampler(false, false));
		}

		KuroEngine::Instance().Graphics().SetPipeline(BOX_PIPELINE[Format][BlendMode]);

		if (BOX_VERTEX_BUFF.size() < (DRAW_BOX_COUNT + 1))
		{
			BOX_VERTEX_BUFF.emplace_back(D3D12App::Instance()->GenerateVertexBuffer(sizeof(BoxVertex), 1, nullptr, ("DrawBox -" + std::to_string(DRAW_BOX_COUNT)).c_str()));
		}

		BoxVertex vertex(LeftUpPos, RightBottomPos, BoxColor);
		BOX_VERTEX_BUFF[DRAW_BOX_COUNT]->Mapping(&vertex);

		KuroEngine::Instance().Graphics().ObjectRender(BOX_VERTEX_BUFF[DRAW_BOX_COUNT], { KuroEngine::Instance().GetParallelMatProjBuff() }, { CBV }, 0.0f, true);

		DRAW_BOX_COUNT++;
	}
	//�O�g�����̂Ƃ���DrawLine�ōς܂�
	else
	{
		const float width = RightBottomPos.x - LeftUpPos.x;
		Vec2<float>pos[4] =
		{
			LeftUpPos,
			{LeftUpPos.x + width,LeftUpPos.y},
			RightBottomPos,
			{RightBottomPos.x - width,RightBottomPos.y}
		};
		for (int i = 0; i < 4; ++i)
		{
			int next = 4 <= (i + 1) ? 0 : i + 1;
			DrawLine2D(pos[i], pos[next], BoxColor, BlendMode);
		}
	}
}

void DrawFunc::DrawCircle2D(const Vec2<float>& Center, const float& Radius, const Color& CircleColor, const bool& FillFlg, const int& LineThickness, const AlphaBlendMode& BlendMode)
{
	//DrawCircle��p���_
	class CircleVertex
	{
	public:
		Vec2<float>center;
		float radius;
		Color color;
		unsigned int fillFlg;
		int thickness;

		CircleVertex(const Vec2<float>& Center, const float& Radius, const Color& Color, const bool& FillFlg, const int& Thickness)
			:center(Center), radius(Radius), color(Color), fillFlg(FillFlg ? 0 : 1), thickness(Thickness) {}
	};

	//�p�C�v���C��������
	if (!CIRCLE_PIPELINE[0])
	{
		//�p�C�v���C���ݒ�
		static PipelineInitializeOption PIPELINE_OPTION(D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT, D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
		PIPELINE_OPTION.depthTest = false;

		//�V�F�[�_�[���
		static Shaders SHADERS;
		SHADERS.vs = D3D12App::Instance()->CompileShader("resource/HLSL/Engine/DrawCircle.hlsl", "VSmain", "vs_5_0");
		SHADERS.gs = D3D12App::Instance()->CompileShader("resource/HLSL/Engine/DrawCircle.hlsl", "GSmain", "gs_5_0");
		SHADERS.ps = D3D12App::Instance()->CompileShader("resource/HLSL/Engine/DrawCircle.hlsl", "PSmain", "ps_5_0");

		//�C���v�b�g���C�A�E�g
		static std::vector<InputLayoutParam>INPUT_LAYOUT =
		{
			InputLayoutParam("CENTER",DXGI_FORMAT_R32G32_FLOAT),
			InputLayoutParam("RADIUS",DXGI_FORMAT_R32_FLOAT),
			InputLayoutParam("COLOR",DXGI_FORMAT_R32G32B32A32_FLOAT),
			InputLayoutParam("FILL",DXGI_FORMAT_R32_UINT),
			InputLayoutParam("THICKNESS",DXGI_FORMAT_R32_SINT),
		};

		//���[�g�p�����[�^
		static std::vector<RootParam>ROOT_PARAMETER =
		{
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"���s���e�s��萔�o�b�t�@")
		};

		//�����_�[�^�[�Q�b�g�`�����
		for (int i = 0; i < AlphaBlendModeNum; ++i)
		{
			std::vector<RenderTargetInfo>RENDER_TARGET_INFO = { RenderTargetInfo(D3D12App::Instance()->GetBackBuffFormat(), (AlphaBlendMode)i) };
			//�p�C�v���C������
			CIRCLE_PIPELINE[i] = D3D12App::Instance()->GenerateGraphicsPipeline(PIPELINE_OPTION, SHADERS, INPUT_LAYOUT, ROOT_PARAMETER, RENDER_TARGET_INFO, WrappedSampler(false, false));
		}
	}

	KuroEngine::Instance().Graphics().SetPipeline(CIRCLE_PIPELINE[BlendMode]);

	if (CIRCLE_VERTEX_BUFF.size() < (DRAW_CIRCLE_COUNT + 1))
	{
		CIRCLE_VERTEX_BUFF.emplace_back(D3D12App::Instance()->GenerateVertexBuffer(sizeof(CircleVertex), 1, nullptr, ("DrawCircle -" + std::to_string(DRAW_CIRCLE_COUNT)).c_str()));
	}

	CircleVertex vertex(Center, Radius, CircleColor, FillFlg, LineThickness);

	CIRCLE_VERTEX_BUFF[DRAW_CIRCLE_COUNT]->Mapping(&vertex);

	KuroEngine::Instance().Graphics().ObjectRender(CIRCLE_VERTEX_BUFF[DRAW_CIRCLE_COUNT], { KuroEngine::Instance().GetParallelMatProjBuff() }, { CBV }, 0.0f, true);

	DRAW_CIRCLE_COUNT++;
}

void DrawFunc::DrawGraph(const Vec2<float>& LeftUpPos, const std::shared_ptr<TextureBuffer>& Tex, const AlphaBlendMode& BlendMode, const Vec2<bool>& Miror)
{
	auto graphSize = Tex->GetGraphSize();
	DrawExtendGraph2D(LeftUpPos, LeftUpPos + graphSize.Float(), Tex, BlendMode, Miror);
}

void DrawFunc::DrawExtendGraph2D(const Vec2<float>& LeftUpPos, const Vec2<float>& RightBottomPos, const std::shared_ptr<TextureBuffer>& Tex, const AlphaBlendMode& BlendMode, const Vec2<bool>& Miror)
{
	//DrawExtendGraph��p���_
	class ExtendGraphVertex
	{
	public:
		Vec2<float>leftUpPos;
		Vec2<float>rightBottomPos;
		Vec2<int> miror;
		ExtendGraphVertex(const Vec2<float>& LeftUpPos, const Vec2<float>& RightBottomPos, const Vec2<bool>& Miror)
			:leftUpPos(LeftUpPos), rightBottomPos(RightBottomPos), miror({ Miror.x ? 1 : 0 ,Miror.y ? 1 : 0 }) {}
	};

	//�p�C�v���C��������
	if (!EXTEND_GRAPH_PIPELINE[0])
	{
		//�p�C�v���C���ݒ�
		static PipelineInitializeOption PIPELINE_OPTION(D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT, D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
		PIPELINE_OPTION.depthTest = false;

		//�V�F�[�_�[���
		static Shaders SHADERS;
		SHADERS.vs = D3D12App::Instance()->CompileShader("resource/HLSL/Engine/DrawExtendGraph.hlsl", "VSmain", "vs_5_0");
		SHADERS.gs = D3D12App::Instance()->CompileShader("resource/HLSL/Engine/DrawExtendGraph.hlsl", "GSmain", "gs_5_0");
		SHADERS.ps = D3D12App::Instance()->CompileShader("resource/HLSL/Engine/DrawExtendGraph.hlsl", "PSmain", "ps_5_0");

		//�C���v�b�g���C�A�E�g
		static std::vector<InputLayoutParam>INPUT_LAYOUT =
		{
			InputLayoutParam("POSITION_L_U",DXGI_FORMAT_R32G32_FLOAT),
			InputLayoutParam("POSITION_R_B",DXGI_FORMAT_R32G32_FLOAT),
			InputLayoutParam("MIROR",DXGI_FORMAT_R32G32_SINT)
		};

		//���[�g�p�����[�^
		static std::vector<RootParam>ROOT_PARAMETER =
		{
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"���s���e�s��萔�o�b�t�@"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"�e�N�X�`�����\�[�X")
		};

		//�����_�[�^�[�Q�b�g�`�����
		for (int i = 0; i < AlphaBlendModeNum; ++i)
		{
			std::vector<RenderTargetInfo>RENDER_TARGET_INFO = { RenderTargetInfo(D3D12App::Instance()->GetBackBuffFormat(), (AlphaBlendMode)i) };
			//�p�C�v���C������
			EXTEND_GRAPH_PIPELINE[i] = D3D12App::Instance()->GenerateGraphicsPipeline(PIPELINE_OPTION, SHADERS, INPUT_LAYOUT, ROOT_PARAMETER, RENDER_TARGET_INFO, WrappedSampler(true, false));
		}
	}

	KuroEngine::Instance().Graphics().SetPipeline(EXTEND_GRAPH_PIPELINE[BlendMode]);

	if (EXTEND_GRAPH_VERTEX_BUFF.size() < (DRAW_EXTEND_GRAPH_COUNT + 1))
	{
		EXTEND_GRAPH_VERTEX_BUFF.emplace_back(D3D12App::Instance()->GenerateVertexBuffer(sizeof(ExtendGraphVertex), 1, nullptr, ("DrawExtendGraph -" + std::to_string(DRAW_EXTEND_GRAPH_COUNT)).c_str()));
	}

	ExtendGraphVertex vertex(LeftUpPos, RightBottomPos, Miror);
	EXTEND_GRAPH_VERTEX_BUFF[DRAW_EXTEND_GRAPH_COUNT]->Mapping(&vertex);

	KuroEngine::Instance().Graphics().ObjectRender(EXTEND_GRAPH_VERTEX_BUFF[DRAW_EXTEND_GRAPH_COUNT], { KuroEngine::Instance().GetParallelMatProjBuff(),Tex }, { CBV,SRV }, 0.0f, true);

	DRAW_EXTEND_GRAPH_COUNT++;
}

void DrawFunc::DrawRotaGraph2D(const Vec2<float>& Center, const Vec2<float>& ExtRate, const float& Radian,
	const std::shared_ptr<TextureBuffer>& Tex, const Vec2<float>& RotaCenterUV, const AlphaBlendMode& BlendMode, const Vec2<bool>& Miror)
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
		RotaGraphVertex(const Vec2<float>& Center, const Vec2<float>& ExtRate, const float& Radian,
			const Vec2<float>& RotaCenterUV, const Vec2<bool>& Miror)
			:center(Center), extRate(ExtRate), radian(Radian),rotaCenterUV(RotaCenterUV), miror({ Miror.x ? 1 : 0,Miror.y ? 1 : 0 }) {}
	};

	//�p�C�v���C��������
	if (!ROTA_GRAPH_PIPELINE[0])
	{
		//�p�C�v���C���ݒ�
		static PipelineInitializeOption PIPELINE_OPTION(D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT, D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
		PIPELINE_OPTION.depthTest = false;

		//�V�F�[�_�[���
		static Shaders SHADERS;
		SHADERS.vs = D3D12App::Instance()->CompileShader("resource/HLSL/Engine/DrawRotaGraph.hlsl", "VSmain", "vs_5_0");
		SHADERS.gs = D3D12App::Instance()->CompileShader("resource/HLSL/Engine/DrawRotaGraph.hlsl", "GSmain", "gs_5_0");
		SHADERS.ps = D3D12App::Instance()->CompileShader("resource/HLSL/Engine/DrawRotaGraph.hlsl", "PSmain", "ps_5_0");

		//�C���v�b�g���C�A�E�g
		static std::vector<InputLayoutParam>INPUT_LAYOUT =
		{
			InputLayoutParam("CENTER",DXGI_FORMAT_R32G32_FLOAT),
			InputLayoutParam("EXT_RATE",DXGI_FORMAT_R32G32_FLOAT),
			InputLayoutParam("RADIAN",DXGI_FORMAT_R32_FLOAT),
			InputLayoutParam("ROTA_CENTER_UV",DXGI_FORMAT_R32G32_FLOAT),
			InputLayoutParam("MIROR",DXGI_FORMAT_R32G32_SINT)
		};

		//���[�g�p�����[�^
		static std::vector<RootParam>ROOT_PARAMETER =
		{
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"���s���e�s��萔�o�b�t�@"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"�e�N�X�`�����\�[�X")
		};

		//�����_�[�^�[�Q�b�g�`�����
		for (int i = 0; i < AlphaBlendModeNum; ++i)
		{
			std::vector<RenderTargetInfo>RENDER_TARGET_INFO = { RenderTargetInfo(D3D12App::Instance()->GetBackBuffFormat(), (AlphaBlendMode)i) };
			//�p�C�v���C������
			ROTA_GRAPH_PIPELINE[i] = D3D12App::Instance()->GenerateGraphicsPipeline(PIPELINE_OPTION, SHADERS, INPUT_LAYOUT, ROOT_PARAMETER, RENDER_TARGET_INFO, WrappedSampler(true, false));
		}
	}

	KuroEngine::Instance().Graphics().SetPipeline(ROTA_GRAPH_PIPELINE[BlendMode]);

	if (ROTA_GRAPH_VERTEX_BUFF.size() < (DRAW_ROTA_GRAPH_COUNT + 1))
	{
		ROTA_GRAPH_VERTEX_BUFF.emplace_back(D3D12App::Instance()->GenerateVertexBuffer(sizeof(RotaGraphVertex), 1, nullptr, ("DrawRotaGraph -" + std::to_string(DRAW_ROTA_GRAPH_COUNT)).c_str()));
	}

	RotaGraphVertex vertex(Center, ExtRate, Radian,RotaCenterUV, Miror);
	ROTA_GRAPH_VERTEX_BUFF[DRAW_ROTA_GRAPH_COUNT]->Mapping(&vertex);

	KuroEngine::Instance().Graphics().ObjectRender(ROTA_GRAPH_VERTEX_BUFF[DRAW_ROTA_GRAPH_COUNT], { KuroEngine::Instance().GetParallelMatProjBuff(),Tex }, { CBV,SRV }, 0.0f, true);

	DRAW_ROTA_GRAPH_COUNT++;
}

