#include "StunEffect.h"
#include "Camera.h"
#include "SlowMgr.h"
#include "TexHandleMgr.h"
#include "ScrollMgr.h"
#include "DrawFunc.h"
#include "KuroMath.h"
#include "WinApp.h"
#include "FaceIcon.h"

StunEffect::StunEffect()
{

	/*===== �R���X�g���N�^ =====*/

	focusTime = 0;
	focusPos = {};
	isActive = false;

	enemyBreakHandle = TexHandleMgr::LoadGraph("resource/ChainCombat/UI/break_enemy.png");
	playerBreakHandle = TexHandleMgr::LoadGraph("resource/ChainCombat/UI/break_player.png");

}

void StunEffect::Init()
{

	/*===== ���������� =====*/

	focusTime = 0;
	focusPos = {};
	isActive = false;

}

void StunEffect::Activate(Vec2<float>& focusPos, const Vec2<float>& stunPos, const Vec2<float>& iconPos, const bool& isBoss)
{

	/*===== �������� =====*/

	//focusTime = FOCUS_TIME;
	this->focusPos = &focusPos;
	this->iconPos = iconPos;
	isActive = true;
	status = StunEffect::STATUS_GENERATE;
	breakUITimer = 0;
	breakUIRadian = DEF_RADIAN + DEF_RADIAN / BREAK_UI_GENERATE_TIMER;


	// ������stunPos�̓��[���h���W�n�Ȃ̂ŁA���[�J�����W�n�ɕϊ�����B
	breakUIPos = stunPos - ScrollMgr::Instance()->scrollAmount;
	breakUIPosDef = breakUIPos;

	this->isBoss = isBoss;
	if (isBoss) {
		useHandle = enemyBreakHandle;
	}
	else {
		useHandle = playerBreakHandle;
	}

}

void StunEffect::Update()
{

	/*===== �X�V���� =====*/

	// �L��������Ă��Ȃ������珈�����΂��B
	if (!isActive) {

		SlowMgr::Instance()->slowAmount += (1.0f - SlowMgr::Instance()->slowAmount) / 10.0f;

		return;

	}

	// �^�C�}�[���X�V����B
	//if (0 < focusTime)--focusTime;

	// �t�H�[�J�X����B
	Camera::Instance()->Focus(*focusPos + Vec2<float>(0, -100), FOCUS_ZOOM);

	// �X���[��������B
	SlowMgr::Instance()->slowAmount += (0.0f - SlowMgr::Instance()->slowAmount) / 10.0f;


	// switch�����Ŏg�p����ϐ��B
	float easingAmount = 0;
	Vec2<float> windowCenterPos = { (float)WinApp::Instance()->GetWinCenter().x,(float)WinApp::Instance()->GetWinCenter().y };
	Vec2<float> changePos = {};

	// ���݂̃X�e�[�^�X�ɉ����ď�����ς���B
	switch (status)
	{
	case StunEffect::STATUS_GENERATE:

		/*-- �������ĉ�ʒ����Ɍ��������� --*/

		// �C�[�W���O�ʂ����߂�B
		easingAmount = KuroMath::Ease(In, Quint, (float)breakUITimer / BREAK_UI_GENERATE_TIMER, 0, 1.0f);

		// ���W�̕ω���
		changePos = windowCenterPos - breakUIPosDef;

		// ���W�ƃX�P�[���Ɖ�]�ʂ𓮂����B
		breakUIPos = changePos * easingAmount + breakUIPosDef;
		breakUIExtRate = EXT_RATE * easingAmount;
		breakUIRadian -= DEF_RADIAN / BREAK_UI_GENERATE_TIMER;

		++breakUITimer;

		// �^�C�}�[������l�𒴂��Ă����玟�̏����ցB
		if (BREAK_UI_GENERATE_TIMER < breakUITimer) {

			status = StunEffect::STATUS_STOP;
			breakUITimer = 0;

		}

		break;
	case StunEffect::STATUS_STOP:

		/*-- ��ʒ��S�ɂ��Ă����莞�Ԓ�~���鏈�� --*/

		// �^�C�}�[���K��l�̔����𒴂��Ă�����B
		if (BREAK_UI_STOP_TIMER / 2.0f <= breakUITimer) {

			easingAmount = KuroMath::Ease(Out, Sine, (breakUITimer - BREAK_UI_STOP_TIMER / 2.0f) / (BREAK_UI_STOP_TIMER / 2.0f), 0, 1.0f);

			// �T�C�Y�ɃC�[�W���O�������đ傫������B
			breakUIExtRate = ADD_EXT_RATE * easingAmount + EXT_RATE;

		}

		++breakUITimer;

		// �^�C�}�[������l�𒴂��Ă����玟�̏����ցB
		if (BREAK_UI_STOP_TIMER < breakUITimer) {

			status = StunEffect::STATUS_ICON;
			breakUITimer = 0;

		}

		break;
	case StunEffect::STATUS_ICON:

		/*-- ��ʒ�������A�C�R���Ɍ��������� --*/

		// �C�[�W���O�ʂ����߂�B
		easingAmount = KuroMath::Ease(In, Cubic, (float)breakUITimer / BREAK_UI_ICON_TIMER, 0, 1.0f);

		// ���W�̕ω���
		changePos = iconPos - windowCenterPos;

		// ���W�ƃX�P�[���Ɖ�]�ʂ𓮂����B
		breakUIPos = changePos * easingAmount + windowCenterPos;
		breakUIExtRate = -EXT_RATE - ADD_EXT_RATE + ((EXT_RATE + ADD_EXT_RATE) * easingAmount);
		breakUIRadian = 10.0f * easingAmount + 3.14f;

		++breakUITimer;

		// �^�C�}�[������l�𒴂��Ă����玟�̏����ցB
		if (BREAK_UI_ICON_TIMER < breakUITimer) {

			Camera::Instance()->Init();
			isActive = false;

			if(isBoss){
				FaceIcon::Instance()->Change(WHICH_FACE::RIGHT_FACE, FACE_STATUS::DAMAGE);
			}
			else{
				FaceIcon::Instance()->Change(WHICH_FACE::LEFT_FACE, FACE_STATUS::DAMAGE);
			}

		}

		break;
	default:
		break;
	}




	//// �^�C�}�[��0�ɂȂ�����B
	//if (focusTime <= 0) {

	//	// �L�����ƃt�H�[�J�X��؂�B
	//	Camera::Instance()->Init();
	//	isActive = false;

	//}

}

void StunEffect::Draw()
{

	/*===== �`�揈�� =====*/

	if (!isActive) return;

	DrawFunc::DrawRotaGraph2D(breakUIPos, Vec2<float>{breakUIExtRate, breakUIExtRate}, breakUIRadian, TexHandleMgr::GetTexBuffer(useHandle));

}
