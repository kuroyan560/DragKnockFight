#pragma once
#include"../KuroEngine.h"

class DoorBlock
{
public:
	DoorBlock();

	void Init(const Vec2<float> &LEFT_UP_POS, const Vec2<float> &RIGHT_DOWN_POS, const int &DOOR_CHIP_NUM);
	void Finalize();
	bool Collision(const Vec2<float> &PLAYER_POS, const Vec2<float> &SIZE, const Vec2<float> &PLAYER_VEL, const Vec2<float> &PLAYER_PREV_POS);

	int chipNumber;
	Vec2<float>responePos;
	Vec2<float>restartPos;
private:
	Vec2<float> leftUpPos;
	Vec2<float> size;
};

