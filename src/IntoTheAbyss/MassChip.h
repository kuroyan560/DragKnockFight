#pragma once
#include"../KuroEngine.h"
#include<vector>
#include<array>
#include"SelectStage.h"
#include"StageMgr.h"

class MassChip
{
public:
	MassChip();

	bool Check(const Vec2<int> &MAPCHIP_POS, const int &CHIP_NUMBER);
	void Finalize();

	const Vec2<float>&GetLeftUpPos();
	const Vec2<float>&GetRightDownPos();
	bool sideOrUpDownFlag;
private:
	enum { UP, DOWN, LEFT, RIGHT };
	std::array<std::vector<Vec2<float>>, 4> registDirChipPos;
	std::vector<Vec2<float>>registChipPos;

	Vec2<float> leftUpPos;
	Vec2<float> rightDownPos;
	bool MassChip::CheckUsedData(std::vector<Vec2<float>> DATA, Vec2<float> DATA2)
	{
		for (int i = 0; i < DATA.size(); ++i)
		{
			if (DATA[i] == DATA2)
			{
				return true;
			}
		}
		return false;
	}
};

