#pragma once
#include"../Common/Singleton.h"
#include"../Common/Vec.h"

struct Square
{
	Vec2<float> *center;
	Vec2<float> size;
};

class AreaCollider :public Singleton<AreaCollider>
{
public:
	bool CheckHitArea(const Square &OBJ_1, const Square &OBJ_2);
};
