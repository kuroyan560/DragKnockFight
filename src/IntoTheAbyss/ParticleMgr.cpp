#include "ParticleMgr.h"
#include"KuroEngine.h"

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
			InputLayoutParam("COLOR",DXGI_FORMAT_R32G32B32A32_FLOAT),
		};

		//ルートパラメータ
		static std::vector<RootParam>ROOT_PARAMETER =
		{
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"平行投影行列定数バッファ"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"ズームとスクロール"),
		};

		//レンダーターゲット描画先情報
		static std::vector<RenderTargetInfo>RENDER_TARGET_INFO =
		{
			//バックバッファのフォーマット、アルファブレンド
			RenderTargetInfo(D3D12App::Instance()->GetBackBuffFormat(),AlphaBlendMode_Trans),
		};

		//パイプライン生成
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

	// 生成する数分処理を回す。
	for (int generateCount = 0; generateCount < GENERATE_PARTICLE * par; ++generateCount) {

		// 生成する位置をランダムにずらす。
		Vec2<float> pos = generatePos;
		//pos.x += GetRand(6) - 3;
		//pos.y += GetRand(6) - 3;
		pos.x += KuroFunc::GetRand(6) - 3;
		pos.y += KuroFunc::GetRand(6) - 3;

		// 生成する。
		particles[idx++].Generate(pos, forwardVec);
		if (idx == MAX_NUM)idx = 0;
	}
	buff->Mapping(particles);
}

void ParticleMgr::GeneratePer(const Vec2<float>& generatePos, const Vec2<float>& forwardVec, const float& par, const int& generate)
{
	memcpy(particles, buff->GetRWStructuredBuff().lock()->GetResource().lock()->GetBuffOnCpu<Particle>(), MAX_NUM * sizeof(Particle));

	// 生成する数分処理を回す。
	for (int generateCount = 0; generateCount < generate * par; ++generateCount) {

		// 生成する位置をランダムにずらす。
		Vec2<float> pos = generatePos;
		pos.x += KuroFunc::GetRand(6) - 3;
		pos.y += KuroFunc::GetRand(6) - 3;

		// 生成する。
		particles[idx++].Generate(pos, forwardVec);
		if (idx == MAX_NUM)idx = 0;
	}
	buff->Mapping(particles);
}

void ParticleMgr::Particle::Generate(const Vec2<float>& generatePos, const Vec2<float>& forwardVec)
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
}
