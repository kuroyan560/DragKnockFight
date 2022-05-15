#include "Stamina.h"
#include "ScrollMgr.h"
#include "DrawFunc.h"
#include"AudioApp.h"

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

		// �}�b�N�X�ɂȂ����g���K�[�̎��Ɉ�C�ɑ傫������B
		if (!isActivate) {

			SetExp(true);

		}

		isActivate = true;

	}

	// �g�債�Ă����炻�̒l��0�ɋ߂Â���B
	expAmount.x -= expAmount.x / 5.0f;
	expAmount.y -= expAmount.y / 5.0f;

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
		DrawFunc::DrawBox2D(ScrollMgr::Instance()->Affect(DrawPos - offset - expAmount), ScrollMgr::Instance()->Affect(DrawPos + Vec2<float>(Width, Height) + offset + expAmount), outerColor, true);

		// �����ɖ��邢�l�p�`��`��
		DrawFunc::DrawBox2D(ScrollMgr::Instance()->Affect(DrawPos - expAmount), ScrollMgr::Instance()->Affect(DrawPos + Vec2<float>(Width, Height) + expAmount), innerColor, true);

	}
	else {

		Vec2<float> nowGaugePos = DrawPos + Vec2<float>(Width * per, 0);
		DrawFunc::DrawBox2D(ScrollMgr::Instance()->Affect(DrawPos - expAmount), ScrollMgr::Instance()->Affect(nowGaugePos + Vec2<float>(0, Height) + expAmount), innerColor, true);

		// �X�^�~�i�����܂��Ă��Ȃ�������`��
		Color outerColorBuff = outerColor;
		outerColorBuff /= 3.0f;
		outerColorBuff.Alpha() = 50;
		DrawFunc::DrawBox2D(ScrollMgr::Instance()->Affect(nowGaugePos - expAmount), ScrollMgr::Instance()->Affect(DrawPos + Vec2<float>(Width, Height) + expAmount), outerColorBuff, true, AlphaBlendMode_Trans);

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

		// �Q�[�W���}�b�N�X�ɂȂ����̂Ŋg�傷�鉉�o������B
		SetExp(true);

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

void Stamina::SetExp(const bool& isBig)
{

	if (isBig) {
		expAmount = { BIG_EXP_AMOUNT,BIG_EXP_AMOUNT };
	}
	else {
		expAmount = { EXP_AMOUNT,EXP_AMOUNT };
	}

}

void Stamina::SetColor(const Color& InnerColor, const Color& OuterColor)
{

	innerColor = InnerColor;
	outerColor = OuterColor;

}

int StaminaMgr::STAMINA_HEAL_SE[STAMINA_NUM] = { -1 };

StaminaMgr::StaminaMgr()
{
	if (STAMINA_HEAL_SE[0] == -1)
	{
		for (int i = 0; i < STAMINA_NUM; ++i)
		{
			STAMINA_HEAL_SE[i] = AudioApp::Instance()->LoadAudio("resource/ChainCombat/sound/staminaBarHeal_" + std::to_string(i) + ".wav");
		}
	}

	// ������ ���ŃX�^�~�i�Q�[�W��5�ݒ肵�Ă��܂��B
	for (int index = 0; index < STAMINA_NUM; ++index) {
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

	outOfStaminaEffect.Init();
	emptyTrigger = false;
}

#include"SlowMgr.h"
void StaminaMgr::Update(const bool& Heal, const Vec2<float>& CharacterPos)
{
	const bool oldFullFlg = 100.0f <= stamina.back().GetNowGauge();

	static const float HEAL_AMOUNT = 1.5f;
	// �X�^�~�i�Q�[�W�͉������ĂȂ��Ă��������񕜂���B
	const int STAMINA_COUNT = stamina.size();

	if (Heal)
	{
		for (int index = 0; index < STAMINA_COUNT; ++index) {

			// ���Ƀ}�b�N�X�������珈�����΂��B
			if (stamina[index].GetIsActivate()) continue;

			stamina[index].AddNowGauge(HEAL_AMOUNT * SlowMgr::Instance()->slowAmount);
			if (100.0f <= stamina[index].GetNowGauge())AudioApp::Instance()->PlayWave(STAMINA_HEAL_SE[index]);

			// ��O�����������X�ɉ񕜂��Ă������߁A���^�[���B
			break;

		}
	}

	// �X�V����
	for (int index = 0; index < STAMINA_COUNT; ++index) {

		stamina[index].Update();

	}

	//�X�^�~�i���o
	static const float OFFSET_Y = -32.0f;
	outOfStaminaEffect.baseEmptyStringPos = { CharacterPos.x,CharacterPos.y + OFFSET_Y };
	outOfStaminaEffect.baseMaxStringPos = { CharacterPos.x,CharacterPos.y + OFFSET_Y };
	outOfStaminaEffect.Update();

	emptyTrigger = false;

	if (!oldFullFlg && 100.0f <= stamina.back().GetNowGauge())outOfStaminaEffect.Full();
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

	outOfStaminaEffect.Draw();
}

void StaminaMgr::SetColor(const Color& InnerColor, const Color& OuterColor)
{

	const int STAMINA_COUNT = stamina.size();
	for (int index = 0; index < STAMINA_COUNT; ++index) {

		stamina[index].SetColor(InnerColor, OuterColor);

	}

}

void StaminaMgr::ConsumesStamina(const int& ConsumesStamina)
{

	/*===== �X�^�~�i������ =====*/

	//���X����ۂ�
	const bool oldEmptyFlg = stamina.front().GetNowGauge() <= 0.0f;

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

	//�����
	if (!oldEmptyFlg && stamina.front().GetNowGauge() <= 0.0f)
	{
		emptyTrigger = true;
		outOfStaminaEffect.Empty();
	}
}

void StaminaMgr::AddStamina(const int& AddStamina)
{
	/*===== �X�^�~�i���񕜂����� =====*/

	if (AddStamina <= 0) return;

	std::vector<int>seHandles;

	//���X�ő傩
	const bool oldFullFlg = 100.0f <= stamina.back().GetNowGauge();

	int addStamina = AddStamina;

	const int STAMINA_COUNT = stamina.size();
	for (int index = 0; index < STAMINA_COUNT; ++index) {

		// ���Ƀ}�b�N�X�������珈�����΂��B
		if (stamina[index].GetIsActivate()) continue;

		// �I�[�o�[���ĉ񕜂��Ȃ��悤�ɂ���B
		if (addStamina < 100.0f - stamina[index].GetNowGauge()) {

			stamina[index].AddNowGauge(addStamina);
			stamina[index].SetExp();
			if (100.0f <= stamina[index].GetNowGauge())seHandles.emplace_back(STAMINA_HEAL_SE[index]);

			break;

		}
		// �I�[�o�[���Ă�����B
		else {

			// �I�[�o�[���Ă���ʂ�ۑ�����B
			addStamina -= 100.0f - stamina[index].GetNowGauge();

			// �񕜂���B
			stamina[index].AddNowGauge(100.0f - stamina[index].GetNowGauge());
			stamina[index].SetExp(true);
			if (100.0f <= stamina[index].GetNowGauge())seHandles.emplace_back(STAMINA_HEAL_SE[index]);
		}
	}

	AudioApp::Instance()->PlayWaveArray(seHandles);
	//�ő�ɂȂ���
	if (!oldFullFlg && 100.0f <= stamina.back().GetNowGauge())outOfStaminaEffect.Full();
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

void StaminaMgr::ConsumesStaminaByGauge(const float& CounsumeStaminaGauge)
{
	//���X����ۂ�
	const bool oldEmptyFlg = stamina.front().GetNowGauge() <= 0.0f;

	float consumeAmount = CounsumeStaminaGauge;

	for (int index = stamina.size() - 1; 0 <= index; --index) {

		//�����ł��c���Ă��������
		if (stamina[index].GetNowGauge()) {

			consumeAmount = stamina[index].AddNowGauge(-consumeAmount);

			//�����
			if (consumeAmount <= 0)
			{
				break;
			}
		}
	}

	//�����
	if (!oldEmptyFlg && stamina.front().GetNowGauge() <= 0.0f)
	{
		emptyTrigger = true;
		outOfStaminaEffect.Empty();
	}

}
