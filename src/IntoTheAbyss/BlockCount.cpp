#include "BlockCount.h"
#include"TexHandleMgr.h"
#include"../Engine/DrawFunc.h"

BlockCount::BlockCount()
{
	number.resize(12);
	number.reserve(12);
	texSize = { 48,44 };
	TexHandleMgr::LoadDivGraph("resource/ChainCombat/UI/num.png", 12, { 12, 1 }, number.data());
}

void BlockCount::Init()
{
	countAllBlockNum = StageMgr::Instance()->GetAllLocalWallBlocksNum();
	maxNumber = CountNumber(countAllBlockNum);
	countNowBlockNum = countAllBlockNum;
}


void BlockCount::Update()
{
	countNowBlockNum = StageMgr::Instance()->GetAllLocalWallBlocksNum();
	nowNumber = CountNumber(countNowBlockNum);

	basePos = { 1280.0f / 2.0f - 200.0f,90.0f };
}

void BlockCount::Draw()
{
	Vec2<float>texSize(48.0f, 44.0f);
	Vec2<float>centralPos;
	int offset = 0;
	float size = 0.4f;
	//����
	for (int i = 0; i < nowNumber.size(); i++)
	{
		offset = i;

		centralPos = { 65.0f + basePos.x + (i * texSize.x - 15.0f * i), basePos.y };
		DrawFunc::DrawRotaGraph2D(centralPos, Vec2<float>(size, size), 0.0f, TexHandleMgr::GetTexBuffer(number[nowNumber[i]]));
	}

	++offset;
	centralPos = { basePos.x + offset * texSize.x,basePos.y };
	DrawFunc::DrawRotaGraph2D(centralPos, Vec2<float>(size, size), 0.0f, TexHandleMgr::GetTexBuffer(number[10]));
	++offset;

	//�ő�
	for (int i = 0; i < maxNumber.size(); i++)
	{
		centralPos = { basePos.x - 17.0f + (offset + i) * texSize.x - 15.0f * i, basePos.y };
		DrawFunc::DrawRotaGraph2D(centralPos, Vec2<float>(size, size), 0.0f, TexHandleMgr::GetTexBuffer(number[maxNumber[i]]));
	}
}

std::vector<int> BlockCount::CountNumber(int TIME)
{
	float score = TIME;
	std::vector<int> Number(KuroFunc::GetDigit(TIME));

	int tmp = score;
	//�X�R�A�v�Z
	for (int i = 0; tmp > 0; i++)
	{
		float result = tmp % 10;
		//Number.push_back(result);
		Number[i] = result;
		tmp /= 10.0f;
	}
	//0����
	for (int i = 0; i < Number.size(); i++)
	{
		if (Number[i] == -1)
		{
			Number[i] = 0;
		}
	}
	std::reverse(Number.begin(), Number.end());
	return Number;
	return std::vector<int>();
}
