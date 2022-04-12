#pragma once
#include"../Common/Singleton.h"
#include"../Common/Vec.h"
#include"Bullet.h"
#include<vector>
#include<array>
#include<memory>

using namespace std;

class BossBulletManager :public Singleton<BossBulletManager>
{
	int bulletGraph;

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
	BossBulletManager();

	// 生成処理
	void Generate(const Vec2<float> &generatePos, const float &forwardAngle, const float &speed);

	// 更新処理
	void Update();

	// 描画処理
	void Draw();

	// 弾の当たり判定
	//void CheckHit(const vector<vector<int>> &mapData, vector<Bubble> &bubble);

	const shared_ptr<Bullet> &GetBullet(const int &index) { return bullets[index]; };

};

