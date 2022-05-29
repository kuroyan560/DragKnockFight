#include "BackGroundParticle.h"
#include "KuroMath.h"
#include "KuroFunc.h"
#include "ScrollMgr.h"
#include "DrawFunc.h"
#include "TexHandleMgr.h"

float BackGroundParticle::lissajousTimer = 0;

BackGroundParticle::BackGroundParticle()
{

	/*===== コンストラクタ =====*/

	pos = {};
	exp = {};
	angle = {};
	isActive = false;
	timer = 0;

}

void BackGroundParticle::Init()
{

	/*===== 初期化処理 =====*/

	pos = {};
	exp = {};
	angle = {};
	isActive = false;
	timer = 0;
	desTimer = 0;

}

void BackGroundParticle::Generate(const Vec2<float>& Pos, const STATUS& Status, const Vec2<float>& StageSize)
{

	/*===== 初期化処理 =====*/

	pos = Pos;
	stageSize = StageSize;
	exp = {};
	angle = {};
	isActive = true;
	targetExp = { 1.0f,1.0f };
	timer = 0;
	desTimer = 0;
	defDesTimer = KuroFunc::GetRand(MAX_DES_TIMER / 2, MAX_DES_TIMER);
	status = Status;

	// ステータスが流れ星だったら。
	if (status == STATUS::SHOOTING_STAR) {

		defDesTimer = KuroFunc::GetRand(MAX_DES_TIMER / 4, MAX_DES_TIMER);
		speed = KuroFunc::GetRand(MIN_SPEED, MAX_SPEED);

	}

}

void BackGroundParticle::Update()
{

	/*===== 更新処理 =====*/

	static const int INIT_TIMER = 100000;	// 各タイマーがデカくなり過ぎたら初期化する用


	// 色々便利に使えるタイマー
	++timer;
	if (INIT_TIMER < timer) timer = 0;

	switch (status)
	{
	case BackGroundParticle::STATUS::STAY:

		/*===== その場にとどまるやつ =====*/

		// リサージュ曲線に使用するタイマーを更新。
		if (INIT_TIMER < lissajousTimer) lissajousTimer = 0;

		// タイマーが既定値に達したら回転させる。
		if (timer % 180 == 0) {

			angle = DirectX::XM_2PI * 10.0f;

		}

		// 再生成するまでのタイマーを更新。
		++desTimer;
		if (defDesTimer < desTimer) {

			Vec2<float> generatePos = {};
			generatePos.x = KuroFunc::GetRand(-stageSize.x, stageSize.x * 4.0f);
			generatePos.y = KuroFunc::GetRand(-stageSize.y, stageSize.y * 2.0f);

			Generate(generatePos, STATUS::STAY, stageSize);

		}
		// 再生成するまでのタイマーにある程度近づけたらサイズを小さくする。
		if (defDesTimer - 5.0f < desTimer) {

			targetExp = {};

		}

		// 大きさをデフォルトに近づける。
		exp.x += (targetExp.x - exp.x) / 5.0f;
		exp.y += (targetExp.y - exp.y) / 5.0f;
		angle -= angle / 5.0f;

		break;
	case BackGroundParticle::STATUS::SHOOTING_STAR:

		/*===== 流れ星 =====*/

		// 消えるまでのタイマーを更新
		++desTimer;
		if (defDesTimer < desTimer) {

			targetExp = {};

			// 大きさが一定以下になったら初期化。
			if (exp.x < 0.01f) Init();

		}

		// 大きさをデフォルトに近づける。
		exp.x += (targetExp.x - exp.x) / 5.0f;
		exp.y += (targetExp.y - exp.y) / 5.0f;

		// 移動させる。
		pos += Vec2<float>(-1, 1) * speed;

		// 回転させる。
		angle += 0.1f;


		break;
	default:
		break;
	}

}

void BackGroundParticle::Draw()
{

	/*===== 描画処理 =====*/

	static const int STAR_GRAPH = TexHandleMgr::LoadGraph("resource/ChainCombat/star.png");

	// 描画するリサージュ曲線のいちを求める。
	float lissajousMove = 50.0f;
	Vec2<float> lissajousCurve = Vec2<float>(cosf(1.0f * lissajousTimer) * lissajousMove, sinf(2.0f * lissajousTimer) * lissajousMove);

	// 現在のズーム量
	float zoom = ScrollMgr::Instance()->zoom;

	// 通常だったら
	if (status == STATUS::STAY) {

		DrawFunc::DrawRotaGraph2D(ScrollMgr::Instance()->Affect(pos + lissajousCurve), exp * Vec2<float>(zoom, zoom), angle, TexHandleMgr::GetTexBuffer(STAR_GRAPH));

	}
	// 流れ星だったら
	else {

		DrawFunc::DrawRotaGraph2D(ScrollMgr::Instance()->Affect(pos), exp * Vec2<float>(zoom, zoom), angle, TexHandleMgr::GetTexBuffer(STAR_GRAPH));

	}

}

BackGroundParticleMgr::BackGroundParticleMgr()
{

	defShootingStarGenerateTimer = 0;
	shootingStarGenerateTimer = 0;

}

void BackGroundParticleMgr::Init()
{

	/*===== 初期化処理 =====*/

	for (auto& index : particles) {

		index.Init();

	}

}

void BackGroundParticleMgr::StageStartGenerate(const Vec2<float>& StageSize)
{

	/*===== 生成処理 =====*/

	int generateCount = 150;
	int counter = 0;

	stageSize = StageSize;

	for (auto& index : particles) {

		if (index.isActive) continue;

		Vec2<float> generatePos;
		generatePos.x = KuroFunc::GetRand(-stageSize.x, stageSize.x * 4.0f);
		generatePos.y = KuroFunc::GetRand(-stageSize.y, stageSize.y * 2.0f);

		index.Generate(generatePos, BackGroundParticle::STATUS::STAY, StageSize);

		++counter;
		if (generateCount < counter) break;

	}

}

void BackGroundParticleMgr::Update()
{

	/*===== 更新処理 =====*/

	for (auto& index : particles) {

		if (!index.isActive) continue;

		index.Update();

	}

	// 流れ星を生成するタイマーを更新。
	++shootingStarGenerateTimer;
	if (defShootingStarGenerateTimer < shootingStarGenerateTimer) {

		shootingStarGenerateTimer = 0;

		// 次生成するまでのタイマーをランダムに決める。
		defShootingStarGenerateTimer = KuroFunc::GetRand(0, 180);

		for (auto& index : particles) {

			if (index.isActive) continue;

			Vec2<float> generatePos;
			generatePos.x = KuroFunc::GetRand(-stageSize.x, stageSize.x * 4.0f) + stageSize.x;
			generatePos.y = KuroFunc::GetRand(-stageSize.y, stageSize.y * 2.0f) - stageSize.y;

			index.Generate(generatePos, BackGroundParticle::STATUS::SHOOTING_STAR, stageSize);

			break;

		}


	}


	// パーティクルのリサージュ曲線の動きを更新。 staticにしちゃったのでここで更新します。
	BackGroundParticle::lissajousTimer += 0.01f;

}

void BackGroundParticleMgr::Draw()
{

	/*===== 描画処理 =====*/

	for (auto& index : particles) {

		if (!index.isActive) continue;

		index.Draw();

	}

}
