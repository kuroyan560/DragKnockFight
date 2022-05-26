#include "DistanceCounter.h"
#include "CharacterManager.h"
#include "TexHandleMgr.h"
#include "ScrollMgr.h"
#include "KuroFunc.h"
#include "DrawFunc.h"

DistanceCounter::DistanceCounter()
{

	/*===== �R���X�g���N�^ =====*/

	distance = 0;
	lineCenterPos = { -1000,-10000 };

	TexHandleMgr::LoadDivGraph("resource/ChainCombat/UI/num.png", 12, Vec2<int>(12, 1), fontGraph.data());

}

void DistanceCounter::Init()
{

	/*===== ������ =====*/

	distance = 0;
	lineCenterPos = { -1000,-10000 };

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

}

void DistanceCounter::Draw()
{

	/*===== �`�揈�� =====*/

	// �C���f�b�N�X���Ƃɂ��炷��
	const float INDEX_OFFSET = 256.0f;
	// �`�悷�鐔�������߂�B
	const int distanceDisitCount = KuroFunc::GetDigit(distance);
	// �`��ł��炷�I�t�Z�b�g�̒l�B
	const float OFFSET_X = static_cast<float>(distanceDisitCount) / 2.0f;
	for (int index = 0; index < distanceDisitCount; ++index) {

		// �`�悷�鐔���B
		const int drawDisit = KuroFunc::GetSpecifiedDigitNum(distance, index);

		if (drawDisit < 0 || 9 < drawDisit) continue;

		float zoom = ScrollMgr::Instance()->zoom;
		zoom = 1.0f - zoom;

		// �`�悷��B
		DrawFunc::DrawRotaGraph2D(ScrollMgr::Instance()->Affect(lineCenterPos - Vec2<float>((INDEX_OFFSET * zoom) * index, 0)), Vec2<float>(1.0f * zoom, 1.0f * zoom), 0, TexHandleMgr::GetTexBuffer(fontGraph[drawDisit]));

	}

}
