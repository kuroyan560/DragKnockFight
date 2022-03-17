#include "DossunBlock.h"
#include "ScrollMgr.h"
#include "ShakeMgr.h"

#include"DrawFunc.h"

DossunBlock::DossunBlock()
{

	/*===== �R���X�g���N�^ =====*/

	// �e�ϐ����������B
	pos = {};
	size = {};
	moveDir = {};
	speed = 0;
	isHitPlayer = false;
	isMoveTimer = 0;
	id = {};

}

void DossunBlock::Init()
{

	/*===== ���������� =====*/

	// �e�ϐ����������B
	pos = {};
	size = {};
	moveDir = {};
	speed = 0;
	isHitPlayer = false;
	isMoveTimer = 0;
	id = {};

}

void DossunBlock::Generate(Vec2<float> generatePos, Vec2<float> endPos, const Vec2<float>& size, const DossunID& dossunID)
{

	/*===== �������� =====*/

	// �����ʒu���������B
	pos = generatePos;

	// �傫����������
	this->size = size;

	// �ړ��������������B
	this->moveDir = endPos - generatePos;
	this->moveDir.Normalize();

	// �h�b�X��ID���������B
	id = dossunID;

	// ���̑��ϐ����������B
	speed = 0;
	isMoveTimer = 0;
	isHitPlayer = false;

}

void DossunBlock::Update()
{

	/*===== �X�V���� =====*/

	// �v���C���[�Ɠ������Ă�����^�C�}�[�����Z����B
	if (isHitPlayer && isMoveTimer < IS_MOVE_TIMER) {

		// �^�C�}�[�����Z����B
		++isMoveTimer;

	}

	// �^�C�}�[�����ȏゾ������ړ��������s���B
	if (isMoveTimer >= IS_MOVE_TIMER) {

		// �ړ�����B
		pos = moveDir * Vec2<float>(speed, speed);

		// ID�ɉ����Ĉړ��ʂ����Z����B
		switch (id)
		{
		case DOSSUN_OFF_POWER:

			/*-- �d��OFF�h�b�X���̏ꍇ --*/

			break;
		case DOSSUN_LOW_POWER:

			/*-- ��o�̓h�b�X���̏ꍇ --*/

			// �ړ����x�����Z����B
			speed += LOW_POWER_ADD_SPEED;

			// �ړ����x�����E�𒴂��Ȃ��悤�ɂ���B
			if (speed >= LOW_POWER_MAX_SPEED) {

				speed = LOW_POWER_MAX_SPEED;

			}


			break;
		case DOSSUN_HIGH_POWER:

			/*-- ���o�̓h�b�X���̏ꍇ --*/

			break;
		default:
			break;
		}

	}

}

void DossunBlock::Draw()
{

	/*===== �`�揈�� =====*/

	Vec2<float> scrollShakeZoom = { ScrollMgr::Instance()->scrollAmount.x * ScrollMgr::Instance()->zoom + ShakeMgr::Instance()->shakeAmount.x * ScrollMgr::Instance()->zoom,
	ScrollMgr::Instance()->scrollAmount.y * ScrollMgr::Instance()->zoom + ShakeMgr::Instance()->shakeAmount.y * ScrollMgr::Instance()->zoom };

	Vec2<float> posZoom = { pos.x * ScrollMgr::Instance()->zoom,pos.y * ScrollMgr::Instance()->zoom };

	Vec2<float> sizeZoom = { size.x * ScrollMgr::Instance()->zoom, size.y * ScrollMgr::Instance()->zoom };

	Vec2<float>leftUp = { posZoom.x - sizeZoom.x - scrollShakeZoom.x,
		posZoom.y - sizeZoom.y - scrollShakeZoom.y };
	Vec2<float>rightBottom = { posZoom.x + sizeZoom.x - scrollShakeZoom.x,
		posZoom.y + sizeZoom.y - scrollShakeZoom.y };

	DrawFunc::DrawBox2D(leftUp, rightBottom, Color(100, 100, 100, 255), true);
}

