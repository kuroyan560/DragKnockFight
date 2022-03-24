#pragma once
#include"Singleton.h"
#include<memory>
#include"Vec.h"
#include"Color.h"
#include"WinApp.h"

class VertexBuffer;
class ComputePipeline;
class GraphicsPipeline;
class ConstantBuffer;

class ParticleMgr : public Singleton<ParticleMgr>
{
	static const int MAX_NUM = 300;
	const int GENERATE_PARTICLE = 5;							// 生成するパーティクル

	struct Particle
	{
		// 切り上げ
		static int RoundUp(int size, int align) {
			return UINT(size + align - 1) & ~(align - 1);
		}

		Vec2<float> pos;			// 座標
		Vec2<float> forwardVec;	// 移動方向
		Vec2<float> movedVel;		// 移動した量
		float alpha;
		float speed;		// 移動速度
		char isAlive = 0;		// 生存フラグ
		float scale;
		Color color;	//色

		// 生成処理
		void Generate(const Vec2<float>& generatePos, const Vec2<float>& forwardVec);
	};
	struct ZoomAndScroll
	{
		float zoom;
		Vec2<float>scroll;
	};

	friend class Singleton<ParticleMgr>;
	ParticleMgr();

	//次に生成するパーティクルのインデックス
	int idx = 0;
	//パーティクルインスタンス
	Particle particles[MAX_NUM];
	//パーティクルの情報格納バッファ、描画に利用
	std::shared_ptr<VertexBuffer>buff;
	//パーティクルの更新を行うコンピュートパイプライン
	std::shared_ptr<ComputePipeline>cPipeline;
	//描画用パイプライン
	std::shared_ptr<GraphicsPipeline>gPipeline;

	//ズームとスクロールを送信するためのバッファ
	std::shared_ptr<ConstantBuffer>zoomAndScroll;

public:
	void Init();
	void Update();
	void Draw();

	// 生成処理
	void Generate(const Vec2<float>& generatePos, const Vec2<float>& forwardVec, const float& par = 1.0f);
	void GeneratePer(const Vec2<float>& generatePos, const Vec2<float>& forwardVec, const float& par = 1.0f, const int& generateCount = 5.0f);
};