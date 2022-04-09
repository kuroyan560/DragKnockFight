#pragma once
#include"../IntoTheAbyss/IBossPattern.h"

class BossPatternNormalMove :public IBossPattern
{
public:
	BossPatternNormalMove();
	~BossPatternNormalMove() {};

	void Init()override;
	void Update(BossPatternData *DATA)override;

private:
};

