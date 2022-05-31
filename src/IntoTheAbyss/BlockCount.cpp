#include "BlockCount.h"
#include"TexHandleMgr.h"
#include"../Engine/DrawFunc.h"
#include"SelectStage.h"
#include"ScoreManager.h"
#include"CharacterManager.h"

BlockCount::BlockCount()
{
	number.resize(12);
	number.reserve(12);
	texSize = { 48,44 };
	TexHandleMgr::LoadDivGraph("resource/ChainCombat/UI/num.png", 12, { 12, 1 }, number.data());
	appearFlag = false;
}

void BlockCount::Init(int COUNT_MAX, bool MODE, bool DISAPPEAR_FLAG)
{
	countBlockModeFlag = MODE;
	//ブロックを全部壊すモード
	if (MODE)
	{
		countAllBlockNum = COUNT_MAX;
		maxNumber = CountNumber(countAllBlockNum);
		countNowBlockNum = countAllBlockNum;
		nowNumber = CountNumber(countNowBlockNum);
	}
	//一定数破壊するモード
	else
	{
		countAllBlockNum = COUNT_MAX;
		maxNumber = CountNumber(countAllBlockNum);
		countNowBlockNum = 0;
		nowNumber = CountNumber(countNowBlockNum);
	}




	nowScoreNum = ScoreManager::Instance()->GetScore();
	if (DISAPPEAR_FLAG)
	{
		t = 0.0f;
	}
	else
	{
		t = 1.0f;
	}

	appearFlag = false;
}


void BlockCount::Update()
{
	if (countBlockModeFlag)
	{
		countNowBlockNum = StageMgr::Instance()->GetAllLocalWallBlocksNum();
		nowNumber = CountNumber(countNowBlockNum);
		maxNumber = CountNumber(countAllBlockNum);
	}
	else
	{
		countNowBlockNum = CharacterManager::Instance()->Left()->swingDestroyCounter.totalCounter;
		nowNumber = CountNumber(countNowBlockNum);
		maxNumber = CountNumber(countAllBlockNum);
	}

	if (appearFlag)
	{
		if (t <= 1.0f)
		{
			t += 1.0f / 60.0f;
		}
		if (1.0f <= t)
		{
			t = 1.0f;
		}
	}

	hideValue = 1000.0f;
	easeVel = KuroMath::Ease(Out, Cubic, t, 0.0f, 1.0f) * -hideValue;
	basePos = { 1280.0f / 2.0f + hideValue + easeVel ,90.0f };
}

#include"DrawFunc_Mask.h"
#include"EavaluationDataMgr.h"
#include<algorithm>
#include"GameSceneCamerMove.h"
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
		DrawFunc::DrawRotaGraph2D(drawPos + GameSceneCameraMove::Instance()->move - Vec2<float>(25.0f, 0.0f), Vec2<float>(size, size), 0.0f, TexHandleMgr::GetTexBuffer(number[nowNumber[i]]));
	}

	DrawFunc::DrawRotaGraph2D(basePos + Vec2<float>(-8.0f, 0.0f) + GameSceneCameraMove::Instance()->move, Vec2<float>(size, size), 0.0f, TexHandleMgr::GetTexBuffer(number[11]));
	drawPos = basePos + Vec2<float>(10.0f, 0.0f);
	//�ő�
	for (int i = 0; i < maxNumber.size(); i++)
	{
		drawPos.x += texSize.x;
		DrawFunc::DrawRotaGraph2D(drawPos + GameSceneCameraMove::Instance()->move - Vec2<float>(20.0f, 0.0f), Vec2<float>(size, size), 0.0f, TexHandleMgr::GetTexBuffer(number[maxNumber[i]]));
	}

	static const int GET_SCORE_RATE_GAUGE_FRAME = TexHandleMgr::LoadGraph("resource/ChainCombat/UI/getScoreRateGauge.png");
	static const int RETRY_SELECT_UI = TexHandleMgr::LoadGraph("resource/ChainCombat/tutorial/retryselect.png");
	static const int GOOD_GAUGE = TexHandleMgr::LoadGraph("resource/ChainCombat/UI/goodGauge.png");
	static const int GREAT_GAUGE = TexHandleMgr::LoadGraph("resource/ChainCombat/UI/greatGauge.png");
	static const int EXCELLENT_GAUGE = TexHandleMgr::LoadGraph("resource/ChainCombat/UI/excellentGauge.png");
	static const int PERFECT_GAUGE = TexHandleMgr::LoadGraph("resource/ChainCombat/UI/perfectGauge.png");
	float totalGetRate = 0.0f;
	if (countBlockModeFlag)
	{
		totalGetRate = static_cast<float>((countAllBlockNum - countNowBlockNum)) / static_cast<float>(countAllBlockNum);
	}
	else
	{
		totalGetRate = 1.0f - static_cast<float>((countAllBlockNum - countNowBlockNum)) / static_cast<float>(countAllBlockNum);
	}

	const Vec2<float>POS = { 430 + hideValue + easeVel,22 };
	//const Vec2<float>POS = { 430,650 };
	const float GAUGE_OFFSET_Y = 10;

	//リトライセレクトボタン
	const Vec2<float>RS_POS = { 1140 + hideValue + easeVel,550 };
	DrawFunc::DrawGraph(RS_POS + GameSceneCameraMove::Instance()->move, TexHandleMgr::GetTexBuffer(RETRY_SELECT_UI));
	
	//ゲージ枠
	DrawFunc::DrawGraph(POS + GameSceneCameraMove::Instance()->move, TexHandleMgr::GetTexBuffer(GET_SCORE_RATE_GAUGE_FRAME));

	StageEvaluationData data = EvaluationMgr::Instance()->GetData(SelectStage::Instance()->GetStageNum(), SelectStage::Instance()->GetRoomNum());

	//GOOD
	const float goodRate = min(totalGetRate, data.goodRate);
	const Vec2<float>goodOffset = { 10,GAUGE_OFFSET_Y };
	DrawFunc_Mask::DrawGraph(POS + goodOffset + GameSceneCameraMove::Instance()->move, TexHandleMgr::GetTexBuffer(GOOD_GAUGE), POS + goodOffset,
		POS + goodOffset + TexHandleMgr::GetTexBuffer(GOOD_GAUGE)->GetGraphSize().Float() * Vec2<float>(goodRate / data.goodRate, 1.0f));

	//GREAT
	if (data.goodRate < totalGetRate)
	{
		const float greatRate = min(totalGetRate, data.greatRate);
		const Vec2<float>greatOffset = { 426,GAUGE_OFFSET_Y };
		DrawFunc_Mask::DrawGraph(POS + greatOffset + GameSceneCameraMove::Instance()->move, TexHandleMgr::GetTexBuffer(GREAT_GAUGE), POS + greatOffset,
			POS + greatOffset + TexHandleMgr::GetTexBuffer(GREAT_GAUGE)->GetGraphSize().Float() * Vec2<float>(greatRate / data.greatRate, 1.0f));
	}
	//EXCELLENT
	if (data.greatRate < totalGetRate)
	{
		const float excellentRate = min(totalGetRate, data.excellentRate);
		const Vec2<float>excellentOffset = { 670,GAUGE_OFFSET_Y };
		DrawFunc_Mask::DrawGraph(POS + excellentOffset + GameSceneCameraMove::Instance()->move, TexHandleMgr::GetTexBuffer(EXCELLENT_GAUGE), POS + excellentOffset,
			POS + excellentOffset + TexHandleMgr::GetTexBuffer(EXCELLENT_GAUGE)->GetGraphSize().Float() * Vec2<float>(excellentRate / data.excellentRate, 1.0f));
	}
	//PERFECT
	if (data.perfectRate < totalGetRate)
	{
		const float perfectRate = min(totalGetRate, data.perfectRate);
		const Vec2<float>perfectOffset = { 755.5f,GAUGE_OFFSET_Y };
		DrawFunc_Mask::DrawGraph(POS + perfectOffset + GameSceneCameraMove::Instance()->move, TexHandleMgr::GetTexBuffer(PERFECT_GAUGE), POS + perfectOffset,
			POS + perfectOffset + TexHandleMgr::GetTexBuffer(PERFECT_GAUGE)->GetGraphSize().Float() * Vec2<float>(perfectRate / data.perfectRate, 1.0f));
	}
}

void BlockCount::Appear()
{
	appearFlag = true;
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
