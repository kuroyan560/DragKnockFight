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
	const float SHOT_BULLET_MAX;//ˆê“x‚ÉŒ‚‚Â’e‚Ì‘”
	const float MAX_SPEED;//ˆê“x‚ÉŒ‚‚Â’e‚Ì‘”
	const int SHOT_INTERVAL_MAX;//’e‚ğŒ‚‚ÂŠÔŠu

};

