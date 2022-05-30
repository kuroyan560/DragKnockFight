#include "RoundCountMgr.h"
#include "DrawFunc.h"
#include "TexHandleMgr.h"
#include"StageMgr.h"
#include"SelectStage.h"

RoundCount::RoundCount()
{

	/*===== �R���X�g���N�^ =====*/

	pos = {};
	isActive = false;
	isFill = false;

}

void RoundCount::Init()
{

	/*===== ���������� =====*/

	pos = {};
	isActive = false;
	isFill = false;
	angle = 0;
	exp = { 3,3 };
	alpha = 0;

}

void RoundCount::Generate(const Vec2<float>& Pos)
{

	/*===== �������� =====*/
	initPos = Pos + Vec2<float>(150.0f, 0.0f);
	pos = initPos;
	vel = Pos - initPos;

	isActive = true;
	isFill = false;
	angle = 0;
	exp = { 20,20 };
	alpha = 0;
	t = 0.0f;
	appearFlag = false;
}

void RoundCount::Update()
{

	/*===== �X�V���� =====*/

	if (isFill) {

		angle += (DirectX::XM_2PI * 2.0f - angle) / 5.0f;
		alpha += (255 - alpha) / 5.0f;
		exp.x += (1.0f - exp.x) / 5.0f;
		exp.y += (1.0f - exp.y) / 5.0f;

	}

	if (appearFlag)
	{
		if (t < 1.0f)
		{
			t += 1.0f / 60.0f;
		}
		if (1.0f <= t)
		{
			t = 1.0f;
		}

		pos.x = initPos.x + KuroMath::Ease(Out, Cubic, t, 0.0f, 1.0f) * vel.x;
	}

}

void RoundCount::Draw(const int& FrameGraph, const int& InnerGraph)
{

	/*===== �`�揈�� =====*/

	// �O����`��
	DrawFunc::DrawRotaGraph2D(pos, Vec2<float>(1.0f, 1.0f), 0, TexHandleMgr::GetTexBuffer(FrameGraph));

	// ������`��
	if (isFill) {
		DrawFunc::DrawRotaGraph2D(pos, exp, 0, TexHandleMgr::GetTexBuffer(InnerGraph), Color(255, 255, 255, alpha));
	}

}

void RoundCount::Appear()
{
	appearFlag = true;
}

RoundCountMgr::RoundCountMgr()
{

	/*===== �R���X�g���N�^ =====*/

	//nowRound = 0;
	//maxRound = 0;
	frameGraph = TexHandleMgr::LoadGraph("resource/ChainCombat/UI/roundGaugeFrame.png");
	innerGraph = TexHandleMgr::LoadGraph("resource/ChainCombat/UI/roundGauge.png");
	roundGraph = TexHandleMgr::LoadGraph("resource/ChainCombat/UI/roundSmall.png");

}

void RoundCountMgr::Init(int MaxRound)
{

	/*===== ���������� =====*/

	int num = (2 - (MaxRound - 1));
	if (num <= 0)
	{
		num = 0;
	}

	const float UI_OFFSET_SIZE = 45.0f;
	Vec2<float> UI_OFFSET_POS = Vec2<float>(1140 + UI_OFFSET_SIZE * num, 100);

	startPos = UI_OFFSET_POS;

	int indexCounter = 0;

	for (auto& index : counter) {

		index.Init();

		// ����������W�����߂�B
		Vec2<float> generatePos = UI_OFFSET_POS + Vec2<float>(indexCounter * UI_OFFSET_SIZE, 0);

		// ���݂̃��E���h����UI�𐶐��B
		index.Generate(generatePos);

		++indexCounter;
	}

	nowRound = 0;
	maxRound = MaxRound;

}

void RoundCountMgr::Update()
{

	/*===== �X�V���� =====*/
	for (auto &index : counter) {

		index.Update();

	}

}

void RoundCountMgr::Draw()
{

	/*===== �`�揈�� =====*/

	Vec2<float> UI_OFFSET_POS = Vec2<float>(startPos.x - 116.0f, 102);
	DrawFunc::DrawRotaGraph2D(UI_OFFSET_POS, Vec2<float>(1, 1), 0, TexHandleMgr::GetTexBuffer(roundGraph));

	for (int i = 0; i < maxRound; ++i)
	{
		counter[i].Draw(frameGraph, innerGraph);
	}

}

void RoundCountMgr::RoundIncrement()
{

	/*===== nowRound���C���N�������g =====*/

	// �C���N�������g����MaxRound���傫���Ȃ�Ȃ��悤�ɂ���B
	++nowRound;
	if (maxRound <= nowRound) nowRound = maxRound;

	counter[nowRound - 1].SetFill();
}

void RoundCountMgr::Appear()
{
	for (auto &index : counter) {

		index.Appear();

	}
}
