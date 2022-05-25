#pragma once
#include"Vec.h"
#include<array>
#include<vector>
static const enum MAP_CHIP_GENERATOR 
{
	SPLINE_ORBIT,
	RAND_PATTERN,
	CHANGE_MAP,
	MAP_CHIP_GENERATOR_NUM
};

//マップチップを能動的に生成する機能
class MapChipGenerator
{
protected:
	void Generate(const Vec2<float>& GeneratePos);
public:
	virtual void Init() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;
};

//とりあえずスプラインで動くように
class MapChipGenerator_SplineOrbit : public MapChipGenerator
{
private:
	const float RADIUS = 64.0f;

	Vec2<float>pos;
	float t = 0.0f;
	static const int ARRAY_SIZE = 4;
	std::array<Vec2<float>, ARRAY_SIZE>targetPos;	//向かう先配列
	float GetDistRate();
	Vec2<float> GetRandPos();

public:
	void Init()override;
	void Update()override;
	void Draw()override;
};

//ランダムで生成これから出る位置に予測
class MapChipGenerator_RandPattern : public MapChipGenerator
{
	typedef std::vector<Vec2<int>>OffsetPattern;
	std::vector<Vec2<int>>predictionIdxArray;
	int span;
	int timer;
	int GetSpan();
	void DesideNextIndices();	//次の生成位置を決定

public:
	void Init()override;
	void Update()override;
	void Draw()override;
};

//マップチップを一定時間ごとに切り替える
class MapChipGenerator_ChangeMap :public MapChipGenerator
{
public:
	void Init()override;
	void Update()override;
	void Draw()override;

private:
	int changeMapTimer;
	int changeMapMaxTimer;

	int setMapNumber;
	int stageNumber;
	int maxRoomNumber;
};
