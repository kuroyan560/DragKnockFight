#pragma once
#include"../KuroEngine.h"
#include"../Engine/DrawFunc.h"
#include<array>

class StageRap
{
public:
	StageRap();

	void Init(int RAP_MAX_NUM);
	void Update();
	void Draw();

	void Increment();
private:
	Vec2<float>pos,size;
	int rapNum, rapMaxNum;


	int slashHandle;
	std::array<int, 12> numberHandle;
};
