#pragma once
#include"Vec.h"
#include<array>
#include<vector>
static const enum MAP_CHIP_GENERATOR 
{
	NON_GENERATE,
	SPLINE_ORBIT,
	RAND_PATTERN,
	CLOSSING,
	MAP_CHIP_GENERATOR_NUM
};

//マップチップを能動的に生成する機能
class MapChipGenerator
{
protected:
	bool CanChange(const Vec2<int>& Idx);
	int GetRandChipType();
	void Generate(const Vec2<int>& GenerateIdx, const int& ChipType);
	void Generate(const Vec2<float>& GeneratePos, const int& ChipType);
public:
	virtual void Init() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;
};

class MapChipGenerator_Non : public MapChipGenerator
{
public:
	void Init()override {};
	void Update()override {};
	void Draw()override {};
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
protected:
	static const enum PATTERN_TYPE { CROSS, CUBE, CIRCLE, HORIZON, VERT, NUM };

	typedef std::vector<Vec2<int>>OffsetPattern;
	struct Prediction
	{
		Vec2<int>idx;
		int type;
	};
	std::vector<Prediction>predictionIdxArray;
	int span;
	int timer;
	int GetRandSpan();
	void DesideNextIndices(const PATTERN_TYPE& PatternType, const Vec2<int>& GenerateIdx);	//次の生成位置を決定

public:
	void Init()override;
	void Update()override;
	void Draw()override;
};

//マップチップをラウンドごとに切り替える
class MapChipGenerator_ChangeMap :public MapChipGenerator
{
	struct Prediction
	{
		Vec2<int>idx;
		int type;
	};
	std::vector<Prediction>predictionIdxArray;
public:
	void Init()override;
	void Update()override;
	void Draw()override;
	void RegisterMap();

private:
	int setMapNumber;
	int stageNumber;
};

//上下左右からマップチップが出現十字形で横切る
class MapChipGenerator_Crossing : public MapChipGenerator_RandPattern
{
	Vec2<int> generateLine;	//列生成する軸
public:
	void Init()override;
	void Update()override;
	void Draw()override;
};

//１辺からせり上がってくる
class MapChipGenerator_RiseUp : public MapChipGenerator
{

};