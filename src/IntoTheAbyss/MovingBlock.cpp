#include "MovingBlock.h"
#include "ScrollMgr.h"
#include "ShakeMgr.h"
#include "MapChipCollider.h"

MovingBlock::MovingBlock(const Vec2<float>& generatePos)
{

	/*===== �R���X�g���N�^ =====*/

	pos = generatePos;
	prevFramePos = generatePos;
	vel = {};
	gravity = 0;
	onGround = false;
	isCloseToThePlayer = true;

}

void MovingBlock::Init()
{
	pos = {};
	prevFramePos = {};
	vel = {};
	gravity = 0;
	onGround = false;
	isCloseToThePlayer = true;
}

void MovingBlock::Update(const Vec2<float>& playerPos)
{

	/*===== �X�V���� =====*/

	// �v���C���[�Ƃ̋��������߂�B
	float playerDistance = pos.Distance(playerPos);

	// ���������ȏ㗣��Ă����珈�����s��Ȃ��B
	if (playerDistance >= DO_PROCESS_DISTANCE) {

		isCloseToThePlayer = false;
		return;

	}
	else {

		isCloseToThePlayer = true;

	}


	/*-- �d�͂̍X�V���� --*/

	// �ڒn���Ă�����
	if (onGround) {

		// �d�͂�����������B				onGround�����t���[���ς��݂����ɂȂ�����A�����ŏ��������Ă���ʂ�1�Ƃ��ɂ��悤�B
		gravity = 0;

		// Y���̈ړ��ʂ𖳌�������B
		vel.y = 0;

	}
	// �ڒn���Ă��Ȃ�������
	else {

		// �d�͂�ǉ�����B
		gravity += ADD_GRAVITY;

	}

	// �d�͂����E�𒴂��Ȃ��悤�ɂ���B
	if (gravity >= MAX_GRAVITY) gravity = MAX_GRAVITY;


	/*-- �ړ��ʂ̍X�V���� --*/

	// �ړ��ʂ�0�ɋߕt����B
	vel.x -= vel.x / 10.0f;
	vel.y -= vel.y / 10.0f;

	// �ړ��ʂ����ȉ���������0�ɂ���B
	if (fabs(vel.x) <= 0.1f) vel.x = 0;
	if (fabs(vel.y) <= 0.1f) vel.y = 0;


	/*-- �ړ����� --*/

	// ���̃t���[���̍��W��ۑ��B
	prevFramePos = pos;

	// �ړ�������B
	pos += vel;

	// �d�͂�����B
	pos.y += gravity;

}

#include"TexHandleMgr.h"
#include"DrawFunc.h"
void MovingBlock::Draw(const int& graphHandle)
{

	/*===== �`�揈�� =====*/

	// �u���b�N��`�悷��B
	//DrawExtendGraph(pos.x * ScrollMgr::Instance()->zoom - SCALE * ScrollMgr::Instance()->zoom - scrollShakeZoom.x,
	//	pos.y * ScrollMgr::Instance()->zoom - SCALE * ScrollMgr::Instance()->zoom - scrollShakeZoom.y,
	//	pos.x * ScrollMgr::Instance()->zoom + SCALE * ScrollMgr::Instance()->zoom - scrollShakeZoom.x,
	//	pos.y * ScrollMgr::Instance()->zoom + SCALE * ScrollMgr::Instance()->zoom - scrollShakeZoom.y,
	//	graphHandle, FALSE);
	Vec2<float>leftUp = { pos.x - SCALE,pos.y - SCALE };
	leftUp = ScrollMgr::Instance()->Affect(leftUp);
	Vec2<float>rightBottom = { pos.x + SCALE,pos.y + SCALE };
	rightBottom = ScrollMgr::Instance()->Affect(rightBottom);

	DrawFunc::DrawExtendGraph2D(leftUp, rightBottom, TexHandleMgr::GetTexBuffer(graphHandle));

}

void MovingBlock::HitGround()
{

	onGround = true;
	vel.y = 0;
	gravity = 0;

}

void MovingBlock::HitBullet(const Vec2<float>& bulletForwardVec)
{

	vel = { bulletForwardVec.x * ADD_VEL, bulletForwardVec.y * ADD_VEL };

	// �ړ��ʂ����E�𒴂��Ȃ��悤�ɂ���B
	if (vel.x != 0) {

		int sigh = vel.x / vel.x;
		if (fabs(vel.x) >= MAX_VEL) {

			vel.x = MAX_VEL * sigh;

		}

	}
	if (vel.y != 0) {

		int sigh = vel.y / vel.y;
		if (fabs(vel.y) >= MAX_VEL) {

			vel.y = MAX_VEL * sigh;

		}

	}

}

void MovingBlock::CheckHitMapChip(const vector<vector<int>>& mapData)
{

	/*===== �}�b�v�`�b�v�Ƃ̓����蔻�� =====*/

	INTERSECTED_LINE intersectedLine{};

	// �����蔻����s���B
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheVel(pos, prevFramePos, vel, Vec2<float>(SCALE, SCALE), onGround, mapData);

	// �X�P�[���ɂ�铖���蔻����s���ďd�Ȃ�Ȃ��悤�ɂ���B
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(pos, Vec2<float>(SCALE, SCALE), mapData, INTERSECTED_BOTTOM);
	if (intersectedLine == INTERSECTED_TOP) HitGround();
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(pos, Vec2<float>(SCALE, SCALE), mapData, INTERSECTED_TOP);
	if (intersectedLine == INTERSECTED_TOP) HitGround();
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(pos, Vec2<float>(SCALE, SCALE), mapData, INTERSECTED_LEFT);
	if (intersectedLine == INTERSECTED_TOP) HitGround();
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(pos, Vec2<float>(SCALE, SCALE), mapData, INTERSECTED_RIGHT);
	if (intersectedLine == INTERSECTED_TOP) HitGround();

	// �X�ɏd�Ȃ�h�~(�p�΍�)�̏������s���B ���2�̔���ŏd�Ȃ�͂Ȃ��Ȃ��Ă���͂��Ȃ̂ŁA�ȉ��̏����͊p�ȊO�ł͌Ă΂�Ȃ��͂��c

	const int MAP_CHIP_X = mapData[0].size();
	const int MAP_CHIP_Y = mapData.size();
	for (int mapY = 0; mapY < MAP_CHIP_Y; ++mapY) {

		for (int mapX = 0; mapX < MAP_CHIP_X; ++mapX) {

			// ���̃}�b�v�`�b�v��1~9�ȊO�������珈�����΂��B
			if (!(mapData[mapY][mapX] >= 1 && mapData[mapY][mapX] <= 9)) continue;

			// �}�b�v�`�b�v�̍��W�����߂�B
			const float centerX = mapX * MAP_CHIP_SIZE;
			const float centerY = mapY * MAP_CHIP_SIZE;

			// X���AY���ł̋��������߂�B
			float distanceX = centerX - pos.x;
			float distanceY = centerY - pos.y;

			// �����̐�Βl
			float distanceXABS = fabs(distanceX);
			float distanceYABS = fabs(distanceY);

			// �ړ��u���b�N�ƃv���C���[�̑傫���̍��v�B
			Vec2<float> playerMovingBlockSize = { MAP_CHIP_HALF_SIZE + SCALE,MAP_CHIP_HALF_SIZE + SCALE };

			// XY�̋��������I�u���b�N�ƃv���C���[�̃T�C�Y�ȏゾ�����珈�����΂��B
			if (distanceXABS >= playerMovingBlockSize.x ||
				distanceYABS >= playerMovingBlockSize.y) continue;

			// XY�̉��������������ĉ������̂݉����߂��������s���B
			bool isPushBackX = distanceXABS >= distanceYABS;

			// X�������߂��ꍇ�B
			if (isPushBackX) {

				// �v���C���[���E�ɋ����ꍇ
				if (distanceX <= 0) {

					// �����߂����������߂�B
					float pushBackDistance = playerMovingBlockSize.x + distanceX;

					// �����߂��B
					pos.x += pushBackDistance;

				}
				else {

					// �����߂����������߂�B
					float pushBackDistance = playerMovingBlockSize.x - distanceX;

					// �����߂��B
					pos.x -= pushBackDistance;

				}

			}
			// Y�������߂��ꍇ�B
			else {

				if (distanceY >= 0) {

					// �����߂����������߂�B
					float pushBackDistance = playerMovingBlockSize.y - distanceY;

					// �����߂��B
					pos.y -= pushBackDistance;

					// ���̏ꍇ�͒n�ʂɂ���������ԁB
					//HitMapChipBottom();
					onGround = true;

				}
				else {

					// �����߂����������߂�B
					float pushBackDistance = playerMovingBlockSize.y + distanceY;

					// �����߂��B
					pos.y += pushBackDistance;

				}

			}

		}

	}

}
