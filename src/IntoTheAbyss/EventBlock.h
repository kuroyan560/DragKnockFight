#pragma once
#include"../KuroEngine.h"

class EventBlock
{
public:
	EventBlock();
	void Init(const Vec2<float> &POS);
	void Finalize();
	bool HitBox(const Vec2<float>&PLAYER_POS);

private:
	Vec2<float>pos;
	bool initFlag;
};

