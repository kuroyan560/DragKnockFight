#include "ParticleMgr.h"
#include"KuroEngine.h"

void ParticleMgr::Particle::Generate(const Vec2<float>& generatePos, const Vec2<float>& forwardVec, const int& TexIdx)
{
	static const float SCALE = 6.0f;	//HLSL���ƍ��킹��K�v������
	static const float DEF_SPEED = 3.0f;

	// ���W��ݒ�
	pos = generatePos;

	// �d�Ȃ�Ȃ��悤�ɐ؂�グ����B
	pos.x = RoundUp(pos.x, SCALE);
	pos.y = RoundUp(pos.y, SCALE);
	scale = SCALE;

	// �i�s������ݒ�
	this->forwardVec = forwardVec;

	// �ړ������ʂ�������
	movedVel = {};

	// �A���t�@�l��������
	alpha = 255.0f;

	// �ړ��ʂ�������
	speed = KuroFunc::GetRand(DEF_SPEED * 100) / 100.0f;

	// �����t���O��������
	isAlive = 1;

	texIdx = TexIdx;
}

ParticleMgr::ParticleMgr()
{
	buff = D3D12App::Instance()->GenerateVertexBuffer(sizeof(Particle), MAX_NUM, particles, "ParticleMgr - VertexBuffer", true);

	//�R���s���[�g�p�C�v���C��
	{
		//�V�F�[�_
		auto cs = D3D12App::Instance()->CompileShader("resource/HLSL/ParticleMgr.hlsl", "CSmain", "cs_5_0");
		//���[�g�p�����[�^
		std::vector<RootParam>rootParams =
		{
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_UAV,"�p�[�e�B�N���z��"),
		};
		cPipeline = D3D12App::Instance()->GenerateComputePipeline(cs, rootParams);
	}

	//�O���t�B�b�N�X�p�C�v���C��
	{
		//�p�C�v���C���ݒ�
		static PipelineInitializeOption PIPELINE_OPTION(D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT, D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
		PIPELINE_OPTION.depthTest = false;

		//�V�F�[�_�[���
		static Shaders SHADERS;
		SHADERS.vs = D3D12App::Instance()->CompileShader("resource/HLSL/ParticleMgr.hlsl", "VSmain", "vs_5_0");
		SHADERS.gs = D3D12App::Instance()->CompileShader("resource/HLSL/ParticleMgr.hlsl", "GSmain", "gs_5_0");
		SHADERS.ps = D3D12App::Instance()->CompileShader("resource/HLSL/ParticleMgr.hlsl", "PSmain", "ps_5_0");

		//�C���v�b�g���C�A�E�g
		static std::vector<InputLayoutParam>INPUT_LAYOUT =
		{
			InputLayoutParam("POSITION",DXGI_FORMAT_R32G32_FLOAT),
			InputLayoutParam("FORWARD_VEC",DXGI_FORMAT_R32G32_FLOAT),
			InputLayoutParam("MOVED_VEL",DXGI_FORMAT_R32G32_FLOAT),
			InputLayoutParam("ALPHA",DXGI_FORMAT_R32_FLOAT),
			InputLayoutParam("SPEED",DXGI_FORMAT_R32_FLOAT),
			InputLayoutParam("ALIVE",DXGI_FORMAT_R8_SINT),
			InputLayoutParam("SCALE",DXGI_FORMAT_R32_FLOAT),
			InputLayoutParam("TEX_IDX",DXGI_FORMAT_R32_UINT),
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
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"�e�N�X�`����� - 0"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"�e�N�X�`����� - 1"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"�e�N�X�`����� - 2"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"�e�N�X�`����� - 3"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"�e�N�X�`����� - 4"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"�e�N�X�`����� - 5"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"�e�N�X�`����� - 6"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"�e�N�X�`����� - 7"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"�e�N�X�`����� - 8"),
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

	zoomAndScroll = D3D12App::Instance()->GenerateConstantBuffer(sizeof(ZoomAndScroll), 1, &zoomAndScroll, "ParticleMgr - ZoomAndScroll");

	textures[WHITE] = D3D12App::Instance()->GenerateTextureBuffer(Color(1.0f, 1.0f, 1.0f, 1.0f), DXGI_FORMAT_R32G32B32A32_FLOAT, TEX_SIZE);

	auto smokes = D3D12App::Instance()->GenerateTextureBuffer("resource/IntoTheAbyss/Particle/smoke.png", SMOKE_NUM, { SMOKE_NUM,1 });
	for (int i = 0; i < SMOKE_NUM; ++i)
	{
		textures[SMOKE_0 + i] = smokes[i];
	}

	auto smokes_enchant = D3D12App::Instance()->GenerateTextureBuffer("resource/IntoTheAbyss/Particle/smoke_enchant.png", SMOKE_NUM, { SMOKE_NUM,1 });
	for (int i = 0; i < SMOKE_NUM; ++i)
	{
		textures[SMOKE_E_0 + i] = smokes_enchant[i];
	}
}

void ParticleMgr::Init()
{
	idx = 0;
	for (auto& p : particles)
	{
		p.isAlive = 0;
	}
	buff->Mapping(particles);
}

void ParticleMgr::Update()
{
	D3D12App::Instance()->SetDescHeaps();

	auto cmdList = D3D12App::Instance()->GetCmdList();
	cPipeline->SetPipeline(cmdList);
	buff->GetRWStructuredBuff().lock()->SetComputeDescriptorBuffer(cmdList, UAV, 0);

	static const int THREAD_NUM = 10;
	const UINT thread = MAX_NUM / THREAD_NUM;
	cmdList->Dispatch(thread, 1, 1);
}

#include"ScrollMgr.h"
#include"ShakeMgr.h"
#include"LightManager.h"
void ParticleMgr::Draw(LightManager& LigManager)
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

	static std::vector<DESC_HANDLE_TYPE>descTypes = { CBV,CBV,CBV,SRV,SRV,SRV,SRV };
	std::vector<std::shared_ptr<DescriptorData>>descDatas =
	{
		KuroEngine::Instance().GetParallelMatProjBuff(),
		zoomAndScroll,
		LigManager.GetLigNumInfo(),
		LigManager.GetLigInfo(Light::DIRECTION),	//�f�B���N�V�������C�g
		LigManager.GetLigInfo(Light::POINT),	//�|�C���g���C�g
		LigManager.GetLigInfo(Light::SPOT),	//�X�|�b�g���C�g
		LigManager.GetLigInfo(Light::HEMISPHERE),	//�V�����C�g
	};
	for (int i = 0; i < PARTICLE_TEX::TEX_NUM; ++i)
	{
		descDatas.emplace_back(textures[i]);
		descTypes.emplace_back(SRV);
	}

	KuroEngine::Instance().Graphics().SetPipeline(gPipeline);
	KuroEngine::Instance().Graphics().ObjectRender(buff,
		descDatas,
		descTypes, 1.0f, true);
}

int ParticleMgr::GetTex(const PARTICLE_TYPE& Type)
{
	int tex = PARTICLE_TEX::WHITE;
	if (Type == FIRST_DASH)
	{
		tex = KuroFunc::GetRand(SMOKE_NUM - 1) + PARTICLE_TEX::SMOKE_0;
	}
	else if (Type == FISRT_DASH_DOUJI)
	{
		tex = KuroFunc::GetRand(SMOKE_NUM - 1) + PARTICLE_TEX::SMOKE_E_0;
	}
	return tex;
}

void ParticleMgr::Generate(const Vec2<float>& generatePos, const Vec2<float>& forwardVec, const PARTICLE_TYPE& Type, const float& par)
{
	//Copy recently particle's status.
	memcpy(particles, buff->GetRWStructuredBuff().lock()->GetResource().lock()->GetBuffOnCpu<Particle>(), MAX_NUM * sizeof(Particle));

	// �������鐔���������񂷁B
	for (int generateCount = 0; generateCount < GENERATE_PARTICLE * par; ++generateCount) {

		// ��������ʒu�������_���ɂ��炷�B
		Vec2<float> pos = generatePos;
		//pos.x += GetRand(6) - 3;
		//pos.y += GetRand(6) - 3;
		pos.x += KuroFunc::GetRand(6) - 3;
		pos.y += KuroFunc::GetRand(6) - 3;

		// ��������B
		particles[idx++].Generate(pos, forwardVec, GetTex(Type));
		if (idx == MAX_NUM)idx = 0;
	}

	//Send particles's info which also contains new particles.
	buff->Mapping(particles);
}

void ParticleMgr::GeneratePer(const Vec2<float>& generatePos, const Vec2<float>& forwardVec, const PARTICLE_TYPE& Type, const float& par, const int& generate)
{
	//Copy recently particle's status.
	memcpy(particles, buff->GetRWStructuredBuff().lock()->GetResource().lock()->GetBuffOnCpu<Particle>(), MAX_NUM * sizeof(Particle));

	// �������鐔���������񂷁B
	for (int generateCount = 0; generateCount < generate * par; ++generateCount) {

		// ��������ʒu�������_���ɂ��炷�B
		Vec2<float> pos = generatePos;
		pos.x += KuroFunc::GetRand(6) - 3;
		pos.y += KuroFunc::GetRand(6) - 3;

		// ��������B
		particles[idx++].Generate(pos, forwardVec, GetTex(Type));
		if (idx == MAX_NUM)idx = 0;
	}

	//Send particles's info which also contains new particles.
	buff->Mapping(particles);
}

