#pragma once
#include"../KuroEngine.h"
#include"ScrollMgr.h"

class EventBlock
{
public:
	EventBlock();
	void Init(const Vec2<float> &POS);
	void Finalize();
	bool HitBox(const Vec2<float> &PLAYER_POS, const Vec2<float> &SIZE);
	void Draw();

private:
	Vec2<float>pos;
	bool initFlag;
	static int arrayNum;
	int handle;
};

