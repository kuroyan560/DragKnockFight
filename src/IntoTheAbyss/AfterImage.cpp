#include "AfterImage.h"
#include "DrawFunc_FillTex.h"
#include "TexHandleMgr.h"

void AfterImageMgr::Init()
{

	/*===== ���������� =====*/

	for (int index = 0; index < AFTERIMAGE_COUNT; ++index) {

		afterImages[index].alpha = 0;
		afterImages[index].destHandle = 0;
		afterImages[index].srcHandle = 0;
		afterImages[index].isActive = false;
		afterImages[index].pos = {};

	}

	delay = 0;

}

void AfterImageMgr::Generate(const Vec2<float>& Pos, const Vec2<float>& ExtRate, const float& Radian, const int& SrcHandle, const int& DestHandle)
{

	/*===== �������� =====*/

	// �x�����������Ă����珈�����΂��B
	--delay;
	if (0 < delay) return;

	// �������̃I�u�W�F�N�g��T���Đ����B
	for (int index = 0; index < AFTERIMAGE_COUNT; ++index) {

		// �����ς݂������珈�����΂��B
		if (afterImages[index].isActive) continue;

		// ��������B
		afterImages[index].pos = Pos;
		afterImages[index].alpha = 0.5f;
		afterImages[index].srcHandle = SrcHandle;
		afterImages[index].destHandle = DestHandle;
		afterImages[index].extRate = ExtRate;
		afterImages[index].radian = Radian;
		afterImages[index].isActive = true;

		break;

	}

	// �x���𐶐��B
	delay = DELAY;

}

void AfterImageMgr::Update()
{

	/*===== �X�V���� =====*/

	// �A���t�@�l�����񂾂񉺂���X�V�����B
	for (int index = 0; index < AFTERIMAGE_COUNT; ++index) {

		// �������������珈�����΂��B
		if (!afterImages[index].isActive) continue;

		// �A���t�@�l��������B
		static const float SUB_ALPHA = 0.05f;
		afterImages[index].alpha -= SUB_ALPHA;

		// �A���t�@�l��0�����ɂȂ����疳��������B
		if (afterImages[index].alpha < 0) {

			afterImages[index].isActive = false;

		}

	}

}
#include "DrawFunc.h"
#include "ScrollMgr.h"
void AfterImageMgr::Draw()
{

	/*===== �`�揈�� =====*/

	for (int index = 0; index < AFTERIMAGE_COUNT; ++index) {

		// �������������珈�����΂��B
		if (!afterImages[index].isActive) continue;

		// �`��B
		DrawFunc_FillTex::DrawRotaGraph2D(ScrollMgr::Instance()->Affect(afterImages[index].pos), afterImages[index].extRate, afterImages[index].radian, TexHandleMgr::GetTexBuffer(afterImages[index].destHandle), TexHandleMgr::GetTexBuffer(afterImages[index].srcHandle), afterImages[index].alpha);

	}

}
