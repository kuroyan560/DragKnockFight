#include "DistanceCounter.h"
#include "CharacterManager.h"
#include "TexHandleMgr.h"
#include "ScrollMgr.h"
#include "KuroFunc.h"
#include "DrawFunc.h"
#include "DrawFunc_FillTex.h"

DistanceCounter::DistanceCounter()
{

	/*===== �R���X�g���N�^ =====*/

	distance = 0;
	lineCenterPos = { -1000,-10000 };

	TexHandleMgr::LoadDivGraph("resource/ChainCombat/UI/num.png", 12, Vec2<int>(12, 1), fontGraph.data());
	TexHandleMgr::LoadDivGraph("resource/ChainCombat/UI/num_red.png", 11, Vec2<int>(11, 1), redFontGraph.data());

	isExpSmall = false;
	exp = 1.0f;

	shakeAmount = {};

}

void DistanceCounter::Init()
{

	/*===== ������ =====*/

	distance = 0;
	lineCenterPos = { -1000,-10000 };

	isExpSmall = false;
	exp = 1.0f;

	shakeAmount = {};

}

void DistanceCounter::Update()
{

	/*===== �X�V���� =====*/

	// ��L�����̍��W�𑫂����l�B
	Vec2<float> addCharaPos = CharacterManager::Instance()->Right()->pos + CharacterManager::Instance()->Left()->pos;

	// ���̒��S�����߂�B
	lineCenterPos = addCharaPos / 2.0f;

	// 2�_�Ԃ̋��������߂�B
	distance = (CharacterManager::Instance()->Right()->pos - CharacterManager::Instance()->Left()->pos).Length() - DEAD_LINE;

	if (isExpSmall) {

		exp -= exp / 10.0f;

	}
	else {

		exp += (1.0f - exp) / 10.0f;

	}

	// �V�F�C�N�̍X�V����
	if (distance < MAX_SHAKE_DISTANCE) {

		// ���������߂�B
		float shakeRate = distance / MAX_SHAKE_DISTANCE;
		shakeRate = 1.0f - shakeRate;

		float shakeValue = shakeRate * MAX_SHAKE_AMOUNT;

		shakeAmount = { KuroFunc::GetRand(shakeValue * 2.0f) - shakeValue, KuroFunc::GetRand(shakeValue * 2.0f) - shakeValue };

	}

}

#include "GameSceneCamerMove.h"

void DistanceCounter::Draw()
{

	/*===== �`�揈�� =====*/
	static const int BLUE_METER = TexHandleMgr::LoadGraph("resource/ChainCombat/UI/meter_blue.png");
	static const int RED_METER = TexHandleMgr::LoadGraph("resource/ChainCombat/UI/meter_red.png");

	// �C���f�b�N�X���Ƃɂ��炷��
	const float INDEX_OFFSET = 62.0f;
	// �`�悷�鐔�������߂�B
	const int distanceDisitCount = KuroFunc::GetDigit(distance);
	// �`��ł��炷�I�t�Z�b�g�̒l�B
	const float OFFSET_X = static_cast<float>(distanceDisitCount) / 2.0f;
	// �`����W
	Vec2<float> drawPos = ScrollMgr::Instance()->Affect(lineCenterPos + shakeAmount);
	// �����ɂ�郌�[�g
	float rate = 1.0f - (distance / (MAX_SHAKE_DISTANCE * 2.0f));
	if (rate < 0) rate = 0;
	rate = KuroMath::Ease(InOut, Exp, rate, 0.0f, 1.0f);
	float zoom = ScrollMgr::Instance()->zoom;
	zoom = 1.0f - zoom;
	for (int index = 0; index < distanceDisitCount; ++index) {

		// �`�悷�鐔���B
		const int drawDisit = KuroFunc::GetSpecifiedDigitNum(distance, index);

		if (drawDisit < 0 || 9 < drawDisit) continue;

		// �`�悷��B
		DrawFunc_FillTex::DrawRotaGraph2D(drawPos - Vec2<float>(INDEX_OFFSET * zoom * index + OFFSET_X * zoom, 0) + GameSceneCameraMove::Instance()->move, 
			Vec2<float>(zoom * exp, zoom * exp), 0, TexHandleMgr::GetTexBuffer(fontGraph[drawDisit]), TexHandleMgr::GetTexBuffer(redFontGraph[drawDisit]), rate);
	}
	DrawFunc_FillTex::DrawRotaGraph2D(drawPos - Vec2<float>(INDEX_OFFSET * zoom * -1 + OFFSET_X * zoom, -8) + GameSceneCameraMove::Instance()->move,
		Vec2<float>(zoom * exp, zoom * exp), 0, TexHandleMgr::GetTexBuffer(BLUE_METER), TexHandleMgr::GetTexBuffer(RED_METER), rate);

}
