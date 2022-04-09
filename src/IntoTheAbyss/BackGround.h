#pragma once
#include"Singleton.h"
#include"Vec.h"
#include<vector>
#include"Angle.h"
//îwåiÇ‚ëïè¸Ç»Ç«ìùäá
class BackGround : public Singleton<BackGround>
{
	friend class Singleton<BackGround>;
	BackGround();

	static const enum STAR_COLOR { RED, GREEN, YELLOW, BLUE, STAR_NUM };
	struct Star
	{
		Vec2<float>pos;
		Angle flashRad;
		STAR_COLOR type;
		float scaleOffset;
	};
	std::vector<Star>stars;
	int starGraph[STAR_NUM];

public:
	void Init(const Vec2<float>& StageSize);
	void Update();
	void Draw();
};

