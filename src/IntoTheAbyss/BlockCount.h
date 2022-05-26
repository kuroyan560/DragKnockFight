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

	// NowNumberが0かどうかをチェックする。
	inline bool CheckNowNomberIsZero() {

		bool result = true;

		const int NOWNUMBER_SIZE = nowNumber.size();
		for (int index = 0; index < NOWNUMBER_SIZE; ++index) {

			result &= nowNumber[index] == 0;

		}

		return result;
	}

	int countAllBlockNum;
	int countNowBlockNum;


private:
	std::vector<int> number;
	std::vector<int> maxNumber;
	std::vector<int> nowNumber;
	Vec2<float>texSize;
	int countBlockNum;


	Vec2<float>basePos;

	std::vector<int> CountNumber(int TIME);
};
