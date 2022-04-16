#pragma once
#include "Singleton.h"
#include "Vec.h"
#include <memory>
#include <vector>
#include <array>

using namespace std;

// 弾クラス
class Bullet;

// 弾管理クラス
class BulletMgr : public Singleton<BulletMgr> {

public:

	/*-- 定数 --*/

	static const int BULLET_COUNT = 150;	// 弾の最大数
	//const int GENERATE_COUNT = 5;			// 一発で生成する数
	const int GENERATE_COUNT = 1;			// 一発で生成する数
	//const float SLIP_ONE_BULLET = 0.1f;		// 一発の角度のズレ
	const float SLIP_ONE_BULLET = 0.0f;		// 一発の角度のズレ


public:

	/*-- メンバ変数 --*/

	array<shared_ptr<Bullet>, BULLET_COUNT> bullets;	// 弾クラス


public:

	/*-- メンバ関数 --*/

	// セッティング処理 一番最初に一回だけ呼び出す
	void Setting();

	// 生成処理
	void Generate(const int& graphHandle, const Vec2<float>& generatePos, const float& forwardAngle, const bool& isShotRight = false);

	//初期化処理
	void Init();

	// 更新処理
	void Update();

	// 描画処理
	void Draw();

	// 弾の当たり判定
	void CheckHit(const vector<vector<int>>& mapData);

	const shared_ptr<Bullet>& GetBullet(const int& index) { return bullets[index]; };

};