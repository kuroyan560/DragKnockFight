#include "StunEffect.h"
#include "Camera.h"
#include "SlowMgr.h"

StunEffect::StunEffect()
{

	/*===== �R���X�g���N�^ =====*/

	focusTime = 0;
	focusPos = {};
	isActive = false;

}

void StunEffect::Init()
{

	/*===== ���������� =====*/

	focusTime = 0;
	focusPos = {};
	isActive = false;

}

void StunEffect::Activate(Vec2<float>& focusPos)
{

	/*===== �������� =====*/

	focusTime = FOCUS_TIME;
	this->focusPos = &focusPos;
	isActive = true;

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
	if (0 < focusTime)--focusTime;

	// �t�H�[�J�X����B
	Camera::Instance()->Focus(*focusPos, FOCUS_ZOOM);

	// �X���[��������B
	SlowMgr::Instance()->slowAmount += (0.0f - SlowMgr::Instance()->slowAmount) / 10.0f;

	// �^�C�}�[��0�ɂȂ�����B
	if (focusTime <= 0) {

		// �L�����ƃt�H�[�J�X��؂�B
		Camera::Instance()->Init();
		isActive = false;

	}

}
