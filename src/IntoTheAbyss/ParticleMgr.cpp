#include "ParticleMgr.h"
#include"KuroEngine.h"
#include"CharacterInterFace.h"

void ParticleMgr::Particle::Generate(const Vec2<float>& GeneratePos, const Vec2<float>& EmitVec, const int& Type, const int& TexIdx, const Color& MulColor)
{
	mulColor = MulColor;
	pos = GeneratePos;
	emitPos = pos;
	// ���o������ݒ�
	emitVec = EmitVec;
	type = Type;
	texIdx = TexIdx;
	isAlive = 1;
	life = 0;
	alpha = 1.0f;

	if (type == PARTICLE_CUMPUTE_TYPE::NORMAL_SMOKE)
	{
		//�����̔򋗗��Ƃ��Ĉ���
		speed = KuroFunc::GetRand(20.0f, 46.0f);
		emitSpeed = speed;

		scale = 29;
		emitScale = scale;
		radian = 0.0f;
		//��]�p�x�̖ڕW�l�Ƃ��Ĉ���
		emitRadian = Angle::ConvertToRadian(KuroFunc::GetRand(0.0f, 360.0f * 2) * KuroFunc::GetRandPlusMinus());

		lifeSpan = KuroFunc::GetRand(25, 35);
	}
	else if (type == PARTICLE_CUMPUTE_TYPE::FAST_SMOKE)
	{
		//�����̔򋗗��Ƃ��Ĉ���
		speed = KuroFunc::GetRand(150.0f, 200.0f);
		emitSpeed = speed;

		scale = 29;
		emitScale = scale;
		radian = 0.0f;
		//��]�p�x�̖ڕW�l�Ƃ��Ĉ���
		emitRadian = Angle::ConvertToRadian(KuroFunc::GetRand(0.0f, 360.0f * 2) * KuroFunc::GetRandPlusMinus());

		lifeSpan = KuroFunc::GetRand(35, 45);
	}
	else if (type == PARTICLE_CUMPUTE_TYPE::EMIT_STAR)
	{
		//�����̔򋗗��Ƃ��Ĉ���
		speed = KuroFunc::GetRand(150.0f, 200.0f);
		emitSpeed = speed;

		scale = KuroFunc::GetRand(50.0f, 70.0f);
		emitScale = scale;
		radian = 0.0f;
		//��]�p�x�̖ڕW�l�Ƃ��Ĉ���
		emitRadian = Angle::ConvertToRadian(KuroFunc::GetRand(0.0f, 360.0f * 2) * KuroFunc::GetRandPlusMinus());

		lifeSpan = KuroFunc::GetRand(35, 45);
	}
	else if (type == PARTICLE_CUMPUTE_TYPE::SLIME_EXPLOSION)
	{
		//�����̔򋗗��Ƃ��Ĉ���
		//speed = KuroFunc::GetRand(300.0f, 400.0f);
		speed = 400.0f;
		emitSpeed = speed;

		scale = KuroFunc::GetRand(40.0f, 70.0f);
		emitScale = scale;
		radian = Angle::ConvertToRadian(KuroFunc::GetRand(360));
		emitRadian = Angle::ConvertToRadian(2);	//��]���x�Ƃ��Ďg��

		lifeSpan = 50;
	}
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
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"�Y�[���ƃX�N���[���ƃX���[")
		};
		cPipeline = D3D12App::Instance()->GenerateComputePipeline(cs, rootParams, WrappedSampler(false, false));
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
			InputLayoutParam("MUL_COLOR",DXGI_FORMAT_R32G32B32A32_FLOAT),
			InputLayoutParam("POSITION",DXGI_FORMAT_R32G32_FLOAT),
			InputLayoutParam("EMIT_POSITION",DXGI_FORMAT_R32G32_FLOAT),
			InputLayoutParam("EMIT_VEC",DXGI_FORMAT_R32G32_FLOAT),
			InputLayoutParam("SPEED",DXGI_FORMAT_R32_FLOAT),
			InputLayoutParam("EMIT_SPEED",DXGI_FORMAT_R32_FLOAT),
			InputLayoutParam("SCALE",DXGI_FORMAT_R32_FLOAT),
			InputLayoutParam("EMIT_SCALE",DXGI_FORMAT_R32_FLOAT),
			InputLayoutParam("RADIAN",DXGI_FORMAT_R32_FLOAT),
			InputLayoutParam("EMIT_RADIAN",DXGI_FORMAT_R32_FLOAT),
			InputLayoutParam("ALPHA",DXGI_FORMAT_R32_FLOAT),
			InputLayoutParam("LIFE",DXGI_FORMAT_R32_SINT),
			InputLayoutParam("LIFE_SPAN",DXGI_FORMAT_R32_SINT),
			InputLayoutParam("ALIVE",DXGI_FORMAT_R8_SINT),
			InputLayoutParam("TEX_IDX",DXGI_FORMAT_R32_UINT),
			InputLayoutParam("TYPE",DXGI_FORMAT_R32_UINT),
		};

		//���[�g�p�����[�^
		static std::vector<RootParam>ROOT_PARAMETER =
		{
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"���s���e�s��萔�o�b�t�@"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"�Y�[���ƃX�N���[���ƃX���["),
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
		gPipeline = D3D12App::Instance()->GenerateGraphicsPipeline(PIPELINE_OPTION, SHADERS, INPUT_LAYOUT, ROOT_PARAMETER, RENDER_TARGET_INFO, WrappedSampler(false, false));
	}

	zoomAndScroll = D3D12App::Instance()->GenerateConstantBuffer(sizeof(GameInfo), 1, &zoomAndScroll, "ParticleMgr - ZoomAndScroll");

	textures[WHITE] = D3D12App::Instance()->GenerateTextureBuffer(Color(1.0f, 1.0f, 1.0f, 1.0f), DXGI_FORMAT_R32G32B32A32_FLOAT, TEX_SIZE);

	auto smokes = D3D12App::Instance()->GenerateTextureBuffer("resource/ChainCombat/smoke.png", SMOKE_NUM, { SMOKE_NUM,1 });
	for (int i = 0; i < SMOKE_NUM; ++i)
	{
		textures[SMOKE_0 + i] = smokes[i];
	}

	textures[STAR] = D3D12App::Instance()->GenerateTextureBuffer("resource/ChainCombat/star.png");
	textures[SLIME] = D3D12App::Instance()->GenerateTextureBuffer("resource/ChainCombat/slime.png");
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
#include"SlowMgr.h"
void ParticleMgr::Draw()
{
	Vec2<float> scrollShakeZoom = ScrollMgr::Instance()->scrollAmount + ShakeMgr::Instance()->shakeAmount;
	scrollShakeZoom.x *= ScrollMgr::Instance()->zoom;
	scrollShakeZoom.y *= ScrollMgr::Instance()->zoom;
	Vec2<float>scroll = -scrollShakeZoom + ScrollMgr::Instance()->lineCenterOffset;

	static GameInfo ZOOM_AND_SCROLL;
	if (ZOOM_AND_SCROLL.zoom != ScrollMgr::Instance()->zoom || ZOOM_AND_SCROLL.scroll != scroll || ZOOM_AND_SCROLL.gameSpeed != SlowMgr::Instance()->slowAmount)
	{
		ZOOM_AND_SCROLL.zoom = ScrollMgr::Instance()->zoom;
		ZOOM_AND_SCROLL.scroll = scroll;
		ZOOM_AND_SCROLL.gameSpeed = SlowMgr::Instance()->slowAmount;
		zoomAndScroll->Mapping(&ZOOM_AND_SCROLL);
	}

	static std::vector<DESC_HANDLE_TYPE>descTypes = { CBV,CBV };
	std::vector<std::shared_ptr<DescriptorData>>descDatas =
	{
		KuroEngine::Instance().GetParallelMatProjBuff(),
		zoomAndScroll,
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

void ParticleMgr::EmitParticle(const Vec2<float>& EmitPos, const Vec2<float>& EmitVec, const int& Type, const int& TexIdx, const Color& MulColor)
{
	particles[idx++].Generate(EmitPos, EmitVec, Type, TexIdx, MulColor);
	if (idx == MAX_NUM)idx = 0;
}

void ParticleMgr::Generate(const Vec2<float>& EmitPos, const Vec2<float>& EmitVec, const PARTICLE_TYPE& Type)
{
	//Copy recently particle's status.
	memcpy(particles, buff->GetRWStructuredBuff().lock()->GetResource().lock()->GetBuffOnCpu<Particle>(), MAX_NUM * sizeof(Particle));

	if (Type == BULLET)
	{
		static const int EMIT_MAX = 25;
		static const int EMIT_MIN = 15;
		const int num = KuroFunc::GetRand(EMIT_MIN, EMIT_MAX);

		static const float EMIT_DEGREE = 100;	//���o�p�x
		const Vec2<float>startEmitVec = KuroMath::RotateVec2(-EmitVec, Angle::ConvertToRadian(-EMIT_DEGREE / 2.0f));

		for (int i = 0; i < num; ++i)
		{
			const Vec2<float>emitVec = KuroMath::RotateVec2(startEmitVec, Angle::ConvertToRadian(KuroFunc::GetRand(EMIT_DEGREE)));
			EmitParticle(EmitPos, emitVec, PARTICLE_CUMPUTE_TYPE::NORMAL_SMOKE, PARTICLE_TEX::SMOKE_0 + KuroFunc::GetRand(SMOKE_NUM - 1));
		}
	}
	else if (Type == CRASH_R || Type == CRASH_G || Type == CRASH_W)
	{
		Color mulCol;
		if (Type == CRASH_R)mulCol = CharacterInterFace::TEAM_COLOR[RIGHT_TEAM];
		else if (Type == CRASH_G)mulCol = CharacterInterFace::TEAM_COLOR[LEFT_TEAM];

		const int smokeNum = KuroFunc::GetRand(15, 25);

		static const float SMOKE_EMIT_DEGREE = 45;
		const Vec2<float>smokeStartEmitVec = KuroMath::RotateVec2(-EmitVec, Angle::ConvertToRadian(-SMOKE_EMIT_DEGREE / 2.0f));

		Vec2<float>emitVec;
		for (int i = 0; i < smokeNum; ++i)
		{
			emitVec = KuroMath::RotateVec2(smokeStartEmitVec, Angle::ConvertToRadian(KuroFunc::GetRand(SMOKE_EMIT_DEGREE)));
			EmitParticle(EmitPos, emitVec, PARTICLE_CUMPUTE_TYPE::FAST_SMOKE, PARTICLE_TEX::SMOKE_0 + KuroFunc::GetRand(SMOKE_NUM - 1), mulCol);
		}

		static const int STAR_EMIT_MAX = 5;
		static const int STAR_EMIT_MIN = 2;
		const int starNum = KuroFunc::GetRand(STAR_EMIT_MIN, STAR_EMIT_MAX);
		const float starRadianUint = SMOKE_EMIT_DEGREE / starNum;

		for (int i = 0; i < starNum; ++i)
		{
			emitVec = KuroMath::RotateVec2(smokeStartEmitVec, Angle::ConvertToRadian(starRadianUint * i));
			EmitParticle(EmitPos, emitVec, PARTICLE_CUMPUTE_TYPE::EMIT_STAR, PARTICLE_TEX::STAR);
		}
	}
	else if (Type == BOUND)
	{
		const int num = 18;
		float radUint = Angle::ROUND() / num;

		for (int i = 0; i < num; ++i)
		{
			const Vec2<float>emitVec(cos(radUint * i), sin(radUint * i));
			EmitParticle(EmitPos, emitVec, PARTICLE_CUMPUTE_TYPE::SLIME_EXPLOSION, PARTICLE_TEX::SLIME);
		}
	}
	
	//Send particles's info which also contains new particles.
	buff->Mapping(particles);
}