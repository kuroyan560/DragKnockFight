#pragma once
#include"../KuroEngine.h"
#include"../Engine/DrawFunc.h"
#include<array>

class StageRap
{
public:
	StageRap();

	void Init();
	void Update();
	void Draw();

private:
	Vec2<float>pos,size;
	int rapNum, rapMaxNum;


	int slashHandle;
	std::array<int, 12> numberHandle;
};
