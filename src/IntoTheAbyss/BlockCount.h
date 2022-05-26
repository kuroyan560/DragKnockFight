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

	// NowNumber��0���ǂ������`�F�b�N����B
	inline bool CheckNowNomberIsZero() {

		bool result = true;

		const int NOWNUMBER_SIZE = nowNumber.size();
		for (int index = 0; index < NOWNUMBER_SIZE; ++index) {

			result &= nowNumber[index] == 0;

		}

		return result;
	}

	// NowNumber��int�^�ŕԂ��B
	inline int GetNowNumberInt() {

		int resultValue = 0;

		const int NOWNUMBER_SIZE = nowNumber.size();
		for (int index = 0; index < NOWNUMBER_SIZE; ++index) {

			resultValue += nowNumber[index] + (index * 10.0f);

		}

		return resultValue;

	}

	// MaxNumber��int�^�ŕԂ��B
	inline int GetMaxNumberInt() {

		int resultValue = 0;

		const int NOWNUMBER_SIZE = maxNumber.size();
		for (int index = 0; index < NOWNUMBER_SIZE; ++index) {

			resultValue += maxNumber[index] + (index * 10.0f);

		}

		return resultValue;

	}


private:
	std::vector<int> number;
	std::vector<int> maxNumber;
	std::vector<int> nowNumber;
	Vec2<float>texSize;
	int countBlockNum;


	Vec2<float>basePos;

	std::vector<int> CountNumber(int TIME);
};
