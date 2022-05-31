#include "AfterImage.h"
#include "DrawFunc_Color.h"
#include "TexHandleMgr.h"

void AfterImageMgr::Init()
{

	/*===== ���������� =====*/

	for (int index = 0; index < AFTERIMAGE_COUNT; ++index) {

		afterImages[index].pos = {};
		afterImages[index].alpha = 0.8f;
		afterImages[index].handle = 0;
		afterImages[index].extRate = {};
		afterImages[index].radian = 0;
		afterImages[index].srcColor = Color();
		afterImages[index].isActive = false;

	}

	delay = 0;

}

void AfterImageMgr::Generate(const Vec2<float>& Pos, const Vec2<float>& ExtRate, const float& Radian, const int& Handle, const Color& SrcColor, const bool& IsExtendGraph, const Vec2<float>& Size)
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
		afterImages[index].handle = Handle;
		afterImages[index].extRate = ExtRate;
		afterImages[index].radian = Radian;
		afterImages[index].srcColor = SrcColor;
		afterImages[index].isActive = true;
		afterImages[index].size = Size;
		afterImages[index].isExtendGraph = IsExtendGraph;

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
		static const float SUB_ALPHA = 0.04f;
		afterImages[index].alpha -= SUB_ALPHA;

		// �A���t�@�l��0�����ɂȂ����疳��������B
		if (afterImages[index].alpha < 0) {

			afterImages[index].isActive = false;

		}

	}

}
#include "DrawFunc.h"
#include "ScrollMgr.h"
#include "GameSceneCamerMove.h"
void AfterImageMgr::Draw()
{

	/*===== �`�揈�� =====*/

	for (int index = 0; index < AFTERIMAGE_COUNT; ++index) {

		// �������������珈�����΂��B
		if (!afterImages[index].isActive) continue;

		// �`��B
		Color texColor = afterImages[index].srcColor;
		texColor.a = afterImages[index].alpha;

		// ExtendGraph�t���O�������Ă�����B
		if (afterImages[index].isExtendGraph) {

			DrawFunc_Color::DrawExtendGraph2D(ScrollMgr::Instance()->Affect(afterImages[index].pos - afterImages[index].size) + GameSceneCameraMove::Instance()->move, ScrollMgr::Instance()->Affect(afterImages[index].pos + afterImages[index].size), TexHandleMgr::GetTexBuffer(afterImages[index].handle), texColor, AlphaBlendMode_None);

		}
		else {

			DrawFunc_Color::DrawRotaGraph2D(ScrollMgr::Instance()->Affect(afterImages[index].pos) + GameSceneCameraMove::Instance()->move, afterImages[index].extRate, afterImages[index].radian, TexHandleMgr::GetTexBuffer(afterImages[index].handle), texColor);

		}
	}

}
