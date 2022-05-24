#include "SwingDestroyCounter.h"
#include "KuroMath.h"

SwingDestroyCounter::SwingDestroyCounter()
{

	/*===== �R���X�g���N�^ =====*/

	for (int index = 0; index < DESTROY_COUNTER; ++index) {



	}

	nowCounter = 0;

}

void SwingDestroyCounter::Init()
{

	/*===== ���������� =====*/

	for (int index = 0; index < DESTROY_COUNTER; ++index) {



	}

	nowCounter = 0;

}

void SwingDestroyCounter::Update()
{

	/*===== �X�V���� =====*/

	for (int index = 0; index < DESTROY_COUNTER; ++index) {



	}

}

void SwingDestroyCounter::Draw()
{

	/*===== �`�揈�� =====*/

	for (int index = 0; index < DESTROY_COUNTER; ++index) {



	}

}

void SwingDestroyCounter::Increment()
{

	/*===== �J�E���g���X�V���� =====*/

	++nowCounter;

}

DestroyCounter::DestroyCounter()
{

	/*===== �R���X�g���N�^ =====*/

	disit = 0;
	easingTimer = 0;
	isActive = false;
	status = STATUS::EXIT;

}

void DestroyCounter::Init()
{

	/*===== ���������� =====*/

	disit = 0;
	easingTimer = 0;
	isActive = false;
	status = STATUS::EXIT;

}

void DestroyCounter::SetExtry(const int& Disit)
{

	/*===== �J�n���� =====*/

	disit = Disit;
	easingTimer = 0;
	isActive = true;
	status = STATUS::ENTRY;

}

void DestroyCounter::SetExit()
{

	/*===== �I������ =====*/

	easingTimer = 0;
	isActive = true;
	status = STATUS::EXIT;

}

void DestroyCounter::Update(const Vec2<float>& CharaPos)
{

	/*===== �X�V���� =====*/

	switch (status)
	{
	case DestroyCounter::STATUS::ENTRY:

		/*----- �J�n�i�K�������� -----*/

		easingTimer += ADD_EASING_TIMER;

		if (1.0f <= easingTimer) {

			easingTimer = 1.0f;

		}

		// �C�[�W���O�����ɍ��W���v�Z����B
		float easingAmount = KuroMath::Ease(Out, Exp, easingTimer, 0.0f, 1.0f);
		pos = CharaPos;
		pos.y -= easingAmount * EASING_CHANGE;

		// �C�[�W���O�^�C�}�[��1.0f�𒴂�����X�e�[�^�X���X�V����B
		if (1.0f <= easingTimer) {

			status = STATUS::STAY;

		}

		break;
	case DestroyCounter::STATUS::STAY:

		/*----- ��~�i�K�������� -----*/

		// ���̂Ƃ���͉����s��Ȃ��B

		break;
	case DestroyCounter::STATUS::EXIT:

		/*----- �I���i�K�������� -----*/

		easingTimer += ADD_EASING_TIMER;

		if (1.0f <= easingTimer) {

			easingTimer = 1.0f;

		}

		// �C�[�W���O�����ɍ��W���v�Z����B
		float easingAmount = KuroMath::Ease(Out, Exp, easingTimer, 0.0f, 1.0f);
		pos = CharaPos;
		pos.y += easingAmount * EASING_CHANGE;

		// �C�[�W���O�^�C�}�[��1.0f�𒴂�����X�e�[�^�X���X�V����B
		if (1.0f <= easingTimer) {

			Init();

		}


		break;
	default:
		break;
	}

}

void DestroyCounter::Draw()
{
}
