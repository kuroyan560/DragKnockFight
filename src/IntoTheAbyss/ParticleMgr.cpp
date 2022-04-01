#include "ParticleMgr.h"
#include"KuroEngine.h"

void ParticleMgr::Particle::Generate(const Vec2<float>& generatePos, const Vec2<float>& forwardVec, const int& TexIdx)
{
	static const float SCALE = 6.0f;	//HLSL側と合わせる必要がある
	static const float DEF_SPEED = 3.0f;

	// 座標を設定
	pos = generatePos;

	// 重ならないように切り上げする。
	pos.x = RoundUp(pos.x, SCALE);
	pos.y = RoundUp(pos.y, SCALE);
	scale = SCALE;

	// 進行方向を設定
	this->forwardVec = forwardVec;

	// 移動した量を初期化
	movedVel = {};

	// アルファ値を初期化
	alpha = 255.0f;

	// 移動量を初期化
	speed = KuroFunc::GetRand(DEF_SPEED * 100) / 100.0f;

	// 生存フラグを初期化
	isAlive = 1;

	texIdx = TexIdx;
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
			InputLayoutParam("FORWARD_VEC",DXGI_FORMAT_R32G32_FLOAT),
			InputLayoutParam("MOVED_VEL",DXGI_FORMAT_R32G32_FLOAT),
			InputLayoutParam("ALPHA",DXGI_FORMAT_R32_FLOAT),
			InputLayoutParam("SPEED",DXGI_FORMAT_R32_FLOAT),
			InputLayoutParam("ALIVE",DXGI_FORMAT_R8_SINT),
			InputLayoutParam("SCALE",DXGI_FORMAT_R32_FLOAT),
			InputLayoutParam("TEX_IDX",DXGI_FORMAT_R32_UINT),
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

	// 生成する数分処理を回す。
	for (int generateCount = 0; generateCount < GENERATE_PARTICLE * par; ++generateCount) {

		// 生成する位置をランダムにずらす。
		Vec2<float> pos = generatePos;
		//pos.x += GetRand(6) - 3;
		//pos.y += GetRand(6) - 3;
		pos.x += KuroFunc::GetRand(6) - 3;
		pos.y += KuroFunc::GetRand(6) - 3;

		// 生成する。
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

	// 生成する数分処理を回す。
	for (int generateCount = 0; generateCount < generate * par; ++generateCount) {

		// 生成する位置をランダムにずらす。
		Vec2<float> pos = generatePos;
		pos.x += KuroFunc::GetRand(6) - 3;
		pos.y += KuroFunc::GetRand(6) - 3;

		// 生成する。
		particles[idx++].Generate(pos, forwardVec, GetTex(Type));
		if (idx == MAX_NUM)idx = 0;
	}

	//Send particles's info which also contains new particles.
	buff->Mapping(particles);
}

