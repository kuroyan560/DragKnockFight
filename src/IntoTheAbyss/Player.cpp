#include "Player.h"
#include "PlayerHand.h"
#include "BulletMgr.h"
#include "ScrollMgr.h"
#include "ShakeMgr.h"
#include "MapChipCollider.h"
#include "MovingBlockMgr.h"
#include "MovingBlock.h"
#include "SwingMgr.h"
//#include "BulletParticleMgr.h"
#include "StunEffect.h"
#include "Collider.h"
#include <cmath>

#include"TexHandleMgr.h"
#include"UsersInput.h"
#include"DrawFunc.h"
#include"DrawFunc_Shadow.h"
#include"DrawFunc_FillTex.h"
#include"WinApp.h"
#include "ParticleMgr.h"
#include "EventCollider.h"
#include"DebugParameter.h"
#include"SuperiorityGauge.h"
#include"AudioApp.h"
#include "SlowMgr.h"

Vec2<float> Player::GetGeneratePos()
{
	return WinApp::Instance()->GetExpandWinCenter() * Vec2<float>(1.0f, 0.25f);
}

Player::Player()
{
	/*====== �R���X�g���N�^ =====*/

	lHand = make_unique<PlayerHand>(TexHandleMgr::LoadGraph("resource/IntoTheAbyss/aim_time.png"));
	rHand = make_unique<PlayerHand>(TexHandleMgr::LoadGraph("resource/IntoTheAbyss/aim_tele.png"));

	// �摜�����[�h
	//playerGraph = TexHandleMgr::LoadGraph("resource/IntoTheAbyss/Player.png");

	shotSE = AudioApp::Instance()->LoadAudio("resource/ChainCombat/sound/shot.wav");
	AudioApp::Instance()->ChangeVolume(shotSE, 0.2f);

	this->Init(GetGeneratePos());

}

Player::~Player()
{
}

void Player::Init(const Vec2<float> &INIT_POS)
{

	/*===== ���������� =====*/

	//�v���C���[�̌���������
	//playerDir = DEFAULT;

	//�A�j���[�V����������
	anim.Init();

	// �v���C���[�̈ʒu���Z�b�g
	centerPos = INIT_POS;
	//prevFrameCenterPos = INIT_POS;

	// �ړ��ʂ����Z�b�g
	vel = {};

	// �d�͂����Z�b�g
	gravity = 0;

	// �ڒn���Ă��Ȃ��t���O�����Z�b�g
	onGround = false;
	firstShot = false;

	// �r���Z�b�g
	static const float OFFSET = -8.0f;

	lHand->Init(-GetPlayerGraphSize().x + OFFSET);
	rHand->Init(GetPlayerGraphSize().x - OFFSET);

	// �A�˃^�C�}�[��������
	rapidFireTimerLeft = 0;
	rapidFireTimerRight = 0;

	// �d�͖������^�C�}�[���������B
	gravityInvalidTimer = 0;

	isPrevFrameShotBeacon = false;
	firstShot = false;

	// �ǃt���O��������
	isWallRight = false;
	isWallLeft = false;

	isDouji = false;

	isDead = false;

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

	//�X�g���b�`������
	stretch_LU = { 0.0f,0.0f };
	stretch_RB = { 0.0f,0.0f };
	stretchTimer = STRETCH_RETURN_TIME;

	//�e���|�[�g���̓_�ł̏�����
	flashTimer = TELE_FLASH_TIME;
	flashTotalTime = TELE_FLASH_TIME;

	firstRecoilParticleTimer = 0;
	drawCursorFlag = true;

	//�����V���b�g����^�C�}�[���Z�b�g
	isLeftFirstShotTimer = DOUJI_ALLOWANCE_FRAME;
	isRightFirstShotTimer = DOUJI_ALLOWANCE_FRAME;

	isZeroGravity = false;
	changeGravityTimer = 0;

	stuckWindowTimer = 0;

	//muffler.Init(INIT_POS);

	areaHitBox.center = &centerPos;
	areaHitBox.size = PLAYER_HIT_SIZE;

	swingCoolTime = 0;


	bulletHitBox = std::make_shared<SphereCollision>();
	bulletHitBox->center = &centerPos;
	bulletHitBox->radius = 10.0f;

	size = { 3.0f,3.0f };
}

void Player::Update(const vector<vector<int>> mapData, const Vec2<float> &bossPos)
{

	if (1.0f <= size.x && 1.0f <= size.y)
	{
		size.x -= 0.1f;
		size.y -= 0.1f;
		doorMoveUpDownFlag = true;
	}


	//�f�o�b�N�p�̒l�ύX
	std::shared_ptr<PlayerDebugParameterData> data = DebugParameter::Instance()->nowData;

	ADD_GRAVITY = data->ADD_GRAVITY;
	MAX_GRAVITY = data->MAX_GRAVITY;
	RECOIL_AMOUNT = data->RECOIL_AMOUNT;
	FIRST_RECOIL_AMOUNT = data->FIRST_RECOIL_AMOUNT;
	MAX_RECOIL_AMOUNT = data->MAX_RECOIL_AMOUNT;
	RAPID_FIRE_TIMER = data->RAPID_FIRE_TIMER;


	/*===== ���͏��� =====*/

	if (!doorMoveLeftRightFlag && !doorMoveUpDownFlag && !isDead)
	{
		// ���͂Ɋւ���X�V�������s���B
		Input(mapData, bossPos);
	}

	/*===== �X�V���� =====*/
	if (!doorMoveLeftRightFlag && !doorMoveUpDownFlag)
	{
		//�ړ��Ɋւ��鏈��
		Move();
	}

	//ScrollMgr::Instance()->CalucurateScroll(prevFrameCenterPos - centerPos);
	prevFrameCenterPos = centerPos;

	if (!doorMoveLeftRightFlag)
	{
		// �d�͂Ɋւ���X�V����
		UpdateGravity();
	}
	doorMoveLeftRightFlag = false;
	doorMoveUpDownFlag = false;
	doorMoveDownFlag = false;




	// �A�˃^�C�}�[���X�V
	if (rapidFireTimerLeft > 0) --rapidFireTimerLeft;
	if (rapidFireTimerRight > 0) --rapidFireTimerRight;

	// �r���X�V
	lHand->Update(centerPos + anim.GetHandCenterOffset());
	rHand->Update(centerPos + anim.GetHandCenterOffset());

	// �e���X�V
	BulletMgr::Instance()->Update();

	// ���͂��󂯕t���Ȃ��t���O��������
	isWallRight = false;
	isWallLeft = false;

	// �v���C���[�̂������̃}�b�v�`�b�v���W����������B
	int mapX = centerPos.x / MAP_CHIP_SIZE;
	int mapY = (centerPos.y - PLAYER_HIT_SIZE.y - 1.0f) / MAP_CHIP_SIZE;
	if (mapX <= 0) mapX = 1;
	if (mapX >= mapData[0].size()) mapX = mapData[0].size() - 1;
	if (mapY <= 0) mapY = 1;
	if (mapY >= mapData.size()) mapY = mapData.size() - 1;
	if (mapY <= 0) mapY = 1;
	if (mapX <= 0) mapX = 1;
	if (mapX >= mapData[mapY].size()) mapX = mapData[mapY].size() - 1;

	// ���̃}�b�v�`�b�v���u���b�N�ŁAX�������̈ړ��ʂ����ȏゾ������p�[�e�B�N���𐶐�����B
	if (mapData[mapY][mapX] > 0 && mapData[mapY][mapX] < 10 && fabs(vel.x) >= 10.0f)ParticleMgr::Instance()->Generate(Vec2<float>(centerPos.x, centerPos.y - GetPlayerGraphSize().y), Vec2<float>(0, -1), HIT_MAP);

	// �ǃY���̃p�[�e�B�N���𐶐��B
	if (isSlippingWall[PLAYER_TOP]) ParticleMgr::Instance()->Generate(Vec2<float>(centerPos.x, centerPos.y - GetPlayerGraphSize().y), Vec2<float>(0, -1), HIT_MAP);
	if (isSlippingWall[PLAYER_RIGHT]) ParticleMgr::Instance()->Generate(Vec2<float>(centerPos.x + GetPlayerGraphSize().x, centerPos.y), Vec2<float>(1, 0), HIT_MAP);
	if (isSlippingWall[PLAYER_BOTTOM]) ParticleMgr::Instance()->Generate(Vec2<float>(centerPos.x, centerPos.y + GetPlayerGraphSize().y), Vec2<float>(0, 1), HIT_MAP);
	if (isSlippingWall[PLAYER_LEFT]) ParticleMgr::Instance()->Generate(Vec2<float>(centerPos.x - GetPlayerGraphSize().x, centerPos.y), Vec2<float>(-1, 0), HIT_MAP);


	// �ǃY���t���O���������B
	for (int index = 0; index < 4; ++index) isSlippingWall[index] = false;

	// ��莞�ԓ��͂��Ȃ�������r�������ʒu�ɖ߂��^�C�}�[���X�V�B
	if (handReturnTimer > 0) {
		--handReturnTimer;
	}
	// ��������̈ړ��ʂ����݂�����NoInput������������B
	if (vel.x != 0 || vel.y != 0 || gimmickVel.x != 0 || gimmickVel.y != 0 || gravity != 0) handReturnTimer = DEF_HAND_RETURN_TIMER;
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
	//gimmickVel = {};

	// ���͂���Ă��琔�t���[�����擾����^�C�}�[���X�V�B
	if (asSoonAsInputTimer > 0) --asSoonAsInputTimer;

	//�X�g���b�`�X�V
	UpdateStretch();

	//�A�j���[�V�����X�V
	anim.Update();


	// �ŏ��̈ꔭ�̃^�C�}�[���N�����Ă�����p�[�e�B�N���𐶐�����B
	if (0 < firstRecoilParticleTimer) {

		// �ړ����Ă�����������߂�B
		Vec2<float> invForwardVec = -vel;

		// �ō����x����̃p�[�Z���e�[�W�����߂�B
		float per = (float)firstRecoilParticleTimer / (float)FIRST_SHOT_RECOIL_PARTICLE_TIMER;

		// ���K������B
		invForwardVec.Normalize();

		if (!isDouji)
		{
			ParticleMgr::Instance()->Generate(Vec2<float>(centerPos.x - GetPlayerGraphSize().x / 2.0f, centerPos.y), invForwardVec, FIRST_DASH);
			ParticleMgr::Instance()->Generate(Vec2<float>(centerPos.x, centerPos.y - GetPlayerGraphSize().y / 2.0f), invForwardVec, FIRST_DASH);
			ParticleMgr::Instance()->Generate(Vec2<float>(centerPos.x + GetPlayerGraphSize().x / 2.0f, centerPos.y), invForwardVec, FIRST_DASH);
			ParticleMgr::Instance()->Generate(Vec2<float>(centerPos.x, centerPos.y + GetPlayerGraphSize().y / 2.0f), invForwardVec, FIRST_DASH);
		}
		else
		{
			ParticleMgr::Instance()->Generate(Vec2<float>(centerPos.x - GetPlayerGraphSize().x / 2.0f, centerPos.y), invForwardVec, FIRST_DASH_DOUJI);
			ParticleMgr::Instance()->Generate(Vec2<float>(centerPos.x, centerPos.y - GetPlayerGraphSize().y / 2.0f), invForwardVec, FIRST_DASH_DOUJI);
			ParticleMgr::Instance()->Generate(Vec2<float>(centerPos.x + GetPlayerGraphSize().x / 2.0f, centerPos.y), invForwardVec, FIRST_DASH_DOUJI);
			ParticleMgr::Instance()->Generate(Vec2<float>(centerPos.x, centerPos.y + GetPlayerGraphSize().y / 2.0f), invForwardVec, FIRST_DASH_DOUJI);
		}

		--firstRecoilParticleTimer;
	}


	++changeGravityTimer;
	if (CHANGE_GRAVITY_TIMER <= changeGravityTimer) {

		//isZeroGravity = isZeroGravity ? false : true;

		//changeGravityTimer = 0;

	}

	if (isZeroGravity) gravity = 0;

	//�e���|�[�g���̃t���b�V���̃^�C�}�[�v��
	if (flashTimer < flashTotalTime)flashTimer++;


	// �E�B���h�E�ɋ��܂ꂽ�^�C�}�[���X�V
	if (0 < stuckWindowTimer) --stuckWindowTimer;

	// �U��񂵂̃N�[���^�C�����X�V
	if (0 < swingCoolTime) --swingCoolTime;

	//muffler.Update(centerPos);
}

void Player::Draw(LightManager &LigManager)
{
	//if (vel.y < 0)playerDir = BACK;
	if (vel.y < 0)anim.ChangeAnim(DEFAULT_BACK);
	//if (0 < vel.y)playerDir = FRONT;
	if (0 < vel.y)anim.ChangeAnim(DEFAULT_FRONT);

	/*===== �`�揈�� =====*/

	// �v���C���[�̕`�揈��
	//Vec2<float>leftUp = { centerPos.x * ScrollMgr::Instance()->zoom - GetPlayerGraphSize().x * ScrollMgr::Instance()->zoom - scrollShakeZoom.x,
	//	centerPos.y * ScrollMgr::Instance()->zoom - GetPlayerGraphSize().y * ScrollMgr::Instance()->zoom - scrollShakeZoom.y };
	//Vec2<float>rightBottom = { centerPos.x * ScrollMgr::Instance()->zoom + GetPlayerGraphSize().x * ScrollMgr::Instance()->zoom - scrollShakeZoom.x,
	//	centerPos.y * ScrollMgr::Instance()->zoom + GetPlayerGraphSize().y * ScrollMgr::Instance()->zoom - scrollShakeZoom.y };
	//�c���`��
	afImg.Draw();

	//muffler.Draw(LigManager);

	static const int ARM_GRAPH_L = TexHandleMgr::LoadGraph("resource/ChainCombat/arm_L.png");
	static const int ARM_GRAPH_R = TexHandleMgr::LoadGraph("resource/ChainCombat/arm_R.png");
	rHand->Draw(LigManager, EXT_RATE * size.x, ARM_GRAPH_R, DEF_RIGHT_HAND_ANGLE, { 0.0f,0.0f }, drawCursorFlag);
	lHand->Draw(LigManager, EXT_RATE * size.y, ARM_GRAPH_L, DEF_LEFT_HAND_ANGLE, { 1.0f,0.0f }, drawCursorFlag);

	//�X�g���b�`���Z
	//leftUp += stretch_LU;
	//rightBottom += stretch_RB;
	const Vec2<float>drawPos = ScrollMgr::Instance()->Affect(centerPos);
	//����
	auto bodyTex = TexHandleMgr::GetTexBuffer(anim.GetGraphHandle());
	const Vec2<float> expRateBody = ((GetPlayerGraphSize() - stretch_LU + stretch_RB) / GetPlayerGraphSize());
	DrawFunc::DrawRotaGraph2D(drawPos, expRateBody * ScrollMgr::Instance()->zoom * EXT_RATE * size, 0.0f, bodyTex);

	//�e���|�[�g���̃t���b�V��
	static auto white = D3D12App::Instance()->GenerateTextureBuffer(Color());
	const float flashAlpha = KuroMath::Ease(Out, Quint, flashTimer, flashTotalTime, 1.0f, 0.0f);
	DrawFunc_FillTex::DrawRotaGraph2D(drawPos, expRateBody * ScrollMgr::Instance()->zoom, 0.0f, bodyTex, white, flashAlpha);

	// �e��`��
	BulletMgr::Instance()->Draw();

}

void Player::CheckHit(const vector<vector<int>> mapData, vector<Bubble> &bubble, vector<DossunBlock> &dossun, const Vec2<float> &bossPos, bool &isHitMapChip, const Vec2<float> &lineCenterPos)
{

	/*===== �}�b�v�`�b�v�ƃv���C���[�Ƃ̓����蔻��S�� =====*/

	{

		// �E�B���h�E�ɋ��܂��Ă��Ȃ�������
		if (stuckWindowTimer <= 0) {

			// �}�b�v�`�b�v�ƃv���C���[�̓����蔻�� ��΂Ɋђʂ����Ȃ��ׂ̏���
			CheckHitMapChipVel(centerPos, mapData, bossPos, isHitMapChip);

			// �}�b�v�`�b�v�ƃv���C���[�̓����蔻�� �X�P�[��
			CheckHitSize(centerPos, mapData, bossPos, isHitMapChip);

			// ���E�ɓ��������ۂɕǒނ肳���邽�߂̏����B
			//int yChip = (centerPos.y + MAP_CHIP_HALF_SIZE) / MAP_CHIP_SIZE;
			//int xChip = (centerPos.x - PLAYER_HIT_SIZE.x * 1.2f + MAP_CHIP_HALF_SIZE) / MAP_CHIP_SIZE;
			//if (yChip <= 0) yChip = 1;
			//if (yChip >= mapData.size()) yChip = mapData.size() - 1;
			//if (xChip <= 0) xChip = 1;
			//if (xChip >= mapData[yChip].size()) xChip = mapData[yChip].size() - 1;
			//// �v���C���[�̍������}�b�v�`�b�v��������
			//if (mapData[yChip][xChip] == 1 && mapData[yChip - 1][xChip] != 0) {
			//	HitMapChipLeft();
			//}
			//xChip = (centerPos.x + PLAYER_HIT_SIZE.x + MAP_CHIP_HALF_SIZE) / MAP_CHIP_SIZE;
			//if (xChip < 0) xChip = 0;
			//if (xChip >= mapData[yChip].size()) xChip = mapData[yChip].size() - 1;
			//// �v���C���[�̉E�����}�b�v�`�b�v��������
			//if (mapData[yChip][xChip] == 1 && mapData[yChip - 1][xChip] != 0) {
			//	HitMapChipRight();
			//}

			// �����L�΂�����̕⏕�̂��߂̏���
			float bossAngle = atan2(bossPos.y - centerPos.y, bossPos.x - centerPos.x);
			bossAngle = fabs(bossAngle);
			int yChip = (centerPos.y + MAP_CHIP_HALF_SIZE) / MAP_CHIP_SIZE;
			int xChip = (centerPos.x - PLAYER_HIT_SIZE.x * 1.2f + MAP_CHIP_HALF_SIZE) / MAP_CHIP_SIZE;
			if (yChip <= 0) yChip = 1;
			if (yChip >= mapData.size()) yChip = mapData.size() - 1;
			if (xChip <= 0) xChip = 1;
			if (xChip >= mapData[yChip].size()) xChip = mapData[yChip].size() - 1;
			// �v���C���[�̍������}�b�v�`�b�v��������
			if (mapData[yChip][xChip] == 1 && mapData[yChip - 1][xChip] != 0) {
				// �G���w��͈̔͂̒��ɂ�����
				const float MIN_ANGLE = 2.35619f;
				const float MAX_ANGLE = 3.92699f;
				// �p�x�����̒l�̊Ԃ�������
				if (MIN_ANGLE <= bossAngle && bossAngle <= MAX_ANGLE) {
					// �����������Ă���B
					isHitMapChip = true;
				}
			}
			xChip = (centerPos.x + PLAYER_HIT_SIZE.x + MAP_CHIP_HALF_SIZE) / MAP_CHIP_SIZE;
			if (xChip < 0) xChip = 0;
			if (xChip >= mapData[yChip].size()) xChip = mapData[yChip].size() - 1;
			// �v���C���[�̉E�����}�b�v�`�b�v��������
			if (mapData[yChip][xChip] == 1 && mapData[yChip - 1][xChip] != 0) {
				// �G���w��͈̔͂̒��ɂ�����
				const float MIN_ANGLE = 0.785398f;
				const float MAX_ANGLE = 5.49779f;
				// �p�x�����̒l�̊Ԃ�������
				if (MAX_ANGLE <= bossAngle || bossAngle <= MIN_ANGLE) {
					// �����������Ă���B
					isHitMapChip = true;
				}
			}

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
			Vec2<float> playerMovingBlockSize = { MovingBlockMgr::Instance()->movingBlock[movingIndex]->SCALE + PLAYER_HIT_SIZE.x,MovingBlockMgr::Instance()->movingBlock[movingIndex]->SCALE + PLAYER_HIT_SIZE.y };

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

	// ���Ԓ�~�̒Z���ƃh�b�X���u���b�N���������Ă��邩��ۑ�����ϐ�
	bool isHitTeleportPikeDossun = false;
	int hitTeleportPikeIndex = 0;

	{

		//	// �u�Ԉړ��̒Z���̓����蔻����s���B
		//	if (!rHand->teleportPike.isHitWall && rHand->teleportPike.isActive) {
		//		bool onGround = false;
		//		INTERSECTED_LINE buff = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheVel(rHand->teleportPike.pos, rHand->teleportPike.prevFramePos, rHand->teleportPike.forwardVec, Vec2<float>(rHand->teleportPike.SCALE, rHand->teleportPike.SCALE), onGround, mapData);
		//		if (buff != INTERSECTED_NONE) { rHand->teleportPike.isHitWall = true; }

		//		// �����߂�
		//		Vec2<float> scale = Vec2<float>(rHand->teleportPike.SCALE * 1.0f, rHand->teleportPike.SCALE * 1.0f);
		//		buff = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(rHand->teleportPike.pos, scale, mapData, INTERSECTED_BOTTOM);
		//		if (buff != INTERSECTED_NONE) { rHand->teleportPike.isHitWall = true; }
		//		buff = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(rHand->teleportPike.pos, scale, mapData, INTERSECTED_TOP);
		//		if (buff != INTERSECTED_NONE) { rHand->teleportPike.isHitWall = true; }
		//		buff = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(rHand->teleportPike.pos, scale, mapData, INTERSECTED_LEFT);
		//		if (buff != INTERSECTED_NONE) { rHand->teleportPike.isHitWall = true; }
		//		buff = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(rHand->teleportPike.pos, scale, mapData, INTERSECTED_RIGHT);
		//		if (buff != INTERSECTED_NONE) { rHand->teleportPike.isHitWall = true; }

		//	}

		//	// ���Ԓ�~�̒Z���ƃh�b�X���u���b�N�̓����蔻����s���B
		//	const int DOSSUN_COUNT = dossun.size();
		//	for (int index = 0; index < DOSSUN_COUNT; ++index) {

		//		//if (rHand->teleportPike.isHitWall) continue;

		//		// �܂��͓������Ă��邩���`�F�b�N����B
		//		if (fabs(rHand->teleportPike.pos.x - dossun[index].pos.x) > rHand->teleportPike.SCALE + dossun[index].size.x) continue;
		//		if (fabs(rHand->teleportPike.pos.y - dossun[index].pos.y) > rHand->teleportPike.SCALE + dossun[index].size.y) continue;

		//		// �u�Ԉړ��̒Z���ɕǂɂ��������t���O��t����B
		//		rHand->teleportPike.isHitWall = true;

		//		rHand->teleportPike.gimmickVel = dossun[index].moveDir * Vec2<float>(dossun[index].speed, dossun[index].speed);

		//		isHitTeleportPikeDossun = true;
		//		hitTeleportPikeIndex = index;

		//	}

		//	// ���Ԓ�~�̒Z���̓����蔻����s���B
		//	if (!lHand->timeStopPike.isHitWall && lHand->timeStopPike.isActive) {
		//		bool onGround = false;
		//		INTERSECTED_LINE buff = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheVel(lHand->timeStopPike.pos, lHand->timeStopPike.prevFramePos, lHand->timeStopPike.forwardVec, Vec2<float>(lHand->timeStopPike.SCALE, lHand->timeStopPike.SCALE), onGround, mapData);
		//		if (buff != INTERSECTED_NONE) { lHand->timeStopPike.isHitWall = true; }

		//		// �����߂�
		//		Vec2<float> scale = Vec2<float>(lHand->timeStopPike.SCALE * 1.0f, lHand->timeStopPike.SCALE * 1.0f);
		//		buff = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(lHand->timeStopPike.pos, scale, mapData, INTERSECTED_BOTTOM);
		//		if (buff != INTERSECTED_NONE) { lHand->timeStopPike.isHitWall = true; }
		//		buff = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(lHand->timeStopPike.pos, scale, mapData, INTERSECTED_TOP);
		//		if (buff != INTERSECTED_NONE) { lHand->timeStopPike.isHitWall = true; }
		//		buff = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(lHand->timeStopPike.pos, scale, mapData, INTERSECTED_LEFT);
		//		if (buff != INTERSECTED_NONE) { lHand->timeStopPike.isHitWall = true; }
		//		buff = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(lHand->timeStopPike.pos, scale, mapData, INTERSECTED_RIGHT);
		//		if (buff != INTERSECTED_NONE) { lHand->timeStopPike.isHitWall = true; }
		//	}

		//	// ���Ԓ�~�̒Z���ƃh�b�X���u���b�N�̓����蔻����s���B
		//	for (int index = 0; index < DOSSUN_COUNT; ++index) {

		//		if (lHand->timeStopPike.isHitWall) continue;

		//		// �܂��͓������Ă��邩���`�F�b�N����B
		//		if (fabs(lHand->timeStopPike.pos.x - dossun[index].pos.x) > lHand->timeStopPike.SCALE + dossun[index].size.x) continue;
		//		if (fabs(lHand->timeStopPike.pos.y - dossun[index].pos.y) > lHand->timeStopPike.SCALE + dossun[index].size.y) continue;

		//		lHand->timeStopPike.isHitWall = true;
		//		lHand->timeStopPike.stopPos = dossun[index].speed;
		//		lHand->timeStopPike.stopTargetPos = &dossun[index].speed;

		//		// ���̎��_�Ńh�b�X���u���b�N��1F���ړ���������Ă���̂ŁA�����߂��B
		//		dossun[index].pos -= dossun[index].moveDir * Vec2<float>(dossun[index].speed, dossun[index].speed);

		//		// �Ղꂢ��[���������ǂ��B
		//		centerPos -= gimmickVel;

		//		dossun[index].isTimeStopPikeAlive = &(lHand->timeStopPike.isHitWall);

		//	}

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
			bool isHitBubbleX = centerPos.Distance(bubble[index].pos) <= PLAYER_HIT_SIZE.x + bubble[index].RADIUS;
			bool isHitBubbleY = centerPos.Distance(bubble[index].pos) <= PLAYER_HIT_SIZE.y + bubble[index].RADIUS;

			// isHit
			if (!isHitBubbleX && !isHitBubbleY) continue;

			// If there is an input
			if (inBubble && asSoonAsInputTimer >= 1) {

				// break a bubble
				bubble[index].isBreak = true;
				bubble[index].breakCoolTime = bubble[index].BREAK_COOL_TIME;
				inBubble = false;

			}
			else {

				// �������ŏ���1F�ڂ�������
				if (fabs(vel.x) > 0 || fabs(vel.y) > 0 || fabs(gravity) > ADD_GRAVITY) bubble[index].addEasingTimer = 30.0f;

				vel = {};
				gravity = {};
				gimmickVel = {};

				// Get closer to the bubble
				centerPos.x += (bubble[index].pos.x - centerPos.x) / 5.0f;
				centerPos.y += (bubble[index].pos.y - centerPos.y) / 5.0f;

				inBubble = true;

				// Recovery of recoil
				rHand->isFirstShot = false;
				lHand->isFirstShot = false;
				isDouji = false;
			}

		}

	}


	/*===== �v���C���[�ƃh�b�X���u���b�N�̓����蔻�� =====*/

	bool isHitDossun = false;

	const int DOSSUN_COUNT = dossun.size();
	for (int index = 0; index < DOSSUN_COUNT; ++index) {

		bool isDossunVel = Collider::Instance()->CheckHitVel(centerPos, prevFrameCenterPos, vel + gimmickVel, PLAYER_HIT_SIZE, dossun[index].pos, dossun[index].size) != INTERSECTED_NONE;
		bool isDossunTop = Collider::Instance()->CheckHitSize(centerPos, PLAYER_HIT_SIZE, dossun[index].pos, dossun[index].size, INTERSECTED_TOP) != INTERSECTED_NONE;
		bool isDossunRight = Collider::Instance()->CheckHitSize(centerPos, PLAYER_HIT_SIZE, dossun[index].pos, dossun[index].size, INTERSECTED_RIGHT) != INTERSECTED_NONE;
		bool isDossunLeft = Collider::Instance()->CheckHitSize(centerPos, PLAYER_HIT_SIZE, dossun[index].pos, dossun[index].size, INTERSECTED_LEFT) != INTERSECTED_NONE;
		bool isDossunBottom = Collider::Instance()->CheckHitSize(centerPos, PLAYER_HIT_SIZE, dossun[index].pos, dossun[index].size, INTERSECTED_BOTTOM) != INTERSECTED_NONE;

		// �ǂ�������ɂԂ����Ă���Γ�����������ɂ���B
		if (!isDossunTop && (isDossunVel || isDossunRight || isDossunLeft || isDossunBottom)) {

			// �v���C���[�Ƀh�b�X���u���b�N�̈ړ��ʂ�n���B
			gimmickVel = Vec2<float>(dossun[index].speed, dossun[index].speed) * dossun[index].moveDir;

			isHitDossun = true;

			// �h�b�X���̈ړ��ʃ^�C�}�[���X�V�B
			dossun[index].isHitPlayer = true;

			// �v���C���[�̈ړ��ʂ����������B
			gravity *= 0.0f;
			vel *= {0.0f, 0.0f};

			// �ŏ��̈ꔭ�t���O��傫������B
			lHand->isFirstShot = false;
			rHand->isFirstShot = false;
			isDouji = false;
		}
		else if (isHitTeleportPikeDossun && hitTeleportPikeIndex == index) {

			// �h�b�X���̈ړ��ʃ^�C�}�[���X�V�B
			dossun[index].isHitPlayer = true;

		}
		else {

			// �h�b�X���̈ړ��ʃ^�C�}�[���������B
			dossun[index].isHitPlayer = false;
			//isMoveTimer = 0;

		}

		if (isDossunTop) vel.y /= 2.0f;

		if (!isHitDossun) gimmickVel = {};

	}


	/*===== �v���C���[���h�b�X���ɋ��܂ꂽ���� =====*/

	// �v���C���[�̍��E���h�b�X���������̓}�b�v�`�b�v�Ɠ������Ă����玀�S������s���B
	{

		float offset = 1.0f;

		bool isHitLeft = false;
		bool isHitRight = false;

		// �}�b�v�`�b�v�Ƃ̓����蔻��B
		{

			// �v���C���[�̍����̃}�b�v�`�b�v�ԍ������߂�B
			int playerChipX = (centerPos.x - PLAYER_HIT_SIZE.x - offset + MAP_CHIP_HALF_SIZE) / MAP_CHIP_SIZE;
			int playerChipY = (centerPos.y + MAP_CHIP_HALF_SIZE) / MAP_CHIP_SIZE;


			// ���߂��ԍ����͈͊O����Ȃ�������B
			if (0 < (centerPos.y - PLAYER_HIT_SIZE.y) && 0 <= playerChipY && playerChipY < mapData.size() && 0 <= playerChipX && playerChipX < mapData[playerChipY].size()) {

				// ���̃}�b�v�`�b�v�̔ԍ���1~9�̊Ԃ�������B
				if (0 < mapData[playerChipY][playerChipX] && mapData[playerChipY][playerChipX] < 10) {

					// �������Ă���
					isHitLeft = true;

				}

			}

		}

		// �h�b�X���Ƃ̓����蔻��
		{

			Vec2<float> checkPos = { centerPos.x - PLAYER_HIT_SIZE.x - offset, centerPos.y };

			// �S�Ẵh�b�X���u���b�N�Ɣ�����s���B
			const int DOSSUN_COUNT = dossun.size();
			for (int index = 0; index < DOSSUN_COUNT; ++index) {

				// ���߂���W���h�b�X���u���b�N�͈͓̔��ɂ��邩�ǂ������`�F�b�N����B
				if (checkPos.x < dossun[index].pos.x - dossun[index].size.x || dossun[index].pos.x + dossun[index].size.x < checkPos.x) continue;
				if (checkPos.y < dossun[index].pos.y - dossun[index].size.y || dossun[index].pos.y + dossun[index].size.y < checkPos.y) continue;

				// �����܂ł���Γ������Ă��锻��B
				isHitLeft = true;

			}

		}

		// �}�b�v�`�b�v�Ƃ̓����蔻��B
		{
			// �v���C���[�̉E���̃}�b�v�`�b�v�ԍ������߂�B
			int playerChipX = (centerPos.x + PLAYER_HIT_SIZE.x + offset + MAP_CHIP_HALF_SIZE) / MAP_CHIP_SIZE;
			int playerChipY = (centerPos.y + MAP_CHIP_HALF_SIZE) / MAP_CHIP_SIZE;


			// ���߂��ԍ����͈͊O����Ȃ�������B
			if (0 <= playerChipY && playerChipY < mapData.size() && 0 <= playerChipX && playerChipX < mapData[playerChipY].size()) {

				// ���̃}�b�v�`�b�v�̔ԍ���1~9�̊Ԃ�������B
				if (0 < mapData[playerChipY][playerChipX] && mapData[playerChipY][playerChipX] < 10) {

					// �������Ă���
					isHitRight = true;

				}

			}

		}

		// �h�b�X���Ƃ̓����蔻��
		{

			Vec2<float> checkPos = { centerPos.x + PLAYER_HIT_SIZE.x + offset, centerPos.y };

			// �S�Ẵh�b�X���u���b�N�Ɣ�����s���B
			const int DOSSUN_COUNT = dossun.size();
			for (int index = 0; index < DOSSUN_COUNT; ++index) {

				// ���߂���W���h�b�X���u���b�N�͈͓̔��ɂ��邩�ǂ������`�F�b�N����B
				if (checkPos.x < dossun[index].pos.x - dossun[index].size.x || dossun[index].pos.x + dossun[index].size.x < checkPos.x) continue;
				if (checkPos.y < dossun[index].pos.y - dossun[index].size.y || dossun[index].pos.y + dossun[index].size.y < checkPos.y) continue;

				// �����܂ł���Γ������Ă��锻��B
				isHitRight = true;

			}

		}

		// ���܂�Ă����玀�S
		if (isHitRight && isHitLeft) {

			isDead = true;

		}

	}

	// �v���C���[�̏㉺���h�b�X���������̓}�b�v�`�b�v�Ɠ������Ă����玀�S������s���B
	{

		float offset = 1.0f;

		bool isHitTop = false;
		bool isHitBottom = false;

		// �}�b�v�`�b�v�Ƃ̓����蔻��B
		{

			// �v���C���[�̏㑤�̃}�b�v�`�b�v�ԍ������߂�B
			int playerChipX = (centerPos.x + MAP_CHIP_HALF_SIZE) / MAP_CHIP_SIZE;
			int playerChipY = (centerPos.y - PLAYER_HIT_SIZE.y - offset + MAP_CHIP_HALF_SIZE) / MAP_CHIP_SIZE;



			// ���߂��ԍ����͈͊O����Ȃ�������B
			if (0 <= playerChipY && playerChipY < mapData.size() && 0 <= playerChipX && playerChipX < mapData[playerChipY].size()) {

				// ���̃}�b�v�`�b�v�̔ԍ���1~9�̊Ԃ�������B
				if (0 < mapData[playerChipY][playerChipX] && mapData[playerChipY][playerChipX] < 10) {

					// �������Ă���
					isHitTop = true;

				}

			}

		}

		// �h�b�X���Ƃ̓����蔻��
		{

			Vec2<float> checkPos = { centerPos.x, centerPos.y - PLAYER_HIT_SIZE.y - offset };

			// �S�Ẵh�b�X���u���b�N�Ɣ�����s���B
			const int DOSSUN_COUNT = dossun.size();
			for (int index = 0; index < DOSSUN_COUNT; ++index) {

				// ���߂���W���h�b�X���u���b�N�͈͓̔��ɂ��邩�ǂ������`�F�b�N����B
				if (checkPos.x < dossun[index].pos.x - dossun[index].size.x || dossun[index].pos.x + dossun[index].size.x < checkPos.x) continue;
				if (checkPos.y < dossun[index].pos.y - dossun[index].size.y || dossun[index].pos.y + dossun[index].size.y < checkPos.y) continue;

				// �����܂ł���Γ������Ă��锻��B
				isHitTop = true;

			}

		}

		// �}�b�v�`�b�v�Ƃ̓����蔻��B
		{

			// �v���C���[�̉����̃}�b�v�`�b�v�ԍ������߂�B
			int playerChipX = (centerPos.x + MAP_CHIP_HALF_SIZE) / MAP_CHIP_SIZE;
			int playerChipY = (centerPos.y + PLAYER_HIT_SIZE.y + offset + MAP_CHIP_HALF_SIZE) / MAP_CHIP_SIZE;


			// ���߂��ԍ����͈͊O����Ȃ�������B
			if (0 <= playerChipY && playerChipY < mapData.size() && 0 <= playerChipX && playerChipX < mapData[playerChipY].size()) {

				// ���̃}�b�v�`�b�v�̔ԍ���1~9�̊Ԃ�������B
				if (0 < mapData[playerChipY][playerChipX] && mapData[playerChipY][playerChipX] < 10) {

					// �������Ă���
					isHitBottom = true;

				}

			}

		}

		// �h�b�X���Ƃ̓����蔻��
		{

			Vec2<float> checkPos = { centerPos.x, centerPos.y + PLAYER_HIT_SIZE.y + offset };

			// �S�Ẵh�b�X���u���b�N�Ɣ�����s���B
			const int DOSSUN_COUNT = dossun.size();
			for (int index = 0; index < DOSSUN_COUNT; ++index) {

				// ���߂���W���h�b�X���u���b�N�͈͓̔��ɂ��邩�ǂ������`�F�b�N����B
				if (checkPos.x < dossun[index].pos.x - dossun[index].size.x || dossun[index].pos.x + dossun[index].size.x < checkPos.x) continue;
				if (checkPos.y < dossun[index].pos.y - dossun[index].size.y || dossun[index].pos.y + dossun[index].size.y < checkPos.y) continue;

				// �����܂ł���Γ������Ă��锻��B
				isHitBottom = true;

			}

		}

		// ���܂�Ă����玀�S
		if (isHitTop && isHitBottom) {

			isDead = true;

		}

	}

	if (isDead) {

		int a = 0;

	}

	isDead = false;


	// �}�b�v�`�b�v�ɂ������Ă����Ԃŉ�ʊO�ɏo����
	if (isHitMapChip) {

		Vec2<int> windowSize = WinApp::Instance()->GetWinCenter();
		windowSize *= Vec2<int>(2, 2);

		// �{�X�ƃv���C���[�̋���
		float distanceX = fabs(lineCenterPos.x - centerPos.x);
		float disntaceY = fabs(lineCenterPos.y - centerPos.y);

		// �E�B���h�E���E
		if (windowSize.x <= centerPos.x + PLAYER_HIT_SIZE.x - ScrollMgr::Instance()->scrollAmount.x || centerPos.x - PLAYER_HIT_SIZE.x - ScrollMgr::Instance()->scrollAmount.x <= 0) {

			stuckWindowTimer = STRUCK_WINDOW_TIMER;
			ShakeMgr::Instance()->SetShake(20);
			SuperiorityGauge::Instance()->AddEnemyGauge(10);

		}
		// �E�B���h�E�㉺
		if (windowSize.y <= centerPos.y + PLAYER_HIT_SIZE.y - ScrollMgr::Instance()->scrollAmount.y || centerPos.y - PLAYER_HIT_SIZE.y - ScrollMgr::Instance()->scrollAmount.y <= 0) {

			stuckWindowTimer = STRUCK_WINDOW_TIMER;
			ShakeMgr::Instance()->SetShake(20);
			SuperiorityGauge::Instance()->AddEnemyGauge(10);

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
	stretch_LU.x = 0.0f;

	// Y���̈ړ��ʂ̍��v�����ȏゾ�����疀�C�����B
	if (fabs(vel.y + gravity) >= STOP_DEADLINE_Y || fabs(gravity) >= MAX_GRAVITY) {

		// ���C������B
		//vel.y *= 0.95f;
		vel.y *= VEL_MUL_AMOUNT;

		gravity *= 0.8f;

		// �ǃY���t���O��ݒ�
		isSlippingWall[PLAYER_LEFT] = true;

		//���C�p�A�j���[�V����
		//anim.ChangeAnim(ON_WALL_DASH);
	}
	else {

		// X�����̈ړ��ʂ𖳌�������B
		vel.x = 0;
		vel.y = 0;
		gravity = 0;

		firstRecoilParticleTimer = 0;

		//���C�����Ƃ��̓X�g���b�`���キ����
		stretch_RB.y /= STRETCH_DIV_RATE;
		stretch_LU.y /= STRETCH_DIV_RATE;

		//�Ǔ\��t���A�j���[�V����
		//anim.ChangeAnim(ON_WALL_WAIT);
	}

	// �ŏ��̈ꔭ�͔����������t���O������������B
	firstShot = false;

	// ���͂��󂯕t���Ȃ��t���O��ݒ�
	isWallLeft = true;

	// �ŏ��̈ꔭ�t���O��������
	lHand->isFirstShot = false;
	rHand->isFirstShot = false;
	isDouji = false;
}

void Player::HitMapChipRight()
{
	stretch_RB.x = 0.0f;

	// Y���̈ړ��ʂ̍��v�����ȏゾ�����疀�C�����B
	if (fabs(vel.y + gravity) >= STOP_DEADLINE_Y || fabs(gravity) >= MAX_GRAVITY) {

		// ���C������B
		vel.y *= VEL_MUL_AMOUNT;

		gravity *= 0.8f;

		// �ǃY���t���O��ݒ�
		isSlippingWall[PLAYER_RIGHT] = true;

		//���C�p�A�j���[�V����
		//anim.ChangeAnim(ON_WALL_DASH);
	}
	else {

		// X�����̈ړ��ʂ𖳌�������B
		vel.x = 0;
		vel.y = 0;
		gravity = 0;

		firstRecoilParticleTimer = 0;

		//���C�����Ƃ��̓X�g���b�`���キ����
		stretch_RB.y /= STRETCH_DIV_RATE;
		stretch_LU.y /= STRETCH_DIV_RATE;

		//�Ǔ\��t���A�j���[�V����
		//anim.ChangeAnim(ON_WALL_WAIT);
	}

	// �ŏ��̈ꔭ�͔����������t���O������������B
	firstShot = false;

	gravity *= 0.9f;

	// ���͂��󂯕t���Ȃ��t���O��ݒ�
	isWallRight = true;

	// �ŏ��̈ꔭ�t���O��������
	lHand->isFirstShot = false;
	rHand->isFirstShot = false;
	isDouji = false;
}

void Player::HitMapChipBottom()
{
	stretch_RB.y = 0.0f;

	// �ڒn�t���O�𗧂Ă�B
	//onGround = true;

	// X���̈ړ��ʂ̍��v�����ȏゾ�����疀�C�����B
	if (fabs(vel.x) >= STOP_DEADLINE_X) {

		// ���C������B
		vel.y *= VEL_MUL_AMOUNT;
		vel.x *= VEL_MUL_AMOUNT;

		// �d�͂𖳌�������B
		gravity = 0.5f;

		// �ǃY���t���O��ݒ�
		isSlippingWall[PLAYER_BOTTOM] = true;

		//���C�p�A�j���[�V����
		//anim.ChangeAnim(ON_GROUND_DASH);
	}
	else {

		// X�����̈ړ��ʂ𖳌�������B
		vel.x = 0;
		vel.y = 0;

		// �d�͂𖳌�������B
		gravity = 0.5f;

		firstRecoilParticleTimer = 0;

		//���C�����Ƃ��̓X�g���b�`���キ����
		stretch_RB.x /= STRETCH_DIV_RATE;
		stretch_LU.x /= STRETCH_DIV_RATE;

		//�ҋ@�A�j���[�V�����ɖ߂�
		//anim.ChangeAnim(ON_GROUND_WAIT);
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
	isDouji = false;
}

void Player::StopDoorLeftRight()
{
	doorMoveLeftRightFlag = true;
}

void Player::StopDoorUpDown()
{
	doorMoveUpDownFlag = true;
}

void Player::Input(const vector<vector<int>> mapData, const Vec2<float> &bossPos)
{
	/*===== ���͏��� =====*/

	//if (UsersInput::Instance()->OnTrigger(XBOX_BUTTON::LT)) {

	//	isZeroGravity = isZeroGravity ? false : true;
	//	changeGravityTimer = 0;

	//}

	// �X�^�����o������������͂��󂯕t���Ȃ��B
	if (StunEffect::Instance()->isActive) return;

	//�����V���b�g����^�C�}�[�v��
	if (isLeftFirstShotTimer < DOUJI_ALLOWANCE_FRAME)isLeftFirstShotTimer++;
	if (isRightFirstShotTimer < DOUJI_ALLOWANCE_FRAME)isRightFirstShotTimer++;

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
		//handReturnTimer = DEF_HAND_RETURN_TIMER;

	}

	// �U��񂵒��y�ѐU��񂳂ꒆ���𔻒f
	bool isSwingNow = SwingMgr::Instance()->isSwingBoss || SwingMgr::Instance()->isSwingPlayer;

	// LB�������ꂽ�甽��������B
	if (!isSwingNow && UsersInput::Instance()->Input(XBOX_BUTTON::LB) && rapidFireTimerLeft <= 0) {

		// ����������B
		float rHandAngle = lHand->GetAngle();

		// Get�����l�͎�̌����Ă�������Ȃ̂ŁA-180�x����B
		rHandAngle -= Angle::PI();

		bool isFirstShot = false;

		// �ړ��ʂ����Z
		if (!lHand->isFirstShot) {

			// �����������ۑ��B
			isLeftFirstShotTimer = 0;

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
			ShakeMgr::Instance()->SetShake(ShakeMgr::Instance()->FIRST_SHOT_SHAKE_AMOUNT);

			// �v���C���[�̘r�𓮂����B
			lHand->Shot(Vec2<float>(cosf(rHandAngle), sinf(rHandAngle)), true);

			firstRecoilParticleTimer = FIRST_SHOT_RECOIL_PARTICLE_TIMER;

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
		const float ARM_DISTANCE = 20.0f;
		const float OFFSET_Y = -14.0f;
		const float OFFSET_X = 12.0f;

		float angle = lHand->GetAngle();

		AudioApp::Instance()->PlayWave(shotSE);
		BulletMgr::Instance()->Generate(lHand->handPos + Vec2<float>(cosf(angle) * ARM_DISTANCE + OFFSET_X, sinf(angle) * ARM_DISTANCE + OFFSET_Y), angle, isFirstShot, false);

		// �A�˃^�C�}�[���Z�b�g
		rapidFireTimerLeft = RAPID_FIRE_TIMER;

		// ��莞�ԓ��͂��Ȃ������珉���ʒu�ɖ߂�
		handReturnTimer = DEF_HAND_RETURN_TIMER;

		// ���̓^�C�}�[���Z�b�g�B
		asSoonAsInputTimer = AS_SOON_AS_INPUT_TIMER;

		//�X�g���b�`
		CalculateStretch(vel);
	}

	// RB�������ꂽ�甽��������B
	if (!isSwingNow && UsersInput::Instance()->Input(XBOX_BUTTON::RB) && rapidFireTimerRight <= 0) {

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

			// �E���Ɍ����������ۑ�����B
			isRightFirstShotTimer = 0;

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
			ShakeMgr::Instance()->SetShake(ShakeMgr::Instance()->FIRST_SHOT_SHAKE_AMOUNT);

			// �v���C���[�̘r�𓮂����B
			rHand->Shot(Vec2<float>(cosf(lHandAngle), sinf(lHandAngle)), true);

			firstRecoilParticleTimer = FIRST_SHOT_RECOIL_PARTICLE_TIMER;

		}
		else {
			// onGround��true��������ړ��ʂ����Z���Ȃ��B
				//if (!onGround || sinf(lHandAngle) < 0) {
			vel.x += cosf(lHandAngle) * RECOIL_AMOUNT;
			//			//}

			{
				//=======
				//		if (!onGround || sinf(lHandAngle) < 0) {
				//			vel.x += cosf(lHandAngle) * RECOIL_AMOUNT;
				//		}
				//>>>>>>> スクロールの中忁E��線�Eの中忁E��するためのチE��トブランチE
			}

			vel.y += sinf(lHandAngle) * RECOIL_AMOUNT;

			// �v���C���[�̘r�𓮂����B
			rHand->Shot(Vec2<float>(cosf(lHandAngle), sinf(lHandAngle)), false);

		}

		// �e�𐶐�����B
		const float ARM_DISTANCE = 20.0f;
		const float OFFSET_Y = -14.0f;
		const float OFFSET_X = -12.0f;

		float angle = rHand->GetAngle();

		AudioApp::Instance()->PlayWave(shotSE);
		BulletMgr::Instance()->Generate(rHand->handPos + Vec2<float>(cosf(angle) * ARM_DISTANCE + OFFSET_X, sinf(angle) * ARM_DISTANCE + OFFSET_Y), angle, isFirstShot, true);

		// �A�˃^�C�}�[���Z�b�g
		rapidFireTimerRight = RAPID_FIRE_TIMER;

		// ��莞�ԓ��͂��Ȃ������珉���ʒu�ɖ߂�
		handReturnTimer = DEF_HAND_RETURN_TIMER;

		// ���̓^�C�}�[���Z�b�g�B
		asSoonAsInputTimer = AS_SOON_AS_INPUT_TIMER;

		//�X�g���b�`
		CalculateStretch(vel);
	}

	// �����Ɍ������t���O���擾����B
	if (!isDouji && isLeftFirstShotTimer < DOUJI_ALLOWANCE_FRAME && isRightFirstShotTimer < DOUJI_ALLOWANCE_FRAME) {

		isDouji = true;

		if (isLeftFirstShotTimer == 0 || isRightFirstShotTimer == 0)
		{
			//�����V���b�g�G�t�F�N�g
			flashTimer = 0;
			flashTotalTime = DOUJI_FLASH_TIME;
			UsersInput::Instance()->ShakeController(1.0f, 10);
		}
	}

	// �ړ����x�����E�l�𒴂��Ȃ��悤�ɂ���B
	if (vel.x >= MAX_RECOIL_AMOUNT) vel.x = MAX_RECOIL_AMOUNT;
	if (vel.x <= -MAX_RECOIL_AMOUNT) vel.x = -MAX_RECOIL_AMOUNT;
	if (vel.y >= MAX_RECOIL_AMOUNT) vel.y = MAX_RECOIL_AMOUNT;
	if (vel.y <= -MAX_RECOIL_AMOUNT) vel.y = -MAX_RECOIL_AMOUNT;

	// RT�������ꂽ��
	if (swingCoolTime <= 0 && UsersInput::Instance()->OnTrigger(XBOX_BUTTON::RT)) {

		// �U��񂵂̏���

		// �U��񂵂̃g���K�[����
		if (!SwingMgr::Instance()->isSwingPlayer) {

			// �U��񂵂̊J�n�x�N�g�����擾�B
			SwingMgr::Instance()->easingStartVec = bossPos - centerPos;
			SwingMgr::Instance()->easingStartVec.Normalize();
			SwingMgr::Instance()->easingNowVec = SwingMgr::Instance()->easingStartVec;

			// �U��񂵂̏I���x�N�g�����擾�B
			SwingMgr::Instance()->easingEndVec = (bossPos - centerPos) * Vec2<float>(1.0f, -1.0f);
			SwingMgr::Instance()->easingEndVec.Normalize();

			// �U��񂵃t���O��L��������B
			SwingMgr::Instance()->isSwingPlayer = true;

			// �e�^�C�}�[���������B
			SwingMgr::Instance()->easingTimer = 0;
			SwingMgr::Instance()->easeAmount = 0;
			SwingMgr::Instance()->easeChangeAmountY = SwingMgr::Instance()->easingEndVec.y - SwingMgr::Instance()->easingStartVec.y;

			// �N�[���^�C����ݒ�B
			swingCoolTime = SWING_COOLTIME;

		}



		// �Z���̎c�[
		{

			//// �u�Ԉړ��̒Z���Ɋւ��鏈�����s���B

			//// �E�r�̒e�����˂���Ă��Ȃ�������B
			//if (!rHand->teleportPike.isActive && !isPrevFrameShotBeacon) {

			//	// �N�[���^�C����0�ȉ���������
			//	if (rHand->pikeCooltime <= 0) {

			//		const float ARM_DISTANCE = 20.0f;
			//		const float OFFSET_Y = -14.0f;
			//		const float OFFSET_X = -12.0f;

			//		float angle = rHand->GetAngle();

			//		// �e�𔭎˂��鏈�����s���B
			//		rHand->teleportPike.Generate(rHand->GetPos() + Vec2<float>(cosf(angle) * ARM_DISTANCE + OFFSET_X, sinf(angle) * ARM_DISTANCE + OFFSET_Y), Vec2<float>(cosf(angle), sinf(angle)), PIKE_TELEPORT);

			//	}

			//}
			//// �r�[�R�������˂���Ă�����B
			//else if (rHand->teleportPike.isActive && !isPrevFrameShotBeacon) {

			//	auto vec = rHand->teleportPike.pos - centerPos;

			//	//�����ς�
			//	if (vec.x < 0)playerDir = LEFT;
			//	if (0 < vec.x)playerDir = RIGHT;

			//	//�摜�T�C�Y�̈Ⴂ�ɂ���ĕǂɂ������Ȃ��Ƃ�������̂ŏ����߂荞�ނ悤�ɂ���
			//	static const float OFFSET = 3.0f;
			//	rHand->teleportPike.pos.x += (playerDir == RIGHT) ? OFFSET : -OFFSET;
			//	vec = rHand->teleportPike.pos - centerPos;	//�ړ��ʍX�V

			//	//�X�g���b�`
			//	CalculateStretch(vec);

			//	//�c���G�~�b�g
			//	afImg.EmitArray(centerPos, rHand->teleportPike.pos, anim.GetGraphHandle(), GetPlayerGraphSize(), { playerDir != DEFAULT,false });
			//	Vec2<int>graphSize = TexHandleMgr::GetTexBuffer(GetHandGraph(true))->GetGraphSize();
			//	graphSize *= EXT_RATE / 2.0f;
			//	lHand->EmitAfterImg(vec, GetHandGraph(true), graphSize.Float(), { false,false });
			//	graphSize = TexHandleMgr::GetTexBuffer(GetHandGraph(false))->GetGraphSize();
			//	graphSize *= EXT_RATE / 2.0f;
			//	rHand->EmitAfterImg(vec, GetHandGraph(false), graphSize.Float(), { false,false });

			//	//�_��
			//	flashTimer = 0;
			//	flashTotalTime = TELE_FLASH_TIME;

			//	// �v���C���[���u�Ԉړ�������B
			//	centerPos = rHand->teleportPike.pos;

			//	ScrollMgr::Instance()->WarpScroll(centerPos);

			//	// �v���C���[�̏d�͂𖳌���������B
			//	gravity = 0;

			//	// �ړ��ʂ𖳌�������B
			//	vel = {};

			//	// �ړ��ʂ𖳌�������B
			//	gimmickVel = {};

			//	// �d�͖������^�C�}�[��ݒ�B
			//	gravityInvalidTimer = GRAVITY_INVALID_TIMER;

			//	// �r�[�R��������������B
			//	rHand->teleportPike.Init();

			//	// �r�[�R���̃N�[���^�C����ݒ�
			//	rHand->pikeCooltime = rHand->PIKE_COOL_TIME;
			//}

			//isPrevFrameShotBeacon = true;
		}

	}
	if (!UsersInput::Instance()->Input(XBOX_BUTTON::RT)) {

		// �U��񂵃t���O��܂�B
		SwingMgr::Instance()->isSwingPlayer = false;

	}


	if (UsersInput::Instance()->OnTrigger(XBOX_BUTTON::LT)) {






		// �Z���̎c�[
		{

			//// ���Ԓ�~�̒Z���Ɋւ��鏈�����s���B

			//// ���r�̒e�����˂���Ă��Ȃ�������B
			//if (!lHand->timeStopPike.isActive && !isPrevFrameShotBeacon) {

			//	// �N�[���^�C����0�ȉ���������
			//	if (lHand->pikeCooltime <= 0) {

			//		const float ARM_DISTANCE = 20.0f;
			//		const float OFFSET_Y = -14.0f;
			//		const float OFFSET_X = 12.0f;

			//		float angle = lHand->GetAngle();

			//		// �e�𔭎˂��鏈�����s���B
			//		lHand->timeStopPike.Generate(lHand->GetPos() + Vec2<float>(cosf(angle) * ARM_DISTANCE + OFFSET_X, sinf(angle) * ARM_DISTANCE + OFFSET_Y), Vec2<float>(cosf(angle), sinf(angle)), PIKE_TIMESTOP);

			//	}

			//}
			//// �r�[�R�������˂���Ă�����B
			//else if (lHand->timeStopPike.isActive && !isPrevFrameShotBeacon) {

			//	// �~�߂��Ă������̂𓮂����B
			//	lHand->timeStopPike.MoveAgain();

			//	// �r�[�R��������������B
			//	lHand->timeStopPike.Init();

			//	// �r�[�R���̃N�[���^�C����ݒ�
			//	lHand->pikeCooltime = lHand->PIKE_COOL_TIME;

			//}

			//isPrevFrameShotBeacon = true;
		}

	}
	else {

		isPrevFrameShotBeacon = false;

	}
}

void Player::Move()
{

	/*===== �ړ��Ɋւ��鏈�� =====*/

	// �ړ�����
	//centerPos += vel;

	// �M�~�b�N����^������ړ��ʂ����Z
	centerPos += gimmickVel;

	// �X�N���[���ʂ��X�V
	//ScrollMgr::Instance()->honraiScrollAmount -= prevFrameCenterPos - centerPos;


	// �ړ��ʂ�0�ɋߕt����B
	//vel.x -= vel.x / 25.0f;
	//vel.y -= vel.y / 25.0f;
	vel = KuroMath::Lerp(vel, { 0.0f,0.0f }, 0.05f);

	// ���S���W��ۑ�
	//prevFrameCenterPos = centerPos;

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
	//centerPos.y += gravity;

	// �ڒn�t���O�������Ă����d�͖������^�C�}�[��0�ȉ���������Ȃ�������d�͂����Z�B
	if (!onGround && gravityInvalidTimer <= 0) {

		// �d�͂����Z�B
		//gravity += ADD_GRAVITY;

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
	const float PLAYER_RIGHT_X = centerPos.x + PLAYER_HIT_SIZE.x;
	if (PLAYER_RIGHT_X > WIN_WIDTH) {

		// ���������߂��B
		centerPos.x -= PLAYER_RIGHT_X - WIN_WIDTH;

		// X�����̈ړ��ʂ𖳌�������B
		vel.x = 0;

	}

	// ����
	const float PLAYER_LEFT_X = centerPos.x - PLAYER_HIT_SIZE.x;
	if (PLAYER_LEFT_X < 0) {

		// ���������߂��B
		centerPos.x -= PLAYER_LEFT_X;

		// X�����̈ړ��ʂ𖳌�������B
		vel.x = 0;

	}


	// Y���Ɋւ��鉟���߂�����

	// �㑤
	const float PLAYER_UP_Y = centerPos.y - PLAYER_HIT_SIZE.y;
	if (PLAYER_UP_Y < 0) {

		// ���������߂��B
		centerPos.y -= PLAYER_UP_Y;

		// Y�����̈ړ��ʂ����炷�B
		vel.y /= 2.0f;

	}

	// ����
	const float PLAYER_DOWN_Y = centerPos.y + PLAYER_HIT_SIZE.y;
	if (PLAYER_DOWN_Y >= WIN_HEIGHT) {

		// ���������߂��B
		centerPos.y -= PLAYER_DOWN_Y - WIN_HEIGHT;

		// �ڒn�t���O�𗧂Ă�B
		//onGround = true;

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

void Player::CalculateStretch(const Vec2<float> &Move)
{
	Vec2<float> stretchRate = { abs(Move.x / MAX_RECOIL_AMOUNT),abs(Move.y / MAX_RECOIL_AMOUNT) };

	//X�ړ��̐����̕�������
	if (stretchRate.y < stretchRate.x)
	{
		//anim.ChangeAnim(ON_AIR_DASH_X);
	}
	//Y�ړ��̐����̕�������
	else if (stretchRate.x < stretchRate.y)
	{
		//anim.ChangeAnim(ON_AIR_DASH_Y);
	}

	if (1.0f < stretchRate.x)stretchRate.x = 1.0f;
	if (1.0f < stretchRate.y)stretchRate.y = 1.0f;

	//��
	if (Move.x < 0.0f)
	{
		//�E�ɐL�т�
		stretch_RB.x += MAX_STRETCH.x * stretchRate.x;
		if (MAX_STRETCH.x < stretch_RB.x)stretch_RB.x = MAX_STRETCH.x;
	}
	//�E
	else if (0.0f < Move.x)
	{
		//���ɐL�т�
		stretch_LU.x -= MAX_STRETCH.x * stretchRate.x;
		if (stretch_LU.x < -MAX_STRETCH.x)stretch_LU.x = -MAX_STRETCH.x;
	}

	static const float SHRINK_RATE = 1.0f / 2.0f;
	static const float OTHER_STRETCH_BOOL_RATE = 0.8f;

	//���E�ړ���
	if (Move.x != 0.0f && stretchRate.y < OTHER_STRETCH_BOOL_RATE)
	{
		//�㉺���k��
		stretch_LU.y += MAX_STRETCH.y * stretchRate.x * SHRINK_RATE;
		if (MAX_STRETCH.y < stretch_LU.y)stretch_LU.y = MAX_STRETCH.y;
		stretch_RB.y -= MAX_STRETCH.y * stretchRate.x * SHRINK_RATE;
		if (stretch_RB.y < -MAX_STRETCH.y)stretch_RB.y = -MAX_STRETCH.y;
	}

	//��
	if (Move.y < 0.0f)
	{
		//���ɐL�т�
		stretch_RB.y += MAX_STRETCH.y * stretchRate.y;
		if (MAX_STRETCH.y < stretch_RB.y)stretch_RB.y = MAX_STRETCH.y;
	}
	//��
	else if (0.0f < Move.y)
	{
		//��ɐL�т�
		stretch_LU.y -= MAX_STRETCH.y * stretchRate.y;
		if (stretch_LU.y < -MAX_STRETCH.y)stretch_LU.y = -MAX_STRETCH.y;
	}

	//�㉺�ړ���
	if (Move.y != 0.0f && stretchRate.x < OTHER_STRETCH_BOOL_RATE)
	{
		//���E���k��
		stretch_LU.x += MAX_STRETCH.x * stretchRate.y * SHRINK_RATE;
		if (MAX_STRETCH.x < stretch_LU.x)stretch_LU.x = MAX_STRETCH.x;
		stretch_RB.x -= MAX_STRETCH.x * stretchRate.y * SHRINK_RATE;
		if (stretch_RB.x < -MAX_STRETCH.x)stretch_RB.x = -MAX_STRETCH.x;
	}

	stretchTimer = 0;
	fromStretch_LU = stretch_LU;
	fromStretch_RB = stretch_RB;
}

#include"KuroMath.h"
void Player::UpdateStretch()
{
	static const EASING_TYPE STRETCH_EASE_TYPE = Cubic;
	stretch_LU = KuroMath::Ease(Out, STRETCH_EASE_TYPE, stretchTimer, STRETCH_RETURN_TIME, fromStretch_LU, { 0.0f,0.0f });
	stretch_RB = KuroMath::Ease(Out, STRETCH_EASE_TYPE, stretchTimer, STRETCH_RETURN_TIME, fromStretch_RB, { 0.0f,0.0f });

	if (stretchTimer < STRETCH_RETURN_TIME)
	{
		stretchTimer++;
	}
}

Vec2<float> Player::GetPlayerGraphSize()
{
	//return { (56 * EXT_RATE) / 2.0f,(144 * EXT_RATE) / 2.0f };			// �v���C���[�̃T�C�Y
	return { (anim.GetGraphSize().x * EXT_RATE) / 2.0f,(anim.GetGraphSize().y * EXT_RATE) / 2.0f };			// �v���C���[�̃T�C�Y
}

void Player::CheckHitMapChipVel(const Vec2<float> &checkPos, const vector<vector<int>> &mapData, const Vec2<float> &bossPos, bool &isHitMapChip)
{
	// �}�b�v�`�b�v�ƃv���C���[�̓����蔻�� ��΂Ɋђʂ����Ȃ��ׂ̏���
	//Vec2<float> upperPlayerPos = centerPos - Vec2<float>(0, PLAYER_HIT_SIZE.y / 2.0f);
	//Vec2<float> upperPlayerPosBuff = upperPlayerPos;
	INTERSECTED_LINE intersectedLine = {};

	// �ʏ펞�̓����蔻��
	Vec2<float> moveDir = prevFrameCenterPos - centerPos;
	moveDir.Normalize();
	//intersectedLine = (INTERSECTED_LINE)MapChipCollider::Instance()->CheckHitMapChipBasedOnTheVel(centerPos, prevFrameCenterPos, vel + gimmickVel, Vec2<float>(PLAYER_HIT_SIZE.x, PLAYER_HIT_SIZE.y), onGround, mapData);
	intersectedLine = (INTERSECTED_LINE)MapChipCollider::Instance()->CheckHitMapChipBasedOnTheVel(centerPos, prevFrameCenterPos, moveDir * Vec2<float>(PLAYER_HIT_SIZE.x, PLAYER_HIT_SIZE.y), Vec2<float>(PLAYER_HIT_SIZE.x, PLAYER_HIT_SIZE.y / 2.0f), onGround, mapData);


	// ����(�����߂��ꂽ�l�����W�������)
	//centerPos += upperPlayerPos - upperPlayerPosBuff;

	// �{�X�ƃv���C���[�̊p�x
	float bossAngle = atan2(bossPos.y - centerPos.y, bossPos.x - centerPos.x);
	bossAngle = fabs(bossAngle);

	// ���������ʒu�ɉ����ď����𕪂���B
	if (intersectedLine == INTERSECTED_TOP) {

		// ���ɓ��������ꍇ
		HitMapChipBottom();

		// �}�b�v�`�b�v�̏�ɂ������Ă����Ƃ������Ƃ́A�{�X���������ɂ����OK�I
		// �������̋�̓I�Ȓl��
		const float MIN_ANGLE = 0.785398f;
		const float MAX_ANGLE = 2.35619f;

		// �p�x�����̒l�̊Ԃ�������
		if (MIN_ANGLE <= bossAngle && bossAngle <= MAX_ANGLE) {

			// �����������Ă���B
			isHitMapChip = true;

		}

	}
	else if (intersectedLine == INTERSECTED_RIGHT) {

		// ���ɓ��������ꍇ
		HitMapChipLeft();

		// �}�b�v�`�b�v�̉E�ɂ������Ă����Ƃ������Ƃ́A�{�X���������ɂ����OK�I
		// �������̋�̓I�Ȓl��
		const float MIN_ANGLE = 2.35619f;
		const float MAX_ANGLE = 3.92699f;

		// �p�x�����̒l�̊Ԃ�������
		if (MIN_ANGLE <= bossAngle && bossAngle <= MAX_ANGLE) {

			// �����������Ă���B
			isHitMapChip = true;

		}

	}
	else if (intersectedLine == INTERSECTED_BOTTOM) {

		// ��ɓ��������ꍇ
		HitMapChipTop();

		// �}�b�v�`�b�v�̉��ɂ������Ă����Ƃ������Ƃ́A�v���C���[��������ɂ����OK�I
		// ������̋�̓I�Ȓl��
		const float MIN_ANGLE = 3.92699f;
		const float MAX_ANGLE = 5.49779f;

		// �p�x�����̒l�̊Ԃ�������
		if (MIN_ANGLE <= bossAngle && bossAngle <= MAX_ANGLE) {

			// �����������Ă���B
			isHitMapChip = true;

		}

	}
	else if (intersectedLine == INTERSECTED_LEFT) {

		// �E�ɓ��������ꍇ
		HitMapChipRight();

		// �}�b�v�`�b�v�̍��ɂ������Ă����Ƃ������Ƃ́A�v���C���[���E�����ɂ����OK�I
		// �E�����̋�̓I�Ȓl��
		const float MIN_ANGLE = 0.785398f;
		const float MAX_ANGLE = 5.49779f;

		// �p�x�����̒l�̊Ԃ�������
		if (MAX_ANGLE <= bossAngle || bossAngle <= MIN_ANGLE) {

			// �����������Ă���B
			isHitMapChip = true;

		}

	}
	else if (intersectedLine == INTERSECTED_RISE) {

		// �̂��オ��̏ꏊ�ɓ��������ꍇ
		vel.y *= 0.5f;
		gravity = 0;

	}

	//if (intersectedLine == INTERSECTED_NONE) {

	//	Vec2<float> downnerPlayerPos = centerPos - Vec2<float>(0, PLAYER_HIT_SIZE.y / 2.0f);
	//	Vec2<float> downnerPlayerPosBuff = upperPlayerPos;
	//	intersectedLine = (INTERSECTED_LINE)MapChipCollider::Instance()->CheckHitMapChipBasedOnTheVel(downnerPlayerPos, prevFrameCenterPos, moveDir * Vec2<float>(PLAYER_HIT_SIZE.x, PLAYER_HIT_SIZE.y / 2.0f), Vec2<float>(PLAYER_HIT_SIZE.x, PLAYER_HIT_SIZE.y / 2.0f), onGround, mapData);

	//	// ����(�����߂��ꂽ�l�����W�������)
	//	centerPos += downnerPlayerPos - downnerPlayerPosBuff;

	//	// ���������ʒu�ɉ����ď����𕪂���B
	//	if (intersectedLine == INTERSECTED_TOP) {

	//		// ���ɓ��������ꍇ
	//		HitMapChipBottom();

	//		// �}�b�v�`�b�v�̏�ɂ������Ă����Ƃ������Ƃ́A�{�X���������ɂ����OK�I
	//		// �������̋�̓I�Ȓl��
	//		const float MIN_ANGLE = 0.785398f;
	//		const float MAX_ANGLE = 2.35619f;

	//		// �p�x�����̒l�̊Ԃ�������
	//		if (MIN_ANGLE <= bossAngle && bossAngle <= MAX_ANGLE) {

	//			// �����������Ă���B
	//			isHitMapChip = true;

	//		}

	//	}
	//	else if (intersectedLine == INTERSECTED_RIGHT) {

	//		// ���ɓ��������ꍇ
	//		HitMapChipLeft();

	//		// �}�b�v�`�b�v�̉E�ɂ������Ă����Ƃ������Ƃ́A�{�X���������ɂ����OK�I
	//		// �������̋�̓I�Ȓl��
	//		const float MIN_ANGLE = 2.35619f;
	//		const float MAX_ANGLE = 3.92699f;

	//		// �p�x�����̒l�̊Ԃ�������
	//		if (MIN_ANGLE <= bossAngle && bossAngle <= MAX_ANGLE) {

	//			// �����������Ă���B
	//			isHitMapChip = true;

	//		}

	//	}
	//	else if (intersectedLine == INTERSECTED_BOTTOM) {

	//		// ��ɓ��������ꍇ
	//		HitMapChipTop();

	//		// �}�b�v�`�b�v�̉��ɂ������Ă����Ƃ������Ƃ́A�v���C���[��������ɂ����OK�I
	//		// ������̋�̓I�Ȓl��
	//		const float MIN_ANGLE = 3.92699f;
	//		const float MAX_ANGLE = 5.49779f;

	//		// �p�x�����̒l�̊Ԃ�������
	//		if (MIN_ANGLE <= bossAngle && bossAngle <= MAX_ANGLE) {

	//			// �����������Ă���B
	//			isHitMapChip = true;

	//		}

	//	}
	//	else if (intersectedLine == INTERSECTED_LEFT) {

	//		// �E�ɓ��������ꍇ
	//		HitMapChipRight();

	//		// �}�b�v�`�b�v�̍��ɂ������Ă����Ƃ������Ƃ́A�v���C���[���E�����ɂ����OK�I
	//		// �E�����̋�̓I�Ȓl��
	//		const float MIN_ANGLE = 0.785398f;
	//		const float MAX_ANGLE = 5.49779f;

	//		// �p�x�����̒l�̊Ԃ�������
	//		if (MAX_ANGLE <= bossAngle || bossAngle <= MIN_ANGLE) {

	//			// �����������Ă���B
	//			isHitMapChip = true;

	//		}

	//	}
	//	else if (intersectedLine == INTERSECTED_RISE) {

	//		// �̂��オ��̏ꏊ�ɓ��������ꍇ
	//		vel.y *= 0.5f;
	//		gravity = 0;

	//	}

	//}

}

void Player::CheckHitSize(const Vec2<float> &checkPos, const vector<vector<int>> &mapData, const Vec2<float> &bossPos, bool &isHitMapChip)
{

	// �}�b�v�`�b�v�ڐ��łǂ��ɓ���������
	bool isHitTop = false;
	bool isHitRight = false;
	bool isHitLeft = false;
	bool isHitBottom = false;

	// �}�b�v�`�b�v�ƃv���C���[�̓����蔻�� ��΂ɔ킹�Ȃ����߂̏��� ����
	INTERSECTED_LINE intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(centerPos, PLAYER_HIT_SIZE, mapData, INTERSECTED_TOP, 0 < gimmickVel.Length());
	if (intersectedLine == INTERSECTED_TOP) HitMapChipBottom();
	if (intersectedLine == INTERSECTED_TOP) isHitTop = true;
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(centerPos, PLAYER_HIT_SIZE, mapData, INTERSECTED_BOTTOM, 0 < gimmickVel.Length());
	if (intersectedLine == INTERSECTED_TOP) HitMapChipBottom();
	if (intersectedLine == INTERSECTED_BOTTOM) isHitBottom = true;
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(centerPos, PLAYER_HIT_SIZE, mapData, INTERSECTED_LEFT, 0 < gimmickVel.Length());
	if (intersectedLine == INTERSECTED_TOP) HitMapChipBottom();
	if (intersectedLine == INTERSECTED_LEFT) isHitLeft = true;
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(centerPos, PLAYER_HIT_SIZE, mapData, INTERSECTED_RIGHT, 0 < gimmickVel.Length());
	if (intersectedLine == INTERSECTED_TOP) HitMapChipBottom();
	if (intersectedLine == INTERSECTED_RIGHT) isHitRight = true;

	const float OFFSET = 1.0f;

	// �}�b�v�`�b�v�ƃv���C���[�̓����蔻�� ��΂ɔ킹�Ȃ����߂̏��� ��
	Vec2<float> centerPosBuff = centerPos + Vec2<float>(0, -PLAYER_HIT_SIZE.y + OFFSET);
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(centerPosBuff, PLAYER_HIT_SIZE, mapData, INTERSECTED_LEFT, 0 < gimmickVel.Length());
	if (intersectedLine == INTERSECTED_TOP) HitMapChipBottom();
	centerPos.x = centerPosBuff.x;
	centerPos.y = centerPosBuff.y - (-PLAYER_HIT_SIZE.y + OFFSET);
	centerPosBuff = centerPos + Vec2<float>(0, -PLAYER_HIT_SIZE.y + OFFSET);
	if (intersectedLine == INTERSECTED_LEFT) isHitLeft = true;
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(centerPosBuff, PLAYER_HIT_SIZE, mapData, INTERSECTED_RIGHT, 0 < gimmickVel.Length());
	if (intersectedLine == INTERSECTED_TOP) HitMapChipBottom();
	centerPos.x = centerPosBuff.x;
	centerPos.y = centerPosBuff.y - (-PLAYER_HIT_SIZE.y + OFFSET);
	if (intersectedLine == INTERSECTED_RIGHT) isHitRight = true;

	// �}�b�v�`�b�v�ƃv���C���[�̓����蔻�� ��΂ɔ킹�Ȃ����߂̏��� ��
	centerPosBuff = centerPos + Vec2<float>(0, PLAYER_HIT_SIZE.y - OFFSET);
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(centerPosBuff, PLAYER_HIT_SIZE, mapData, INTERSECTED_LEFT, 0 < gimmickVel.Length());
	if (intersectedLine == INTERSECTED_TOP) HitMapChipBottom();
	centerPos.x = centerPosBuff.x;
	centerPos.y = centerPosBuff.y - (PLAYER_HIT_SIZE.y - OFFSET);
	centerPosBuff = centerPos + Vec2<float>(0, PLAYER_HIT_SIZE.y - OFFSET);
	if (intersectedLine == INTERSECTED_LEFT) isHitLeft = true;
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(centerPosBuff, PLAYER_HIT_SIZE, mapData, INTERSECTED_RIGHT, 0 < gimmickVel.Length());
	if (intersectedLine == INTERSECTED_TOP) HitMapChipBottom();
	centerPos.x = centerPosBuff.x;
	centerPos.y = centerPosBuff.y - (PLAYER_HIT_SIZE.y - OFFSET);
	if (intersectedLine == INTERSECTED_RIGHT) isHitRight = true;

	// �}�b�v�`�b�v�ƃv���C���[�̓����蔻�� ��΂ɔ킹�Ȃ����߂̏��� �E
	centerPosBuff = centerPos + Vec2<float>(PLAYER_HIT_SIZE.x - OFFSET, 0);
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(centerPosBuff, PLAYER_HIT_SIZE, mapData, INTERSECTED_TOP);
	if (intersectedLine == INTERSECTED_TOP) HitMapChipBottom();
	centerPos.y = centerPosBuff.y;
	centerPosBuff = centerPos + Vec2<float>(PLAYER_HIT_SIZE.x - OFFSET, 0);
	if (intersectedLine == INTERSECTED_TOP) isHitTop = true;
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(centerPosBuff, PLAYER_HIT_SIZE, mapData, INTERSECTED_BOTTOM);
	if (intersectedLine == INTERSECTED_TOP) HitMapChipBottom();
	centerPos.y = centerPosBuff.y;
	if (intersectedLine == INTERSECTED_BOTTOM) isHitBottom = true;

	// �}�b�v�`�b�v�ƃv���C���[�̓����蔻�� ��΂ɔ킹�Ȃ����߂̏��� ��
	centerPosBuff = centerPos + Vec2<float>(-PLAYER_HIT_SIZE.x + OFFSET, 0);
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(centerPosBuff, PLAYER_HIT_SIZE, mapData, INTERSECTED_TOP);
	if (intersectedLine == INTERSECTED_TOP) HitMapChipBottom();
	centerPos.y = centerPosBuff.y;
	centerPosBuff = centerPos + Vec2<float>(-PLAYER_HIT_SIZE.x + OFFSET, 0);
	if (intersectedLine == INTERSECTED_TOP) isHitTop = true;
	intersectedLine = MapChipCollider::Instance()->CheckHitMapChipBasedOnTheScale(centerPosBuff, PLAYER_HIT_SIZE, mapData, INTERSECTED_BOTTOM);
	if (intersectedLine == INTERSECTED_TOP) HitMapChipBottom();
	centerPos.y = centerPosBuff.y;
	if (intersectedLine == INTERSECTED_BOTTOM) isHitBottom = true;

	// �{�X�ƃv���C���[�̊p�x
	float bossAngle = atan2(bossPos.y - centerPos.y, bossPos.x - centerPos.x);
	bossAngle = fabs(bossAngle);

	// ���������t���O�ɂ���ď����𕪂���B
	if (isHitTop) {

		// �}�b�v�`�b�v�̏�ɂ������Ă����Ƃ������Ƃ́A�{�X���������ɂ����OK�I
		// �������̋�̓I�Ȓl��
		const float MIN_ANGLE = 0.785398f;
		const float MAX_ANGLE = 2.35619f;

		// �p�x�����̒l�̊Ԃ�������
		if (MIN_ANGLE <= bossAngle && bossAngle <= MAX_ANGLE) {

			// �����������Ă���B
			isHitMapChip = true;

		}

	}
	if (isHitBottom) {

		// �}�b�v�`�b�v�̉��ɂ������Ă����Ƃ������Ƃ́A�v���C���[��������ɂ����OK�I
		// ������̋�̓I�Ȓl��
		const float MIN_ANGLE = 3.92699f;
		const float MAX_ANGLE = 5.49779f;

		// �p�x�����̒l�̊Ԃ�������
		if (MIN_ANGLE <= bossAngle && bossAngle <= MAX_ANGLE) {

			// �����������Ă���B
			isHitMapChip = true;

		}

	}
	if (isHitLeft) {

		// �}�b�v�`�b�v�̍��ɂ������Ă����Ƃ������Ƃ́A�v���C���[���E�����ɂ����OK�I
		// �E�����̋�̓I�Ȓl��
		const float MIN_ANGLE = 0.785398f;
		const float MAX_ANGLE = 5.49779f;

		// �p�x�����̒l�̊Ԃ�������
		if (MAX_ANGLE <= bossAngle || bossAngle <= MIN_ANGLE) {

			// �����������Ă���B
			isHitMapChip = true;

		}

	}
	if (isHitRight) {

		// �}�b�v�`�b�v�̉E�ɂ������Ă����Ƃ������Ƃ́A�{�X���������ɂ����OK�I
		// �������̋�̓I�Ȓl��
		const float MIN_ANGLE = 2.35619f;
		const float MAX_ANGLE = 3.92699f;

		// �p�x�����̒l�̊Ԃ�������
		if (MIN_ANGLE <= bossAngle && bossAngle <= MAX_ANGLE) {

			// �����������Ă���B
			isHitMapChip = true;

		}

	}

}
