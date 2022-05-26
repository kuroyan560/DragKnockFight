#include "RoundFinishParticle.h"
#include "KuroFunc.h"
#include "ScrollMgr.h"
#include "DrawFunc.h"
#include "TexHandleMgr.h"

RoundFinishParticle::RoundFinishParticle()
{

	/*===== �R���X�g���N�^ =====*/

	pos = {};
	dir = {};
	changeLength = 0;
	useGraphHanlde = 0;
	isActive = false;
	isReturn = false;

}

void RoundFinishParticle::Init()
{

	/*===== ���������� =====*/

	pos = {};
	dir = {};
	changeLength = 0;
	useGraphHanlde = 0;
	isActive = false;
	isReturn = false;

}

void RoundFinishParticle::Generate(const int& UseGraphHandle)
{

	/*===== ���������� =====*/

	pos = {};
	dir = Vec2<float>(KuroFunc::GetRand(2.0f) - 1.0f, KuroFunc::GetRand(2.0f) - 1.0f);
	changeLength = KuroFunc::GetRand(MIN_MOVE_LENGTH + MAX_MOVE_LENGTH) - MIN_MOVE_LENGTH;
	useGraphHanlde = UseGraphHandle;
	isActive = true;
	isReturn = false;

}

void RoundFinishParticle::Update(const Vec2<float>& CharaPos, const float& Rate, const Vec2<float>& LeftCharaPos)
{

	/*===== �X�V���� =====*/


	if (isReturn) {

		pos.x += (pos.x - LeftCharaPos.x) / 30.0f;
		pos.y += (pos.y - LeftCharaPos.y) / 30.0f;

	}
	else {
		pos = CharaPos + dir * (Rate * changeLength);
	}

}

void RoundFinishParticle::Draw()
{

	/*===== �`�揈�� =====*/

	DrawFunc::DrawRotaGraph2D(ScrollMgr::Instance()->Affect(pos), Vec2<float>(ScrollMgr::Instance()->zoom, ScrollMgr::Instance()->zoom), 0, TexHandleMgr::GetTexBuffer(useGraphHanlde));

}

void RoundFinishParticleMgr::Init()
{

	/*===== ���������� =====*/

	static int first = 0;
	if (first == 0) {

		TexHandleMgr::LoadDivGraph("resource/ChainCombat/backGround_star.png", 3, Vec2<int>(3, 1), particleGraph.data());

		++first;

	}


	for (auto& index : particles) {

		index.Init();

	}

	easingTimer = 0;

}

void RoundFinishParticleMgr::Generate()
{

	/*=====	�������� =====*/

	for (auto& index : particles) {

		// �g�p����摜�������_���ɐ����B
		int useHandle = KuroFunc::GetRand(2);

		index.Generate(particleGraph[useHandle]);

	}

	easingTimer = 0;

}

void RoundFinishParticleMgr::Update(const Vec2<float>& CharaPos, const Vec2<float>& LeftCharaPos)
{

	/*===== �X�V���� =====*/

	float nowRate = static_cast<float>(easingTimer) / static_cast<float>(EASING_TIMER);
	float nowEasing = KuroMath::Ease(Out, Cubic, nowRate, 0.0f, 1.0f);

	for (auto& index : particles) {

		index.Update(CharaPos, nowEasing, LeftCharaPos);

	}

	++easingTimer;
	if (EASING_TIMER <= easingTimer) {

		easingTimer = EASING_TIMER;

	}

}

void RoundFinishParticleMgr::Draw()
{

	/*=====	�`�揈�� =====*/

	for (auto& index : particles) {

		if (!index.isActive) continue;

		index.Draw();

	}

}

void RoundFinishParticleMgr::SetReturn()
{

	/*=====	�߂鏈�� =====*/

	for (auto& index : particles) {

		if (!index.isActive) continue;

		index.isReturn = true;

	}

}
