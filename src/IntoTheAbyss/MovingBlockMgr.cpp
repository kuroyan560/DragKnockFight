#include "MovingBlockMgr.h"
#include "MovingBlock.h"
#include "CSVLoader.h"
#include "MapChipCollider.h"
#include "BulletMgr.h"
#include "Bullet.h"

void MovingBlockMgr::Load(const vector<vector<int>>& mapData)
{

	/*===== ���I�u���b�N�����[�h =====*/

	const int MOVING_SIZE_Y = mapData.size();		// �c�̗v�f��
	const int MOVING_SIZE_X = mapData[0].size();	// ���̗v�f��
	const int MOVING_BLOCK_INDEX = 10;				// ���I�u���b�N�𐶐�����C���f�b�N�X�ԍ�

	for (int height = 0; height < MOVING_SIZE_Y; ++height) {

		for (int width = 0; width < MOVING_SIZE_X; ++width) {

			// ���̗v�f�����I�u���b�N�𐶐�����C���f�b�N�X�ԍ��������炾������B
			if (mapData[height][width] != MOVING_BLOCK_INDEX) continue;

			// ��������B
			movingBlock.push_back(make_shared<MovingBlock>(Vec2<float>(width * MAP_CHIP_SIZE, height * MAP_CHIP_SIZE)));

		}

	}

}

void MovingBlockMgr::Init()
{
	for (int index = 0; index < movingBlock.size(); ++index) movingBlock[index]->Init();
}

void MovingBlockMgr::Update(const Vec2<float>& playerPos)
{

	/*===== �X�V���� =====*/

	const int BLOCK_SIZE = movingBlock.size();
	for (int index = 0; index < BLOCK_SIZE; ++index) {
		movingBlock[index]->Update(playerPos);
	}

}

void MovingBlockMgr::Draw(const int& graphHandle)
{

	/*===== �`�揈�� =====*/

	const int BLOCK_SIZE = movingBlock.size();
	for (int index = 0; index < BLOCK_SIZE; ++index) {
		movingBlock[index]->Draw(graphHandle);
	}

}

void MovingBlockMgr::CheckHit(const vector<vector<int>>& mapData)
{

	/*===== �u���b�N�ƃu���b�N�̓����蔻�� =====*/

	const int MOVING_BLOCK_SIZE = movingBlock.size();

	// ���I�u���b�N�̃T�C�Y��0�ȉ��������珈�����s��Ȃ��B
	if (MOVING_BLOCK_SIZE <= 0) return;

	for (int baseIndex = 0; baseIndex < MOVING_BLOCK_SIZE; ++baseIndex) {

		// �������s��Ȃ��t���O�������Ă����珈�����΂��B
		if (!movingBlock[baseIndex]->isCloseToThePlayer) continue;

		for (int targetIndex = 0; targetIndex < MOVING_BLOCK_SIZE; ++targetIndex) {

			// �C���f�b�N�X�������������瓖���蔻����s��Ȃ��B
			if (baseIndex == targetIndex) continue;

			// �Ώۂ̃u���b�N�̏������s��Ȃ��t���O�������Ă����珈�����΂��B
			if (!movingBlock[targetIndex]->isCloseToThePlayer) continue;

			// X���AY���ł̋��������߂�B
			float distanceX = MovingBlockMgr::Instance()->movingBlock[baseIndex]->pos.x - MovingBlockMgr::Instance()->movingBlock[targetIndex]->pos.x;
			float distanceY = MovingBlockMgr::Instance()->movingBlock[baseIndex]->pos.y - MovingBlockMgr::Instance()->movingBlock[targetIndex]->pos.y;

			// �����̐�Βl
			float distanceXABS = fabs(distanceX);
			float distanceYABS = fabs(distanceY);

			// �ړ��u���b�N�ƃv���C���[�̑傫���̍��v�B
			Vec2<float> playerMovingBlockSize = { MovingBlockMgr::Instance()->movingBlock[baseIndex]->SCALE + MovingBlockMgr::Instance()->movingBlock[targetIndex]->SCALE,
				MovingBlockMgr::Instance()->movingBlock[baseIndex]->SCALE + MovingBlockMgr::Instance()->movingBlock[targetIndex]->SCALE };

			// XY�̋��������I�u���b�N�ƃv���C���[�̃T�C�Y�ȏゾ�����珈�����΂��B
			if (distanceXABS >= playerMovingBlockSize.x ||
				distanceYABS >= playerMovingBlockSize.y) continue;

			// XY�̉��������������ĉ������̂݉����߂��������s���B
			bool isPushBackX = distanceXABS >= distanceYABS;

			// �����̈ړ��ʂ����߂āA�ړ��ʂ����������𓮂����B
			int moveIndex = 0;
			int moveIndexY = 0;
			Vec2<float> velTarget = MovingBlockMgr::Instance()->movingBlock[targetIndex]->vel;
			velTarget.y += MovingBlockMgr::Instance()->movingBlock[targetIndex]->gravity;
			Vec2<float> velBase = MovingBlockMgr::Instance()->movingBlock[baseIndex]->vel;
			velBase.y += MovingBlockMgr::Instance()->movingBlock[baseIndex]->gravity;
			if (velTarget.Length() >= velBase.Length()) {

				moveIndex = baseIndex;
				moveIndexY = targetIndex;

			}
			else {

				moveIndex = targetIndex;
				moveIndexY = baseIndex;

			}

			// X�������߂��ꍇ�B
			if (isPushBackX) {

				// �v���C���[���E�ɋ����ꍇ
				if (distanceX <= 0) {

					// �����߂����������߂�B
					float pushBackDistance = playerMovingBlockSize.x + distanceX;

					// �����߂��B
					MovingBlockMgr::Instance()->movingBlock[moveIndex]->pos.x += pushBackDistance;

				}
				else {

					// �����߂����������߂�B
					float pushBackDistance = playerMovingBlockSize.x - distanceX;

					// �����߂��B
					MovingBlockMgr::Instance()->movingBlock[moveIndex]->pos.x -= pushBackDistance;

				}

			}
			// Y�������߂��ꍇ�B
			else {

				if (distanceY >= 0) {

					// �����߂����������߂�B
					float pushBackDistance = playerMovingBlockSize.y - distanceY;

					// �����߂��B
					MovingBlockMgr::Instance()->movingBlock[moveIndexY]->pos.y -= pushBackDistance;

					// ���̏ꍇ�͒n�ʂɂ���������ԁB
					MovingBlockMgr::Instance()->movingBlock[targetIndex]->onGround = true;
					MovingBlockMgr::Instance()->movingBlock[baseIndex]->onGround = true;
					MovingBlockMgr::Instance()->movingBlock[targetIndex]->gravity = 0;
					MovingBlockMgr::Instance()->movingBlock[baseIndex]->gravity = 0;

				}
				else {

					// �����߂����������߂�B
					float pushBackDistance = playerMovingBlockSize.y + distanceY;

					// �����߂��B
					MovingBlockMgr::Instance()->movingBlock[moveIndexY]->pos.y += pushBackDistance;

					MovingBlockMgr::Instance()->movingBlock[targetIndex]->onGround = true;
					MovingBlockMgr::Instance()->movingBlock[baseIndex]->onGround = true;
					MovingBlockMgr::Instance()->movingBlock[targetIndex]->gravity = 0;
					MovingBlockMgr::Instance()->movingBlock[baseIndex]->gravity = 0;

				}

			}

		}

	}


	/*===== �}�b�v�`�b�v�Ƃ̓����蔻�� =====*/

	for (int index = 0; index < MOVING_BLOCK_SIZE; ++index) {

		// �v���C���[�Ɨ���Ă����珈�����s��Ȃ��B
		if (!movingBlock[index]->isCloseToThePlayer) continue;

		// �}�b�v�`�b�v�Ƃ̓����蔻��
		movingBlock[index]->CheckHitMapChip(mapData);

	}


	/*===== �e�Ƃ̓����蔻�� =====*/

	// �u���b�N�ƒe�̍��v�T�C�Y�B
	const float BULLET_BLOCK_SIZE = movingBlock[0]->SCALE + BulletMgr::Instance()->bullets[0]->MAX_RADIUS;

	// �u���b�N�̐����������s���B
	for (int blockIndex = 0; blockIndex < MOVING_BLOCK_SIZE; ++blockIndex) {

		// �u���b�N���������s��Ȃ��t���O�������Ă����珈�����΂��B
		if (!movingBlock[blockIndex]->isCloseToThePlayer) continue;

		Vec2<float>* blockPos = &(movingBlock[blockIndex]->pos);

		// �e�̐����������s���B
		const int BULLET_COUNT = BulletMgr::Instance()->BULLET_COUNT;
		for (int bulletIndex = 0; bulletIndex < BULLET_COUNT; ++bulletIndex) {

			// �e�̐����t���O�������Ă��Ȃ������珈�����΂��B
			if (!BulletMgr::Instance()->bullets[bulletIndex]->isActive) continue;

			Vec2<float>* bulletPos = &(BulletMgr::Instance()->bullets[bulletIndex]->pos);

			// ���������߂�֐����d���̂ŁA�����ő�G�c�ɗ���Ă����珈�����΂������������B �d��������R�����g�A�E�g���O���B
			//if (!(blockPos->x + BULLET_BLOCK_SIZE >= bulletPos->x && blockPos->x - BULLET_BLOCK_SIZE <= bulletPos->x &&
			//	blockPos->y + BULLET_BLOCK_SIZE >= bulletPos->y && blockPos->y - BULLET_BLOCK_SIZE <= bulletPos->y)) continue;

			// �e�Ƃ̋��������߂�B
			float bulletDistance = blockPos->Distance(*bulletPos);

			// ���������ȏ㗣��Ă����珈�����΂��B
			if (bulletDistance > BULLET_BLOCK_SIZE) continue;

			// �����܂ŗ����瓖�����Ă���Ƃ������ƁB

			// �u���b�N��e�̐i�s�����x�N�g���̕��ɔ�΂��B
			movingBlock[blockIndex]->HitBullet(BulletMgr::Instance()->bullets[bulletIndex]->forwardVec);

			// �e�����ł�����B
			BulletMgr::Instance()->bullets[bulletIndex]->Init();

		}

	}

}
