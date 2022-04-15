#pragma once
#include"../IntoTheAbyss/IBossPattern.h"
#include"../Common/Vec.h"

class BossPatternSwing :public IBossPattern
{
public:
	BossPatternSwing();
	~BossPatternSwing() {};

	void Init()override;
	void Update(BossPatternData *DATA)override;

private:
	bool swingFlag;
	bool initSwingFlag;
};

