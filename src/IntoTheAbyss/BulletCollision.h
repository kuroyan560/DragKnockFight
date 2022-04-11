#pragma once
#include"../Common/Singleton.h"
#include"../KuroEngine.h"
#include"../Common/Vec.h"

struct SphereCollision
{
	// 中心座標
	Vec2<float> *center;
	// 半径
	float radius = 1.0f;
};

class BulletCollision :public Singleton<BulletCollision>
{
public:

	/// <summary>
	/// 球と球の判定を取ります
	/// </summary>
	/// <param name="SPHERE_1">球データ1</param>
	/// <param name="SPHERE_2">球データ2</param>
	/// <returns>true...衝突する,false...衝突していない</returns>
	bool CheckSphereAndSphere(const SphereCollision &SPHERE_1, const SphereCollision &SPHERE_2);
};

