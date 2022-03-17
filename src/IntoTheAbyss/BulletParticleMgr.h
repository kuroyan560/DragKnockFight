#pragma once
#include "Singleton.h"
#include "Vec.h"
#include <memory>
#include <array>

using namespace std;

// 弾パーティクルクラス
class BulletParticle;

// 弾パーティクル管理クラス
class BulletParticleMgr : public Singleton<BulletParticleMgr> {

public:

	/*===== 定数 =====*/

	static const int MAX_PARTICLE = 300;						// 弾パーティクルの最大数
	const int GENERATE_PARTICLE = 5;							// 生成するパーティクル


public:

	/*===== メンバ変数 =====*/

	array<shared_ptr<BulletParticle>, MAX_PARTICLE> particle;	// 弾パーティクル


public:

	/*===== メンバ関数 =====*/

	// セッティング処理
	void Setting();

	// 初期化処理
	void Init();

	// 生成処理
	void Generate(const Vec2<float>& generatePos, const Vec2<float>& forwardVec);

	// 更新処理
	void Update();

	// 描画処理
	void Draw();

};