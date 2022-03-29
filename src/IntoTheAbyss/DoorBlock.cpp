#include "DoorBlock.h"
#include"EventCollider.h"
#include"StageMgr.h"
#include"SelectStage.h"

DoorBlock::DoorBlock() :chipNumber(-1)
{
}

void DoorBlock::Init(const Vec2<float> &LEFT_UP_POS, const Vec2<float> &RIGHT_DOWN_POS, const int &DOOR_CHIP_NUM)
{
	leftUpPos = LEFT_UP_POS;
	size = RIGHT_DOWN_POS - leftUpPos;
	chipNumber = DOOR_CHIP_NUM;

	Vec2<float>mapChipSize(50.0f, 50.0f);
	Vec2<float>mapChipHalfSize(25.0f, 25.0f);

	Vec2<float>topBlockPos((leftUpPos + mapChipHalfSize) / 50.0f);
	Vec2<float>bottomBlockPos((RIGHT_DOWN_POS - mapChipHalfSize) / 50.0f);


	//�h�A�̌����w��
	//������W�ƉE�����W����㉺���E�ǂ���ɋ�Ԃ����邩���ׂ�
	//���E�ɐL�тĂ���Ȃ�㉺�`�F�b�N
	if (false)
	{
		int topRightChipNum = StageMgr::Instance()->GetMapChipBlock(SelectStage::Instance()->GetStageNum(), SelectStage::Instance()->GetRoomNum(), topBlockPos + Vec2<float>(0.0f, 1.0f));
		int topLeftChipNum = StageMgr::Instance()->GetMapChipBlock(SelectStage::Instance()->GetStageNum(), SelectStage::Instance()->GetRoomNum(), topBlockPos + Vec2<float>(0.0f, -1.0f));

		int buttomRightChipNum = StageMgr::Instance()->GetMapChipBlock(SelectStage::Instance()->GetStageNum(), SelectStage::Instance()->GetRoomNum(), bottomBlockPos + Vec2<float>(0.0f, 1.0f));
		int buttomLeftChipNum = StageMgr::Instance()->GetMapChipBlock(SelectStage::Instance()->GetStageNum(), SelectStage::Instance()->GetRoomNum(), bottomBlockPos + Vec2<float>(0.0f, -1.0f));

		//��ɕǂ�����Ɣ��肵����
		if (topRightChipNum || buttomRightChipNum)
		{

		}
		//���ɕǂ�����Ɣ��肵����
		if (topLeftChipNum || buttomLeftChipNum)
		{

		}
	}

	//�㉺�ɐL�тĂ���Ȃ獶�E�`�F�b�N
	if (true)
	{
		int topRightChipNum = StageMgr::Instance()->GetMapChipBlock(SelectStage::Instance()->GetStageNum(), SelectStage::Instance()->GetRoomNum(), topBlockPos + Vec2<float>(1.0f, 0.0f));
		int topLeftChipNum = StageMgr::Instance()->GetMapChipBlock(SelectStage::Instance()->GetStageNum(), SelectStage::Instance()->GetRoomNum(), topBlockPos + Vec2<float>(-1.0f, 0.0f));

		int buttomRightChipNum = StageMgr::Instance()->GetMapChipBlock(SelectStage::Instance()->GetStageNum(), SelectStage::Instance()->GetRoomNum(), bottomBlockPos + Vec2<float>(1.0f, 0.0f));
		int buttomLeftChipNum = StageMgr::Instance()->GetMapChipBlock(SelectStage::Instance()->GetStageNum(), SelectStage::Instance()->GetRoomNum(), bottomBlockPos + Vec2<float>(-1.0f, 0.0f));

		//�E�ɕǂ�����Ɣ��肵����
		if (topRightChipNum || buttomRightChipNum)
		{
			responePos =
			{
				bottomBlockPos.x * 50.0f + mapChipSize.x * 3.0f,
				bottomBlockPos.y * 50.0f
			};

			restartPos =
			{
				bottomBlockPos.x * 50.0f + -mapChipSize.x * 1.0f,
				bottomBlockPos.y * 50.0f
			};
			doorDir = DOOR_LEFT;
		}


		//���ɕǂ�����Ɣ��肵����
		if (topLeftChipNum || buttomLeftChipNum)
		{
			responePos =
			{
				bottomBlockPos.x * 50.0f + -mapChipSize.x * 3.0f,
				bottomBlockPos.y * 50.0f
			};

			restartPos =
			{
				bottomBlockPos.x * 50.0f + mapChipSize.x * 1.0f,
				bottomBlockPos.y * 50.0f
			};
			doorDir = DOOR_RIGHT;
		}
	}

}

void DoorBlock::Finalize()
{
	chipNumber = -1;
}

bool DoorBlock::Collision(const Vec2<float> &PLAYER_POS, const Vec2<float> &SIZE, const Vec2<float> &PLAYER_VEL, const Vec2<float> &PLAYER_PREV_POS)
{
	//����������Ă��Ȃ��Ȃ瓖���蔻���ʂ��Ȃ�
	if (chipNumber == -1)
	{
		return false;
	}


	bool isDossunVel = EventCpllider::Instance()->CheckHitVel(PLAYER_POS, PLAYER_PREV_POS, PLAYER_VEL, SIZE, leftUpPos, size) != INTERSECTED_NONE;
	bool topHitFlag = EventCpllider::Instance()->CheckHitSize(leftUpPos, size, PLAYER_POS, SIZE, INTERSECTED_TOP);
	bool buttomFlag = EventCpllider::Instance()->CheckHitSize(leftUpPos, size, PLAYER_POS, SIZE, INTERSECTED_BOTTOM);
	bool leftHitFlag = EventCpllider::Instance()->CheckHitSize(leftUpPos, size, PLAYER_POS, SIZE, INTERSECTED_LEFT);
	bool rightHitFlag = EventCpllider::Instance()->CheckHitSize(leftUpPos, size, PLAYER_POS, SIZE, INTERSECTED_RIGHT);

	if (isDossunVel || topHitFlag || buttomFlag || leftHitFlag || rightHitFlag)
	{
		return true;
	}
	else
	{
		return false;
	}

}

