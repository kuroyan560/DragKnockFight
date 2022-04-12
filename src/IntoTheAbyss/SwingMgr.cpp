#include "SwingMgr.h"
#include "KuroMath.h"

SwingMgr::SwingMgr()
{

	/*===== �R���X�g���N�^ =====*/

	isSwingPlayer = false;
	isSwingBoss = false;
	easingTimer = 0;
	easingStartVec = {};
	easingEndVec = {};

}

void SwingMgr::Init()
{

	/*===== ���������� =====*/

	isSwingPlayer = false;
	isSwingBoss = false;
	easingTimer = 0;
	easingStartVec = {};
	easingEndVec = {};

}

void SwingMgr::Update(const Vec2<float>& playerPos, const Vec2<float>& bossPos, const float& lineLength)
{

	/*===== �X�V���� =====*/

	// ���W��ۑ��B
	this->playerPos = playerPos;
	this->bossPos = bossPos;
	// �R�̒�����ۑ��B
	this->lineLength = lineLength;

	// �U��񂵒�����Ȃ������珈�����΂��B
	if (!(isSwingPlayer || isSwingBoss)) return;

	// �C�[�W���O�^�C�}�[�����E�𒴂�����t���O��܂�B
	if (1.0f <= easingTimer) {

		isSwingPlayer = false;
		isSwingBoss = false;

	}

	// �C�[�W���O�^�C�}�[���X�V����B
	if (easingTimer < 1.0f) {

		// �C�[�W���O�^�C�}�[���X�V����B
		easingTimer += ADD_EASINGTIMER;

		// ���E�l�𒴂��Ȃ��悤�ɂ���B
		if (1.0f <= easingTimer) easingTimer = 1.0f;

		// �C�[�W���O�^�C�}�[���狁�߂���C�[�W���O�ʂ����߂�B
		easeAmount = KuroMath::Ease(In, Exp, easingTimer, 0.0f, 1.0f);

		// ���݂̃x�N�g�������߂�B
		easingNowVec.x = easingStartVec.x;
		easingNowVec.y = easingStartVec.y + (easeAmount * easeChangeAmountY);
		easingNowVec.Normalize();

	}

}

#include"AudioApp.h"
void SwingMgr::PlaySE()
{
	static int SE = -1;
	if (SE == -1)
	{
		SE = AudioApp::Instance()->LoadAudio("resource/ChainCombat/sound/swing.wav");
		AudioApp::Instance()->ChangeVolume(SE, 0.55f);
	}
	AudioApp::Instance()->PlayWave(SE);
}
