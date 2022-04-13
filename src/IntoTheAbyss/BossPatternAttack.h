#pragma once
#include"../IntoTheAbyss/IBossPattern.h"

/// <summary>
/// ’e–‹‚ğŒ‚‚Âƒpƒ^[ƒ“
/// </summary>
class BossPatternAttack:public IBossPattern
{
public:
	BossPatternAttack();
	~BossPatternAttack() {};

	void Init()override;
	void Update(BossPatternData *DATA)override;

private:
	int shotInterval;
	bool changeAngleFlag;
};

