#include "Player.h"
#include "PlayerHand.h"
#include "ScrollMgr.h"
#include "ShakeMgr.h"
#include "MapChipCollider.h"
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
#include"DebugParameter.h"
#include"SuperiorityGauge.h"
#include"AudioApp.h"
#include "SlowMgr.h"
#include"CrashMgr.h"
#include"Tutorial.h"
#include "AfterImage.h"

Vec2<float> Player::GetGeneratePos()
{
	return WinApp::Instance()->GetExpandWinCenter() * Vec2<float>(1.0f, 0.25f);
}

static const float EXT_RATE = 0.6f;	//Player's expand rate used in Draw().
static const Vec2<float> PLAYER_HIT_SIZE = { (80 * EXT_RATE) / 2.0f,(80 * EXT_RATE) / 2.0f };			// �v���C���[�̃T�C�Y
Player::Player(const PLAYABLE_CHARACTER_NAME& CharacterName, const int& ControllerIdx, const std::shared_ptr<Tutorial>& Tutorial)
	:CharacterInterFace(PLAYER_HIT_SIZE), anim(CharacterName), controllerIdx(ControllerIdx), tutorial(Tutorial)
{
	/*====== �R���X�g���N�^ =====*/
	if (PLAYER_CHARACTER_NUM <= CharacterName)assert(0);
	static const std::string NAME_DIR[PLAYER_CHARACTER_NUM] =
	{
		"luna",
		"lacy"
	};
	const int ARM_GRAPH_L = TexHandleMgr::LoadGraph("resource/ChainCombat/player/" + NAME_DIR[CharacterName] + "/arm_L.png");
	const int ARM_GRAPH_R = TexHandleMgr::LoadGraph("resource/ChainCombat/player/" + NAME_DIR[CharacterName] + "/arm_R.png");
	const int AIM_GRAPH = TexHandleMgr::LoadGraph("resource/ChainCombat/player/" + NAME_DIR[CharacterName] + "/aim.png");
	lHand = make_unique<PlayerHand>(ARM_GRAPH_L, AIM_GRAPH);
	rHand = make_unique<PlayerHand>(ARM_GRAPH_R, AIM_GRAPH);

	// �摜�����[�h
	//playerGraph = TexHandleMgr::LoadGraph("resource/IntoTheAbyss/Player.png");

	shotSE = AudioApp::Instance()->LoadAudio("resource/ChainCombat/sound/shot.wav");
	AudioApp::Instance()->ChangeVolume(shotSE, 0.2f);

	bulletGraph = TexHandleMgr::LoadGraph("resource/ChainCombat/player/" + NAME_DIR[CharacterName] + "/bullet.png");
}

Player::~Player()
{
}

void Player::OnInit()
{

	/*===== ���������� =====*/

	//�v���C���[�̌���������
	//playerDir = DEFAULT;

	//�A�j���[�V����������
	anim.Init();

	// �r���Z�b�g
	static const float OFFSET = -8.0f;

	lHand->Init(-GetPlayerGraphSize().x + OFFSET);
	rHand->Init(GetPlayerGraphSize().x - OFFSET);

	// �A�˃^�C�}�[��������
	rapidFireTimerLeft = 0;
	rapidFireTimerRight = 0;

	// ��������ʒu�ɖ߂��B
	rHand->SetAngle(DEF_RIGHT_HAND_ANGLE);
	lHand->SetAngle(DEF_LEFT_HAND_ANGLE);

	//�X�g���b�`������
	stretch_LU = { 0.0f,0.0f };
	stretch_RB = { 0.0f,0.0f };
	stretchTimer = STRETCH_RETURN_TIME;

	drawCursorFlag = true;

	swingCoolTime = 0;

	initSize = { 5.0f,5.0f };
	size = initSize;
	sizeVel = 120.0f;
	initPaticleFlag = false;
	moveTimer = 0;

	playerDirX = GetWhichTeam() == LEFT_TEAM ? PLAYER_LEFT : PLAYER_RIGHT;
	playerDirY = PLAYER_FRONT;

	// �E�̃L�������������
	if (GetWhichTeam() == RIGHT_TEAM) {
		charaColor = { 239, 1, 144,255 };
	}
	// �Ђ���̃L�������������
	else if (GetWhichTeam() == LEFT_TEAM) {
		charaColor = { 47, 255, 139,255 };
	}

	swingVec = { -1.0f,0.0f };

	inputInvalidTimerByCrash = 0;

	dashAftImgTimer = 0;
	isInputRightStick = false;
	isPrevInputRightStick = false;
	prevInputRightStick = {};
}

void Player::OnUpdate(const vector<vector<int>>& MapData)
{
	//�f�o�b�N�p�̒l�ύX
	std::shared_ptr<PlayerDebugParameterData> data = DebugParameter::Instance()->nowData;

	ADD_GRAVITY = data->ADD_GRAVITY;
	MAX_GRAVITY = data->MAX_GRAVITY;
	RECOIL_AMOUNT = data->RECOIL_AMOUNT;
	FIRST_RECOIL_AMOUNT = data->FIRST_RECOIL_AMOUNT;
	MAX_RECOIL_AMOUNT = data->MAX_RECOIL_AMOUNT;
	RAPID_FIRE_TIMER = data->RAPID_FIRE_TIMER;


	/*===== ���͏��� =====*/
	// ���͂Ɋւ���X�V�������s���B
	if (GetCanMove())Input(MapData);

	/*===== �X�V���� =====*/

	// ��s���̓^�C�}�[���X�V�B
	if (isAdvancedEntrySwing) {

		--advancedEntrySwingTimer;
		if (advancedEntrySwingTimer < 0) isAdvancedEntrySwing = false;

	}

	// �������U��񂵂����Ă�����B
	if (partner.lock()->GetNowSwing()) {

		// �c����ۑ��B
		Vec2<float> extRate = ((GetPlayerGraphSize() - stretch_LU + stretch_RB) / GetPlayerGraphSize()) * ScrollMgr::Instance()->zoom * EXT_RATE * stagingDevice.GetExtRate() * size;
		AfterImageMgr::Instance()->Generate(pos, extRate, stagingDevice.GetSpinRadian(), anim.GetGraphHandle(), charaColor);

	}

	// �������U��񂵂����Ă�����X�V�������s��Ȃ��B
	if (!(!partner.lock()->GetNowSwing() && !nowSwing)) return;

	//�ړ��Ɋւ��鏈��
	Move();

	if (isGripPowerEmpty)
	{
		outOfStaminaEffect.Start(pos, MAX_GRIP_POWER_TIMER);
	}
	outOfStaminaEffect.baseEmptyStringPos = pos;
	outOfStaminaEffect.baseMaxStringPos = pos;
	outOfStaminaEffect.Update();


	// �A�˃^�C�}�[���X�V
	if (rapidFireTimerLeft > 0) --rapidFireTimerLeft;
	if (rapidFireTimerRight > 0) --rapidFireTimerRight;

	// �v���C���[�̂������̃}�b�v�`�b�v���W����������B
	int mapX = pos.x / MAP_CHIP_SIZE;
	int mapY = (pos.y - PLAYER_HIT_SIZE.y - 1.0f) / MAP_CHIP_SIZE;
	if (mapX <= 0) mapX = 1;
	if (mapX >= MapData[0].size()) mapX = MapData[0].size() - 1;
	if (mapY <= 0) mapY = 1;
	if (mapY >= MapData.size()) mapY = MapData.size() - 1;
	if (mapY <= 0) mapY = 1;
	if (mapX <= 0) mapX = 1;
	if (mapX >= MapData[mapY].size()) mapX = MapData[mapY].size() - 1;

	// ���̃}�b�v�`�b�v���u���b�N�ŁAX�������̈ړ��ʂ����ȏゾ������p�[�e�B�N���𐶐�����B
	//if (MapData[mapY][mapX] > 0 && MapData[mapY][mapX] < 10 && fabs(vel.x) >= 10.0f)ParticleMgr::Instance()->Generate(Vec2<float>(pos.x, pos.y - GetPlayerGraphSize().y), Vec2<float>(0, -1), HIT_MAP);

	// �U��񂵂̃N�[���^�C�����X�V
	if (0 < swingCoolTime) --swingCoolTime;

	//muffler.Update(pos);

	// ���͂𖳌�������^�C�}�[���X�V�B
	if (0 < inputInvalidTimerByCrash)
	{
		--inputInvalidTimerByCrash;
		if (inputInvalidTimerByCrash <= 0 && !GetNowBreak())
		{
			anim.ChangeAnim(DEFAULT_FRONT);
		}
	}

	// �O���b�v�̓^�C�}�[��0�ɂȂ�����A���S�ɉ񕜂���܂œ��񒣂�Ȃ��悤�ɂ���B
	if (gripPowerTimer <= 0) {

		isGripPowerEmpty = true;
		anim.ChangeAnim(TIRED);
	}

	//�_�b�V���̎c��
	if (dashAftImgTimer)
	{
		Vec2<float> extRate = ((GetPlayerGraphSize() - stretch_LU + stretch_RB) / GetPlayerGraphSize()) * ScrollMgr::Instance()->zoom * EXT_RATE * stagingDevice.GetExtRate() * size;
		AfterImageMgr::Instance()->Generate(pos, extRate, 0.0f, anim.GetGraphHandle(), GetTeamColor());
		dashAftImgTimer--;
	}

	// �U��񂵂Ă��Ȃ�������
	if (!nowSwing) {

		if (anim.GetNowAnim() == HOLD)anim.ChangeAnim(DEFAULT_FRONT);

		// �R���ݏ��(���񒣂���)����������B
		isHold = false;

		tutorial.lock()->SetRstickInput(false);

	}

}

void Player::OnUpdateNoRelatedSwing()
{
	// �r���X�V
	lHand->Update(pos + anim.GetHandCenterOffset());
	rHand->Update(pos + anim.GetHandCenterOffset());

	// ���̓^�C�}�[���K��l�ɋ߂Â���B
	if (!nowSwing && gripPowerTimer < MAX_GRIP_POWER_TIMER) {
		gripPowerTimer += SlowMgr::Instance()->slowAmount;

		// �ő�l�ɂȂ����爬�͂��g���؂��Ă���񕜂��Ă����ԃt���O��܂�B
		if (MAX_GRIP_POWER_TIMER <= gripPowerTimer) {

			gripPowerTimer = MAX_GRIP_POWER_TIMER;
			isGripPowerEmpty = false;
			anim.ChangeAnim(DEFAULT_FRONT);
		}
	}

	//�X�g���b�`�X�V
	UpdateStretch();

	//�A�j���[�V�����X�V
	anim.Update();
}

void Player::OnDraw()
{
	//if (vel.y < 0)playerDir = BACK;
	auto moveInput = UsersInput::Instance()->GetLeftStickVec(controllerIdx, { 0.5f,0.5f });

	if (!isHold && anim.GetNowAnim() != SWINGED && !isGripPowerEmpty && 20 <= moveTimer)
	{
		if (moveInput.x)
		{
			if (playerDirY == PLAYER_BACK)anim.ChangeAnim(PULL_BACK);
			else anim.ChangeAnim(PULL_FRONT);
		}
		else
		{
			if (playerDirY == PLAYER_BACK)anim.ChangeAnim(DEFAULT_BACK);
			else anim.ChangeAnim(DEFAULT_FRONT);
		}
	}

	/*===== �`�揈�� =====*/

	// �v���C���[�̕`�揈��
	//Vec2<float>leftUp = { pos.x * ScrollMgr::Instance()->zoom - GetPlayerGraphSize().x * ScrollMgr::Instance()->zoom - scrollShakeZoom.x,
	//	pos.y * ScrollMgr::Instance()->zoom - GetPlayerGraphSize().y * ScrollMgr::Instance()->zoom - scrollShakeZoom.y };
	//Vec2<float>rightBottom = { pos.x * ScrollMgr::Instance()->zoom + GetPlayerGraphSize().x * ScrollMgr::Instance()->zoom - scrollShakeZoom.x,
	//	pos.y * ScrollMgr::Instance()->zoom + GetPlayerGraphSize().y * ScrollMgr::Instance()->zoom - scrollShakeZoom.y };
	//�c���`��
	afImg.Draw();

	//muffler.Draw(LigManager);


	//rHand->Draw(EXT_RATE, DEF_RIGHT_HAND_ANGLE, { 0.0f,0.0f }, drawCursorFlag);
	//lHand->Draw(EXT_RATE, DEF_LEFT_HAND_ANGLE, { 1.0f,0.0f }, drawCursorFlag);

	//�X�g���b�`���Z
	//leftUp += stretch_LU;
	//rightBottom += stretch_RB;
	static auto CRASH_TEX = D3D12App::Instance()->GenerateTextureBuffer(Color(255, 0, 0, 255));
	const Vec2<float>drawPos = ScrollMgr::Instance()->Affect(pos + stagingDevice.GetShake());
	//����
	auto bodyTex = TexHandleMgr::GetTexBuffer(anim.GetGraphHandle());
	const Vec2<float> expRateBody = ((GetPlayerGraphSize() - stretch_LU + stretch_RB) / GetPlayerGraphSize());
	bool mirorX = playerDirX == PLAYER_RIGHT || (isHold && (partner.lock()->pos - pos).x < 0);
	DrawFunc_FillTex::DrawRotaGraph2D(drawPos, expRateBody * ScrollMgr::Instance()->zoom * EXT_RATE * stagingDevice.GetExtRate() * size,
		stagingDevice.GetSpinRadian(), bodyTex, CRASH_TEX, stagingDevice.GetFlashAlpha(), { 0.5f,0.5f }, { mirorX,false });
}

void Player::OnDrawUI()
{
	static const int LINE_ALPHA = 120;
	static std::shared_ptr<TextureBuffer> COLOR_TEX[TEAM_NUM] =
	{
		 D3D12App::Instance()->GenerateTextureBuffer(Color(47,255,139,LINE_ALPHA)),
		 D3D12App::Instance()->GenerateTextureBuffer(Color(239,1,144,LINE_ALPHA))
	};
	static const int RETICLE_GRAPH[TEAM_NUM] = { TexHandleMgr::LoadGraph("resource/ChainCombat/reticle_player.png"),TexHandleMgr::LoadGraph("resource/ChainCombat/reticle_enemy.png") };
	static const int ARROW_GRAPH[TEAM_NUM] = { TexHandleMgr::LoadGraph("resource/ChainCombat/arrow_player.png"),TexHandleMgr::LoadGraph("resource/ChainCombat/arrow_enemy.png") };
	static const Angle ARROW_ANGLE_OFFSET = Angle(1);
	static const float ARROW_DIST_OFFSET = 32.0f;

	const auto rightStickVec = UsersInput::Instance()->GetRightStickVec(controllerIdx, { 0.5f,0.5f });

	if (isHold && !GetNowSwing() && !StunEffect::Instance()->isActive)
	{
		const Vec2<float>drawScale = { ScrollMgr::Instance()->zoom ,ScrollMgr::Instance()->zoom };
		const auto team = GetWhichTeam();

		//�U��񂵐�`��
		float dist = partner.lock()->pos.Distance(pos);
		Vec2<float>target = pos + rightStickVec * dist;
		DrawFunc::DrawLine2DGraph(ScrollMgr::Instance()->Affect(pos), ScrollMgr::Instance()->Affect(target), COLOR_TEX[GetWhichTeam()], 2, AlphaBlendMode_Trans);
		//DrawFunc::DrawRotaGraph2D(ScrollMgr::Instance()->Affect(target), drawScale * 0.8f, 0.0f, TexHandleMgr::GetTexBuffer(RETICLE_GRAPH[team]));

		//�U��񂵕����`��
		bool clockWise = 0 < rightStickVec.Cross(partner.lock()->pos - pos);
		Angle arrowPosAngle = KuroFunc::GetAngle(pos, partner.lock()->pos);
		Angle rotateAngle = arrowPosAngle + Angle(90 * (clockWise ? -1 : 1));
		Vec2<float>vec = partner.lock()->pos - pos;
		vec.Normalize();
		DrawFunc::DrawRotaGraph2D(ScrollMgr::Instance()->Affect(partner.lock()->pos + vec * ARROW_DIST_OFFSET), drawScale * 0.5f, rotateAngle, TexHandleMgr::GetTexBuffer(ARROW_GRAPH[team]), { 0.0f,0.5f });
	}

	const auto leftStickVec = UsersInput::Instance()->GetLeftStickVec(controllerIdx, { 0.5f,0.5f });
	const auto leftTrigger = UsersInput::Instance()->ControllerInput(controllerIdx, XBOX_BUTTON::LT);
	const auto rightTrigger = UsersInput::Instance()->ControllerInput(controllerIdx, XBOX_BUTTON::RT);

	tutorial.lock()->Draw(leftStickVec, rightStickVec, leftTrigger, rightTrigger);
}

void Player::OnHitMapChip(const HIT_DIR& Dir)
{
	if (Dir == TOP)
	{
		// Y�����̈ړ��ʂ����炷�B
		vel.y /= 2.0f;
	}
	else if (Dir == BOTTOM)
	{
		stretch_RB.y = 0.0f;

		// �ڒn�t���O�𗧂Ă�B
		//onGround = true;

		// X���̈ړ��ʂ̍��v�����ȏゾ�����疀�C�����B
		if (fabs(vel.x) >= STOP_DEADLINE_X) {

			// ���C������B
			vel.y *= VEL_MUL_AMOUNT;
			vel.x *= VEL_MUL_AMOUNT;
		}
		else {

			// X�����̈ړ��ʂ𖳌�������B
			vel.x = 0;
			vel.y = 0;

			//���C�����Ƃ��̓X�g���b�`���キ����
			stretch_RB.x /= STRETCH_DIV_RATE;
			stretch_LU.x /= STRETCH_DIV_RATE;

			//�ҋ@�A�j���[�V�����ɖ߂�
			//anim.ChangeAnim(ON_GROUND_WAIT);
		}

		vel.y = 0;

		// �ړ��ʂ����ȉ��ɂȂ�����0�ɂ���B
		if (fabs(vel.x) <= 1.0f) vel.x = 0;
	}
	else if (Dir == LEFT)
	{
		stretch_LU.x = 0.0f;

		// X�����̈ړ��ʂ𖳌�������B
		vel.x = 0;
		//vel.y = 0;

		//���C�����Ƃ��̓X�g���b�`���キ����
		stretch_RB.y /= STRETCH_DIV_RATE;
		stretch_LU.y /= STRETCH_DIV_RATE;

		//�Ǔ\��t���A�j���[�V����
		//anim.ChangeAnim(ON_WALL_WAIT);
	}
	else if (Dir == RIGHT)
	{
		stretch_RB.x = 0.0f;

		// X�����̈ړ��ʂ𖳌�������B
		vel.x = 0;
		//vel.y = 0;

		//���C�����Ƃ��̓X�g���b�`���キ����
		stretch_RB.y /= STRETCH_DIV_RATE;
		stretch_LU.y /= STRETCH_DIV_RATE;

		//�Ǔ\��t���A�j���[�V����
		//anim.ChangeAnim(ON_WALL_WAIT);
	}
}

void Player::Input(const vector<vector<int>>& MapData)
{
	/*===== ���͏��� =====*/

	// �X�^�����o������������͂��󂯕t���Ȃ��B
	if (StunEffect::Instance()->isActive) return;

	// ���͂��󂯕t���Ȃ��^�C�}�[��0���傫�������珈�����΂��B
	if (0 < inputInvalidTimerByCrash) return;

	// �ǂɋ��܂��Ĕ��肪����������Ă���Ԃ͏������󂯕t���Ȃ��B
	if (0 < GetStackWinTimer()) return;

	// �U��񂵂Ă���Ƃ��͓��͂��󂯕t���Ȃ��B
	if (nowSwing) return;

	// �U��񂳂�Ă���t���O
	bool isSwingPartner = partner.lock()->GetNowSwing();

	// �U��񂳂�Ă����Ԃ̎��ɓ��񒣂�̏�ԂɂȂ��Ă��邩�t���O(���ۂɂ͓��񒣂��Ă��Ȃ�)�B
	bool isSwingPartnerHold = false;

	const float INPUT_DEAD_LINE = 0.3f;

	Vec2<float> inputLeftVec;

	inputLeftVec = UsersInput::Instance()->GetLeftStickVecFuna(controllerIdx);
	inputLeftVec /= {32768.0f, 32768.0f};

	Vec2<float> inputRightVec = UsersInput::Instance()->GetRightStickVecFuna(controllerIdx);
	inputRightVec /= {32768.0f, 32768.0f};

	// ���͏���ۑ�����B
	isPrevInputRightStick = isInputRightStick;
	isInputRightStick = 0.9f < inputRightVec.Length();

	// ���͂̃f�b�h���C����݂���B
	float inputRate = inputLeftVec.Length();
	if (inputRate >= 0.5f) {

		// �ړ����󂯕t����B
		if (vel.Length() < MOVE_SPEED_PLAYER) {
			vel.x = inputLeftVec.x * (MOVE_SPEED_PLAYER * inputRate);
			vel.y = inputLeftVec.y * (MOVE_SPEED_PLAYER * inputRate);
		}

		// �E��̊p�x���X�V
		lHand->SetAngle(KuroFunc::GetAngle(inputLeftVec));

		if (inputLeftVec.x < 0)playerDirX = PLAYER_LEFT;
		else if (0 < inputLeftVec.x)playerDirX = PLAYER_RIGHT;
		if (inputLeftVec.y < 0)playerDirY = PLAYER_BACK;
		else if (0 < inputLeftVec.y)playerDirY = PLAYER_FRONT;
	}

	inputRightVec = UsersInput::Instance()->GetRightStickVecFuna(controllerIdx);
	inputRightVec /= {32768.0f, 32768.0f};

	// ���͂̃f�b�h���C����݂���B
	if (inputRightVec.Length() >= 0.5f) {

		// ����̊p�x���X�V
		rHand->SetAngle(KuroFunc::GetAngle(inputRightVec));

		// ��莞�ԓ��͂��Ȃ������珉���ʒu�ɖ߂�
		//handReturnTimer = DEF_HAND_RETURN_TIMER;

	}

	// [LT�������ꂽ��] [���͂��c���Ă�����] [���͂��g���؂��Ă���񕜂��Ă����Ԃ���Ȃ�������]
	bool isInputLB = UsersInput::Instance()->ControllerInput(controllerIdx, XBOX_BUTTON::LB);
	bool isInputRB = UsersInput::Instance()->ControllerOnTrigger(controllerIdx, XBOX_BUTTON::RB);
	if ((!isInputRightStick && isPrevInputRightStick) && 0 < gripPowerTimer && !isGripPowerEmpty) {

		// �U��񂳂�Ă�����
		if (isSwingPartner) {

			isSwingPartnerHold = true;

		}
		else {

			anim.ChangeAnim(HOLD);

			// �R���ݏ��(���񒣂���)�ɂ���B
			isHold = true;

			tutorial.lock()->SetRstickInput(true);

			// �ړ��ʂ�0�ɂ���B
			vel = {};

		}

	}


	//�`���[�g���A���̕\�� / ��\��
	if (UsersInput::Instance()->ControllerOnTrigger(controllerIdx, XBOX_BUTTON::BACK))
	{
		tutorial.lock()->TurnActive();
	}

#pragma region itiou nokosite okimasu

	// LB�������ꂽ�甽��������B
	//if (UsersInput::Instance()->ControllerInput(controllerIdx, XBOX_BUTTON::LB) && rapidFireTimerLeft <= 0) {

	//	// ����������B
	//	float rHandAngle = lHand->GetAngle();

	//	// Get�����l�͎�̌����Ă�������Ȃ̂ŁA-180�x����B
	//	rHandAngle -= Angle::PI();

	//	// onGround��true��������ړ��ʂ����Z���Ȃ��B
	//	//if (!onGround || sinf(rHandAngle) < 0.5f) {
	//	].x += cosf(rHandAngle) * RECOIL_AMOUNT;
	//	//}

	//	vel.y += sinf(rHandAngle) * RECOIL_AMOUNT;

	//	// �v���C���[�̘r�𓮂����B
	//	lHand->Shot(Vec2<float>(cosf(rHandAngle), sinf(rHandAngle)), false);


	//	// �e�𐶐�����B
	//	const float ARM_DISTANCE = 20.0f;
	//	const float OFFSET_Y = -14.0f;
	//	const float OFFSET_X = 12.0f;

	//	float angle = lHand->GetAngle();

	//	AudioApp::Instance()->PlayWave(shotSE);
	//	Shot(lHand->handPos + Vec2<float>(cosf(angle) * ARM_DISTANCE + OFFSET_X, sinf(angle) * ARM_DISTANCE + OFFSET_Y), angle);

	//	// �A�˃^�C�}�[���Z�b�g
	//	rapidFireTimerLeft = RAPID_FIRE_TIMER;

	//	//�X�g���b�`
	//	CalculateStretch(vel);
	//}

	//// RB�������ꂽ�甽��������B
	//if (UsersInput::Instance()->ControllerInput(controllerIdx, XBOX_BUTTON::RB) && rapidFireTimerRight <= 0) {

	//	// ����������B
	//	float lHandAngle = rHand->GetAngle();

	//	// Get�����l�͎�̌����Ă�������Ȃ̂ŁA-180�x����B
	//	lHandAngle -= Angle::PI();

	//	// �v���C���[�̂ЂƂ�̃u���b�N����������ׂ̏����B
	//	int mapX = pos.x / MAP_CHIP_SIZE;
	//	int mapY = pos.y / MAP_CHIP_SIZE;
	//	if (mapX <= 0) mapX = 1;
	//	if (mapX >= MapData[0].size()) mapX = MapData[0].size() - 1;
	//	if (mapY <= 0) mapY = 1;
	//	if (mapY >= MapData.size()) mapY = MapData.size() - 1;

	//	// onGround��true��������ړ��ʂ����Z���Ȃ��B
	//		//if (!onGround || sinf(lHandAngle) < 0) {
	//	vel.x += cosf(lHandAngle) * RECOIL_AMOUNT;



	//	vel.y += sinf(lHandAngle) * RECOIL_AMOUNT;

	//	// �v���C���[�̘r�𓮂����B
	//	rHand->Shot(Vec2<float>(cosf(lHandAngle), sinf(lHandAngle)), false);


	//	// �e�𐶐�����B
	//	const float ARM_DISTANCE = 20.0f;
	//	const float OFFSET_Y = -14.0f;
	//	const float OFFSET_X = -12.0f;

	//	float angle = rHand->GetAngle();

	//	AudioApp::Instance()->PlayWave(shotSE);
	//	Shot(rHand->handPos + Vec2<float>(cosf(angle) * ARM_DISTANCE + OFFSET_X, sinf(angle) * ARM_DISTANCE + OFFSET_Y), angle);

	//	// �A�˃^�C�}�[���Z�b�g
	//	rapidFireTimerRight = RAPID_FIRE_TIMER;

	//	//�X�g���b�`
	//	CalculateStretch(vel);
	//}

	// �ړ����x�����E�l�𒴂��Ȃ��悤�ɂ���B
	//if (vel.x >= MAX_RECOIL_AMOUNT) vel.x = MAX_RECOIL_AMOUNT;
	//if (vel.x <= -MAX_RECOIL_AMOUNT) vel.x = -MAX_RECOIL_AMOUNT;
	//if (vel.y >= MAX_RECOIL_AMOUNT) vel.y = MAX_RECOIL_AMOUNT;
	//if (vel.y <= -MAX_RECOIL_AMOUNT) vel.y = -MAX_RECOIL_AMOUNT;


#pragma endregion


	// ���͂����ɐU��񂵃x�N�g�����X�V�B

	// �����Ƃ̈ʒu�֌W�ɂ����Ă̋t�x�N�g���ɐU��񂷁B
	Vec2<float> subPos = pos - partner.lock()->pos;
	swingVec = (subPos).GetNormal();

	// RT�������ꂽ��
	bool canSwing = (!isInputRightStick && isPrevInputRightStick);
	if ((!isSwingPartner && canSwing || isAdvancedEntrySwing) && !isGripPowerEmpty) {

		// �U��񂵂̏���

		// �O�ς̍��E����ɂ��A���v��肩�����v���Ƃ��擾����B���̒l�����A���̒l���E�B
		Vec2<float> posBuff = partner.lock()->pos - pos;
		posBuff.Normalize();
		bool isClockWise = 0 < posBuff.Cross(prevInputRightStick);

		SwingPartner({ swingVec }, isClockWise);

		isInputSwingRB = isInputRB;

	}
	else if (isSwingPartner && canSwing && !isGripPowerEmpty && isInputRightStick) {

		// ��s���͂�ۑ��B
		isAdvancedEntrySwing = true;
		advancedEntrySwingTimer = ADVANCED_ENTRY_SWING_TIMER;

	}


	// ���͂̃f�b�h���C����݂���B
	inputLeftVec = UsersInput::Instance()->GetLeftStickVecFuna(controllerIdx);
	inputLeftVec /= {32768.0f, 32768.0f};
	inputRate = inputLeftVec.Length();
	if (isInputLB && !isPrevLeftBottom && 0.5f <= inputRate && !isGripPowerEmpty) {

		// inputVec = �Ђ���X�e�B�b�N�̓��͕���
		const float DASH_SPEED = 30.0f;
		vel += inputLeftVec * DASH_SPEED;

		// �X�^�~�i������
		const int DASH_GRIP_POWER = 20;
		gripPowerTimer -= DASH_GRIP_POWER;

		//��
		ParticleMgr::Instance()->Generate(pos, -inputLeftVec, BULLET);
		//�c��
		dashAftImgTimer = 10;

	}

	// �E�X�e�B�b�N�����͂���Ă�����A�\�������o���B
	if (isInputRightStick && !isGripPowerEmpty) {

		// ���v��肩�ǂ����B���̒l�����A���̒l���E�B
		inputRightVec.Normalize();
		Vec2<float> posBuff = partner.lock()->pos - pos;
		posBuff.Normalize();
		bool isClockWise = 0 < posBuff.Cross(inputRightVec);

		if (isClockWise) {

			CWSwingSegmentMgr.Update(pos, Vec2<float>(partner.lock()->pos - pos).GetNormal(), Vec2<float>(pos - partner.lock()->pos).Length(), MapData);
			CCWSwingSegmentMgr.Init();

		}
		else {

			CCWSwingSegmentMgr.Update(pos, Vec2<float>(partner.lock()->pos - pos).GetNormal(), Vec2<float>(pos - partner.lock()->pos).Length(), MapData);
			CWSwingSegmentMgr.Init();

		}

	}

	// ���͂���Ă��Ȃ��āA�X�C���O������Ȃ�������\�����������B
	if (inputRightVec.Length() <= 0.5f && !nowSwing) {

		CCWSwingSegmentMgr.Init();
		CWSwingSegmentMgr.Init();

	}

	// ���͂�ۑ��B
	Vec2<float> buff = UsersInput::Instance()->GetRightStickVecFuna(controllerIdx);
	buff /= {32768.0f, 32768.0f};
	if (0.5f <= buff.Length()) {

		prevInputRightStick = buff;

	}

	// ���V�����_�[�̓��͏���ۑ��B
	isPrevLeftBottom = isInputLB;

}

void Player::Move()
{

	/*===== �ړ��Ɋւ��鏈�� =====*/

	// �ړ��ʂ�0�ɋߕt����B
	vel = KuroMath::Lerp(vel, { 0.0f,0.0f }, 0.05f);

	// �ړ��ʂ����E�𒴂��Ȃ��悤�ɂ���B
	//if (fabs(vel.x) > MAX_RECOIL_AMOUNT) {

	//	// ������ۑ�����B
	//	bool sign = std::signbit(vel.x);

	//	// �����^����B
	//	vel.x = MAX_RECOIL_AMOUNT;

	//	// ������������B
	//	vel.x *= sign ? -1 : 1;

	//}
	//if (fabs(vel.y) > MAX_RECOIL_AMOUNT) {

	//	// ������ۑ�����B
	//	bool sign = std::signbit(vel.y);

	//	// �����^����B
	//	vel.y = MAX_RECOIL_AMOUNT;

	//	// ������������B
	//	vel.y *= sign ? -1 : 1;
	//}
}

void Player::Shot(const Vec2<float>& GeneratePos, const float& ForwardAngle)
{
	//�e��
	static const float BULLET_SPEED = 30.0f;
	bulletMgr.Generate(bulletGraph, GeneratePos, ForwardAngle, BULLET_SPEED);
}

void Player::PushBackWall()
{
	const int WIN_WIDTH = WinApp::Instance()->GetWinSize().x;
	const int WIN_HEIGHT = WinApp::Instance()->GetWinSize().y;

	/*===== �ǂƂ̉����߂����� =====*/

	// X���Ɋւ��鉟���߂������B

	// �E��
	const float PLAYER_RIGHT_X = pos.x + PLAYER_HIT_SIZE.x;
	if (PLAYER_RIGHT_X > WIN_WIDTH) {

		// ���������߂��B
		pos.x -= PLAYER_RIGHT_X - WIN_WIDTH;

		// X�����̈ړ��ʂ𖳌�������B
		vel.x = 0;

	}

	// ����
	const float PLAYER_LEFT_X = pos.x - PLAYER_HIT_SIZE.x;
	if (PLAYER_LEFT_X < 0) {

		// ���������߂��B
		pos.x -= PLAYER_LEFT_X;

		// X�����̈ړ��ʂ𖳌�������B
		vel.x = 0;

	}


	// Y���Ɋւ��鉟���߂�����

	// �㑤
	const float PLAYER_UP_Y = pos.y - PLAYER_HIT_SIZE.y;
	if (PLAYER_UP_Y < 0) {

		// ���������߂��B
		pos.y -= PLAYER_UP_Y;

		// Y�����̈ړ��ʂ����炷�B
		vel.y /= 2.0f;

	}

	// ����
	const float PLAYER_DOWN_Y = pos.y + PLAYER_HIT_SIZE.y;
	if (PLAYER_DOWN_Y >= WIN_HEIGHT) {

		// ���������߂��B
		pos.y -= PLAYER_DOWN_Y - WIN_HEIGHT;

		// �ڒn�t���O�𗧂Ă�B
		//onGround = true;

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

void Player::CalculateStretch(const Vec2<float>& Move)
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

void Player::CheckHitMapChipVel(const Vec2<float>& checkPos, const vector<vector<int>>& MapData)
{
	// �}�b�v�`�b�v�ƃv���C���[�̓����蔻�� ��΂Ɋђʂ����Ȃ��ׂ̏���
	//Vec2<float> upperPlayerPos = pos - Vec2<float>(0, PLAYER_HIT_SIZE.y / 2.0f);
	//Vec2<float> upperPlayerPosBuff = upperPlayerPos;
	INTERSECTED_LINE intersectedLine = {};

	// �ʏ펞�̓����蔻��
	Vec2<float> moveDir = prevPos - pos;
	moveDir.Normalize();
	//intersectedLine = (INTERSECTED_LINE)MapChipCollider::Instance()->CheckHitMapChipBasedOnTheVel(pos, prevFrameCenterPos, vel + gimmickVel, Vec2<float>(PLAYER_HIT_SIZE.x, PLAYER_HIT_SIZE.y), onGround, MapData);
	intersectedLine = (INTERSECTED_LINE)MapChipCollider::Instance()->CheckHitMapChipBasedOnTheVel(pos, prevPos, moveDir * Vec2<float>(PLAYER_HIT_SIZE.x, PLAYER_HIT_SIZE.y), Vec2<float>(PLAYER_HIT_SIZE.x, PLAYER_HIT_SIZE.y / 2.0f), MapData);


	// ����(�����߂��ꂽ�l�����W�������)
	//pos += upperPlayerPos - upperPlayerPosBuff;

	// �����ƃv���C���[�̊p�x
	auto partnerPos = GetPartnerPos();
	float bossAngle = atan2(partnerPos.y - pos.y, partnerPos.x - pos.x);
	bossAngle = fabs(bossAngle);

	// ���������ʒu�ɉ����ď����𕪂���B
	if (intersectedLine == INTERSECTED_TOP) {

		// ���ɓ��������ꍇ
		mapChipHit[BOTTOM] = true;

		// �}�b�v�`�b�v�̏�ɂ������Ă����Ƃ������Ƃ́A�{�X���������ɂ����OK�I
		// �������̋�̓I�Ȓl��
		const float MIN_ANGLE = 0.785398f;
		const float MAX_ANGLE = 2.35619f;

		// �p�x�����̒l�̊Ԃ�������
		if (MIN_ANGLE <= bossAngle && bossAngle <= MAX_ANGLE) {

			// �����������Ă���B
			stackMapChip = true;

		}

	}
	else if (intersectedLine == INTERSECTED_RIGHT) {

		// ���ɓ��������ꍇ
		mapChipHit[LEFT] = true;

		// �}�b�v�`�b�v�̉E�ɂ������Ă����Ƃ������Ƃ́A�{�X���������ɂ����OK�I
		// �������̋�̓I�Ȓl��
		const float MIN_ANGLE = 2.35619f;
		const float MAX_ANGLE = 3.92699f;

		// �p�x�����̒l�̊Ԃ�������
		if (MIN_ANGLE <= bossAngle && bossAngle <= MAX_ANGLE) {

			// �����������Ă���B
			stackMapChip = true;
		}

	}
	else if (intersectedLine == INTERSECTED_BOTTOM) {

		// ��ɓ��������ꍇ
		mapChipHit[TOP] = true;

		// �}�b�v�`�b�v�̉��ɂ������Ă����Ƃ������Ƃ́A�v���C���[��������ɂ����OK�I
		// ������̋�̓I�Ȓl��
		const float MIN_ANGLE = 3.92699f;
		const float MAX_ANGLE = 5.49779f;

		// �p�x�����̒l�̊Ԃ�������
		if (MIN_ANGLE <= bossAngle && bossAngle <= MAX_ANGLE) {

			// �����������Ă���B
			stackMapChip = true;
		}

	}
	else if (intersectedLine == INTERSECTED_LEFT) {

		// �E�ɓ��������ꍇ
		mapChipHit[RIGHT] = true;

		// �}�b�v�`�b�v�̍��ɂ������Ă����Ƃ������Ƃ́A�v���C���[���E�����ɂ����OK�I
		// �E�����̋�̓I�Ȓl��
		const float MIN_ANGLE = 0.785398f;
		const float MAX_ANGLE = 5.49779f;

		// �p�x�����̒l�̊Ԃ�������
		if (MAX_ANGLE <= bossAngle || bossAngle <= MIN_ANGLE) {

			// �����������Ă���B
			stackMapChip = true;
		}

	}
	else if (intersectedLine == INTERSECTED_RISE) {

		// �̂��オ��̏ꏊ�ɓ��������ꍇ
		vel.y *= 0.5f;
	}

}

bool Player::Appear()
{
	if (20 <= moveTimer)
	{
		return true;
	}

	//�T�C�Y��1.0f�ɂȂ�܂œ����Ȃ�
	if (1.0f < size.x && 1.0f < size.y)
	{
		float time = 30.0f;
		size -= initSize / time;
	}
	else
	{
		if (!initPaticleFlag)
		{
			Vec2<float>radian(cosf(Angle::ConvertToRadian(0.0f)), sinf(Angle::ConvertToRadian(0.0f)));
			ParticleMgr::Instance()->Generate(pos, radian, BULLET);

			radian = { cosf(Angle::ConvertToRadian(90.0f)), sinf(Angle::ConvertToRadian(90.0f)) };
			ParticleMgr::Instance()->Generate(pos, radian, BULLET);

			radian = { cosf(Angle::ConvertToRadian(180.0f)), sinf(Angle::ConvertToRadian(180.0f)) };
			ParticleMgr::Instance()->Generate(pos, radian, BULLET);

			radian = { cosf(Angle::ConvertToRadian(270.0f)), sinf(Angle::ConvertToRadian(270.0f)) };
			ParticleMgr::Instance()->Generate(pos, radian, BULLET);
			initPaticleFlag = true;
		}

		size = { 1.0f,1.0f };
		++moveTimer;
	}

	return false;
}
