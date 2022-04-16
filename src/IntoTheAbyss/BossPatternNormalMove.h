#pragma once
#include"../IntoTheAbyss/IBossPattern.h"
#include<vector>
#include<utility>
#include"../Common/Vec.h"

class BossPatternNormalMove :public IBossPattern
{
public:
	BossPatternNormalMove();
	~BossPatternNormalMove() {};

	void Init()override;
	void Update(BossPatternData *DATA)override;

private:
	int PULL_TIMER;
	const int MAP_CHIP_SIZE = 50;
	const int MAP_CHIP_HALF_SIZE = MAP_CHIP_SIZE / 2;
	bool CheckMapChipWallAndRay(const Vec2<float> &START_POS, const Vec2<float>&END_POS);
};

