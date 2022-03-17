#include "DossunBlock.h"
#include "ScrollMgr.h"
#include "ShakeMgr.h"
#include "Collider.h"
#include "MapChipCollider.h"

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
	changeDirTimer = 0;
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
	changeDirTimer = 0;
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
	changeDirTimer = 0;
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

	// �v���C���[�Ɠ������Ă��Ȃ�������ړ��ʂ�0�ɂ���B
	if (!isHitPlayer) {

		// �ړ��ʂ����������B
		speed = 0;

	}

	// �ړ������`�F���W�^�C�}�[�����ȏゾ����������]������B
	if (changeDirTimer >= CHANGE_DIR_TIMER) {

		changeDirTimer = 0;
		moveDir *= {-1.0f, -1.0f};

	}

	// �^�C�}�[�����ȏゾ������ړ��������s���B
	if (isMoveTimer >= IS_MOVE_TIMER) {

		// �ړ�����B
		pos += moveDir * Vec2<float>(speed, speed);

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

void DossunBlock::CheckHit(Player& player, const vector<vector<int>>& mapData)
{

	// �h�b�X���u���b�N�ƃv���C���[�̓����蔻��

	bool isDossunVel = Collider::Instance()->CheckHitVel(player.centerPos, player.prevFrameCenterPos, player.vel, player.PLAYER_SIZE, pos, size) != INTERSECTED_NONE;
	bool isDossunTop = Collider::Instance()->CheckHitSize(player.centerPos, player.PLAYER_SIZE, pos, size, INTERSECTED_TOP) != INTERSECTED_NONE;
	bool isDossunRight = Collider::Instance()->CheckHitSize(player.centerPos, player.PLAYER_SIZE, pos, size, INTERSECTED_RIGHT) != INTERSECTED_NONE;
	bool isDossunLeft = Collider::Instance()->CheckHitSize(player.centerPos, player.PLAYER_SIZE, pos, size, INTERSECTED_LEFT) != INTERSECTED_NONE;
	bool isDossunBottom = Collider::Instance()->CheckHitSize(player.centerPos, player.PLAYER_SIZE, pos, size, INTERSECTED_BOTTOM) != INTERSECTED_NONE;

	// �ǂ�������ɂԂ����Ă���Γ�����������ɂ���B
	if (isDossunVel || isDossunTop || isDossunRight || isDossunLeft || isDossunBottom) {

		// �v���C���[�Ƀh�b�X���u���b�N�̈ړ��ʂ�n���B
		player.gimmickVel = Vec2<float>(speed, speed) * moveDir;

		// �h�b�X���̈ړ��ʃ^�C�}�[���X�V�B
		isHitPlayer = true;

		// �v���C���[�̈ړ��ʂ����������B
		player.gravity *= 0.0f;
		player.vel *= {0.5f,0.5f};

	}
	else {

		// �h�b�X���̈ړ��ʃ^�C�}�[���������B
		isHitPlayer = false;
		isMoveTimer = 0;

	}

	// �t���O���������B
	isDossunTop = false;
	isDossunRight = false;
	isDossunLeft = false;
	isDossunBottom = false;

	// �}�b�v�`�b�v�ƃh�b�X���u���b�N�̓����蔻��

	if (moveDir.y != 0) {
		isDossunTop = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(pos, size, mapData, INTERSECTED_TOP) != INTERSECTED_NONE;
	}
	if (moveDir.x != 0) {
		isDossunRight = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(pos, size, mapData, INTERSECTED_RIGHT) != INTERSECTED_NONE;
	}
	if (moveDir.x != 0) {
		isDossunLeft = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(pos, size, mapData, INTERSECTED_LEFT) != INTERSECTED_NONE;
	}
	if (moveDir.y != 0) {
		isDossunBottom = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(pos, size, mapData, INTERSECTED_BOTTOM) != INTERSECTED_NONE;
	}

	// �ǂ�������ɂԂ����Ă���Γ�����������ɂ���B
	if (isDossunTop || isDossunRight || isDossunLeft || isDossunBottom) {

		// �����]���^�C�}�[�����Z�B
		++changeDirTimer;

	}
	else {

		// �����]���^�C�}�[���������B
		changeDirTimer = 0;

	}

}

