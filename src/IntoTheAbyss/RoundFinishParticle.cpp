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

void RoundFinishParticle::Generate(const Vec2<float>& CharaPos, const int& UseGraphHandle)
{

	/*===== ���������� =====*/

	pos = CharaPos;
	dir = Vec2<float>(KuroFunc::GetRand(2.0f) - 1.0f, KuroFunc::GetRand(2.0f) - 1.0f);
	changeLength = KuroFunc::GetRand(MIN_MOVE_LENGTH + MAX_MOVE_LENGTH) - MIN_MOVE_LENGTH;
	useGraphHanlde = UseGraphHandle;
	isActive = true;
	isReturn = false;

}

void RoundFinishParticle::Update(const Vec2<float>& LeftCharaPos)
{

	/*===== �X�V���� =====*/


	if (isReturn) {

		pos.x += (LeftCharaPos.x - pos.x) / 10.0f;
		pos.y += (LeftCharaPos.y - pos.y) / 10.0f;

	}
	else {
		changeLength -= changeLength / 10.0f;
		pos += dir * changeLength;
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

void RoundFinishParticleMgr::Generate(const Vec2<float>& CharaPos)
{

	/*=====	�������� =====*/

	for (auto& index : particles) {

		// �g�p����摜�������_���ɐ����B
		int useHandle = KuroFunc::GetRand(2);

		index.Generate(CharaPos, particleGraph[useHandle]);

	}

	easingTimer = 0;

}

void RoundFinishParticleMgr::Update(const Vec2<float>& LeftCharaPos)
{

	/*===== �X�V���� =====*/

	for (auto& index : particles) {

		if (!index.isActive) continue;

		index.Update(LeftCharaPos);

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
