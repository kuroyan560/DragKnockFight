#include "Beacon.h"
#include "ScrollMgr.h"
#include "ShakeMgr.h"
#include "SlowMgr.h"

#include"WinApp.h"
#include"DrawFunc.h"

Beacon::Beacon()
{

	/*===== �R���X�g���N�^ =====*/

	pos = {};
	forwardVec = {};
	speed = 0;
	isActive = false;
	isHitWall = false;
	prevFrameScroll = {};

}

void Beacon::Init()
{

	/*===== ���������� =====*/

	pos = { -1000,-1000 };
	isActive = false;
	isHitWall = false;
	isHitWindow = false;
	stopTargetPos = nullptr;

}

void Beacon::Generate(const Vec2<float>& generatePos, const Vec2<float>& forwadVec, const PIKE_ID& id)
{

	/*===== �������� =====*/

	pos = generatePos;
	this->forwardVec = forwadVec;
	isActive = true;
	isHitWall = false;
	isHitWindow = false;
	stopTargetPos = nullptr;
	gimmickVel = {};

	// ����ID��ݒ�
	this->id = id;

}

void Beacon::Update()
{

	/*===== �X�V���� =====*/

	// �ǂɂ������Ă��Ȃ�������ړ�������B
	if (!isHitWall) {

		// ���W��ۑ�
		prevFramePos = pos;

		pos += Vec2<float>(forwardVec.x * MAX_SPEED * SlowMgr::Instance()->playerDeadSlow, forwardVec.y * MAX_SPEED * SlowMgr::Instance()->playerDeadSlow);

		//// ���W����ʊO�ɂ������炻�̏ꏊ�Ŏ~�߂�B
		//const int WIN_WIDTH = WinApp::Instance()->GetWinSize().x;
		//const int WIN_HEIGHT = WinApp::Instance()->GetWinSize().y;
		//if (pos.x - ScrollMgr::Instance()->scrollAmount.x - SCALE * 2.0f <= 0 || pos.x - ScrollMgr::Instance()->scrollAmount.x + SCALE * 2.0f >= WIN_WIDTH ||
		//	pos.y - ScrollMgr::Instance()->scrollAmount.y - SCALE * 2.0f <= 0 || pos.y - ScrollMgr::Instance()->scrollAmount.y + SCALE * 2.0f >= WIN_HEIGHT) {

		//	isHitWall = true;
		//	isHitWindow = true;

		//	// �X�N���[���ʂ�ۑ��B
		//	prevFrameScroll = ScrollMgr::Instance()->scrollAmount;

		//}

	}

	if (isHitWall && stopTargetPos != nullptr) {

		*stopTargetPos = 0;

	}

	// �E�B���h�E�Ɏh�����Ă�����
	if (isHitWindow) {

		// �X�N���[���ʂ���ړ��ʂ����߂�B
		Vec2<float> moveAmount = ScrollMgr::Instance()->scrollAmount - prevFrameScroll;

		// �ړ�������B
		pos += moveAmount;

		// �X�N���[���ʂ�ۑ��B
		prevFrameScroll = ScrollMgr::Instance()->scrollAmount;

	}

	pos += gimmickVel;
	gimmickVel = {};

}

void Beacon::Draw()
{

	/*===== �`�揈�� =====*/

	//unsigned int color = GetColor(0x18, 0xB5, 0x76);
	Color color = Color(24, 181, 118, 255);
	//unsigned int outerColor = GetColor(0x72, 0x74, 0xA1);s
	Color outerColor = Color(114, 116, 161, 255);

	// ���Ԓ�~�̒Z����������F��ς���B
	//if(id == PIKE_TIMESTOP) color = GetColor(0xD9, 0x1A, 0x60);
	if (id == PIKE_TIMESTOP) color = Color(217, 26, 96, 255);

	Vec2<float> scrollShakeZoom = ScrollMgr::Instance()->scrollAmount + ShakeMgr::Instance()->shakeAmount;
	scrollShakeZoom.x *= ScrollMgr::Instance()->zoom;
	scrollShakeZoom.y *= ScrollMgr::Instance()->zoom;

	//DrawCircle(pos.x * ScrollMgr::Instance()->zoom - scrollShakeZoom.x,
	//	pos.y * ScrollMgr::Instance()->zoom - scrollShakeZoom.y,
	//	SCALE * ScrollMgr::Instance()->zoom, color, TRUE);
	//DrawCircle(pos.x * ScrollMgr::Instance()->zoom - scrollShakeZoom.x,
	//	pos.y * ScrollMgr::Instance()->zoom - scrollShakeZoom.y,
	//	SCALE * ScrollMgr::Instance()->zoom, outerColor, FALSE, 2);

	Vec2<float>center = { pos.x * ScrollMgr::Instance()->zoom - scrollShakeZoom.x,
		pos.y * ScrollMgr::Instance()->zoom - scrollShakeZoom.y };

	DrawFunc::DrawCircle2D(center, SCALE * ScrollMgr::Instance()->zoom, color, TRUE);
	DrawFunc::DrawCircle2D(center, SCALE * ScrollMgr::Instance()->zoom, outerColor, FALSE, 2);

}
