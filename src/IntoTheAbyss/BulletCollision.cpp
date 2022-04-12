#include "BulletCollision.h"

bool BulletCollision::CheckSphereAndSphere(const SphereCollision &SPHERE_1, const SphereCollision &SPHERE_2)
{
	Vec2<float> tmp;
	tmp.x = SPHERE_1.center->x - SPHERE_2.center->x;
	tmp.y = SPHERE_1.center->y - SPHERE_2.center->y;

	float distance = sqrtf(pow(tmp.x, 2) + pow(tmp.y, 2));
	float sumRadist = SPHERE_1.radius + SPHERE_2.radius;

	return (distance <= sumRadist);
}