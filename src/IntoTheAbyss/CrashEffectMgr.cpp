#include "CrashEffectMgr.h"
#include "KuroMath.h"
#include "DrawFunc.h"
#include "ScrollMgr.h"

void ClassEffect::Init()
{

	/*===== �������� =====*/

	isActive = false;

}

void ClassEffect::Generate(const Vec2<float>& Pos, const Color& Col)
{

	/*===== �������� =====*/

	pos = Pos;
	size = { 30.0f, 0.0f };
	isExpY = false;
	isActive = true;
	timer = 0;

	col = Col;
}

void ClassEffect::Update()
{

	/*===== �X�V���� =====*/

	// �^�C�}�[���X�V�B
	++timer;

	// Y���g���i�K��������
	if (isExpY) {

		// �^�C�}�[������l�𒴂����玟�̒i�K�֐i�ށB
		if (EXP_Y_TIMER < timer) {

			timer = 0;
			isExpY = false;

		}
		else {

			// �C�[�W���O�ʂ����߂�B
			float easingAmount = KuroMath::Ease(Out, Sine, (float)timer / EXP_Y_TIMER, 0.0f, 1.0f);

			// Y�����g������B
			size.y = easingAmount * EXP_Y_AMOUNT;

		}

	}
	// X���g���i�K��������
	else {

		// �^�C�}�[������l�𒴂����疳��������B
		if (EXP_X_TIMER < timer) {

			isActive = false;

		}
		else {

			// �C�[�W���O�ʂ����߂�B
			float easingAmount = KuroMath::Ease(Out, Sine, (float)timer / EXP_X_TIMER, 0.0f, 1.0f);

			// Y�����k������B
			size.y = EXP_Y_AMOUNT - (easingAmount * EXP_Y_AMOUNT);

			// X�����g������B
			size.x = easingAmount * EXP_X_AMOUNT + 1.0f;

		}

	}

}

void ClassEffect::Draw()
{

	/*===== �`�揈�� =====*/

	Color drawCol = col;
	//int rand = KuroFunc::GetRand(0, 1);
	//if (rand == 0) {
	//	drawCol = Color(239, 1, 144, 255);
	//}
	//else if (rand == 1) {
	//	drawCol = Color(47, 255, 139, 255);
	//}
	drawCol.r *=  KuroFunc::GetRand(0.5f, 1.5f);
	drawCol.g *=  KuroFunc::GetRand(0.5f, 1.5f);
	drawCol.b *=  KuroFunc::GetRand(0.5f, 1.5f);
	static const float MAX_OFFSET = 0.0f;
	Vec2<float>offset = { KuroFunc::GetRand(-MAX_OFFSET,MAX_OFFSET),KuroFunc::GetRand(-MAX_OFFSET,MAX_OFFSET) };

	Vec2<float>drawSize = size;
	DrawFunc::DrawBox2D(ScrollMgr::Instance()->Affect(pos - drawSize + offset), ScrollMgr::Instance()->Affect(pos + drawSize + offset), drawCol, true, AlphaBlendMode_Add);
	drawSize = { size.y,size.x };
	DrawFunc::DrawBox2D(ScrollMgr::Instance()->Affect(pos - drawSize + offset), ScrollMgr::Instance()->Affect(pos + drawSize + offset), drawCol, true, AlphaBlendMode_Add);
}

void CrashEffectMgr::Init()
{

	/*===== ���������� =====*/

	for (int index = 0; index < CRASH_EFFECT_AMOUNT; ++index) {

		crashEffects[index].Init();

	}

}

void CrashEffectMgr::Generate(const Vec2<float>& Pos, const Color& Col)
{

	/*===== �������� =====*/
	Vec2<float>randomPos = { 0,0 };

	static const int GENERATE_COUNT = 1;
	for (int generateCount = 0; generateCount < GENERATE_COUNT; ++generateCount) {

		for (int index = 0; index < CRASH_EFFECT_AMOUNT; ++index) {

			// �����ς݂������珈�����΂��B
			if (crashEffects[index].GetIsActive()) continue;

			// �����n�_���烉���_���ňʒu�����炷�B
			const float RANDOM = 5.0f;
			crashEffects[index].Generate(Pos + randomPos, Col);
			randomPos = { KuroFunc::GetRand(-RANDOM,RANDOM),KuroFunc::GetRand(-RANDOM,RANDOM) };

			break;

		}

	}

}

void CrashEffectMgr::Update()
{

	/*===== �X�V���� =====*/

	for (int index = 0; index < CRASH_EFFECT_AMOUNT; ++index) {

		if (!crashEffects[index].GetIsActive()) continue;

		crashEffects[index].Update();

	}

}

void CrashEffectMgr::Draw()
{

	/*=====	�`�揈�� =====*/

	for (int index = 0; index < CRASH_EFFECT_AMOUNT; ++index) {

		if (!crashEffects[index].GetIsActive()) continue;

		crashEffects[index].Draw();

	}

}
