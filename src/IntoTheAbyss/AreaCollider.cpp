#include "AreaCollider.h"

bool AreaCollider::CheckHitArea(const Square &OBJ_1, const Square &OBJ_2)
{
	Vec2<float> adjPos[2] = { OBJ_1.size,OBJ_2.size };
	bool isHit = fabs(OBJ_1.center->x - OBJ_2.center->x) <= adjPos[0].x + adjPos[1].x
		&& fabs(OBJ_1.center->y - OBJ_2.center->y) <= adjPos[0].y + adjPos[1].y;
	return isHit;
}