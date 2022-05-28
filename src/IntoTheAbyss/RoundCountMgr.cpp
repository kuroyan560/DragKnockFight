#include "RoundCountMgr.h"
#include "DrawFunc.h"
#include "TexHandleMgr.h"

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

	pos = Pos;
	isActive = true;
	isFill = false;
	angle = 0;
	exp = { 10,10 };
	alpha = 0;

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

RoundCountMgr::RoundCountMgr()
{

	/*===== �R���X�g���N�^ =====*/

	//nowRound = 0;
	//maxRound = 0;
	frameGraph = TexHandleMgr::LoadGraph("resource/ChainCombat/UI/roundGaugeFrame.png");
	innerGraph = TexHandleMgr::LoadGraph("resource/ChainCombat/UI/roundGauge.png");
	roundGraph = TexHandleMgr::LoadGraph("resource/ChainCombat/UI/roundSmall.png");

}

void RoundCountMgr::Init(const int& MaxRound)
{

	/*===== ���������� =====*/

	Vec2<float> UI_OFFSET_POS = Vec2<float>(1140, 100);
	const float UI_OFFSET_SIZE = 45.0f;

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
	maxRound = 3;

}

void RoundCountMgr::Update()
{

	/*===== �X�V���� =====*/

	for (auto& index : counter) {

		index.Update();

	}

}

void RoundCountMgr::Draw()
{

	/*===== �`�揈�� =====*/

	Vec2<float> UI_OFFSET_POS = Vec2<float>(1024, 102);
	DrawFunc::DrawRotaGraph2D(UI_OFFSET_POS, Vec2<float>(1, 1), 0, TexHandleMgr::GetTexBuffer(roundGraph));

	for (auto& index : counter) {

		index.Draw(frameGraph, innerGraph);

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
