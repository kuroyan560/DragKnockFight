#include "DossunBlock.h"
#include "ScrollMgr.h"
#include "ShakeMgr.h"
#include "Collider.h"
#include "MapChipCollider.h"
#include "ParticleMgr.h"

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
	isMove = false;
	isReturn = false;
	isMoveTimer = 0;
	changeDirTimer = 0;
	id = {};
	sightData.pos = &pos;
	sightData.scale = size;

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
	isMove = false;
	isReturn = false;
	isMoveTimer = 0;
	changeDirTimer = 0;
	id = {};

}

void DossunBlock::Generate(Vec2<float> generatePos, Vec2<float> endPos, const Vec2<float>& size, const E_GIMMICK& dossunID)
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
	isMove = false;
	isReturn = false;
	isTimeStopPikeAlive = nullptr;
	sightData = { &pos,size/* * Vec2<float>(2.0f,2.0f)*/ };
	alpha = 1;

}

void DossunBlock::Update()
{

	/*===== �X�V���� =====*/

	// �v���C���[�Ɠ������Ă�����^�C�}�[�����Z����B
	if (isHitPlayer) {

		// �^�C�}�[�����Z����B
		++isMoveTimer;

	}
	else {

		isMoveTimer = 0;

	}

	// �ړ�����܂ł̃^�C�}�[�����ȏ�ɂȂ�����ړ�������B
	if (isMoveTimer >= IS_MOVE_TIMER) {

		isMove = true;

	}

	// �ړ������`�F���W�^�C�}�[�����ȏゾ����������]������B
	if (changeDirTimer >= CHANGE_DIR_TIMER) {
		moveDir *= {-1.0f, -1.0f};

		noCheckHitTimer = 10.0f;

		bool prevFrameFlag = isReturn;

		// �߂��Ă���t���O�𔽓]������B
		isReturn = isReturn ? false : true;

		// �߂����u�Ԃ͈ړ����~������B
		if (prevFrameFlag && !isReturn && isHitPlayer) {

		}
		else if (prevFrameFlag && !isReturn) {

			speed = 0;
			isMove = false;

		}

		if (isMove) {
			changeDirTimer = 0;
		}

	}

	// �^�C�}�[�����ȏゾ������ړ��������s���B
	if (isMove) {

		if (speed <= 1.0f) speed = 1.0f;

		// ID�ɉ����Ĉړ��ʂ����Z����B
		switch (id)
		{
		case GIMMICK_DOSSN_OFF:

			/*-- �d��OFF�h�b�X���̏ꍇ --*/

			break;
		case GIMMICK_DOSSN_ON_LOW:

			/*-- ��o�̓h�b�X���̏ꍇ --*/

			// �ړ����x�����Z����B
			speed += LOW_POWER_ADD_SPEED;

			// �ړ����x�����E�𒴂��Ȃ��悤�ɂ���B
			if (speed >= LOW_POWER_MAX_SPEED) {

				speed = LOW_POWER_MAX_SPEED;

			}


			break;
		case GIMMICK_DOSSN_ON_HIGH:

			/*-- ���o�̓h�b�X���̏ꍇ --*/

			break;
		default:
			break;
		}

		if (isTimeStopPikeAlive != nullptr) speed = 0;
		if (changeDirTimer > 0) speed = 0;

		// �ړ�����B
		pos += moveDir * Vec2<float>(speed, speed);

	}

	if (isTimeStopPikeAlive != nullptr && *isTimeStopPikeAlive == false) isTimeStopPikeAlive = nullptr;

	if (0 < noCheckHitTimer) --noCheckHitTimer;


	// �A���t�@�l�Ɋւ��鏈��

	// �A���t�@�𖈃t���[��0�ɋ߂Â���B
	alpha -= alpha / 10.0f;

	if (isHitPlayer) {

		alpha = (float)isMoveTimer / IS_MOVE_TIMER * 255.0f;

	}
	if (isMove) {

		alpha = 255;

	}


}

#include"D3D12App.h"
void DossunBlock::Draw()
{

	/*===== �`�揈�� =====*/

	Vec2<float> scrollShakeZoom = { ScrollMgr::Instance()->scrollAmount.x * ScrollMgr::Instance()->zoom - ShakeMgr::Instance()->shakeAmount.x * ScrollMgr::Instance()->zoom,
	ScrollMgr::Instance()->scrollAmount.y * ScrollMgr::Instance()->zoom - ShakeMgr::Instance()->shakeAmount.y * ScrollMgr::Instance()->zoom };

	Vec2<float> posZoom = { pos.x * ScrollMgr::Instance()->zoom,pos.y * ScrollMgr::Instance()->zoom };

	Vec2<float> sizeZoom = { size.x * ScrollMgr::Instance()->zoom, size.y * ScrollMgr::Instance()->zoom };

	Vec2<float>leftUp = { posZoom.x - sizeZoom.x - scrollShakeZoom.x,
		posZoom.y - sizeZoom.y - scrollShakeZoom.y };
	Vec2<float>rightBottom = { posZoom.x + sizeZoom.x - scrollShakeZoom.x,
		posZoom.y + sizeZoom.y - scrollShakeZoom.y };

	DrawFunc::DrawBox2D(leftUp, rightBottom, Color(100, 100, 100, 255), D3D12App::Instance()->GetBackBuffFormat(), true);

	// �ړ����n�߂�Ƃ��Ɍ��点��p
	DrawFunc::DrawBox2D(leftUp, rightBottom, Color(255, 255, 255, alpha), D3D12App::Instance()->GetBackBuffFormat(), true, AlphaBlendMode_Trans);
}

void DossunBlock::CheckHit(const vector<vector<int>>& mapData)
{

	// �t���O���������B
	bool isDossunTop = false;
	bool isDossunRight = false;
	bool isDossunLeft = false;
	bool isDossunBottom = false;

	// �}�b�v�`�b�v�ƃh�b�X���u���b�N�̓����蔻��

	float offset = 1.0f;

	if (noCheckHitTimer <= 0) {

		if (moveDir.y != 0) {
			isDossunTop = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(pos, size, mapData, INTERSECTED_TOP) != INTERSECTED_NONE;
			if (!isDossunTop) isDossunTop = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(pos + Vec2<float>(-size.x + 10.0f, 0), size, mapData, INTERSECTED_TOP) != INTERSECTED_NONE;
			if (!isDossunTop) isDossunTop = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(pos + Vec2<float>(size.x - 10.0f, 0), size, mapData, INTERSECTED_TOP) != INTERSECTED_NONE;
			pos.y -= offset;
		}
		if (moveDir.x != 0) {
			isDossunRight = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(pos, size, mapData, INTERSECTED_RIGHT) != INTERSECTED_NONE;
			if (!isDossunRight) isDossunRight = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(pos + Vec2<float>(0, -size.y + 10.0f), size, mapData, INTERSECTED_RIGHT) != INTERSECTED_NONE;
			if (!isDossunRight) isDossunRight = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(pos + Vec2<float>(0, size.y - 10.0f), size, mapData, INTERSECTED_RIGHT) != INTERSECTED_NONE;
			pos.x -= offset;
		}
		if (moveDir.x != 0) {
			isDossunLeft = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(pos, size, mapData, INTERSECTED_LEFT) != INTERSECTED_NONE;
			if (!isDossunLeft) isDossunLeft = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(pos + Vec2<float>(0, -size.y + 10.0f), size, mapData, INTERSECTED_LEFT) != INTERSECTED_NONE;
			if (!isDossunLeft) isDossunLeft = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(pos + Vec2<float>(0, size.y - 10.0f), size, mapData, INTERSECTED_LEFT) != INTERSECTED_NONE;
			pos.x += offset;
		}
		if (moveDir.y != 0) {
			isDossunBottom = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(pos, size, mapData, INTERSECTED_BOTTOM) != INTERSECTED_NONE;
			if (!isDossunBottom) isDossunBottom = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(pos + Vec2<float>(-size.x + 10.0f, 0), size, mapData, INTERSECTED_BOTTOM) != INTERSECTED_NONE;
			if (!isDossunBottom) isDossunBottom = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(pos + Vec2<float>(size.x - 10.0f, 0), size, mapData, INTERSECTED_BOTTOM) != INTERSECTED_NONE;
			pos.y += offset;
		}

	}

	// �ǂ�������ɂԂ����Ă���Γ�����������ɂ���B
	if (isDossunTop || isDossunRight || isDossunLeft || isDossunBottom) {

		// �ŏ���1F�ڂ̓V�F�C�N������B
		if (changeDirTimer == 1) {

			ShakeMgr::Instance()->SetShake(ShakeMgr::Instance()->DOSSUN_LOW_POWER_SHAKE_AMOUNT);

			const float div = 5.0f;

			// �ړ����Ă�������ɉ����Đ�������ʒu��ς���B
			if (0 < fabs(moveDir.x)) {

				// ���E�ɓ����Ă���ꍇ

				const float generateCount = size.y * 2.0f / div;

				for (int index = 0; index < generateCount; ++index) {

					ParticleMgr::Instance()->Generate(pos + Vec2<float>(size.x * moveDir.x, -size.y + (index * div)), moveDir, DOSSUN);

				}

				// ��Ɖ��ɐ�������B
				ParticleMgr::Instance()->Generate(pos + Vec2<float>(size.x * moveDir.x, -size.y), moveDir + Vec2<float>(0, -1.0f), DOSSUN);
				ParticleMgr::Instance()->Generate(pos + Vec2<float>(size.x * moveDir.x, size.y), moveDir + Vec2<float>(0, 1.0f), DOSSUN);
				ParticleMgr::Instance()->Generate(pos + Vec2<float>(size.x * moveDir.x, -size.y), Vec2<float>(0.0f, -1.0f), DOSSUN);
				ParticleMgr::Instance()->Generate(pos + Vec2<float>(size.x * moveDir.x, size.y), Vec2<float>(0.0f, 1.0f), DOSSUN);

			}
			else if (0 < fabs(moveDir.y)) {

				// �㉺�ɓ����Ă���ꍇ

				const float generateCount = size.x * 2.0f / div;

				for (int index = 0; index < generateCount; ++index) {

					ParticleMgr::Instance()->Generate(pos + Vec2<float>(-size.x + (index * div), size.y * moveDir.y), moveDir, DOSSUN);

				}

				// ��Ɖ��ɐ�������B
				ParticleMgr::Instance()->Generate(pos + Vec2<float>(-size.x, size.y * moveDir.y), moveDir + Vec2<float>(-1.0f, 0.0f), DOSSUN);
				ParticleMgr::Instance()->Generate(pos + Vec2<float>(size.x, size.y * moveDir.y), moveDir + Vec2<float>(1.0f, 0.0f), DOSSUN);
				ParticleMgr::Instance()->Generate(pos + Vec2<float>(-size.x, size.y * moveDir.y), Vec2<float>(-1.0f, 0.0f), DOSSUN);
				ParticleMgr::Instance()->Generate(pos + Vec2<float>(size.x, size.y * moveDir.y), Vec2<float>(1.0f, 0.0f), DOSSUN);

			}

		}

		// �����]���^�C�}�[�����Z�B
		if (isMove) {
			++changeDirTimer;
		}

	}
	else {

		// �����]���^�C�}�[���������B
		changeDirTimer = 0;

	}

}

