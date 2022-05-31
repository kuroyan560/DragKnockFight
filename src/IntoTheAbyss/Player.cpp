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
#include "AfterImage.h"
#include "DrawFunc.h"
#include "Stamina.h"

#include"CharacterAIData.h"

Vec2<float> Player::GetGeneratePos()
{
	return WinApp::Instance()->GetExpandWinCenter() * Vec2<float>(1.0f, 0.25f);
}

static const float EXT_RATE = 0.6f;	//Player's expand rate used in Draw().
static const Vec2<float> PLAYER_HIT_SIZE = { (80 * EXT_RATE) / 2.0f,(80 * EXT_RATE) / 2.0f };			// �v���C���[�̃T�C�Y
Player::Player(const PLAYABLE_CHARACTER_NAME& CharacterName, const WHICH_TEAM& Team)
	:CharacterInterFace(PLAYER_HIT_SIZE), controllerIdx((int)Team), tutorial(Team)
{
	/*====== �R���X�g���N�^ =====*/
	if (PLAYER_CHARACTER_NUM <= CharacterName)assert(0);
	static const std::string PLAYER_DIR = "resource/ChainCombat/player/";
	static const std::string NAME_DIR[PLAYER_CHARACTER_NUM] =
	{
		"luna",
		"lacy"
	};
	const int AIM_GRAPH = TexHandleMgr::LoadGraph(PLAYER_DIR + NAME_DIR[CharacterName] + "/aim.png");

	// �摜�����[�h
	//playerGraph = TexHandleMgr::LoadGraph("resource/IntoTheAbyss/Player.png");

	shotSE = AudioApp::Instance()->LoadAudio("resource/ChainCombat/sound/shot.wav", 0.2f);

	bulletGraph = TexHandleMgr::LoadGraph(PLAYER_DIR + NAME_DIR[CharacterName] + "/bullet.png");

	playerPilotGraph[0] = TexHandleMgr::LoadGraph(PLAYER_DIR + NAME_DIR[CharacterName] + "/pilot.png");
	playerPilotGraph[1] = TexHandleMgr::LoadGraph(PLAYER_DIR + NAME_DIR[CharacterName] + "/pilot_back.png");
	pilotGraph = playerPilotGraph[0];

	std::vector<Anim>animations;
	animations.resize(PLAYER_ANIM_NUM);
	static const int DEFAULT_FRONT_NUM = 1;
	animations[DEFAULT_FRONT].graph.resize(DEFAULT_FRONT_NUM);
	//TexHandleMgr::LoadDivGraph("resource/IntoTheAbyss/Player/on_ground_wait.png", ON_GROUND_WAIT_NUM, { ON_GROUND_WAIT_NUM,1 }, animations[ON_GROUND_WAIT].graph.data());
	animations[DEFAULT_FRONT].graph[0] = TexHandleMgr::LoadGraph(PLAYER_DIR + NAME_DIR[CharacterName] + "/player.png");
	animations[DEFAULT_FRONT].interval = 10;
	animations[DEFAULT_FRONT].loop = true;

	static const int DEFAULT_BACK_NUM = 1;
	animations[DEFAULT_BACK].graph.resize(DEFAULT_BACK_NUM);
	animations[DEFAULT_BACK].graph[0] = TexHandleMgr::LoadGraph(PLAYER_DIR + NAME_DIR[CharacterName] + "/player_back.png");
	animations[DEFAULT_BACK].interval = 10;
	animations[DEFAULT_BACK].loop = true;

	static const int PULL_FRONT_NUM = 3;
	animations[PULL_FRONT].graph.resize(PULL_FRONT_NUM);
	TexHandleMgr::LoadDivGraph(PLAYER_DIR + NAME_DIR[CharacterName] + "/pull_front.png", PULL_FRONT_NUM, { PULL_FRONT_NUM,1 }, animations[PULL_FRONT].graph.data());
	animations[PULL_FRONT].interval = 6;
	animations[PULL_FRONT].loop = true;

	static const int PULL_BACK_NUM = 3;
	animations[PULL_BACK].graph.resize(PULL_FRONT_NUM);
	TexHandleMgr::LoadDivGraph(PLAYER_DIR + NAME_DIR[CharacterName] + "/pull_back.png", PULL_BACK_NUM, { PULL_BACK_NUM,1 }, animations[PULL_BACK].graph.data());
	animations[PULL_BACK].interval = 6;
	animations[PULL_BACK].loop = true;

	static const int HOLD_NUM = 5;
	animations[HOLD].graph.resize(HOLD_NUM);
	TexHandleMgr::LoadDivGraph(PLAYER_DIR + NAME_DIR[CharacterName] + "/hold.png", HOLD_NUM, { HOLD_NUM,1 }, animations[HOLD].graph.data());
	animations[HOLD].interval = 3;
	animations[HOLD].loop = false;

	static const int SWINGED_NUM = 1;
	animations[SWINGED].graph.resize(SWINGED_NUM);
	animations[SWINGED].graph[0] = TexHandleMgr::LoadGraph(PLAYER_DIR + NAME_DIR[CharacterName] + "/swinged.png");
	animations[SWINGED].interval = 10;
	animations[SWINGED].loop = true;

	static const int TIRED_NUM = 3;
	animations[TIRED].graph.resize(TIRED_NUM);
	TexHandleMgr::LoadDivGraph(PLAYER_DIR + NAME_DIR[CharacterName] + "/tired.png", TIRED_NUM, { TIRED_NUM,1 }, animations[TIRED].graph.data());
	animations[TIRED].interval = 6;
	animations[TIRED].loop = true;

	static const int KNOCK_OUT_NUM = 1;
	animations[KNOCK_OUT].graph.resize(KNOCK_OUT_NUM);
	animations[KNOCK_OUT].graph[0] = TexHandleMgr::LoadGraph(PLAYER_DIR + NAME_DIR[CharacterName] + "/swinged.png");
	animations[KNOCK_OUT].interval = 10;
	animations[KNOCK_OUT].loop = true;

	static const int NON_PILOT_NUM = 1;
	animations[NON_PILOT].graph.resize(NON_PILOT_NUM);
	animations[NON_PILOT].graph[0] = TexHandleMgr::LoadGraph(PLAYER_DIR + NAME_DIR[CharacterName] + "/nonPilot.png");
	animations[NON_PILOT].interval = 10;
	animations[NON_PILOT].loop = true;

	anim = std::make_shared<PlayerAnimation>(animations);
}

Player::~Player()
{
}

void Player::OnInit()
{

	/*===== ���������� =====*/

	// ���ݐU��񂹂�ő吔���擾�B
	maxStrongSwingCount = StageMgr::Instance()->GetSwingCount(SelectStage::Instance()->GetStageNum(), SelectStage::Instance()->GetRoomNum());
	nowStrongSwingCount = 0;

	strongSwingTutorialUI.Init();

	//�v���C���[�̌���������
	//playerDir = DEFAULT;

	//�A�j���[�V����������
	anim->Init(DEFAULT_FRONT);
	tiredTimer = TIRED_DRAW_TIME;

	// �r���Z�b�g
	static const float OFFSET = -8.0f;

	// �A�˃^�C�}�[��������
	rapidFireTimerLeft = 0;
	rapidFireTimerRight = 0;

	//�X�g���b�`������
	stretch_LU = { 0.0f,0.0f };
	stretch_RB = { 0.0f,0.0f };
	stretchTimer = STRETCH_RETURN_TIME;

	drawCursorFlag = true;

	swingCoolTime = 0;

	sizeVel = 120.0f;

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

	autoPilotMove = { 0,0 };

	CWSwingSegmentMgr.Init();
	CCWSwingSegmentMgr.Init();

	pilotGraph = playerPilotGraph[0];

	swingGauge.Init(100);

	consecutiveSwingTimer = 0;
	DebugParameter::Instance()->useFinishSwingFlag = true;

	strongSwingUI.clear();
	strongSwingUI.resize(maxStrongSwingCount);
}

void Player::OnUpdate(const MapChipArray& MapData)
{
	//�f�o�b�N�p�̒l�ύX
	std::shared_ptr<PlayerDebugParameterData> data = DebugParameter::Instance()->nowData;

	/*RECOIL_AMOUNT = data->RECOIL_AMOUNT;
	FIRST_RECOIL_AMOUNT = data->FIRST_RECOIL_AMOUNT;
	MAX_RECOIL_AMOUNT = data->MAX_RECOIL_AMOUNT;*/

	CharacterAIData::Instance()->playerData.dashStamina = DASH_STAMINA;
	CharacterAIData::Instance()->playerData.swingStamina = SWING_STAMINA;

	// ���肪�U��񂵂Ă�����A�W���X�g�L�����Z���_�b�V���p�̐����̈ʒu���X�V����B
	if (partner.lock()->GetNowSwing()) {

		const float LINE_LENGTH = 400.0f;

		// �O�t���[���Ƃ̈ʒu�֌W����A������L�΂��ׂ����������߂�B
		Vec2<float> dir = prevPos - prevPrevPos;
		dir.Normalize();

		justCancelDashStartPos = pos;
		justCancelDashEndPos = pos + dir * LINE_LENGTH;

	}
	else {

		justCancelDashEndPos = { -10000,-100000 };
		justCancelDashStartPos = { -20000,-20000 };

	}


	/*===== ���͏��� =====*/
	// ���͂Ɋւ���X�V�������s���B
	if (GetCanMove())Input(MapData);

	/*===== �X�V���� =====*/

	swingGauge.Update(pos);




	// ��s���̓^�C�}�[���X�V�B
	if (isAdvancedEntrySwing) {

		--advancedEntrySwingTimer;
		if (advancedEntrySwingTimer < 0) isAdvancedEntrySwing = false;

	}

	// �������U��񂵂����Ă�����B
	if (partner.lock()->GetNowSwing()) {

		// �c����ۑ��B
		Vec2<float> extRate = ((GetPlayerGraphSize() - stretch_LU + stretch_RB) / GetPlayerGraphSize()) * ScrollMgr::Instance()->zoom * EXT_RATE * stagingDevice.GetExtRate();
		AfterImageMgr::Instance()->Generate(pos, extRate, stagingDevice.GetSpinRadian(), anim->GetGraphHandle(), charaColor);

	}

	// �������U��񂵂����Ă�����X�V�������s��Ȃ��B
	if (!(!partner.lock()->GetNowSwing() && !nowSwing)) return;

	//�ړ��Ɋւ��鏈��
	Move();

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
			anim->ChangeAnim(DEFAULT_FRONT);
		}
	}

	// �O���b�v�̓^�C�}�[��0�ɂȂ�����A���S�ɉ񕜂���܂œ��񒣂�Ȃ��悤�ɂ���B
	/*if (gripPowerTimer <= 0) {

		isGripPowerEmpty = true;
		anim->ChangeAnim(TIRED);
	}*/

	//�_�b�V���̎c��
	if (dashAftImgTimer)
	{
		Vec2<float> extRate = ((GetPlayerGraphSize() - stretch_LU + stretch_RB) / GetPlayerGraphSize()) * ScrollMgr::Instance()->zoom * EXT_RATE * stagingDevice.GetExtRate();
		AfterImageMgr::Instance()->Generate(pos, extRate, 0.0f, anim->GetGraphHandle(), GetTeamColor());
		dashAftImgTimer--;
	}

	// �U��񂵂Ă��Ȃ�������
	if (!nowSwing) {

		if (anim->GetNowAnim() == HOLD)anim->ChangeAnim(DEFAULT_FRONT);

		// �R���ݏ��(���񒣂���)����������B
		isHold = false;

		//tutorial.lock()->SetRstickInput(false);

	}

}

void Player::OnUpdateNoRelatedSwing()
{
	// ���̓^�C�}�[���K��l�ɋ߂Â���B
	//if (!nowSwing && gripPowerTimer < MAX_GRIP_POWER_TIMER) {
	//	gripPowerTimer += SlowMgr::Instance()->slowAmount;

	//	// �ő�l�ɂȂ����爬�͂��g���؂��Ă���񕜂��Ă����ԃt���O��܂�B
	//	if (MAX_GRIP_POWER_TIMER <= gripPowerTimer) {

	//		gripPowerTimer = MAX_GRIP_POWER_TIMER;
	//		isGripPowerEmpty = false;
	//		anim->ChangeAnim(DEFAULT_FRONT);
	//	}
	//}

	//�X�g���b�`�X�V
	UpdateStretch();

	//�A�j���[�V�����X�V
	anim->Update();

	if (tiredTimer < TIRED_DRAW_TIME)
	{
		tiredTimer++;
		if (TIRED_DRAW_TIME <= tiredTimer)anim->ChangeAnim(DEFAULT_FRONT);
	}

	if (staminaGauge->emptyTrigger)
	{
		tiredTimer = 0;
		anim->ChangeAnim(TIRED);
	}

}

#include"DistanceCounter.h"
#include"GameSceneCamerMove.h"
void Player::OnDraw(const bool& isRoundStartEffect)
{
	//if (vel.y < 0)playerDir = BACK;
	auto moveInput = UsersInput::Instance()->GetLeftStickVec(controllerIdx, { 0.5f,0.5f });

	if (!isHold && !anim->Compare({ SWINGED,TIRED,NON_PILOT,KNOCK_OUT }) && CompleteAppear())
	{
		if (moveInput.x)
		{
			if (playerDirY == PLAYER_BACK)anim->ChangeAnim(PULL_BACK);
			else anim->ChangeAnim(PULL_FRONT);
		}
		else
		{
			if (playerDirY == PLAYER_BACK)anim->ChangeAnim(DEFAULT_BACK);
			else anim->ChangeAnim(DEFAULT_FRONT);
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


	//swingGauge.Draw();

	//muffler.Draw(LigManager);

	// �f�o�b�O�p�ŐU��񂵃L�����Z�����C���̕`����s���B
	//DrawFunc::DrawLine2D(ScrollMgr::Instance()->Affect(justCancelDashStartPos), ScrollMgr::Instance()->Affect(justCancelDashEndPos), Color());

	//rHand->Draw(EXT_RATE, DEF_RIGHT_HAND_ANGLE, { 0.0f,0.0f }, drawCursorFlag);
	//lHand->Draw(EXT_RATE, DEF_LEFT_HAND_ANGLE, { 1.0f,0.0f }, drawCursorFlag);

	//�G������������ʖڂȔ͈�

	//�X�g���b�`���Z
	//leftUp += stretch_LU;
	//rightBottom += stretch_RB;
	static auto CRASH_TEX = D3D12App::Instance()->GenerateTextureBuffer(Color(255, 0, 0, 255));
	const Vec2<float>drawPos = ScrollMgr::Instance()->Affect(pos + stagingDevice.GetShake());
	//����
	auto bodyTex = TexHandleMgr::GetTexBuffer(anim->GetGraphHandle());
	const Vec2<float> expRateBody = ((GetPlayerGraphSize() - stretch_LU + stretch_RB) / GetPlayerGraphSize());
	bool mirorX = playerDirX == PLAYER_RIGHT || (isHold && (partner.lock()->pos - pos).x < 0);

	//float alphaRate = alpha / 255.0f;
	float alphaRate = 1.0f;
	DrawFunc::DrawCircle2D(drawPos + GameSceneCameraMove::Instance()->move, (DistanceCounter::Instance()->DEAD_LINE - 80.0f) * ScrollMgr::Instance()->zoom * alphaRate, Color(8, 217, 255, 130), true, 1, AlphaBlendMode_Trans);
	DrawFunc_FillTex::DrawRotaGraph2D(drawPos + GameSceneCameraMove::Instance()->move, expRateBody * ScrollMgr::Instance()->zoom * EXT_RATE * stagingDevice.GetExtRate() * staminaGauge->outOfStaminaEffect.GetSize() * appearExtRate * alphaRate,
		stagingDevice.GetSpinRadian(), bodyTex, CRASH_TEX, stagingDevice.GetFlashAlpha(), { 0.5f,0.5f }, { mirorX,false });

}

#include "GameSceneCamerMove.h"

void Player::OnDrawUI()
{
	static const int LINE_ALPHA = 120;
	static std::shared_ptr<TextureBuffer> COLOR_TEX[TEAM_NUM] =
	{
		 D3D12App::Instance()->GenerateTextureBuffer(Color(47,255,139,LINE_ALPHA)),
		 D3D12App::Instance()->GenerateTextureBuffer(Color(239,1,144,LINE_ALPHA))
	};
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
		DrawFunc::DrawLine2DGraph(ScrollMgr::Instance()->Affect(pos) + GameSceneCameraMove::Instance()->move, ScrollMgr::Instance()->Affect(target), COLOR_TEX[GetWhichTeam()], 2, AlphaBlendMode_Trans);
		//DrawFunc::DrawRotaGraph2D(ScrollMgr::Instance()->Affect(target), drawScale * 0.8f, 0.0f, TexHandleMgr::GetTexBuffer(RETICLE_GRAPH[team]));

		//�U��񂵕����`��
		bool clockWise = 0 < rightStickVec.Cross(partner.lock()->pos - pos);
		Angle arrowPosAngle = KuroFunc::GetAngle(pos, partner.lock()->pos);
		Angle rotateAngle = arrowPosAngle + Angle(90 * (clockWise ? -1 : 1));
		Vec2<float>vec = partner.lock()->pos - pos;
		vec.Normalize();
		DrawFunc::DrawRotaGraph2D(ScrollMgr::Instance()->Affect(partner.lock()->pos + vec * ARROW_DIST_OFFSET) + GameSceneCameraMove::Instance()->move, drawScale * 0.5f, rotateAngle, TexHandleMgr::GetTexBuffer(ARROW_GRAPH[team]), Color(), { 0.0f,0.5f });
	}

	const auto leftStickVec = UsersInput::Instance()->GetLeftStickVec(controllerIdx, { 0.5f,0.5f });
	//const auto leftTrigger = UsersInput::Instance()->ControllerInput(controllerIdx, XBOX_BUTTON::LT);
	//const auto rightTrigger = UsersInput::Instance()->ControllerInput(controllerIdx, XBOX_BUTTON::RT);
	const auto leftButton = UsersInput::Instance()->ControllerInput(controllerIdx, XBOX_BUTTON::LB);
	const auto rightButton = UsersInput::Instance()->ControllerInput(controllerIdx, XBOX_BUTTON::RB);

	//LT�`���[�g���A��
	for (int i = 0; i < maxStrongSwingCount; ++i)
	{
		strongSwingUI[i].Draw(i);
	}
	strongSwingTutorialUI.Draw(nowStrongSwingCount < maxStrongSwingCount);
}

void Player::OnHitMapChip(const HIT_DIR& Dir)
{
}
void Player::OnPilotControl()
{
	const auto rightStickVec = UsersInput::Instance()->GetRightStickVec(controllerIdx, { 0.9f,0.9f });
	//static const float PILOT_SPEED = 18.0f;
	//auto move = rightStickVec * PILOT_SPEED;
	//pilotPos += move;
	const float PILOT_ACCEL_SPEED = 4.0f;
	//const auto accel = rightStickVec * PILOT_ACCEL_SPEED;
	//pilotVel += accel;
	pilotAccel = KuroMath::Lerp(pilotAccel, rightStickVec * PILOT_ACCEL_SPEED, 0.3f);
	pilotVel += pilotAccel;
	pilotPos += pilotVel;

	//��C��R�Ō���
	pilotVel = KuroMath::Lerp(pilotVel, { 0,0 }, 0.1f);

	if (rightStickVec.x < 0)pilotDrawMiror = false;
	if (0 < rightStickVec.x)pilotDrawMiror = true;

	if (rightStickVec.y < 0)pilotGraph = playerPilotGraph[1];	//������
	if (0 < rightStickVec.y)pilotGraph = playerPilotGraph[0];	//�O����
}

void Player::Input(const MapChipArray& MapData)
{
	/*===== ���͏��� =====*/

	// �X�^�����o������������͂��󂯕t���Ȃ��B
	if (StunEffect::Instance()->isActive) return;

	// ���͂��󂯕t���Ȃ��^�C�}�[��0���傫�������珈�����΂��B
	if (0 < inputInvalidTimerByCrash) return;

	// �ǂɋ��܂��Ĕ��肪����������Ă���Ԃ͏������󂯕t���Ȃ��B
	if (0 < GetStackWinTimer()) return;

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

	// �X�e�B�b�N�����͂���Ă��Ȃ�������A���ŐU��񂵂��邽�߂̃^�C�}�[��ݒ肷��B
	if (!isInputRightStick) {
		consecutiveSwingTimer = 0;
	}
	else {
		++consecutiveSwingTimer;
	}

	// ���͂̃f�b�h���C����݂���B
	float inputRate = inputLeftVec.Length();
	if (inputRate >= 0.5f) {

		// �ړ����󂯕t����B
		vel.x = inputLeftVec.x * (MOVE_SPEED_PLAYER * inputRate);
		vel.y = inputLeftVec.y * (MOVE_SPEED_PLAYER * inputRate);
		autoPilotMove = vel;

		if (inputLeftVec.x < 0)playerDirX = PLAYER_LEFT;
		else if (0 < inputLeftVec.x)playerDirX = PLAYER_RIGHT;
		if (inputLeftVec.y < 0)playerDirY = PLAYER_BACK;
		else if (0 < inputLeftVec.y)playerDirY = PLAYER_FRONT;
	}
	/* ���I�[�g�p�C���b�g���Â炭�������̂ŁA��U�R�����g�A�E�g
	//�I�[�g�p�C���b�g
	else if (IsPilotOutSide())
	{
		static const float AUTO_PILOT_SPEED = 10.0f;
		vel = autoPilotMove.GetNormal() * AUTO_PILOT_SPEED;

		if (autoPilotMove.x < 0)playerDirX = PLAYER_LEFT;
		else if (0 < autoPilotMove.x)playerDirX = PLAYER_RIGHT;
		if (autoPilotMove.y < 0)playerDirY = PLAYER_BACK;
		else if (0 < autoPilotMove.y)playerDirY = PLAYER_FRONT;
	}
	*/

	// ���͂��󂯕t����ϐ�
	bool isInputLB = UsersInput::Instance()->ControllerInput(controllerIdx, XBOX_BUTTON::LB);
	bool isInputRB = UsersInput::Instance()->ControllerOnTrigger(controllerIdx, XBOX_BUTTON::RB);

	//�`���[�g���A���̕\�� / ��\��
	if (UsersInput::Instance()->ControllerOnTrigger(controllerIdx, XBOX_BUTTON::BACK))
	{
		tutorial.TurnActive();
	}

	// �j�󃂁[�h���̃t���O��ۑ��B
	isPrevDestroyMode = isDestroyMode;
	if (maxStrongSwingCount && UsersInput::Instance()->ControllerOnTrigger(controllerIdx, XBOX_BUTTON::LT) && nowStrongSwingCount < maxStrongSwingCount) {

		isDestroyMode = true;
	}
	if (UsersInput::Instance()->ControllerOffTrigger(controllerIdx, XBOX_BUTTON::LT)) {
		isDestroyMode = false;
	}


	// �����Ƃ̈ʒu�֌W�ɂ����Ă̋t�x�N�g���ɐU��񂷁B
	Vec2<float> subPos = pos - partner.lock()->pos;
	swingVec = (subPos).GetNormal();

	// RT�������ꂽ��
	bool canSwing = (!isInputRightStick && isPrevInputRightStick) || (isInputRightStick && CONSECUTIVE_SWING_TIMER < consecutiveSwingTimer);
	if ((!nowSwing && canSwing)) {

		// �U��񂵂̏���

		if (CONSECUTIVE_SWING_TIMER <= consecutiveSwingTimer) {

			consecutiveSwingTimer = 0;

		}

		// �O�ς̍��E����ɂ��A���v��肩�����v���Ƃ��擾����B���̒l�����A���̒l���E�B
		Vec2<float> posBuff = partner.lock()->pos - pos;
		posBuff.Normalize();
		bool isClockWise = 0 < posBuff.Cross(prevInputRightStick);

		SwingPartner({ swingVec }, isClockWise);

		if (swingGauge.canUseGague() && UsersInput::Instance()->ControllerInput(controllerIdx, XBOX_BUTTON::LT))
		{
			swingGauge.Use();
			DebugParameter::Instance()->useFinishSwingFlag = false;
		}

		isInputSwingRB = isInputRB;

		// �ђʐU��񂵏�Ԃ�������A�ђʐU��񂵏�Ԃ̃J�E���g�𑝂₷�B
		if (isDestroyMode) {
			++nowStrongSwingCount;

			if (nowStrongSwingCount <= maxStrongSwingCount)
			{
				strongSwingUI[maxStrongSwingCount - nowStrongSwingCount].Disappear();
			}
		}

		//�L�����N�^�[AI�p�̃f�[�^�W��
		CharacterAIData::Instance()->swingFlag = true;
	}

	if (!GetNowSwing())
	{
		DebugParameter::Instance()->useFinishSwingFlag = true;
	}



	static const int DASH_SE = AudioApp::Instance()->LoadAudio("resource/ChainCombat/sound/dash.wav", 0.4f);

	// ���͂̃f�b�h���C����݂���B
	inputLeftVec = UsersInput::Instance()->GetLeftStickVecFuna(controllerIdx);
	inputLeftVec /= {32768.0f, 32768.0f};
	inputRate = inputLeftVec.Length();
	if (isInputLB && !isPrevLeftBottom && 0.5f <= inputRate) {

		//AudioApp::Instance()->PlayWave(DASH_SE);

		// inputVec = �Ђ���X�e�B�b�N�̓��͕���
		const float DASH_SPEED = 30.0f;
		const float CANCEL_DASH_SPEED = 35.0f;
		const float JUST_CANCEL_DASH_SPEED = 40.0f;
		float speed = 0;

		speed = DASH_SPEED;

		//}

		vel += inputLeftVec * speed;


		// �ړ��ʂ����E�𒴂��Ȃ��悤�ɂ���B
		if (speed < vel.Length()) {

			vel.Normalize();
			vel *= speed;

		}

		//��
		//ParticleMgr::Instance()->Generate(pos, -inputLeftVec, BULLET);
		//�c��
		//dashAftImgTimer = 10;

		// �X�^�~�i������
		//staminaGauge->ConsumesStamina(DASH_STAMINA);

		CharacterAIData::Instance()->dashFlag = true;
	}
	else
	{
		CharacterAIData::Instance()->dashFlag = false;
	}

	// �E�X�e�B�b�N�����͂���Ă�����A�\�������o���B
	if (isInputRightStick) {

		// ���v��肩�ǂ����B���̒l�����A���̒l���E�B
		inputRightVec.Normalize();
		Vec2<float> posBuff = partner.lock()->pos - pos;
		posBuff.Normalize();
		bool isClockWise = 0 < posBuff.Cross(inputRightVec);

		prevSwingFlag = true;

		if (isClockWise) {

			CWSwingSegmentMgr.Update(pos, Vec2<float>(partner.lock()->pos - pos).GetNormal(), Vec2<float>(pos - partner.lock()->pos).Length(), MapData, !isDestroyMode);
			CWSwingSegmentMgr.SetSwingStartPos(partner.lock()->pos);
			CCWSwingSegmentMgr.Init();

		}
		else {

			CCWSwingSegmentMgr.Update(pos, Vec2<float>(partner.lock()->pos - pos).GetNormal(), Vec2<float>(pos - partner.lock()->pos).Length(), MapData, !isDestroyMode);
			CCWSwingSegmentMgr.SetSwingStartPos(partner.lock()->pos);
			CWSwingSegmentMgr.Init();

		}

	}
	else
	{
		if (!IsPilotOutSide())
		{
			//�p�C���b�g�؂藣��
			if (UsersInput::Instance()->ControllerOnTrigger(controllerIdx, XBOX_BUTTON::RB))SetPilotDetachedFlg(true);
		}

		prevSwingFlag = false;
	}
	if (UsersInput::Instance()->ControllerOffTrigger(controllerIdx, XBOX_BUTTON::RB))SetPilotDetachedFlg(false);



	// ���͂���Ă��Ȃ��āA�X�C���O������Ȃ�������\�����������B
	if ((inputRightVec.Length() <= 0.5f && !nowSwing) || IsPilotOutSide()) {

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


	// RT��������Ă��đ�����~�߂��ԂɂȂ��Ă��邩��ۑ��B
	//isPrevStopPartner = isStopPartner;
	isStopPartner = false;

	//bool RTInput = UsersInput::Instance()->ControllerInput(controllerIdx, XBOX_BUTTON::RT);
	//if (RTInput) {


	//	isStopPartner = true;

	//}


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
	//bulletMgr.Generate(bulletGraph, GeneratePos, ForwardAngle, BULLET_SPEED);
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
	return { (anim->GetGraphSize().x * EXT_RATE) / 2.0f,(anim->GetGraphSize().y * EXT_RATE) / 2.0f };			// �v���C���[�̃T�C�Y
}

void Player::CheckHitMapChipVel(const Vec2<float>& checkPos, const vector<vector<int>>& MapData)
{
	// �}�b�v�`�b�v�ƃv���C���[�̓����蔻�� ��΂Ɋђʂ����Ȃ��ׂ̏���
	//Vec2<float> upperPlayerPos = pos - Vec2<float>(0, PLAYER_HIT_SIZE.y / 2.0f);
	//Vec2<float> upperPlayerPosBuff = upperPlayerPos;
	//INTERSECTED_LINE intersectedLine = {};

	//// �ʏ펞�̓����蔻��
	//Vec2<float> moveDir = prevPos - pos;
	//moveDir.Normalize();
	////intersectedLine = (INTERSECTED_LINE)MapChipCollider::Instance()->CheckHitMapChipBasedOnTheVel(pos, prevFrameCenterPos, vel + gimmickVel, Vec2<float>(PLAYER_HIT_SIZE.x, PLAYER_HIT_SIZE.y), onGround, MapData);
	//intersectedLine = (INTERSECTED_LINE)MapChipCollider::Instance()->CheckHitMapChipBasedOnTheVel(pos, prevPos, moveDir * Vec2<float>(PLAYER_HIT_SIZE.x, PLAYER_HIT_SIZE.y), Vec2<float>(PLAYER_HIT_SIZE.x, PLAYER_HIT_SIZE.y / 2.0f), MapData);


	//// ����(�����߂��ꂽ�l�����W�������)
	////pos += upperPlayerPos - upperPlayerPosBuff;

	//// �����ƃv���C���[�̊p�x
	//auto partnerPos = GetPartnerPos();
	//float bossAngle = atan2(partnerPos.y - pos.y, partnerPos.x - pos.x);
	//bossAngle = fabs(bossAngle);

	//// ���������ʒu�ɉ����ď����𕪂���B
	//if (intersectedLine == INTERSECTED_TOP) {

	//	// ���ɓ��������ꍇ
	//	mapChipHit[BOTTOM] = true;

	//	// �}�b�v�`�b�v�̏�ɂ������Ă����Ƃ������Ƃ́A�{�X���������ɂ����OK�I
	//	// �������̋�̓I�Ȓl��
	//	const float MIN_ANGLE = 0.785398f;
	//	const float MAX_ANGLE = 2.35619f;

	//	// �p�x�����̒l�̊Ԃ�������
	//	if (MIN_ANGLE <= bossAngle && bossAngle <= MAX_ANGLE) {

	//		// �����������Ă���B
	//		stackMapChip = true;

	//	}

	//}
	//else if (intersectedLine == INTERSECTED_RIGHT) {

	//	// ���ɓ��������ꍇ
	//	mapChipHit[LEFT] = true;

	//	// �}�b�v�`�b�v�̉E�ɂ������Ă����Ƃ������Ƃ́A�{�X���������ɂ����OK�I
	//	// �������̋�̓I�Ȓl��
	//	const float MIN_ANGLE = 2.35619f;
	//	const float MAX_ANGLE = 3.92699f;

	//	// �p�x�����̒l�̊Ԃ�������
	//	if (MIN_ANGLE <= bossAngle && bossAngle <= MAX_ANGLE) {

	//		// �����������Ă���B
	//		stackMapChip = true;
	//	}

	//}
	//else if (intersectedLine == INTERSECTED_BOTTOM) {

	//	// ��ɓ��������ꍇ
	//	mapChipHit[TOP] = true;

	//	// �}�b�v�`�b�v�̉��ɂ������Ă����Ƃ������Ƃ́A�v���C���[��������ɂ����OK�I
	//	// ������̋�̓I�Ȓl��
	//	const float MIN_ANGLE = 3.92699f;
	//	const float MAX_ANGLE = 5.49779f;

	//	// �p�x�����̒l�̊Ԃ�������
	//	if (MIN_ANGLE <= bossAngle && bossAngle <= MAX_ANGLE) {

	//		// �����������Ă���B
	//		stackMapChip = true;
	//	}

	//}
	//else if (intersectedLine == INTERSECTED_LEFT) {

	//	// �E�ɓ��������ꍇ
	//	mapChipHit[RIGHT] = true;

	//	// �}�b�v�`�b�v�̍��ɂ������Ă����Ƃ������Ƃ́A�v���C���[���E�����ɂ����OK�I
	//	// �E�����̋�̓I�Ȓl��
	//	const float MIN_ANGLE = 0.785398f;
	//	const float MAX_ANGLE = 5.49779f;

	//	// �p�x�����̒l�̊Ԃ�������
	//	if (MAX_ANGLE <= bossAngle || bossAngle <= MIN_ANGLE) {

	//		// �����������Ă���B
	//		stackMapChip = true;
	//	}

	//}
	//else if (intersectedLine == INTERSECTED_RISE) {

	//	// �̂��オ��̏ꏊ�ɓ��������ꍇ
	//	vel.y *= 0.5f;
	//}

}

bool Player::CheckHitMapChip(const Vec2<float>& StartPos, const Vec2<float>& EndPos)
{
	//�ǂ�����Ďg����
	Vec2<float>handSegmentStart(StartPos), handSegmentEnd(EndPos);//����
	Vec2<float>handSegmentDir(EndPos - StartPos);					//�����̕���
	Vec2<float>handPos(StartPos);									//�����̎n�_
	Vec2<float>sightPos;						//���߂�ꂽ��_�̒��̍ŒZ����
	MapChipArray mapData = *StageMgr::Instance()->GetLocalMap();					//�}�b�v
	//�ǂ�����Ďg����


	// �ŒZ������ۑ�����悤�̔z��B
	std::vector<std::pair<Vec2<float>, float>> shortestPoints;

	// �Ə��̃��C�̕����ɂ���ē����蔻��𖳌�������ׂ̃t���O���Z�b�g����B
	bool isTop = handSegmentDir.y < 0;
	bool isLeft = handSegmentDir.x < 0;

	SizeData mapChipSizeData = StageMgr::Instance()->GetMapChipSizeData(MAPCHIP_TYPE_STATIC_BLOCK);

	// ���Ƀ}�b�v�`�b�v�Ƃ̍ŒZ���������߂�B
	const int MAP_Y = mapData.size();
	for (int height = 0; height < MAP_Y; ++height) {

		const int MAP_X = mapData[height].size();
		for (int width = 0; width < MAP_X; ++width) {

			// ���̃}�b�v�`�b�v��1~9�ȊO�������画����΂��B
			if (mapData[height][width].chipType < mapChipSizeData.min ||
				mapChipSizeData.max < mapData[height][width].chipType)
			{
				continue;
			}

			// ���̃C���f�b�N�X�̃u���b�N�̍��W���擾�B
			const Vec2<float> BLOCK_POS = Vec2<float>(width * MAP_CHIP_SIZE, height * MAP_CHIP_SIZE);

			Vec2<int> windowSize = WinApp::Instance()->GetWinCenter();

			// ���͈͈ȊO�������珈�����΂��B
			bool checkInsideTop = BLOCK_POS.y < handPos.y - windowSize.y;
			bool checkInsideBottom = handPos.y + windowSize.y > BLOCK_POS.y;
			bool checkInsideLeft = BLOCK_POS.x < handPos.x + windowSize.x;
			bool checkInsideRight = handPos.x + windowSize.x > BLOCK_POS.x;
			if (checkInsideTop && checkInsideBottom && checkInsideLeft && checkInsideRight) {
				//player.onGround = false;
				continue;
			}

			// ���̃u���b�N�������Ă���u���b�N�������珈�����΂��B
			Vec2<int> mapChipIndex = { width, height };
			if (StageMgr::Instance()->IsItWallIn(SelectStage::Instance()->GetStageNum(), SelectStage::Instance()->GetRoomNum(), mapChipIndex)) {

				continue;

			}

			// ���C�̕����ƃu���b�N�̈ʒu�֌W�ŏ������΂��B�E�B���h�E��4����������
			float offsetHandPos = MAP_CHIP_SIZE;
			if (isLeft && handPos.x + offsetHandPos < BLOCK_POS.x) continue;
			if (!isLeft && BLOCK_POS.x < handPos.x - offsetHandPos) continue;
			if (isTop && handPos.y + offsetHandPos < BLOCK_POS.y) continue;
			if (!isTop && BLOCK_POS.y < handPos.y - offsetHandPos) continue;

			// �����̎n�_�Ƃ̃}�b�v�`�b�v�̋������A���_�ƏI�_�̋������傫�������珈�����s��Ȃ��B
			float startEndDistance = (StartPos - EndPos).Length();
			float blockDistance = (StartPos - BLOCK_POS).Length();
			if (startEndDistance < blockDistance) continue;

			// �l�ӕ���_�����߂�B

			// ��_�ۑ��p
			vector<Vec2<float>> intersectedPos;

			// �����
			if (Collider::Instance()->IsIntersected(handSegmentStart, handSegmentEnd, Vec2<float>(BLOCK_POS.x - MAP_CHIP_HALF_SIZE, BLOCK_POS.y - MAP_CHIP_HALF_SIZE), Vec2<float>(BLOCK_POS.x + MAP_CHIP_HALF_SIZE, BLOCK_POS.y - MAP_CHIP_HALF_SIZE))) {

				// ��_�����߂ĕۑ�����B
				intersectedPos.push_back(Collider::Instance()->CalIntersectPoint(handSegmentStart, handSegmentEnd, Vec2<float>(BLOCK_POS.x - MAP_CHIP_HALF_SIZE, BLOCK_POS.y - MAP_CHIP_HALF_SIZE), Vec2<float>(BLOCK_POS.x + MAP_CHIP_HALF_SIZE, BLOCK_POS.y - MAP_CHIP_HALF_SIZE)));

			}
			// �E����
			if (Collider::Instance()->IsIntersected(handSegmentStart, handSegmentEnd, Vec2<float>(BLOCK_POS.x + MAP_CHIP_HALF_SIZE, BLOCK_POS.y - MAP_CHIP_HALF_SIZE), Vec2<float>(BLOCK_POS.x + MAP_CHIP_HALF_SIZE, BLOCK_POS.y + MAP_CHIP_HALF_SIZE))) {

				// ��_�����߂ĕۑ�����B
				intersectedPos.push_back(Collider::Instance()->CalIntersectPoint(handSegmentStart, handSegmentEnd, Vec2<float>(BLOCK_POS.x + MAP_CHIP_HALF_SIZE, BLOCK_POS.y - MAP_CHIP_HALF_SIZE), Vec2<float>(BLOCK_POS.x + MAP_CHIP_HALF_SIZE, BLOCK_POS.y + MAP_CHIP_HALF_SIZE)));

			}
			// ������
			if (Collider::Instance()->IsIntersected(handSegmentStart, handSegmentEnd, Vec2<float>(BLOCK_POS.x - MAP_CHIP_HALF_SIZE, BLOCK_POS.y + MAP_CHIP_HALF_SIZE), Vec2<float>(BLOCK_POS.x + MAP_CHIP_HALF_SIZE, BLOCK_POS.y + MAP_CHIP_HALF_SIZE))) {

				// ��_�����߂ĕۑ�����B
				intersectedPos.push_back(Collider::Instance()->CalIntersectPoint(handSegmentStart, handSegmentEnd, Vec2<float>(BLOCK_POS.x - MAP_CHIP_HALF_SIZE, BLOCK_POS.y + MAP_CHIP_HALF_SIZE), Vec2<float>(BLOCK_POS.x + MAP_CHIP_HALF_SIZE, BLOCK_POS.y + MAP_CHIP_HALF_SIZE)));

			}
			// ������
			if (Collider::Instance()->IsIntersected(handSegmentStart, handSegmentEnd, Vec2<float>(BLOCK_POS.x - MAP_CHIP_HALF_SIZE, BLOCK_POS.y - MAP_CHIP_HALF_SIZE), Vec2<float>(BLOCK_POS.x - MAP_CHIP_HALF_SIZE, BLOCK_POS.y + MAP_CHIP_HALF_SIZE))) {

				// ��_�����߂ĕۑ�����B
				intersectedPos.push_back(Collider::Instance()->CalIntersectPoint(handSegmentStart, handSegmentEnd, Vec2<float>(BLOCK_POS.x - MAP_CHIP_HALF_SIZE, BLOCK_POS.y - MAP_CHIP_HALF_SIZE), Vec2<float>(BLOCK_POS.x - MAP_CHIP_HALF_SIZE, BLOCK_POS.y + MAP_CHIP_HALF_SIZE)));

			}

			// �ŒZ���������߂�B
			Vec2<float> shortestPos = {};
			float shoterstLength = 1000000.0f;

			// �T�C�Y��0�������珈�����΂��B
			const int INTERSECTED_COUNT = intersectedPos.size();
			if (INTERSECTED_COUNT <= 0) continue;

			// �ŒZ���������߂�B
			for (int index = 0; index < INTERSECTED_COUNT; ++index) {

				// �ۑ�����Ă���f�[�^���傫�������珈�����΂��B
				float lengthBuff = Vec2<float>(intersectedPos[index] - handPos).Length();
				if (lengthBuff >= shoterstLength) continue;

				// �f�[�^��ۑ�����B
				shoterstLength = lengthBuff;
				shortestPos = intersectedPos[index];

			}

			// �ŒZ�̋�����ۑ�����B
			pair<Vec2<float>, float> buff = { shortestPos, shoterstLength };
			shortestPoints.push_back(buff);
		}
	}


	/*-- �����܂ł̉ߒ��ŗl�X�ȍŒZ�����߂邱�Ƃ��ł����B --*/

	// �ŒZ�̍��W��ۑ�����p�ϐ��B
	float shortestLength = 100000.0f;

	// �S�Ă̍ŒZ�̒�����ł��Z�����̂����߂�B
	const int SHORTEST_COUNT = shortestPoints.size();

	// �T�C�Y��0��������Ə����ǂ����ɔ�΂��ă��^�[���B
	if (SHORTEST_COUNT <= 0) {

		sightPos = { -100,-100 };
		return false;
	}

	for (int index = 0; index < SHORTEST_COUNT; ++index) {

		// �ۑ�����Ă���f�[�^���傫�������珈�����΂��B
		if (shortestPoints[index].second >= shortestLength) continue;

		// �f�[�^��ۑ�����B
		shortestLength = shortestPoints[index].second;
		sightPos = shortestPoints[index].first;
	}


	//�ŒZ��������ł��Z�o���ꂽ�瓖���蔻����o��
	return true;

}
