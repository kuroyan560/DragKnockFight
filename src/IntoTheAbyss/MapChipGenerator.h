#pragma once
#include"Vec.h"
#include<array>
//マップチップを能動的に生成する機能
class MapChipGenerator
{
public:
	void Update(const Vec2<float>& GeneratePos);
};

//とりあえずスプラインで動くように
struct MapChipGenerator_Test
{
private:
	const float RADIUS = 64.0f;

	Vec2<float>pos;
	float t = 0.0f;
	static const int ARRAY_SIZE = 4;
	std::array<Vec2<float>, ARRAY_SIZE>targetPos;	//向かう先配列
	MapChipGenerator generator;
	float GetDistRate();
	Vec2<float> GetRandPos();

public:
	void Init();
	void Update();
	void Draw();
};