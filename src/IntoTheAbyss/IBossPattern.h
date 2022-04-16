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

struct BossLimitMoveData
{
	Vec2<float>startPos;
	Vec2<float>endPos;
	bool hitFlag;
};

/// <summary>
/// ボスの行動パターンに必要な情報を渡す
/// </summary>
class CharacterInterFace;
struct BossPatternData
{
	Vec2<float>*moveVel;
	std::array<BossBulletData, 100>bulltData;
	bool swingFlag;
	std::array<BossLimitMoveData, 8> nearLimmitLine;//ボスと壁との距離を調べる為の情報(近距離)
	std::array<BossLimitMoveData, 4> farLimmitLine; //ボスと壁との距離を調べる為の情報(遠距離)

	BossPatternData() :swingFlag(false)
	{

	}
};

/// <summary>
/// ボスのパターンの抽象クラス
/// </summary>
class IBossPattern
{
public:
	virtual ~IBossPattern() {};
	virtual void Init() = 0;
	virtual void Update(BossPatternData* Boss) = 0;
};

