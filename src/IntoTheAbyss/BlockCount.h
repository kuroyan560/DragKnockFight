#pragma once
#include"StageMgr.h"
#include<vector>
#include"../Common/Vec.h"

class BlockCount
{
public:
	BlockCount();

	void Init();
	void Update();
	void Draw();

	std::vector<int> CountNumber(int TIME);

private:
	std::vector<int> number;
	std::vector<int> maxNumber;
	std::vector<int> nowNumber;
	Vec2<float>texSize;
	int countBlockNum;


	Vec2<float>basePos;
};
