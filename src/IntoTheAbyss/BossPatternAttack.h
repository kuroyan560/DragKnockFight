#pragma once
#include"../IntoTheAbyss/IBossPattern.h"

class BossPatternAttack:public IBossPattern
{
public:
	BossPatternAttack();
	~BossPatternAttack() {};

	void Init()override;
	void Update(BossPatternData *DATA)override;
};

