#pragma once
#include"../Common/Vec.h"

/// <summary>
/// ボスの行動パターンに必要な情報を渡す
/// </summary>
struct BossPatternData
{
	Vec2<float>*bossPos;
	int *stuckWindowTimer;
};

/// <summary>
/// ボスのパターンの抽象クラス
/// </summary>
class IBossPattern
{
public:
	virtual ~IBossPattern() {};
	virtual void Init() = 0;
	virtual void Update(BossPatternData *POS) = 0;
};

