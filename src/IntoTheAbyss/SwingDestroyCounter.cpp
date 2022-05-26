#include "SwingDestroyCounter.h"
#include "KuroMath.h"
#include "KuroFunc.h"
#include "TexHandleMgr.h"
#include "ScrollMgr.h"
#include "DrawFunc.h"

SwingDestroyCounter::SwingDestroyCounter()
{

	/*===== �R���X�g���N�^ =====*/

	for (int index = 0; index < DESTROY_COUNTER; ++index) {

		destroyCounter[index].Init();

	}

	nowCounter = 0;

	TexHandleMgr::LoadDivGraph("resource/ChainCombat/UI/num.png", 12, Vec2<int>(12, 1), numberHandle.data());
	TexHandleMgr::LoadDivGraph("resource/ChainCombat/UI/num_yellow.png", 12, Vec2<int>(12, 1), numberHandle_yellow.data());

}

void SwingDestroyCounter::Init()
{

	/*===== ���������� =====*/

	for (int index = 0; index < DESTROY_COUNTER; ++index) {

		destroyCounter[index].Init();

	}

	nowCounter = 0;

}

#include"DebugParameter.h"
#include"ScoreManager.h"
void SwingDestroyCounter::AllExit()
{
	/*===== ���ׂĂ�Exit��Ԃɂ��� =====*/

	for (int index = 0; index < DESTROY_COUNTER; ++index) {

		if (!destroyCounter[index].isActive) continue;

		destroyCounter[index].isExitReservation = true;

	}

	DebugParameter::Instance()->totalCombo += nowCounter;
	//ScoreManager::Instance()->Add(nowCounter);
	ScoreManager::Instance()->destroyPoint = nowCounter;
	nowCounter = 0;

}

void SwingDestroyCounter::Update(const Vec2<float>& CharaPos)
{

	/*===== �X�V���� =====*/

	for (int index = 0; index < DESTROY_COUNTER; ++index) {

		if (!destroyCounter[index].isActive) continue;

		destroyCounter[index].Update(CharaPos);

	}

}

void SwingDestroyCounter::Draw()
{

	/*===== �`�揈�� =====*/

	for (int index = 0; index < DESTROY_COUNTER; ++index) {

		if (!destroyCounter[index].isActive) continue;

		if (nowCounter < 100)
		{
			destroyCounter[index].Draw(numberHandle);
		}
		else
		{
			destroyCounter[index].Draw(numberHandle_yellow);
		}
	}

}

void SwingDestroyCounter::Increment(int INCRE_NUM)
{

	/*===== �J�E���g���X�V���� =====*/

	// ���Z����O�̒l��ۑ��B
	int prevCounter = nowCounter;

	// ���Z�B
	nowCounter += INCRE_NUM;

	// �ǂ����O�t���[���ƕς���Ă��邩�𒲂ׂ�B
	const int NOW_DISIT = KuroFunc::GetDigit(nowCounter);
	for (int index = 0; index < NOW_DISIT; ++index) {

		// �O�t���[���ƕς���Ă�����B
		if (KuroFunc::GetSpecifiedDigitNum(prevCounter, index) == KuroFunc::GetSpecifiedDigitNum(nowCounter, index)) continue;

		// ���̏ꏊ�ɂ��������Exit��Ԃɂ���B
		CheckDisitStayID(static_cast<DestroyCounter::DISIT_ID>(index + 1));

		// �V����Entry��Ԃɂ���B
		for (auto& counterIndex : destroyCounter) {

			if (counterIndex.isActive || counterIndex.status != DestroyCounter::STATUS::NONE) continue;

			counterIndex.SetEntry(KuroFunc::GetSpecifiedDigitNum(nowCounter, index), static_cast<DestroyCounter::DISIT_ID>(index + 1));

			break;

		}

	}

}

void SwingDestroyCounter::CheckDisitStayID(const DestroyCounter::DISIT_ID DisitID)
{

	/*===== �w���DisitID��Index���擾 =====*/

	for (auto& index : destroyCounter) {

		if (!index.isActive) continue;
		if (!(index.disitID == DisitID)) continue;

		index.isExitReservation = true;

	}
}

DestroyCounter::DestroyCounter()
{

	/*===== �R���X�g���N�^ =====*/

	disit = 0;
	easingTimer = 0;
	isActive = false;
	status = STATUS::NONE;

}

void DestroyCounter::Init()
{

	/*===== ���������� =====*/

	disitID = DISIT_ID::DISIT_NONE;
	disit = 0;
	easingTimer = 0;
	isActive = false;
	status = STATUS::NONE;
	isExitReservation = false;
	alpha = 0;

}

void DestroyCounter::SetEntry(const int& Disit, const DISIT_ID DisitID)
{

	/*===== �J�n���� =====*/

	disit = Disit;
	disitID = DisitID;
	easingTimer = 0;
	isActive = true;
	exitTimer = EXIT_TIMER;
	status = STATUS::ENTRY;
	alpha = 0;
	exitReservationTimer = 1;

}

void DestroyCounter::SetExit()
{

	/*===== �I������ =====*/

	easingTimer = 0;
	isActive = true;
	status = STATUS::EXIT;
	alpha = 255;

}

void DestroyCounter::Update(const Vec2<float>& CharaPos)
{

	/*===== �X�V���� =====*/

	float easingAmount = 0;

	switch (status)
	{
	case DestroyCounter::STATUS::ENTRY:

		/*----- �J�n�i�K�������� -----*/

		easingTimer += ADD_EASING_TIMER;

		if (1.0f <= easingTimer) {

			easingTimer = 1.0f;

		}

		// �C�[�W���O�����ɍ��W���v�Z����B
		easingAmount = KuroMath::Ease(Out, Exp, easingTimer, 0.0f, 1.0f);
		pos = CharaPos;
		pos.y += EASING_CHANGE - (easingAmount * EASING_CHANGE);

		alpha = 255 * easingAmount;

		// �C�[�W���O�^�C�}�[��1.0f�𒴂�����X�e�[�^�X���X�V����B
		if (1.0f <= easingTimer) {

			status = STATUS::STAY;

		}

		break;
	case DestroyCounter::STATUS::STAY:

		/*----- ��~�i�K�������� -----*/

		// ���W�����B
		pos = CharaPos;

		// ��Exit����t���O�������Ă�����B
		if (isExitReservation) {

			--exitReservationTimer;
			if (exitReservationTimer <= 0) {

				//status = STATUS::EXIT;
				SetExit();
				Init();

			}

		}

		// �����I�ɏ�����^�C�}�[���X�V����B
		//--exitTimer;
		if (exitTimer <= 0) {

			SetExit();

		}


		break;
	case DestroyCounter::STATUS::EXIT:

		/*----- �I���i�K�������� -----*/

		easingTimer += ADD_EASING_TIMER;

		if (1.0f <= easingTimer) {

			easingTimer = 1.0f;

		}

		// �C�[�W���O�����ɍ��W���v�Z����B
		easingAmount = KuroMath::Ease(Out, Exp, easingTimer, 0.0f, 1.0f);
		pos = CharaPos;
		pos.y -= easingAmount * EASING_CHANGE;

		alpha = 255 - (255 * easingAmount);

		// �C�[�W���O�^�C�}�[��1.0f�𒴂�����X�e�[�^�X���X�V����B
		if (1.0f <= easingTimer) {

			Init();

		}


		break;
	default:
		break;
	}

}

void DestroyCounter::Draw(std::array<int, 12> NumberHandle)
{

	/*===== �`�揈�� =====*/

	float zoom = ScrollMgr::Instance()->zoom;
	DrawFunc::DrawRotaGraph2D(ScrollMgr::Instance()->Affect(pos - Vec2<float>(FONT_SIZE * static_cast<int>(disitID), 0)), Vec2<float>(1.4f * zoom, 1.4f * zoom), 0, TexHandleMgr::GetTexBuffer(NumberHandle[disit]), Color(255, 255, 255, alpha));

}
