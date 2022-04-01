#include "Muffler.h"
#include"KuroEngine.h"

//static const Vec2<float>MUFFLER_SIZE = { 48.0f,64.0f };
static const Vec2<float>MUFFLER_SIZE = { 32.0f,64.0f };
static const Vec2<float>OFFSET_FROM_PLAYER_POS = { 0.0f,-32.0f };

static const int UPDATE_PER_FRAME = 1;

unsigned int GetIndex(const int& x, const int& y, const int& numX)
{
	return y * numX + x;
}

Muffler::Muffler()
{
	//���_���
	const Vec2<float>uvOffset = { 1.0f / (VERTEX_NUM_X - 1),1.0f / (VERTEX_NUM_Y - 1) };
	for (int y = 0; y < VERTEX_NUM_Y; ++y)
	{
		for (int x = 0; x < VERTEX_NUM_X; ++x)
		{
			vertices[y][x].uv = { uvOffset.x * x,uvOffset.y * y };
		}
	}
	vertBuff = D3D12App::Instance()->GenerateVertexBuffer(sizeof(Vertex), VERTEX_NUM_X * VERTEX_NUM_Y, nullptr, "Muffler - Vertex", true);

	//�C���f�b�N�X���
	std::vector<unsigned int>indices;
	for (int y = 0; y < VERTEX_NUM_Y - 1; ++y)
	{
		for (int x = 0; x < VERTEX_NUM_X; ++x)
		{
			//�
			if (y % 2 != 0)
			{
				indices.emplace_back(GetIndex(VERTEX_NUM_X - 1 - x, y, VERTEX_NUM_X));
				indices.emplace_back(GetIndex(VERTEX_NUM_X - 1 - x, y + 1, VERTEX_NUM_X));
			}
			//����
			else
			{
				indices.emplace_back(GetIndex(x, y, VERTEX_NUM_X));
				indices.emplace_back(GetIndex(x, y + 1, VERTEX_NUM_X));
			}
		}
	}
	idxBuff = D3D12App::Instance()->GenerateIndexBuffer(indices.size(), indices.data(), "Muffler - IndexBuff");

	//�e�N�X�`��
	tex = D3D12App::Instance()->GenerateTextureBuffer(Color(217, 26, 96, 255));

	//�R���s���[�g�p�C�v���C��
	{
		auto cs = D3D12App::Instance()->CompileShader("resource/HLSL/Muffler_Compute.hlsl", "CSmain", "cs_5_0");
		//���[�g�p�����[�^
		std::vector<RootParam>rootParams =
		{
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_UAV,"���_�z��"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"�}�t���[��_"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"�}�t���[����"),
		};
		cPipeline = D3D12App::Instance()->GenerateComputePipeline(cs, rootParams);
	}
	playerPosBuff = D3D12App::Instance()->GenerateConstantBuffer(sizeof(Vec2<float>), 1, nullptr, "Muffler - PlayerPos");

	MufflerInfo info;

	info.vertexNum = { VERTEX_NUM_X,VERTEX_NUM_Y };
	info.mufflerSize = MUFFLER_SIZE;
	mufflerInfo = D3D12App::Instance()->GenerateConstantBuffer(sizeof(MufflerInfo), 1, &info, "Muffler - Info");

	//�O���t�B�b�N�X�p�C�v���C��
	{
		//�p�C�v���C���ݒ�
		static PipelineInitializeOption PIPELINE_OPTION(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		PIPELINE_OPTION.depthTest = false;
		PIPELINE_OPTION.calling = false;

		//�V�F�[�_�[���
		static Shaders SHADERS;
		SHADERS.vs = D3D12App::Instance()->CompileShader("resource/HLSL/Muffler_Graphics.hlsl", "VSmain", "vs_5_0");
		SHADERS.ps = D3D12App::Instance()->CompileShader("resource/HLSL/Muffler_Graphics.hlsl", "PSmain", "ps_5_0");

		//�C���v�b�g���C�A�E�g
		static std::vector<InputLayoutParam>INPUT_LAYOUT =
		{
			InputLayoutParam("POSITION",DXGI_FORMAT_R32G32_FLOAT),
			InputLayoutParam("TEXCOORD",DXGI_FORMAT_R32G32_FLOAT),
			InputLayoutParam("VELOCITY",DXGI_FORMAT_R32G32_FLOAT),
			InputLayoutParam("FORCE",DXGI_FORMAT_R32G32_FLOAT),
		};

		//���[�g�p�����[�^
		static std::vector<RootParam>ROOT_PARAMETER =
		{
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"���s���e�s��萔�o�b�t�@"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"�Y�[���ƃX�N���[��"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"�A�N�e�B�u���̃��C�g���o�b�t�@"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"�f�B���N�V�������C�g��� (�\�����o�b�t�@)"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"�|�C���g���C�g��� (�\�����o�b�t�@)"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"�X�|�b�g���C�g��� (�\�����o�b�t�@)"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"�V�����C�g��� (�\�����o�b�t�@)"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"�e�N�X�`�����"),
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
		gPipeline = D3D12App::Instance()->GenerateGraphicsPipeline(PIPELINE_OPTION, SHADERS, INPUT_LAYOUT, ROOT_PARAMETER, RENDER_TARGET_INFO);
	}
	zoomAndScroll = D3D12App::Instance()->GenerateConstantBuffer(sizeof(ZoomAndScroll), 1, nullptr, "Muffler - ZoomAndScroll");
}

void Muffler::Init(const Vec2<float>& InitPos)
{
	Vec2<float>vertexOffset = { MUFFLER_SIZE.x / VERTEX_NUM_X,MUFFLER_SIZE.y / VERTEX_NUM_Y };
	auto initPos = InitPos;
	initPos.x -= MUFFLER_SIZE.x / 2.0f;

	for (int y = 0; y < VERTEX_NUM_Y; ++y)
	{
		for (int x = 0; x < VERTEX_NUM_X; ++x)
		{
			vertices[y][x].vel = { 0.0f,0.0f };
			vertices[y][x].force = { 0.0f,0.0f };
			vertices[y][x].pos = initPos + vertexOffset * Vec2<float>(x, y) + OFFSET_FROM_PLAYER_POS;
		}
	}
	for (int x = 0; x < VERTEX_NUM_X; ++x)vertices[0][x].pos = InitPos + OFFSET_FROM_PLAYER_POS;
	vertBuff->Mapping(&vertices[0][0]);
}

void Muffler::Update(const Vec2<float>& PlayerPos)
{
	// Muffler's starting point.
	static Vec2<float>mufflerBasePos = { 0.0f,0.0f };
	if (mufflerBasePos != PlayerPos + OFFSET_FROM_PLAYER_POS)
	{
		mufflerBasePos = PlayerPos + OFFSET_FROM_PLAYER_POS;
		playerPosBuff->Mapping(&PlayerPos);
	}

	D3D12App::Instance()->SetDescHeaps();
	auto cmdList = D3D12App::Instance()->GetCmdList();
	cPipeline->SetPipeline(cmdList);
	vertBuff->GetRWStructuredBuff().lock()->SetComputeDescriptorBuffer(cmdList, UAV, 0);
	playerPosBuff->SetComputeDescriptorBuffer(cmdList, CBV, 1);
	mufflerInfo->SetComputeDescriptorBuffer(cmdList, CBV, 2);

	for (int i = 0; i < UPDATE_PER_FRAME; ++i)
	{
		//static const int THREAD_NUM = 8;
		//cmdList->Dispatch(VERTEX_NUM_Y / THREAD_NUM, VERTEX_NUM_X / THREAD_NUM, 1);
		cmdList->Dispatch(VERTEX_NUM_Y, VERTEX_NUM_X, 1);
	}
}

#include"ScrollMgr.h"
#include"ShakeMgr.h"
#include"LightManager.h"
void Muffler::Draw(LightManager& LigManager)
{
	Vec2<float> scrollShakeZoom = ScrollMgr::Instance()->scrollAmount + ShakeMgr::Instance()->shakeAmount;
	scrollShakeZoom.x *= ScrollMgr::Instance()->zoom;
	scrollShakeZoom.y *= ScrollMgr::Instance()->zoom;

	static ZoomAndScroll ZOOM_AND_SCROLL;
	if (ZOOM_AND_SCROLL.zoom != ScrollMgr::Instance()->zoom || ZOOM_AND_SCROLL.scroll != scrollShakeZoom)
	{
		ZOOM_AND_SCROLL.zoom = ScrollMgr::Instance()->zoom;
		ZOOM_AND_SCROLL.scroll = scrollShakeZoom;
		zoomAndScroll->Mapping(&ZOOM_AND_SCROLL);
	}

	static std::vector<DESC_HANDLE_TYPE>descTypes = { CBV,CBV,CBV,SRV,SRV,SRV,SRV,SRV };
	std::vector<std::shared_ptr<DescriptorData>>descDatas =
	{
		KuroEngine::Instance().GetParallelMatProjBuff(),
		zoomAndScroll,
		LigManager.GetLigNumInfo(),
		LigManager.GetLigInfo(Light::DIRECTION),	//�f�B���N�V�������C�g
		LigManager.GetLigInfo(Light::POINT),	//�|�C���g���C�g
		LigManager.GetLigInfo(Light::SPOT),	//�X�|�b�g���C�g
		LigManager.GetLigInfo(Light::HEMISPHERE),	//�V�����C�g
		tex
	};

	KuroEngine::Instance().Graphics().SetPipeline(gPipeline);
	KuroEngine::Instance().Graphics().ObjectRender(vertBuff, idxBuff, descDatas, descTypes, 0.0f, true);
}
