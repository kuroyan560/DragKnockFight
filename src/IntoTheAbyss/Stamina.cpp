#include "Stamina.h"
#include "ScrollMgr.h"
#include "DrawFunc.h"

Stamina::Stamina()
{

	/*===== ������ =====*/

	nowGauge = 100;
	isActivate = true;

}

void Stamina::Init()
{

	/*===== ������ =====*/

	nowGauge = 100;
	isActivate = true;

}

void Stamina::Empty()
{

	/*===== ������ =====*/

	nowGauge = 0;
	isActivate = false;

}

void Stamina::Update()
{

	/*===== �X�V���� =====*/

	if (nowGauge < MAX_GAUGE) {

		isActivate = false;

	}
	else {

		isActivate = true;

	}

}

void Stamina::Draw(const Vec2<float>& DrawPos, const float& Width, const float& Height)
{

	/*===== �`�揈�� =====*/

	// �X�^�~�i�����܂��Ă��镔����`��
	float per = nowGauge / MAX_GAUGE;

	// �X�^�~�i���}�b�N�X��������
	if (1.0f <= per) {

		// �A�E�g���C��
		Vec2<float> offset = { 2,2 };
		DrawFunc::DrawBox2D(ScrollMgr::Instance()->Affect(DrawPos - offset), ScrollMgr::Instance()->Affect(DrawPos + Vec2<float>(Width, Height) + offset), Color(0x29, 0xC9, 0xB4, 0xFF), true);

		// �����ɖ��邢�l�p�`��`��
		DrawFunc::DrawBox2D(ScrollMgr::Instance()->Affect(DrawPos), ScrollMgr::Instance()->Affect(DrawPos + Vec2<float>(Width, Height)), Color(0x2F, 0xFF, 0x8B, 0xFF), true);

	}
	else {

		Vec2<float> nowGaugePos = DrawPos + Vec2<float>(Width * per, 0);
		DrawFunc::DrawBox2D(ScrollMgr::Instance()->Affect(DrawPos), ScrollMgr::Instance()->Affect(nowGaugePos + Vec2<float>(0, Height)), Color(0x2F, 0xFF, 0x8B, 0xFF), true);

		// �X�^�~�i�����܂��Ă��Ȃ�������`��
		DrawFunc::DrawBox2D(ScrollMgr::Instance()->Affect(nowGaugePos), ScrollMgr::Instance()->Affect(DrawPos + Vec2<float>(Width, Height)), Color(0x2F, 0xFF, 0x8B, 0x55), true, AlphaBlendMode_Trans);

	}

}

float Stamina::AddNowGauge(const float& Add)
{

	/*===== �Q�[�W�ʂ𑫂� =====*/

	nowGauge += Add;

	// ����𒴂��Ă����獷�������^�[������B
	if (MAX_GAUGE <= nowGauge) {

		float sub = nowGauge - MAX_GAUGE;

		nowGauge -= sub;

		isActivate = true;

		// ���ߗʂ�Ԃ��B���̃Q�[�W�̎��̃Q�[�W�ɑ������߁B
		return sub;
	}

	//�����𒴂��Ă����獷�������^�[������
	if (nowGauge < 0) {

		float sub = 0 - nowGauge;

		nowGauge += sub;

		return sub;
	}

	return 0;

}

StaminaMgr::StaminaMgr()
{

	// ������ ���ŃX�^�~�i�Q�[�W��5�ݒ肵�Ă��܂��B
	const int KARI = 5;
	for (int index = 0; index < KARI; ++index) {
		stamina.push_back(Stamina());
	}

}

void StaminaMgr::Init()
{

	// ������ �S�Ă𖄂߂�B
	const int SIZE = stamina.size();
	for (int index = 0; index < SIZE; ++index) {
		stamina[index].Init();
	}

}

void StaminaMgr::Update(const bool& Heal)
{
	static const float HEAL_AMOUNT = 1.0f;
	// �X�^�~�i�Q�[�W�͉������ĂȂ��Ă��������񕜂���B
	const int STAMINA_COUNT = stamina.size();

	if (Heal)
	{
		for (int index = 0; index < STAMINA_COUNT; ++index) {

			// ���Ƀ}�b�N�X�������珈�����΂��B
			if (stamina[index].GetIsActivate()) continue;

			stamina[index].AddNowGauge(HEAL_AMOUNT);

			// ��O�����������X�ɉ񕜂��Ă������߁A���^�[���B
			break;

		}
	}

	// �X�V����
	for (int index = 0; index < STAMINA_COUNT; ++index) {

		stamina[index].Update();

	}

}

void StaminaMgr::Draw(const Vec2<float>& CharaPos)
{

	static const int STAMINA_GAUGE_WIDTH = 60;
	static const int STAMINA_GAUGE_WIDTH_PER_ONE = 20;
	static const int STAMINA_GAUGE_HEIGHT = 5;
	static const int STAMINA_GAUGE_OFFSET_Y = -64;
	static const int STAMINA_GAUGE_OFFSET_X = 5;
	{

		const int STAMINA_COUNT = stamina.size();
		for (int index = 0; index < STAMINA_COUNT; ++index) {

			// �`�悷����W���v�Z�B
			Vec2<float> drawPos = CharaPos;
			drawPos.y += STAMINA_GAUGE_OFFSET_Y;
			drawPos.x = (drawPos.x - STAMINA_GAUGE_WIDTH) + (index * STAMINA_GAUGE_WIDTH_PER_ONE) + (index * STAMINA_GAUGE_OFFSET_X);

			// �`�悷����W��n���ĕ`��I
			stamina[index].Draw(drawPos, STAMINA_GAUGE_WIDTH_PER_ONE, STAMINA_GAUGE_HEIGHT);

		}

	}

}

void StaminaMgr::ConsumesStamina(const int& ConsumesStamina)
{

	/*===== �X�^�~�i������ =====*/

	// �X�^�~�i������B
	float saveStamina = 0;	// �擪�ɂ��邽�܂肫���Ă��Ȃ��X�^�~�i�������Ȃ��悤�ɂ��邽�߂̕ϐ��B
	for (int consumeCount = 0; consumeCount < ConsumesStamina; ++consumeCount) {

		for (int index = stamina.size() - 1; 0 <= index; --index) {

			// ���܂肫���Ă��Ȃ������珈�����΂��B
			if (!stamina[index].GetIsActivate()) {

				// �l��������Ƃł������Ă�����ۑ����Ă����B
				saveStamina = stamina[index].GetNowGauge();
				stamina[index].Empty();

				continue;
			}

			// ���܂��Ă�����l�������B
			stamina[index].Empty();
			break;

		}

	}

	// �Ō���ɃZ�[�u���Ă������X�^�~�i��ǉ��B
	for (int index = 0; index < stamina.size(); ++index)
	{
		if (stamina[index].GetIsActivate())continue;
		stamina[index].AddNowGauge(saveStamina);
		break;
	}
}

bool StaminaMgr::CheckCanAction(const int& ConsumesStamina)
{

	/*===== �w�肵���A�N�V�������ł��邩�ǂ��� ====*/

	int stainaCounter = 0;

	const int STAMINA_COUNT = stamina.size();
	for (int index = 0; index < STAMINA_COUNT; ++index) {

		stainaCounter += stamina[index].GetIsActivate() ? 1 : 0;

	}

	return ConsumesStamina <= stainaCounter;

}

bool StaminaMgr::ConsumesStaminaByGauge(const float& CounsumeStaminaGauge)
{
	float consumeAmount = CounsumeStaminaGauge;

	for (int index = stamina.size() - 1; 0 <= index; --index) {

		//�����ł��c���Ă��������
		if (stamina[index].GetNowGauge()) {

			consumeAmount = stamina[index].AddNowGauge(-consumeAmount);

			//�����
			if (consumeAmount <= 0)
			{
				return true;
			}
		}
	}

	//�������Ȃ�����
	return false;
}
