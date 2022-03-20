#include "Player.h"
#include "PlayerHand.h"
#include "BulletMgr.h"
#include "ScrollMgr.h"
#include "ShakeMgr.h"
#include "MapChipCollider.h"
#include "MovingBlockMgr.h"
#include "MovingBlock.h"
#include "BulletParticleMgr.h"
#include <cmath>

#include"TexHandleMgr.h"
#include"UsersInput.h"
#include"DrawFunc.h"
#include"WinApp.h"

Vec2<float> Player::GetGeneratePos()
{
	return WinApp::Instance()->GetExpandWinCenter() * Vec2<float>(1.0f, 0.25f);
}

Player::Player()
{
	/*====== �R���X�g���N�^ =====*/

	// �v���C���[�̈ʒu���Z�b�g
	centerPos = GetGeneratePos();

	// �ړ��ʂ����Z�b�g
	vel = {};

	// �d�͂����Z�b�g
	gravity = 0;

	// �ڒn���Ă��Ȃ��t���O�����Z�b�g
	onGround = false;

	// �r���Z�b�g
	static const float OFFSET = -8.0f;
	lHand = make_unique<PlayerHand>();
	lHand->Init(-PLAYER_SIZE.x + OFFSET);
	rHand = make_unique<PlayerHand>();
	rHand->Init(PLAYER_SIZE.x - OFFSET);

	// �A�˃^�C�}�[��������
	rapidFireTimerLeft = 0;
	rapidFireTimerRight = 0;

	// �d�͖������^�C�}�[���������B
	gravityInvalidTimer = 0;

	isPrevFrameShotBeacon = false;
	firstShot = false;

	// �摜�����[�h
	playerGraph = TexHandleMgr::LoadGraph("resource/IntoTheAbyss/Player.png");

	// �ǃt���O��������
	isWallRight = false;
	isWallLeft = false;

	// �ǃY���t���O���������B
	for (int index = 0; index < 4; ++index) isSlippingWall[index] = false;

	// �����ʒu�ɖ߂�܂ł̃^�C�}�[��������
	handReturnTimer = DEF_HAND_RETURN_TIMER;

	// ��������ʒu�ɖ߂��B
	rHand->SetAngle(DEF_RIGHT_HAND_ANGLE);
	lHand->SetAngle(DEF_LEFT_HAND_ANGLE);

	// ���͂��Ă��班������������^�C�}�[��������
	asSoonAsInputTimer = 0;

	inBubble = false;

}

Player::~Player()
{
}

void Player::Init(const Vec2<float>& INIT_POS)
{

	/*===== ���������� =====*/

	// �v���C���[�̈ʒu���Z�b�g
	centerPos = INIT_POS;
	prevFrameCenterPos = INIT_POS;

	// �ړ��ʂ����Z�b�g
	vel = {};

	// �d�͂����Z�b�g
	gravity = 0;

	// �ڒn���Ă��Ȃ��t���O�����Z�b�g
	onGround = false;
	firstShot = false;

	// �r���Z�b�g
	lHand->Init(-PLAYER_SIZE.x);
	rHand->Init(PLAYER_SIZE.x);

	// �A�˃^�C�}�[��������
	rapidFireTimerLeft = 0;
	rapidFireTimerRight = 0;

	// �d�͖������^�C�}�[���������B
	gravityInvalidTimer = 0;

	// �ǃt���O��������
	isWallRight = false;
	isWallLeft = false;

	// �ǃY���t���O���������B
	for (int index = 0; index < 4; ++index) isSlippingWall[index] = false;

	// �����ʒu�ɖ߂�܂ł̃^�C�}�[��������
	handReturnTimer = DEF_HAND_RETURN_TIMER;

	// ��������ʒu�ɖ߂��B
	rHand->SetAngle(DEF_RIGHT_HAND_ANGLE);
	lHand->SetAngle(DEF_LEFT_HAND_ANGLE);

	// ���͂��Ă��班������������^�C�}�[��������
	asSoonAsInputTimer = 0;

	inBubble = false;

}

void Player::Update(const vector<vector<int>> mapData)
{

	/*===== ���͏��� =====*/

	// ���͂Ɋւ���X�V�������s���B
	Input(mapData);

	/*===== �X�V���� =====*/

	// �ړ��Ɋւ��鏈��
	Move();

	// �d�͂Ɋւ���X�V����
	UpdateGravity();

	// �A�˃^�C�}�[���X�V
	if (rapidFireTimerLeft > 0) --rapidFireTimerLeft;
	if (rapidFireTimerRight > 0) --rapidFireTimerRight;

	// �r���X�V
	lHand->Update(centerPos);
	rHand->Update(centerPos);

	// �e���X�V
	BulletMgr::Instance()->Update();

	// ���͂��󂯕t���Ȃ��t���O��������
	isWallRight = false;
	isWallLeft = false;

	// �v���C���[�̂������̃}�b�v�`�b�v���W����������B
	int mapX = centerPos.x / MAP_CHIP_SIZE;
	int mapY = (centerPos.y - PLAYER_SIZE.y - 1.0f) / MAP_CHIP_SIZE;
	if (mapX <= 0) mapX = 1;
	if (mapX >= mapData[0].size()) mapX = mapData[0].size() - 1;
	if (mapY <= 0) mapY = 1;
	if (mapY >= mapData.size()) mapY = mapData.size() - 1;

	// ���̃}�b�v�`�b�v���u���b�N�ŁAX�������̈ړ��ʂ����ȏゾ������p�[�e�B�N���𐶐�����B
	if (mapData[mapY][mapX] > 0 && mapData[mapY][mapX] < 10 && fabs(vel.x) >= 10.0f)BulletParticleMgr::Instance()->Generate(Vec2<float>(centerPos.x, centerPos.y - PLAYER_SIZE.y), Vec2<float>(0, -1));

	// �ǃY���̃p�[�e�B�N���𐶐��B
	if (isSlippingWall[PLAYER_TOP]) BulletParticleMgr::Instance()->Generate(Vec2<float>(centerPos.x, centerPos.y - PLAYER_SIZE.y), Vec2<float>(0, -1));
	if (isSlippingWall[PLAYER_RIGHT]) BulletParticleMgr::Instance()->Generate(Vec2<float>(centerPos.x + PLAYER_SIZE.x, centerPos.y), Vec2<float>(1, 0));
	if (isSlippingWall[PLAYER_BOTTOM]) BulletParticleMgr::Instance()->Generate(Vec2<float>(centerPos.x, centerPos.y + PLAYER_SIZE.y), Vec2<float>(0, 1));
	if (isSlippingWall[PLAYER_LEFT]) BulletParticleMgr::Instance()->Generate(Vec2<float>(centerPos.x - PLAYER_SIZE.x, centerPos.y), Vec2<float>(-1, 0));

	// �ǃY���t���O���������B
	for (int index = 0; index < 4; ++index) isSlippingWall[index] = false;

	// ��莞�ԓ��͂��Ȃ�������r�������ʒu�ɖ߂��^�C�}�[���X�V�B
	if (handReturnTimer > 0) {
		--handReturnTimer;
	}
	// ��������̈ړ��ʂ����݂�����NoInput������������B
	if (vel.x != 0 || vel.y != 0 || gimmickVel.x != 0 || gimmickVel.y != 0) handReturnTimer = DEF_HAND_RETURN_TIMER;
	// 0�ȉ��ɂȂ�����
	if (handReturnTimer <= 0) {

		// ��������ʒu�ɖ߂��B
		rHand->SetAngle(DEF_RIGHT_HAND_ANGLE);
		lHand->SetAngle(DEF_LEFT_HAND_ANGLE);

		// Syokiiti Ni Modosu Timer Ga <=0 no Tokiha Honrai no Amount Wo Herasu
		rHand->SetIsNoInputTimer(true);
		lHand->SetIsNoInputTimer(true);

	}
	else {

		rHand->SetIsNoInputTimer(false);
		lHand->SetIsNoInputTimer(false);

	}

	// �M�~�b�N����^������ړ��ʂ��������B
	gimmickVel = {};

	// ���͂���Ă��琔�t���[�����擾����^�C�}�[���X�V�B
	if (asSoonAsInputTimer > 0) --asSoonAsInputTimer;

}

void Player::Draw()
{
	static enum { LEFT, RIGHT, DEFAULT = RIGHT, DIR_NUM }DIR = DEFAULT;
	if (vel.x < 0)DIR = LEFT;
	if (0 < vel.x)DIR = RIGHT;

	static const int HAND_GRAPH[DIR_NUM] =
	{
		TexHandleMgr::LoadGraph("resource/IntoTheAbyss/hand_L.png"),
		TexHandleMgr::LoadGraph("resource/IntoTheAbyss/hand_R.png"),
	};

	/*===== �`�揈�� =====*/

	Vec2<float> scrollShakeZoom = ScrollMgr::Instance()->scrollAmount + ShakeMgr::Instance()->shakeAmount;
	scrollShakeZoom.x *= ScrollMgr::Instance()->zoom;
	scrollShakeZoom.y *= ScrollMgr::Instance()->zoom;

	// �v���C���[�̕`�揈��
	Vec2<float>leftUp = { centerPos.x * ScrollMgr::Instance()->zoom - PLAYER_SIZE.x * ScrollMgr::Instance()->zoom - scrollShakeZoom.x,
		centerPos.y * ScrollMgr::Instance()->zoom - PLAYER_SIZE.y * ScrollMgr::Instance()->zoom - scrollShakeZoom.y };
	Vec2<float>rightBottom = { centerPos.x * ScrollMgr::Instance()->zoom + PLAYER_SIZE.x * ScrollMgr::Instance()->zoom - scrollShakeZoom.x,
		centerPos.y * ScrollMgr::Instance()->zoom + PLAYER_SIZE.y * ScrollMgr::Instance()->zoom - scrollShakeZoom.y };
	const float expRate = EXT_RATE * ScrollMgr::Instance()->zoom;

	if (DIR == RIGHT)
	{
		rHand->Draw(expRate, HAND_GRAPH[DEFAULT ? RIGHT : LEFT], DEF_RIGHT_HAND_ANGLE, { 0.0f,0.0f });
	}
	else if (DIR == LEFT)
	{
		lHand->Draw(expRate, HAND_GRAPH[DEFAULT ? LEFT : RIGHT], DEF_LEFT_HAND_ANGLE, { 1.0f,0.0f });
	}

	//����
	//DrawFunc::DrawExtendGraph2D(leftUp, rightBottom, TexHandleMgr::GetTexBuffer(playerGraph), AlphaBlendMode_Trans, { DIR == LEFT,false });
	DrawFunc::DrawRotaGraph2D(centerPos * ScrollMgr::Instance()->zoom - scrollShakeZoom, expRate, 0.0f, TexHandleMgr::GetTexBuffer(playerGraph),
		{ 0.5f,0.5f }, AlphaBlendMode_Trans, { DIR == LEFT,false });

	if (DIR == RIGHT)
	{
		lHand->Draw(expRate, HAND_GRAPH[DEFAULT ? LEFT : RIGHT], DEF_LEFT_HAND_ANGLE, { 1.0f,0.0f });
	}
	else if (DIR == LEFT)
	{
		rHand->Draw(expRate, HAND_GRAPH[DEFAULT ? RIGHT : LEFT], DEF_RIGHT_HAND_ANGLE, { 0.0f,0.0f });
	}

	// �e��`��
	BulletMgr::Instance()->Draw();

}

void Player::CheckHit(const vector<vector<int>> mapData, vector<Bubble>& bubble, TimeStopTestBlock& testBlock)
{

	/*===== �}�b�v�`�b�v�ƃv���C���[�Ƃ̓����蔻��S�� =====*/

	{

		// �}�b�v�`�b�v�ƃv���C���[�̓����蔻�� ��΂Ɋђʂ����Ȃ��ׂ̏���
		INTERSECTED_LINE intersectedLine = (INTERSECTED_LINE)MapChipCollider::Instance()->CheckHitMapChipBasedOnTheVel(centerPos, prevFrameCenterPos, vel, PLAYER_SIZE, onGround, mapData);

		// ���������ʒu�ɉ����ď����𕪂���B
		if (intersectedLine == INTERSECTED_TOP) {

			// ���ɓ��������ꍇ
			HitMapChipBottom();

		}
		else if (intersectedLine == INTERSECTED_RIGHT) {

			// ���ɓ��������ꍇ
			HitMapChipLeft();

		}
		else if (intersectedLine == INTERSECTED_BOTTOM) {

			// ��ɓ��������ꍇ
			HitMapChipTop();

		}
		else if (intersectedLine == INTERSECTED_LEFT) {

			// �E�ɓ��������ꍇ
			HitMapChipRight();

		}
		else if (intersectedLine == INTERSECTED_RISE) {

			// �̂��オ��̏ꏊ�ɓ��������ꍇ
			vel.y *= 0.5f;
			gravity = 0;

		}

		// �}�b�v�`�b�v�ƃv���C���[�̓����蔻�� ��΂ɔ킹�Ȃ����߂̏���
		intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(centerPos - Vec2<float>(0, PLAYER_SIZE.y / 2.0f), Vec2<float>(PLAYER_SIZE.x, PLAYER_SIZE.y / 2.0f), mapData, INTERSECTED_TOP);
		if (intersectedLine == INTERSECTED_TOP) HitMapChipBottom();
		intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(centerPos - Vec2<float>(0, PLAYER_SIZE.y / 2.0f), Vec2<float>(PLAYER_SIZE.x, PLAYER_SIZE.y / 2.0f), mapData, INTERSECTED_BOTTOM);
		if (intersectedLine == INTERSECTED_TOP) HitMapChipBottom();
		intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(centerPos - Vec2<float>(0, PLAYER_SIZE.y / 2.0f), Vec2<float>(PLAYER_SIZE.x, PLAYER_SIZE.y / 2.0f), mapData, INTERSECTED_LEFT);
		if (intersectedLine == INTERSECTED_TOP) HitMapChipBottom();
		intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(centerPos - Vec2<float>(0, PLAYER_SIZE.y / 2.0f), Vec2<float>(PLAYER_SIZE.x, PLAYER_SIZE.y / 2.0f), mapData, INTERSECTED_RIGHT);
		if (intersectedLine == INTERSECTED_TOP) HitMapChipBottom();

		intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(centerPos + Vec2<float>(0, PLAYER_SIZE.y / 2.0f), Vec2<float>(PLAYER_SIZE.x, PLAYER_SIZE.y / 2.0f), mapData, INTERSECTED_TOP);
		if (intersectedLine == INTERSECTED_TOP) HitMapChipBottom();
		intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(centerPos + Vec2<float>(0, PLAYER_SIZE.y / 2.0f), Vec2<float>(PLAYER_SIZE.x, PLAYER_SIZE.y / 2.0f), mapData, INTERSECTED_BOTTOM);
		if (intersectedLine == INTERSECTED_TOP) HitMapChipBottom();
		intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(centerPos + Vec2<float>(0, PLAYER_SIZE.y / 2.0f), Vec2<float>(PLAYER_SIZE.x, PLAYER_SIZE.y / 2.0f), mapData, INTERSECTED_LEFT);
		if (intersectedLine == INTERSECTED_TOP) HitMapChipBottom();
		intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(centerPos + Vec2<float>(0, PLAYER_SIZE.y / 2.0f), Vec2<float>(PLAYER_SIZE.x, PLAYER_SIZE.y / 2.0f), mapData, INTERSECTED_RIGHT);
		if (intersectedLine == INTERSECTED_TOP) HitMapChipBottom();

		// ���E�ɓ��������ۂɕǒނ肳���邽�߂̏����B
		int yChip = (centerPos.y + MAP_CHIP_HALF_SIZE) / MAP_CHIP_SIZE;
		int xChip = (centerPos.x - PLAYER_SIZE.x * 1.2f + MAP_CHIP_HALF_SIZE) / MAP_CHIP_SIZE;
		// �v���C���[�̍������}�b�v�`�b�v��������
		if (yChip > 0 && mapData[yChip][xChip] == 1 && mapData[yChip - 1][xChip] != 0) {
			HitMapChipLeft();
		}
		xChip = (centerPos.x + PLAYER_SIZE.x + MAP_CHIP_HALF_SIZE) / MAP_CHIP_SIZE;
		if (xChip >= mapData[yChip].size() - 1) xChip = mapData[yChip].size() - 1;
		// �v���C���[�̉E�����}�b�v�`�b�v��������
		if (yChip > 0 && mapData[yChip][xChip] == 1 && mapData[yChip - 1][xChip] != 0) {
			HitMapChipRight();
		}

	}


	/*===== ���I�u���b�N�ƃv���C���[�̓����蔻�� =====*/

	{

		const int MOVING_BLOCK_COUNT = MovingBlockMgr::Instance()->movingBlock.size();
		for (int movingIndex = 0; movingIndex < MOVING_BLOCK_COUNT; ++movingIndex) {

			// ���I�u���b�N�̃v���C���[����߂��t���O�������Ă��Ȃ������珈�����΂��B
			if (!MovingBlockMgr::Instance()->movingBlock[movingIndex]->isCloseToThePlayer) continue;

			// X���AY���ł̋��������߂�B
			float distanceX = MovingBlockMgr::Instance()->movingBlock[movingIndex]->pos.x - centerPos.x;
			float distanceY = MovingBlockMgr::Instance()->movingBlock[movingIndex]->pos.y - centerPos.y;

			// �����̐�Βl
			float distanceXABS = fabs(distanceX);
			float distanceYABS = fabs(distanceY);

			// �ړ��u���b�N�ƃv���C���[�̑傫���̍��v�B
			Vec2<float> playerMovingBlockSize = { MovingBlockMgr::Instance()->movingBlock[movingIndex]->SCALE + PLAYER_SIZE.x,MovingBlockMgr::Instance()->movingBlock[movingIndex]->SCALE + PLAYER_SIZE.y };

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
					centerPos.x += pushBackDistance;

				}
				else {

					// �����߂����������߂�B
					float pushBackDistance = playerMovingBlockSize.x - distanceX;

					// �����߂��B
					centerPos.x -= pushBackDistance;

				}

			}
			// Y�������߂��ꍇ�B
			else {

				if (distanceY >= 0) {

					// �����߂����������߂�B
					float pushBackDistance = playerMovingBlockSize.y - distanceY;

					// �����߂��B
					centerPos.y -= pushBackDistance;

					// ���̏ꍇ�͒n�ʂɂ���������ԁB
					HitMapChipBottom();

				}
				else {

					// �����߂����������߂�B
					float pushBackDistance = playerMovingBlockSize.y + distanceY;

					// �����߂��B
					centerPos.y += pushBackDistance;

				}

			}

		}

	}


	/*===== �Z���̓����蔻�� =====*/

	{

		// �u�Ԉړ��̒Z���̓����蔻����s���B
		if (!rHand->teleportPike.isHitWall && rHand->teleportPike.isActive) {
			bool onGround = false;
			INTERSECTED_LINE buff = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheVel(rHand->teleportPike.pos, rHand->teleportPike.prevFramePos, rHand->teleportPike.forwardVec, Vec2<float>(rHand->teleportPike.SCALE, rHand->teleportPike.SCALE), onGround, mapData);
			if (buff != INTERSECTED_NONE) { rHand->teleportPike.isHitWall = true; }

			// �����߂�
			Vec2<float> scale = Vec2<float>(rHand->teleportPike.SCALE * 2.0f, rHand->teleportPike.SCALE * 2.0f);
			buff = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(rHand->teleportPike.pos, scale, mapData, INTERSECTED_BOTTOM);
			if (buff != INTERSECTED_NONE) { rHand->teleportPike.isHitWall = true; }
			buff = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(rHand->teleportPike.pos, scale, mapData, INTERSECTED_TOP);
			if (buff != INTERSECTED_NONE) { rHand->teleportPike.isHitWall = true; }
			buff = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(rHand->teleportPike.pos, scale, mapData, INTERSECTED_LEFT);
			if (buff != INTERSECTED_NONE) { rHand->teleportPike.isHitWall = true; }
			buff = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(rHand->teleportPike.pos, scale, mapData, INTERSECTED_RIGHT);
			if (buff != INTERSECTED_NONE) { rHand->teleportPike.isHitWall = true; }

		}

		// ���Ԓ�~�̒Z���̓����蔻����s���B
		if (!lHand->timeStopPike.isHitWall && lHand->timeStopPike.isActive) {
			bool onGround = false;
			INTERSECTED_LINE buff = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheVel(lHand->timeStopPike.pos, lHand->timeStopPike.prevFramePos, lHand->timeStopPike.forwardVec, Vec2<float>(lHand->timeStopPike.SCALE, lHand->timeStopPike.SCALE), onGround, mapData);
			if (buff != INTERSECTED_NONE) { lHand->timeStopPike.isHitWall = true; }

			// �����߂�
			Vec2<float> scale = Vec2<float>(lHand->timeStopPike.SCALE * 2.0f, lHand->timeStopPike.SCALE * 2.0f);
			buff = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(lHand->timeStopPike.pos, scale, mapData, INTERSECTED_BOTTOM);
			if (buff != INTERSECTED_NONE) { lHand->timeStopPike.isHitWall = true; }
			buff = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(lHand->timeStopPike.pos, scale, mapData, INTERSECTED_TOP);
			if (buff != INTERSECTED_NONE) { lHand->timeStopPike.isHitWall = true; }
			buff = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(lHand->timeStopPike.pos, scale, mapData, INTERSECTED_LEFT);
			if (buff != INTERSECTED_NONE) { lHand->timeStopPike.isHitWall = true; }
			buff = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(lHand->timeStopPike.pos, scale, mapData, INTERSECTED_RIGHT);
			if (buff != INTERSECTED_NONE) { lHand->timeStopPike.isHitWall = true; }
		}

		// ���Ԓ�~�̒Z���ƃe�X�g�p�u���b�N�̓����蔻����s���B
		if (!lHand->timeStopPike.isHitWall && lHand->timeStopPike.isActive) {
			// ���Ԓ�~�̒Z���̍��W
			Vec2<float> pikePos = lHand->timeStopPike.pos;
			// �u���b�N�̓����蔻�� ����͉~�œK���ł��B
			if (testBlock.pos.Distance(lHand->timeStopPike.pos) <= testBlock.SCALE * 3.0f) {

				// �Z�����~�߂�B
				lHand->timeStopPike.isHitWall = true;
				// �u���b�N���~�߂�B
				testBlock.isTimeStop = true;

			}
			else {
				// �Z�����������玞�Ԓ�~���������邽�߁B
				testBlock.isTimeStop = false;
			}
		}
		else {
			// �Z�����������玞�Ԓ�~���J�ꂷ�邽�߁B
			testBlock.isTimeStop = false;
		}

	}

	/*===== �r�̓����蔻�� =====*/

	lHand->CheckHit(mapData);
	rHand->CheckHit(mapData);

	/*===== �v���C���[�ƃV���{���ʂ̓����蔻�� =====*/

	{
		const int BUBBLE_COUNT = bubble.size();
		for (int index = 0; index < BUBBLE_COUNT; ++index) {

			if (bubble[index].isBreak) continue;

			// hitcheck
			bool isHitBubbleX = centerPos.Distance(bubble[index].pos) <= PLAYER_SIZE.x + bubble[index].RADIUS;
			bool isHitBubbleY = centerPos.Distance(bubble[index].pos) <= PLAYER_SIZE.y + bubble[index].RADIUS;

			// isHit
			if (!isHitBubbleX && !isHitBubbleY) continue;

			// Recovery of recoil
			rHand->isFirstShot = false;
			lHand->isFirstShot = false;

			// If there is an input
			if (inBubble && asSoonAsInputTimer >= 1) {

				// break a bubble
				bubble[index].isBreak = true;
				bubble[index].breakCoolTime = bubble[index].BREAK_COOL_TIME;
				inBubble = false;

			}
			else {

				vel = {};
				gravity = {};
				gimmickVel = {};

				// Get closer to the bubble
				centerPos.x += (bubble[index].pos.x - centerPos.x) / 5.0f;
				centerPos.y += (bubble[index].pos.y - centerPos.y) / 5.0f;

				inBubble = true;

			}

		}

	}

}

void Player::HitMapChipTop()
{
	// Y�����̈ړ��ʂ����炷�B
	vel.y /= 2.0f;

	// �ǃY���t���O��ݒ�
	isSlippingWall[PLAYER_TOP] = true;

	// �ŏ��̈ꔭ�͔����������t���O������������B
	//rHand->Hit();
	//lHand->Hit();

}

void Player::HitMapChipLeft()
{

	// Y���̈ړ��ʂ̍��v�����ȏゾ�����疀�C�����B
	if (fabs(vel.y + gravity) >= STOP_DEADLINE_Y || fabs(gravity) >= MAX_GRAVITY) {

		// ���C������B
		vel.y *= 0.95f;

		gravity *= 0.8f;

		// �ǃY���t���O��ݒ�
		isSlippingWall[PLAYER_LEFT] = true;

	}
	else {

		// X�����̈ړ��ʂ𖳌�������B
		vel.x = 0;
		vel.y = 0;
		gravity = 0;

	}

	// �ŏ��̈ꔭ�͔����������t���O������������B
	firstShot = false;

	// ���͂��󂯕t���Ȃ��t���O��ݒ�
	isWallLeft = true;

	// �ŏ��̈ꔭ�t���O��������
	lHand->isFirstShot = false;
	rHand->isFirstShot = false;

}

void Player::HitMapChipRight()
{

	// Y���̈ړ��ʂ̍��v�����ȏゾ�����疀�C�����B
	if (fabs(vel.y + gravity) >= STOP_DEADLINE_Y || fabs(gravity) >= MAX_GRAVITY) {

		// ���C������B
		vel.y *= 0.95f;

		gravity *= 0.8f;

		// �ǃY���t���O��ݒ�
		isSlippingWall[PLAYER_RIGHT] = true;

	}
	else {

		// X�����̈ړ��ʂ𖳌�������B
		vel.x = 0;
		vel.y = 0;
		gravity = 0;

	}

	// �ŏ��̈ꔭ�͔����������t���O������������B
	firstShot = false;

	gravity *= 0.9f;

	// ���͂��󂯕t���Ȃ��t���O��ݒ�
	isWallRight = true;

	// �ŏ��̈ꔭ�t���O��������
	lHand->isFirstShot = false;
	rHand->isFirstShot = false;

}

void Player::HitMapChipBottom()
{

	// �ڒn�t���O�𗧂Ă�B
	onGround = true;

	// X���̈ړ��ʂ̍��v�����ȏゾ�����疀�C�����B
	if (fabs(vel.x) >= STOP_DEADLINE_X) {

		// ���C������B
		vel.y *= 0.95f;
		vel.x *= 0.95f;

		// �d�͂𖳌�������B
		gravity = 0.5f;

		// �ǃY���t���O��ݒ�
		isSlippingWall[PLAYER_BOTTOM] = true;

	}
	else {

		// X�����̈ړ��ʂ𖳌�������B
		vel.x = 0;
		vel.y = 0;

		// �d�͂𖳌�������B
		gravity = 0.5f;

	}

	gravity = 0;
	vel.y = 0;

	// �ړ��ʂ����ȉ��ɂȂ�����0�ɂ���B
	if (fabs(vel.x) <= 1.0f) vel.x = 0;

	// �ŏ��̈ꔭ�͔����������t���O������������B
	firstShot = false;

	// �ŏ��̈ꔭ�t���O��������
	lHand->isFirstShot = false;
	rHand->isFirstShot = false;

}

void Player::Input(const vector<vector<int>> mapData)
{

	/*===== ���͏��� =====*/

	const float INPUT_DEAD_LINE = 0.3f;

	Vec2<float> inputVec;

	inputVec = UsersInput::Instance()->GetLeftStickVecFuna();
	inputVec /= {32768.0f, 32768.0f};
	// ���͂̃f�b�h���C����݂���B
	if (inputVec.Length() >= 0.9f) {

		// �E��̊p�x���X�V
		lHand->SetAngle(KuroFunc::GetAngle(inputVec));

		// ��莞�ԓ��͂��Ȃ������珉���ʒu�ɖ߂�
		handReturnTimer = DEF_HAND_RETURN_TIMER;

	}

	inputVec = UsersInput::Instance()->GetRightStickVecFuna();
	inputVec /= {32768.0f, 32768.0f};

	// ���͂̃f�b�h���C����݂���B
	if (inputVec.Length() >= 0.9f) {

		// ����̊p�x���X�V
		rHand->SetAngle(KuroFunc::GetAngle(inputVec));

		// ��莞�ԓ��͂��Ȃ������珉���ʒu�ɖ߂�
		handReturnTimer = DEF_HAND_RETURN_TIMER;

	}

	// LB�������ꂽ�甽��������B
	if (UsersInput::Instance()->OnTrigger(XBOX_BUTTON::LB) && rapidFireTimerLeft <= 0) {

		// ����������B
		float rHandAngle = lHand->GetAngle();

		// Get�����l�͎�̌����Ă�������Ȃ̂ŁA-180�x����B
		rHandAngle -= Angle::PI();

		bool isFirstShot = false;

		// �ړ��ʂ����Z
		if (!lHand->isFirstShot) {

			// onGround��true��������ړ��ʂ����Z���Ȃ��B
			if (!onGround || sinf(rHandAngle) < 0.6f) {
				vel.x += cosf(rHandAngle) * FIRST_RECOIL_AMOUNT;
			}

			// �v���C���[�̂ЂƂ�̃u���b�N����������ׂ̏����B
			int mapX = centerPos.x / MAP_CHIP_SIZE;
			int mapY = centerPos.y / MAP_CHIP_SIZE;
			if (mapX <= 0) mapX = 1;
			if (mapX >= mapData[0].size()) mapX = mapData[0].size() - 1;
			if (mapY <= 0) mapY = 1;
			if (mapY >= mapData.size()) mapY = mapData.size() - 1;

			// ���͂��󂯕t���Ȃ��t���O�������Ă��Ȃ�������
			if (!isWallRight && !isWallLeft) {
				vel.y += sinf(rHandAngle) * FIRST_RECOIL_AMOUNT;
			}
			// �v���C���[�̈��̃}�b�v�`�b�v���u���b�N��������ŏ��Ɉꔭ��傫������t���O��傫������B
			else if (mapData[mapY - 1][mapX] > 0 && mapData[mapY - 1][mapX] < 10 && cosf(rHandAngle) < 0.5f && cosf(rHandAngle) > -0.5f) {
				vel.y += sinf(rHandAngle) * FIRST_RECOIL_AMOUNT;
			}
			else if (isWallRight && cosf(rHandAngle) < 0.0f) {
				vel.y += sinf(rHandAngle) * FIRST_RECOIL_AMOUNT;
			}
			else if (isWallLeft && cosf(rHandAngle) > 0.1f) {
				vel.y += sinf(rHandAngle) * FIRST_RECOIL_AMOUNT;
			}

			isFirstShot = true;

			firstShot = true;

			lHand->isFirstShot = true;

			// �V�F�C�N�ʂ�ݒ�
			ShakeMgr::Instance()->maxShakeAmount = ShakeMgr::Instance()->FIRST_SHOT_SHAKE_AMOUNT;

			// �v���C���[�̘r�𓮂����B
			lHand->Shot(Vec2<float>(cosf(rHandAngle), sinf(rHandAngle)), true);

		}
		else {

			// onGround��true��������ړ��ʂ����Z���Ȃ��B
			//if (!onGround || sinf(rHandAngle) < 0.5f) {
			vel.x += cosf(rHandAngle) * RECOIL_AMOUNT;
			//}

			vel.y += sinf(rHandAngle) * RECOIL_AMOUNT;

			// �v���C���[�̘r�𓮂����B
			lHand->Shot(Vec2<float>(cosf(rHandAngle), sinf(rHandAngle)), false);

		}

		// �e�𐶐�����B
		BulletMgr::Instance()->Generate(lHand->GetPos(), lHand->GetAngle(), isFirstShot, false);

		// �A�˃^�C�}�[���Z�b�g
		rapidFireTimerLeft = RAPID_FIRE_TIMER;

		// ��莞�ԓ��͂��Ȃ������珉���ʒu�ɖ߂�
		handReturnTimer = DEF_HAND_RETURN_TIMER;

		// ���̓^�C�}�[���Z�b�g�B
		asSoonAsInputTimer = AS_SOON_AS_INPUT_TIMER;

	}

	// RB�������ꂽ�甽��������B
	if (UsersInput::Instance()->OnTrigger(XBOX_BUTTON::RB) && rapidFireTimerRight <= 0) {

		// ����������B
		float lHandAngle = rHand->GetAngle();

		// Get�����l�͎�̌����Ă�������Ȃ̂ŁA-180�x����B
		lHandAngle -= Angle::PI();

		bool isFirstShot = false;

		// �v���C���[�̂ЂƂ�̃u���b�N����������ׂ̏����B
		int mapX = centerPos.x / MAP_CHIP_SIZE;
		int mapY = centerPos.y / MAP_CHIP_SIZE;
		if (mapX <= 0) mapX = 1;
		if (mapX >= mapData[0].size()) mapX = mapData[0].size() - 1;
		if (mapY <= 0) mapY = 1;
		if (mapY >= mapData.size()) mapY = mapData.size() - 1;

		// �ړ��ʂ����Z
		if (!rHand->isFirstShot) {

			// onGround��true��������ړ��ʂ����Z���Ȃ��B
			if (!onGround || sinf(lHandAngle) < 0.6f) {
				vel.x += cosf(lHandAngle) * FIRST_RECOIL_AMOUNT;
			}

			// ���͂��󂯕t���Ȃ��t���O�������Ă��Ȃ�������
			if (!isWallLeft && !isWallRight) {
				vel.y += sinf(lHandAngle) * FIRST_RECOIL_AMOUNT;
			}
			// �v���C���[�̈��̃}�b�v�`�b�v���u���b�N��������ŏ��Ɉꔭ��傫������t���O��傫������B
			else if (mapData[mapY - 1][mapX] > 0 && mapData[mapY - 1][mapX] < 10 && cosf(lHandAngle) < 0.5f && cosf(lHandAngle) > -0.5f) {
				vel.y += sinf(lHandAngle) * FIRST_RECOIL_AMOUNT;
			}
			else if (isWallRight && cosf(lHandAngle) < 0.3f) {
				vel.y += sinf(lHandAngle) * FIRST_RECOIL_AMOUNT;
			}
			else if (isWallLeft && cosf(lHandAngle) > 0.1f) {
				vel.y += sinf(lHandAngle) * FIRST_RECOIL_AMOUNT;
			}

			// �ŏ��̈ꔭ���������t���O�𗧂Ă�B
			firstShot = true;

			isFirstShot = true;

			rHand->isFirstShot = true;

			// �V�F�C�N�ʂ�ݒ�
			ShakeMgr::Instance()->maxShakeAmount = ShakeMgr::Instance()->FIRST_SHOT_SHAKE_AMOUNT;

			// �v���C���[�̘r�𓮂����B
			rHand->Shot(Vec2<float>(cosf(lHandAngle), sinf(lHandAngle)), true);

		}
		else {
			// onGround��true��������ړ��ʂ����Z���Ȃ��B
			if (!onGround || sinf(lHandAngle) < 0) {
				vel.x += cosf(lHandAngle) * RECOIL_AMOUNT;
			}

			vel.y += sinf(lHandAngle) * RECOIL_AMOUNT;

			// �v���C���[�̘r�𓮂����B
			rHand->Shot(Vec2<float>(cosf(lHandAngle), sinf(lHandAngle)), false);

		}

		// �e�𐶐�����B
		BulletMgr::Instance()->Generate(rHand->GetPos(), rHand->GetAngle(), isFirstShot, true);

		// �A�˃^�C�}�[���Z�b�g
		rapidFireTimerRight = RAPID_FIRE_TIMER;

		// ��莞�ԓ��͂��Ȃ������珉���ʒu�ɖ߂�
		handReturnTimer = DEF_HAND_RETURN_TIMER;

		// ���̓^�C�}�[���Z�b�g�B
		asSoonAsInputTimer = AS_SOON_AS_INPUT_TIMER;

	}

	// �ړ����x�����E�l�𒴂��Ȃ��悤�ɂ���B
	if (vel.x >= MAX_RECOIL_AMOUNT) vel.x = MAX_RECOIL_AMOUNT;
	if (vel.x <= -MAX_RECOIL_AMOUNT) vel.x = -MAX_RECOIL_AMOUNT;
	if (vel.y >= MAX_RECOIL_AMOUNT) vel.y = MAX_RECOIL_AMOUNT;
	if (vel.y <= -MAX_RECOIL_AMOUNT) vel.y = -MAX_RECOIL_AMOUNT;

	// RT�������ꂽ��
	if (UsersInput::Instance()->OnTrigger(XBOX_BUTTON::RT)) {

		// �u�Ԉړ��̒Z���Ɋւ��鏈�����s���B

		// �E�r�̒e�����˂���Ă��Ȃ�������B
		if (!rHand->teleportPike.isActive && !isPrevFrameShotBeacon) {

			// �N�[���^�C����0�ȉ���������
			if (rHand->pikeCooltime <= 0) {

				// �e�𔭎˂��鏈�����s���B
				rHand->teleportPike.Generate(rHand->GetPos(), Vec2<float>(cosf(rHand->GetAngle()), sinf(rHand->GetAngle())), PIKE_TELEPORT);

			}

		}
		// �r�[�R�������˂���Ă�����B
		else if (rHand->teleportPike.isActive && (rHand->teleportPike.isHitWall || rHand->teleportPike.isHitWindow) && !isPrevFrameShotBeacon) {

			// �v���C���[���u�Ԉړ�������B
			centerPos = rHand->teleportPike.pos;

			ScrollMgr::Instance()->WarpScroll(centerPos);

			// �v���C���[�̏d�͂𖳌���������B
			gravity = 0;

			// �ړ��ʂ𖳌�������B
			vel = {};

			// �ړ��ʂ𖳌�������B
			gimmickVel = {};

			// �d�͖������^�C�}�[��ݒ�B
			gravityInvalidTimer = GRAVITY_INVALID_TIMER;

			// �r�[�R��������������B
			rHand->teleportPike.Init();

			// �r�[�R���̃N�[���^�C����ݒ�
			rHand->pikeCooltime = rHand->PIKE_COOL_TIME;

		}

		isPrevFrameShotBeacon = true;

	}
	else if (UsersInput::Instance()->OnTrigger(XBOX_BUTTON::LT)) {

		// ���Ԓ�~�̒Z���Ɋւ��鏈�����s���B

		// ���r�̒e�����˂���Ă��Ȃ�������B
		if (!lHand->timeStopPike.isActive && !isPrevFrameShotBeacon) {

			// �N�[���^�C����0�ȉ���������
			if (lHand->pikeCooltime <= 0) {

				// �e�𔭎˂��鏈�����s���B
				lHand->timeStopPike.Generate(lHand->GetPos(), Vec2<float>(cosf(lHand->GetAngle()), sinf(lHand->GetAngle())), PIKE_TIMESTOP);

			}

		}
		// �r�[�R�������˂���Ă�����B
		else if (lHand->timeStopPike.isActive && (lHand->timeStopPike.isHitWall || lHand->timeStopPike.isHitWindow) && !isPrevFrameShotBeacon) {

			// �r�[�R��������������B
			lHand->timeStopPike.Init();

			// �r�[�R���̃N�[���^�C����ݒ�
			lHand->pikeCooltime = lHand->PIKE_COOL_TIME;

		}

		isPrevFrameShotBeacon = true;

	}
	else {

		isPrevFrameShotBeacon = false;

	}

}

void Player::Move()
{

	/*===== �ړ��Ɋւ��鏈�� =====*/

	// �ړ�����
	centerPos += vel;

	// �M�~�b�N����^������ړ��ʂ����Z
	centerPos += gimmickVel;

	// �X�N���[���ʂ��X�V
	//ScrollMgr::Instance()->honraiScrollAmount -= prevFrameCenterPos - centerPos;
	ScrollMgr::Instance()->CalucurateScroll(prevFrameCenterPos - centerPos);

	// �ړ��ʂ�0�ɋߕt����B
	vel.x -= vel.x / 25.0f;
	vel.y -= vel.y / 25.0f;

	// ���S���W��ۑ�
	prevFrameCenterPos = centerPos;

	// �ړ��ʂ����E�𒴂��Ȃ��悤�ɂ���B
	if (fabs(vel.x) > MAX_RECOIL_AMOUNT) {

		// ������ۑ�����B
		bool sign = std::signbit(vel.x);

		// �����^����B
		vel.x = MAX_RECOIL_AMOUNT;

		// ������������B
		vel.x *= sign ? -1 : 1;

	}
	if (fabs(vel.y) > MAX_RECOIL_AMOUNT) {

		// ������ۑ�����B
		bool sign = std::signbit(vel.y);

		// �����^����B
		vel.y = MAX_RECOIL_AMOUNT;

		// ������������B
		vel.y *= sign ? -1 : 1;

	}
}

void Player::UpdateGravity()
{

	/*===== �d�͂Ɋւ���X�V���� =====*/

	// �d�͂����Z�B
	centerPos.y += gravity;

	// �ڒn�t���O�������Ă����d�͖������^�C�}�[��0�ȉ���������Ȃ�������d�͂����Z�B
	if (!onGround && gravityInvalidTimer <= 0) {

		// �d�͂����Z�B
		gravity += ADD_GRAVITY;

		// �d�͂����E�𒴂��Ȃ��悤�ɂ���B
		if (gravity > MAX_GRAVITY) {

			gravity = MAX_GRAVITY;

		}

	}

	// �d�͖������^�C�}�[���X�V�B
	if (gravityInvalidTimer > 0) {

		--gravityInvalidTimer;

		gravity = 0;

	}

}

void Player::PushBackWall()
{
	const int WIN_WIDTH = WinApp::Instance()->GetWinSize().x;
	const int WIN_HEIGHT = WinApp::Instance()->GetWinSize().y;

	/*===== �ǂƂ̉����߂����� =====*/

	// X���Ɋւ��鉟���߂������B

	// �E��
	const float PLAYER_RIGHT_X = centerPos.x + PLAYER_SIZE.x;
	if (PLAYER_RIGHT_X > WIN_WIDTH) {

		// ���������߂��B
		centerPos.x -= PLAYER_RIGHT_X - WIN_WIDTH;

		// X�����̈ړ��ʂ𖳌�������B
		vel.x = 0;

	}

	// ����
	const float PLAYER_LEFT_X = centerPos.x - PLAYER_SIZE.x;
	if (PLAYER_LEFT_X < 0) {

		// ���������߂��B
		centerPos.x -= PLAYER_LEFT_X;

		// X�����̈ړ��ʂ𖳌�������B
		vel.x = 0;

	}


	// Y���Ɋւ��鉟���߂�����

	// �㑤
	const float PLAYER_UP_Y = centerPos.y - PLAYER_SIZE.y;
	if (PLAYER_UP_Y < 0) {

		// ���������߂��B
		centerPos.y -= PLAYER_UP_Y;

		// Y�����̈ړ��ʂ����炷�B
		vel.y /= 2.0f;

	}

	// ����
	const float PLAYER_DOWN_Y = centerPos.y + PLAYER_SIZE.y;
	if (PLAYER_DOWN_Y >= WIN_HEIGHT) {

		// ���������߂��B
		centerPos.y -= PLAYER_DOWN_Y - WIN_HEIGHT;

		// �ڒn�t���O�𗧂Ă�B
		onGround = true;

		// �d�͂𖳌�������B
		gravity = 0;

		// Y�����̈ړ��ʂ𖳌�������B
		vel.y = 0;

		// X�����̈ړ��ʂ𖳌�������B
		vel.x += (-vel.x) / 10.0f;

	}
	// �ڒn���Ă��Ȃ�������
	else {

		// �ڒn�t���O��܂�B
		//onGround = false;

	}

}
