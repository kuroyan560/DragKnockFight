#pragma once
#include"../Common/Vec.h"
#include<vector>
#include<memory>
#include<array>

struct BossBulletData
{
	Vec2<float>pos;
	float speed;
	float dir;
	bool initFlag;
	BossBulletData() :initFlag(false)
	{
	};
	void Reset()
	{
		initFlag = false;
		dir = 0.0f;
		speed = 0.0f;
		pos = { 0.0f,0.0f };
	}
};

/// <summary>
/// ボスの行動パターンに必要な情報を渡す
/// </summary>
struct BossPatternData
{
	Vec2<float>*moveVel;
	int *stuckWindowTimer;
	
	std::array<BossBulletData, 100>bulltData;
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

