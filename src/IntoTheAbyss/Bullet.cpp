#include "Bullet.h"
#include "ScrollMgr.h"
#include "ShakeMgr.h"
#include "ViewPortCollider.h"
#include "SlowMgr.h"

#include"KuroFunc.h"
#include"DrawFunc.h"

#include"TexHandleMgr.h"

Bullet::Bullet()
{

	/*-- �R���X�g���N�^ --*/

	// �e��ϐ����������B
	pos = {};
	forwardVec = {};
	isActive = false;

	bulletHitBox = std::make_shared<SphereCollision>();
	bulletHitBox->center = &pos;
	bulletHitBox->radius = 10.0f;

	graph = TexHandleMgr::LoadGraph("resource/ChainCombat/bullet.png");

}

void Bullet::Init()
{

	/*-- ���������� --*/

	// �����t���O��܂�B
	isActive = false;
}

void Bullet::Generate(const Vec2<float> &generatePos, const Vec2<float> forwardVec, const bool isFirstShot, const SHOT_HAND &id, const float &speed, const int &GRPHA_HANDLE)
{

	/*-- �������� --*/
	if (GRPHA_HANDLE != -1)
	{
		graph = GRPHA_HANDLE;
	}

	// ���W���Z�b�g
	pos = generatePos;
	prevPos = generatePos;

	// �i�s�����x�N�g�����Z�b�g
	this->forwardVec = forwardVec;

	// �ꉞ���K��
	this->forwardVec.Normalize();

	// �����t���O�����Ă�B
	isActive = true;

	alpha = 255;

	if (isFirstShot) {
		//speed = GetRand(MAX_SPEED * 0.25f) + MAX_SPEED * 0.75f;
		this->speed = KuroFunc::GetRand(MAX_SPEED * 0.25f) + MAX_SPEED * 0.75f;
	}
	else {
		this->speed = speed;
	}

	deadTimer = DEAD_TIMER;

	// �ŏ���1Frame���o�߂�����
	isFirstFrame = false;

	// ID��ۑ�
	handID = id;

}

void Bullet::Update()
{

	/*-- �X�V���� --*/

	// �ړ�������B

	if (isFirstFrame) {
		prevPos = pos;
		pos += forwardVec * Vec2<float>(speed * SlowMgr::Instance()->slowAmount, speed * SlowMgr::Instance()->slowAmount);
	}

	isFirstFrame = true;

	//alpha -= 10;
	//speed += -speed / 10.0f;
	//if (speed < 0.3f) Init();

	--deadTimer;
	if (deadTimer <= 0) Init();

}

#include"D3D12App.h"
#include"TexHandleMgr.h"
void Bullet::Draw()
{

	/*-- �`�揈�� --*/

	//SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);

	//// �O����`��
	//DrawBox(pos.x * ScrollMgr::Instance()->zoom - scrollShakeZoom.x - MAX_RADIUS * ScrollMgr::Instance()->zoom,
	//	pos.y * ScrollMgr::Instance()->zoom - scrollShakeZoom.y - MAX_RADIUS * ScrollMgr::Instance()->zoom,
	//	pos.x * ScrollMgr::Instance()->zoom - scrollShakeZoom.x + MAX_RADIUS * ScrollMgr::Instance()->zoom,
	//	pos.y * ScrollMgr::Instance()->zoom - scrollShakeZoom.y + MAX_RADIUS * ScrollMgr::Instance()->zoom,
	//	GetColor(0xD9, 0x1A, 0x60), TRUE);

	//// ������`��
	////DrawCircle(pos.x * ScrollMgr::Instance()->zoom - scrollShakeZoom.x,
	////	pos.y * ScrollMgr::Instance()->zoom - scrollShakeZoom.y,
	////	MAX_RADIUS * ScrollMgr::Instance()->zoom, GetColor(0xD9, 0x1A, 0x60), TRUE);

	//SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);

	Vec2<float>leftUp = { pos.x - MAX_RADIUS,pos.y - MAX_RADIUS };
	leftUp = ScrollMgr::Instance()->Affect(leftUp);
	Vec2<float>rightBottom = { pos.x + MAX_RADIUS,pos.y + MAX_RADIUS };
	rightBottom = ScrollMgr::Instance()->Affect(rightBottom);

	DrawFunc::DrawExtendGraph2D(leftUp, rightBottom, TexHandleMgr::GetTexBuffer(graph), AlphaBlendMode_Trans);
	//DrawFunc::DrawBox2D(leftUp, rightBottom, Color(217, 26, 96, (int)alpha), D3D12App::Instance()->GetBackBuffFormat(), true, AlphaBlendMode_Trans);
}
