#pragma once
#include"../Common/Singleton.h"
#include"../KuroEngine.h"
#include"../Common/Vec.h"

struct SphereCollision
{
	// ���S���W
	Vec2<float> *center;
	// ���a
	float radius = 1.0f;
};

class BulletCollision :public Singleton<BulletCollision>
{
public:

	/// <summary>
	/// ���Ƌ��̔�������܂�
	/// </summary>
	/// <param name="SPHERE_1">���f�[�^1</param>
	/// <param name="SPHERE_2">���f�[�^2</param>
	/// <returns>true...�Փ˂���,false...�Փ˂��Ă��Ȃ�</returns>
	bool CheckSphereAndSphere(const SphereCollision &SPHERE_1, const SphereCollision &SPHERE_2);
};

