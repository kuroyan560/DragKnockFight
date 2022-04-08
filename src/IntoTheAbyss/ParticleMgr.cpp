#include "ParticleMgr.h"
#include"KuroEngine.h"

void ParticleMgr::Particle::Generate(const Vec2<float>& GeneratePos, const Vec2<float>& EmitVec, const int& Type, const int& TexIdx)
{
	type = Type;
	texIdx = TexIdx;
	if (type == PARTICLE_CUMPUTE_TYPE::EXPLODE)
	{
		pos = GeneratePos;
		emitPos = pos;

		// 放出方向を設定
		emitVec = EmitVec;

		//爆発の飛距離として扱う
		speed = KuroFunc::GetRand(25.0f, 40.0f);
		emitSpeed = speed;

		scale = 29;
		emitScale = scale;
		radian = 0.0f;
		//回転角度の目標値として扱う
		emitRadian = Angle::ConvertToRadian(KuroFunc::GetRand(0.0f, 360.0f * 2));

		alpha = 1.0f;
		life = 0;
		lifeSpan = KuroFunc::GetRand(30, 40);
		isAlive = 1;
		type = 0;
	}
}

ParticleMgr::ParticleMgr()
{
	buff = D3D12App::Instance()->GenerateVertexBuffer(sizeof(Particle), MAX_NUM, particles, "ParticleMgr - VertexBuffer", true);

	//コンピュートパイプライン
	{
		//シェーダ
		auto cs = D3D12App::Instance()->CompileShader("resource/HLSL/ParticleMgr.hlsl", "CSmain", "cs_5_0");
		//ルートパラメータ
		std::vector<RootParam>rootParams =
		{
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_UAV,"パーティクル配列"),
		};
		cPipeline = D3D12App::Instance()->GenerateComputePipeline(cs, rootParams);
	}

	//グラフィックスパイプライン
	{
		//パイプライン設定
		static PipelineInitializeOption PIPELINE_OPTION(D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT, D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
		PIPELINE_OPTION.depthTest = false;

		//シェーダー情報
		static Shaders SHADERS;
		SHADERS.vs = D3D12App::Instance()->CompileShader("resource/HLSL/ParticleMgr.hlsl", "VSmain", "vs_5_0");
		SHADERS.gs = D3D12App::Instance()->CompileShader("resource/HLSL/ParticleMgr.hlsl", "GSmain", "gs_5_0");
		SHADERS.ps = D3D12App::Instance()->CompileShader("resource/HLSL/ParticleMgr.hlsl", "PSmain", "ps_5_0");

		//インプットレイアウト
		static std::vector<InputLayoutParam>INPUT_LAYOUT =
		{
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

		//ルートパラメータ
		static std::vector<RootParam>ROOT_PARAMETER =
		{
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"平行投影行列定数バッファ"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"ズームとスクロール"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"アクティブ中のライト数バッファ"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"ディレクションライト情報 (構造化バッファ)"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"ポイントライト情報 (構造化バッファ)"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"スポットライト情報 (構造化バッファ)"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"天球ライト情報 (構造化バッファ)"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"テクスチャ情報 - 0"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"テクスチャ情報 - 1"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"テクスチャ情報 - 2"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"テクスチャ情報 - 3"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"テクスチャ情報 - 4"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"テクスチャ情報 - 5"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"テクスチャ情報 - 6"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"テクスチャ情報 - 7"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"テクスチャ情報 - 8"),
		};

		//レンダーターゲット描画先情報
		static std::vector<RenderTargetInfo>RENDER_TARGET_INFO =
		{
			//バックバッファのフォーマット、アルファブレンド
			RenderTargetInfo(D3D12App::Instance()->GetBackBuffFormat(),AlphaBlendMode_Trans),
			//エミッシブ
			RenderTargetInfo(DXGI_FORMAT_R32G32B32A32_FLOAT,AlphaBlendMode_Trans)
		};

		//パイプライン生成
		gPipeline = D3D12App::Instance()->GenerateGraphicsPipeline(PIPELINE_OPTION, SHADERS, INPUT_LAYOUT, ROOT_PARAMETER, RENDER_TARGET_INFO);
	}

	zoomAndScroll = D3D12App::Instance()->GenerateConstantBuffer(sizeof(ZoomAndScroll), 1, &zoomAndScroll, "ParticleMgr - ZoomAndScroll");

	textures[WHITE] = D3D12App::Instance()->GenerateTextureBuffer(Color(1.0f, 1.0f, 1.0f, 1.0f), DXGI_FORMAT_R32G32B32A32_FLOAT, TEX_SIZE);

	auto smokes = D3D12App::Instance()->GenerateTextureBuffer("resource/ChainCombat/smoke.png", SMOKE_NUM, { SMOKE_NUM,1 });
	for (int i = 0; i < SMOKE_NUM; ++i)
	{
		textures[SMOKE_0 + i] = smokes[i];
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
		LigManager.GetLigInfo(Light::DIRECTION),	//ディレクションライト
		LigManager.GetLigInfo(Light::POINT),	//ポイントライト
		LigManager.GetLigInfo(Light::SPOT),	//スポットライト
		LigManager.GetLigInfo(Light::HEMISPHERE),	//天球ライト
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

void ParticleMgr::EmitParticle(const Vec2<float>& EmitPos, const Vec2<float>& EmitVec, const int& Type, const int& TexIdx)
{
	particles[idx++].Generate(EmitPos, EmitVec, Type, TexIdx);
	if (idx == MAX_NUM)idx = 0;
}

void ParticleMgr::Generate(const Vec2<float>& EmitPos, const Vec2<float>& EmitVec, const PARTICLE_TYPE& Type)
{
	//Copy recently particle's status.
	memcpy(particles, buff->GetRWStructuredBuff().lock()->GetResource().lock()->GetBuffOnCpu<Particle>(), MAX_NUM * sizeof(Particle));

	struct Info
	{
		Vec2<float>generatePos;
		Vec2<float>emitVec;
	};

	if (Type == BULLET)
	{
		static const int EMIT_MAX = 20;
		static const int EMIT_MIN = 10;
		const int num = KuroFunc::GetRand(EMIT_MIN, EMIT_MAX);

		static const float EMIT_DEGREE = 100;	//放出角度
		const Vec2<float>startEmitVec = KuroMath::RotateVec2(-EmitVec, Angle::ConvertToRadian(-EMIT_DEGREE / 2.0f));

		for (int i = 0; i < num; ++i)
		{
			const Vec2<float>emitVec = KuroMath::RotateVec2(startEmitVec, Angle::ConvertToRadian(KuroFunc::GetRand(EMIT_DEGREE)));
			EmitParticle(EmitPos, emitVec, PARTICLE_CUMPUTE_TYPE::DEFAULT, PARTICLE_TEX::SMOKE_0 + KuroFunc::GetRand(SMOKE_NUM - 1));
		}
	}
	
	//Send particles's info which also contains new particles.
	buff->Mapping(particles);
}