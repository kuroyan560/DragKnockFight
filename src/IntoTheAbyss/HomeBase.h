#pragma once
#include"../KuroEngine.h"
#include"../Common/Primitive.h"

class HomeBase
{
public:
	HomeBase();

	void Init(const Vec2<float> &LEFT_UP_POS, const Vec2<float> &RIGHT_DOWN_POS);
	bool Collision(const Sphere &OBJ_A);
	void Draw();
	void Debug();

private:
	Vec2<float>leftUpPos, rightDownPos;
	static int AREA_NUM;
	std::string name;
};
