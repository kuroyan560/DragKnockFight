#include "EventBlock.h"
#include"StageMgr.h"
#include"EventCollider.h"

int EventBlock::arrayNum = 0;

EventBlock::EventBlock() :initFlag(false)
{
	handle = arrayNum;
	++arrayNum;
}

void EventBlock::Init(const Vec2<float> &POS)
{
	pos = POS;
	initFlag = true;
}

void EventBlock::Finalize()
{
	initFlag = false;
}

bool EventBlock::HitBox(const Vec2<float> &PLAYER_POS, const Vec2<float> &SIZE)
{
	//���������Ă��Ȃ��u���b�N�̓G���[���������
	if (!initFlag)
	{
		SizeData tmp = StageMgr::Instance()->GetMapChipSizeData(MAPCHIP_TYPE_EVENT);
		int nowHandle = tmp.min + handle;
		std::string handleString = "�C�x���g�`�b�v" + std::to_string(handle) + "(�`�b�v�ԍ�" + std::to_string(nowHandle) + ")" + "��z�u���Ă��Ȃ��̂ɂ�������炸�M�~�b�N���Ŏg�����Ƃ��Ă��܂��B\n�ݒu�����肢���܂�";
		KazErrorHelper::Error(handleString);
		return false;
	}


	bool topHitFlag = EventCpllider::Instance()->CheckHitSize(pos, { 50.0f,50.0f }, PLAYER_POS, SIZE, INTERSECTED_TOP);
	bool buttomFlag = EventCpllider::Instance()->CheckHitSize(pos, { 50.0f,50.0f }, PLAYER_POS, SIZE, INTERSECTED_BOTTOM);
	bool leftHitFlag = EventCpllider::Instance()->CheckHitSize(pos, { 50.0f,50.0f }, PLAYER_POS, SIZE, INTERSECTED_LEFT);
	bool rightHitFlag = EventCpllider::Instance()->CheckHitSize(pos, { 50.0f,50.0f }, PLAYER_POS, SIZE, INTERSECTED_RIGHT);

	if (topHitFlag || buttomFlag || leftHitFlag || rightHitFlag)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void EventBlock::Draw()
{

}
