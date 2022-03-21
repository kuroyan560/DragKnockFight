#include "BulletParticle.h"
#include "ScrollMgr.h"
#include "ShakeMgr.h"

#include"KuroFunc.h"
#include"DrawFunc.h"

BulletParticle::BulletParticle()
{

	/*===== �R���X�g���N�^ =====*/

	pos = {};
	forwardVec = {};
	movedVel = {};
	alpha = 0;
	speed = 0;
	isAlive = false;

}

void BulletParticle::Init()
{

	/*===== ���������� =====*/

	isAlive = false;

}

void BulletParticle::Generate(const Vec2<float>& generatePos, const Vec2<float>& forwardVec)
{

	/*===== �������� =====*/

	// ���W��ݒ�
	pos = generatePos;

	// �d�Ȃ�Ȃ��悤�ɐ؂�グ����B
	pos.x = RoundUp(pos.x, SCALE);
	pos.y = RoundUp(pos.y, SCALE);
	scale = SCALE;

	// �i�s������ݒ�
	this->forwardVec = forwardVec;

	// �ړ������ʂ�������
	movedVel = {};

	// �A���t�@�l��������
	alpha = 255.0f;

	// �ړ��ʂ�������
	//speed = GetRand(DEF_SPEED * 100) / 100.0f;
	speed = KuroFunc::GetRand(DEF_SPEED * 100) / 100.0f;

	// �����t���O��������
	isAlive = true;

}

void BulletParticle::Update()
{

	/*===== �X�V���� =====*/

	// �ړ�������B
	movedVel += Vec2<float>(forwardVec.x * speed, forwardVec.y * speed);

	// �ړ������ʂ��T�C�Y����������T�C�Y�𓮂����B
	if (fabs(movedVel.x) >= scale) {

		// �������擾�B
		int sign = signbit(movedVel.x) ? -1 : 1;

		// ���W�𓮂����B
		pos.x += scale * sign;

		// �ړ������ʂ����炷�B
		movedVel.x -= scale * sign;

	}
	// �ړ������ʂ��T�C�Y����������T�C�Y�𓮂����BY�o�[�W����
	if (fabs(movedVel.y) >= scale) {

		// �������擾�B
		int sign = signbit(movedVel.y) ? -1 : 1;

		// ���W�𓮂����B
		pos.y += scale * sign;

		// �ړ������ʂ����炷�B
		movedVel.y -= SCALE * sign;

	}

	// �X�s�[�h��������B
	if(speed > 0)speed -= SUB_SPEED;

	// �X�s�[�h���K��l�ȉ��ɂȂ�����A���t�@�l�������n�߂�B
	if (speed <= 0.5f) {

		alpha -= alpha / 3.0f;

	}

	// �A���t�@�l�����ȉ��ɂȂ����珉��������B
	if (alpha <= 10.0f) Init();

	if (alpha <= 123)scale = SCALE / 2.0f;

}

void BulletParticle::Draw()
{

	/*===== �`�揈�� =====*/

	Vec2<float> scrollShakeZoom = ScrollMgr::Instance()->scrollAmount + ShakeMgr::Instance()->shakeAmount;
	scrollShakeZoom.x *= ScrollMgr::Instance()->zoom;
	scrollShakeZoom.y *= ScrollMgr::Instance()->zoom;

	float drawScale = (scale / 2.0f) * ScrollMgr::Instance()->zoom;

	//SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);

	//DrawBox(pos.x * ScrollMgr::Instance()->zoom - scale - scrollShakeZoom.x,
	//	pos.y * ScrollMgr::Instance()->zoom - scale - scrollShakeZoom.y,
	//	pos.x * ScrollMgr::Instance()->zoom + scale - scrollShakeZoom.x,
	//	pos.y * ScrollMgr::Instance()->zoom + scale - scrollShakeZoom.y,
	//	GetColor(0xD9, 0x1A, 0x60), TRUE);

	//SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	Vec2<float>leftUp = { pos.x * ScrollMgr::Instance()->zoom - drawScale - scrollShakeZoom.x,
		pos.y * ScrollMgr::Instance()->zoom - drawScale - scrollShakeZoom.y };
	Vec2<float>rightBottom = { pos.x * ScrollMgr::Instance()->zoom + drawScale - scrollShakeZoom.x,
		pos.y * ScrollMgr::Instance()->zoom + drawScale - scrollShakeZoom.y };

	//DrawFunc::DrawBox2D(leftUp, rightBottom, Color(217, 26, 96, (int)alpha), true, AlphaBlendMode_Trans);
	DrawFunc::DrawBox2D(leftUp, rightBottom, Color(255, 255, 255, 255), true, AlphaBlendMode_Trans);

}

