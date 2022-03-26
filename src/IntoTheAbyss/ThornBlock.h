#pragma once
#include"../KuroEngine.h"

class ThornBlock
{
public:
	ThornBlock();
	void Init(const Vec2<float> &LEFT_UP_POS,const Vec2<float>&RIGHT_DOWN_POS);
	void Finalize();
	bool HitBox(const Vec2<float> &PLAYER_POS, const Vec2<float> &SIZE, const Vec2<float> &PLAYER_VEL, const Vec2<float> &PLAYER_PREV_POS);
	void Draw();

private:
	Vec2<float> leftUpPos;
	Vec2<float> size;
	Vec2<float>adjValue;//ìñÇΩÇËîªíËí≤êÆóp
};

