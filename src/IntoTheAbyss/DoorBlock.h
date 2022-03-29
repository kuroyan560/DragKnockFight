#pragma once
#include"../KuroEngine.h"

enum E_DOOR_DIR
{
	DOOR_UP_GORIGHT,		//��ɏo�ĉE�ɍs���h�A
	DOOR_UP_GOLEFT,	//��ɏo�č��ɍs���h�A
	DOOR_DOWN,	//���ɏo��h�A
	DOOR_LEFT,	//���ɏo��h�A
	DOOR_RIGHT,	//�E�ɏo��h�A
	DOOR_Z,		//�^�񒆂���o��h�A
	DOOR_NONE,		//�h�A����
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

