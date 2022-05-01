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
		Vec2<float> offset = {2,2};
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

	// ���E�𒴂��Ă����獷�������^�[������B
	if (MAX_GAUGE < nowGauge) {

		float sub = nowGauge - MAX_GAUGE;

		nowGauge -= sub;

		isActivate = true;

		// ���ߗʂ�Ԃ��B���̃Q�[�W�̎��̃Q�[�W�ɑ������߁B
		return sub;

	}

	return 0;

}
