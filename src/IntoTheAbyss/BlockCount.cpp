#include "BlockCount.h"
#include"TexHandleMgr.h"
#include"../Engine/DrawFunc.h"

BlockCount::BlockCount()
{
	static const int NUM = 13;
	number.resize(NUM);
	number.reserve(NUM);
	texSize = { 48,44 };
	TexHandleMgr::LoadDivGraph("resource/ChainCombat/UI/num.png", NUM, { NUM, 1 }, number.data());
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

	basePos = { 1280.0f / 2.0f,90.0f };
}

void BlockCount::Draw()
{
	Vec2<float>texSize(30.0f, 44.0f);
	Vec2<float>centralPos;
	int offset = 0;
	float size = 0.4f;

	int num = KuroFunc::GetDigit(countAllBlockNum);

	Vec2<float>drawPos = basePos - Vec2<float>(10.0f, 0.0f);
	for (int i = 0; i < num; ++i)
	{
		drawPos.x -= texSize.x;
	}
	//����
	for (int i = 0; i < nowNumber.size(); i++)
	{
		drawPos.x += texSize.x;
		DrawFunc::DrawRotaGraph2D(drawPos - Vec2<float>(25.0f, 0.0f), Vec2<float>(size, size), 0.0f, TexHandleMgr::GetTexBuffer(number[nowNumber[i]]));
	}

	DrawFunc::DrawRotaGraph2D(basePos + Vec2<float>(-8.0f, 0.0f), Vec2<float>(size, size), 0.0f, TexHandleMgr::GetTexBuffer(number[10]));
	drawPos = basePos + Vec2<float>(10.0f, 0.0f);
	//�ő�
	for (int i = 0; i < maxNumber.size(); i++)
	{
		drawPos.x += texSize.x;
		DrawFunc::DrawRotaGraph2D(drawPos - Vec2<float>(25.0f, 0.0f), Vec2<float>(size, size), 0.0f, TexHandleMgr::GetTexBuffer(number[maxNumber[i]]));
	}
}

std::vector<int> BlockCount::CountNumber(int TIME)
{
	float score = TIME;
	std::vector<int> Number(KuroFunc::GetDigit(countAllBlockNum));

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
		if (Number[i] == 0)
		{
			Number[i] = 0;
		}
	}
	std::reverse(Number.begin(), Number.end());
	return Number;
	return std::vector<int>();
}
