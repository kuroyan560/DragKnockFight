#pragma once
#include"../KuroEngine.h"

enum E_DOOR_DIR
{
	DOOR_UP_GORIGHT,		//上に出て右に行くドア
	DOOR_UP_GOLEFT,	//上に出て左に行くドア
	DOOR_DOWN,	//下に出るドア
	DOOR_LEFT,	//左に出るドア
	DOOR_RIGHT,	//右に出るドア
	DOOR_Z,		//真ん中から出るドア
	DOOR_NONE,		//ドア無し
	DOOR_MAX
};

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
	E_DOOR_DIR doorDir;
private:
	Vec2<float> leftUpPos;
	Vec2<float> size;
};

