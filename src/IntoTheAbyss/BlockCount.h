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

	inline bool GetNowNumber() {

		bool result = true;

		for (int index = 0; index < nowNumber.size(); ++index) {

			result &= nowNumber[index] == 0;

		}

		return result;
	}

private:
	std::vector<int> number;
	std::vector<int> maxNumber;
	std::vector<int> nowNumber;
	Vec2<float>texSize;
	int countBlockNum;


	Vec2<float>basePos;
};
