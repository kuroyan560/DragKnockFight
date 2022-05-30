#include "KazHelper.h"
#include"../Common/KuroFunc.h"

std::vector<int> KazHelper::CountNumber(int TIME, int ZERO)
{
	float score = TIME;

	int haveZero = 0;

	//0–„‚ß‚·‚é
	if (ZERO != -1)
	{
		haveZero = ZERO;
	}
	else
	{
		haveZero = KuroFunc::GetDigit(TIME);
	}

	std::vector<int> Number(haveZero);

	int tmp = score;
	for (int i = 0; tmp > 0; i++)
	{
		float result = tmp % 10;
		Number[i] = result;
		tmp /= 10.0f;
	}
	for (int i = 0; i < Number.size(); i++)
	{
		if (Number[i] == 0)
		{
			Number[i] = 0;
		}
	}
	std::reverse(Number.begin(), Number.end());
	return Number;
}
