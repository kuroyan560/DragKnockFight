#include "ParticleMgr.h"
#include"KuroEngine.h"

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
			InputLayoutParam("COLOR",DXGI_FORMAT_R32G32B32A32_FLOAT),
		};

		//���[�g�p�����[�^
		static std::vector<RootParam>ROOT_PARAMETER =
		{
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"���s���e�s��萔�o�b�t�@"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"�Y�[���ƃX�N���[��"),
		};

		//�����_�[�^�[�Q�b�g�`�����
		static std::vector<RenderTargetInfo>RENDER_TARGET_INFO =
		{
			//�o�b�N�o�b�t�@�̃t�H�[�}�b�g�A�A���t�@�u�����h
			RenderTargetInfo(D3D12App::Instance()->GetBackBuffFormat(),AlphaBlendMode_Trans),
		};

		//�p�C�v���C������
		gPipeline = D3D12App::Instance()->GenerateGraphicsPipeline(PIPELINE_OPTION, SHADERS, INPUT_LAYOUT, ROOT_PARAMETER, RENDER_TARGET_INFO);
	}

	zoomAndScroll = D3D12App::Instance()->GenerateConstantBuffer(sizeof(ZoomAndScroll), 1, &zoomAndScroll, "ParticleMgr - ZoomAndScroll");
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
void ParticleMgr::Draw()
{
	ZoomAndScroll info;
	info.zoom = ScrollMgr::Instance()->zoom;

	Vec2<float> scrollShakeZoom = ScrollMgr::Instance()->scrollAmount + ShakeMgr::Instance()->shakeAmount;
	scrollShakeZoom.x *= ScrollMgr::Instance()->zoom;
	scrollShakeZoom.y *= ScrollMgr::Instance()->zoom;
	info.scroll = scrollShakeZoom;
	zoomAndScroll->Mapping(&info);

	KuroEngine::Instance().Graphics().SetPipeline(gPipeline);
	KuroEngine::Instance().Graphics().ObjectRender(buff,
		{ KuroEngine::Instance().GetParallelMatProjBuff(),zoomAndScroll },
		{ CBV,CBV }, 0.0f, true);
}

void ParticleMgr::Generate(const Vec2<float>& generatePos, const Vec2<float>& forwardVec, const float& par)
{
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
		particles[idx++].Generate(pos, forwardVec);
		if (idx == MAX_NUM)idx = 0;
	}
	buff->Mapping(particles);
}

void ParticleMgr::GeneratePer(const Vec2<float>& generatePos, const Vec2<float>& forwardVec, const float& par, const int& generate)
{
	memcpy(particles, buff->GetRWStructuredBuff().lock()->GetResource().lock()->GetBuffOnCpu<Particle>(), MAX_NUM * sizeof(Particle));

	// �������鐔���������񂷁B
	for (int generateCount = 0; generateCount < generate * par; ++generateCount) {

		// ��������ʒu�������_���ɂ��炷�B
		Vec2<float> pos = generatePos;
		pos.x += KuroFunc::GetRand(6) - 3;
		pos.y += KuroFunc::GetRand(6) - 3;

		// ��������B
		particles[idx++].Generate(pos, forwardVec);
		if (idx == MAX_NUM)idx = 0;
	}
	buff->Mapping(particles);
}

void ParticleMgr::Particle::Generate(const Vec2<float>& generatePos, const Vec2<float>& forwardVec)
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
}
