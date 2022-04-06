#include "Boss.h"
#include "ShakeMgr.h"
#include "ScrollMgr.h"
#include "DrawFunc.h"
#include "MapChipCollider.h"
#include"UsersInput.h"

Boss::Boss()
{

	pos = {};
	scale = {};
	vel = {};

}

void Boss::Init()
{

	pos = {};
	scale = {};
	vel = {};

}

void Boss::Generate(const Vec2<float>& generatePos)
{

	/*===== �������� =====*/

	pos = generatePos;
	scale = SCALE;
	vel = { OFFSET_VEL,0 };

}

void Boss::Update()
{

	/*===== �X�V���� =====*/

	// �O�t���[���̍��W��ۑ�
	prevPos = pos;

	vel.x += (OFFSET_VEL - vel.x) / 10.0f;

	if (UsersInput::Instance()->Input(DIK_0)) {

		vel.x = OFFSET_VEL * 5.0f;

	}

}

void Boss::Draw()
{

	/*===== �`�揈�� =====*/

	Vec2<float> scrollShakeAmount = ScrollMgr::Instance()->scrollAmount + ShakeMgr::Instance()->shakeAmount;

	DrawFunc::DrawBox2D(pos - scale - scrollShakeAmount, pos + scale - scrollShakeAmount, Color(230, 38, 113, 255), DXGI_FORMAT_R8G8B8A8_UNORM, true);

}

void Boss::CheckHit(const vector<vector<int>>& mapData, bool& isHitMapChip)
{

	/*===== �����蔻�� =====*/

	bool onGround = false;
	// �ړ��ʂ����ɂ����}�b�v�`�b�v�Ƃ̓����蔻����s���B
	INTERSECTED_LINE intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheVel(pos, prevPos, vel, scale, onGround, mapData);

	// �����蔻���ۑ��B
	isHitMapChip = intersectedLine != INTERSECTED_NONE;

	// �X�P�[�������ɂ����}�b�v�`�b�v�Ƃ̓����蔻����s���B
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(pos - Vec2<float>(scale.x / 2.0f, 0), scale, mapData, INTERSECTED_TOP);
	if (!isHitMapChip && intersectedLine != INTERSECTED_NONE)isHitMapChip = true;
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(pos + Vec2<float>(scale.x / 2.0f, 0), scale, mapData, INTERSECTED_TOP);
	if (!isHitMapChip && intersectedLine != INTERSECTED_NONE)isHitMapChip = true;

	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(pos - Vec2<float>(scale.x / 2.0f, 0), scale, mapData, INTERSECTED_BOTTOM);
	if (!isHitMapChip && intersectedLine != INTERSECTED_NONE)isHitMapChip = true;
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(pos + Vec2<float>(scale.x / 2.0f, 0), scale, mapData, INTERSECTED_BOTTOM);
	if (!isHitMapChip && intersectedLine != INTERSECTED_NONE)isHitMapChip = true;

	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(pos - Vec2<float>(0, scale.y / 2.0f), scale, mapData, INTERSECTED_RIGHT);
	if (!isHitMapChip && intersectedLine != INTERSECTED_NONE)isHitMapChip = true;
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(pos + Vec2<float>(0, scale.y / 2.0f), scale, mapData, INTERSECTED_RIGHT);
	if (!isHitMapChip && intersectedLine != INTERSECTED_NONE)isHitMapChip = true;

	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(pos - Vec2<float>(0, scale.y / 2.0f), scale, mapData, INTERSECTED_LEFT);
	if (!isHitMapChip && intersectedLine != INTERSECTED_NONE)isHitMapChip = true;
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(pos + Vec2<float>(0, scale.y / 2.0f), scale, mapData, INTERSECTED_LEFT);
	if (!isHitMapChip && intersectedLine != INTERSECTED_NONE)isHitMapChip = true;

}
