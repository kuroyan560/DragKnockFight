#include "MassChip.h"
#include "StageMgr.h"

MassChip::MassChip(const int &CHIP_NUM) :chipNumber(CHIP_NUM)
{
}

bool MassChip::Check(const Vec2<int> &MAPCHIP_POS)
{
	for (int i = 0; i < registChipPos.size(); ++i)
	{
		if (registChipPos[i].x == MAPCHIP_POS.x && registChipPos[i].y == MAPCHIP_POS.y)
		{
			return false;
		}
	}
	//�d�������珈�����΂�-----------------------

	int stageNum = SelectStage::Instance()->GetStageNum();
	int roomNum = SelectStage::Instance()->GetRoomNum();

#pragma region �㉺���E�̒T��
	registChipPos.push_back(Vec2<float>(MAPCHIP_POS.x, MAPCHIP_POS.y));
	//��ɂǂꂭ�炢�L�т邩
	for (int upY = 1; 1; ++upY)
	{
		int Y = StageMgr::Instance()->GetMapChipBlock(stageNum, roomNum, Vec2<float>(MAPCHIP_POS.x, MAPCHIP_POS.y - upY));

		if (Y != chipNumber || CheckUsedData(registChipPos, Vec2<float>(MAPCHIP_POS.x, MAPCHIP_POS.y - upY)))
		{
			break;
		}
		else
		{
			registChipPos.push_back(Vec2<float>(MAPCHIP_POS.x, MAPCHIP_POS.y - upY));
			registDirChipPos[UP].push_back(Vec2<float>(MAPCHIP_POS.x, MAPCHIP_POS.y - upY));
		}
	}
	//���ɂǂꂭ�炢�L�т邩
	for (int downY = 1; 1; ++downY)
	{
		int Y = StageMgr::Instance()->GetMapChipBlock(stageNum, roomNum, Vec2<float>(MAPCHIP_POS.x, MAPCHIP_POS.y + downY));
		if (Y != chipNumber || CheckUsedData(registChipPos, Vec2<float>(MAPCHIP_POS.x, MAPCHIP_POS.y + downY)))
		{
			break;
		}
		else
		{
			registChipPos.push_back(Vec2<float>(MAPCHIP_POS.x, MAPCHIP_POS.y + downY));
			registDirChipPos[DOWN].push_back(Vec2<float>(MAPCHIP_POS.x, MAPCHIP_POS.y + downY));
		}
	}

	//�㉺�ɐL�΂������o�����炱���̕������΂�-----------------------
	if (registDirChipPos[DOWN].size() == 0 && registDirChipPos[UP].size() == 0)
	{
		//���ɂǂꂭ�炢�L�т邩
		for (int leftX = 1; 1; ++leftX)
		{
			int X = StageMgr::Instance()->GetMapChipBlock(stageNum, roomNum, Vec2<float>(MAPCHIP_POS.x - leftX, MAPCHIP_POS.y));
			if (X != chipNumber || CheckUsedData(registChipPos, Vec2<float>(MAPCHIP_POS.x - leftX, MAPCHIP_POS.y)))
			{
				break;
			}
			else
			{
				registChipPos.push_back(Vec2<float>(MAPCHIP_POS.x - leftX, MAPCHIP_POS.y));
				registDirChipPos[LEFT].push_back(Vec2<float>(MAPCHIP_POS.x - leftX, MAPCHIP_POS.y));
			}
		}
		//�E�ɂǂꂭ�炢�L�т邩
		for (int rightX = 1; 1; ++rightX)
		{
			int X = StageMgr::Instance()->GetMapChipBlock(stageNum, roomNum, Vec2<float>(MAPCHIP_POS.x + rightX, MAPCHIP_POS.y));
			if (X != chipNumber || CheckUsedData(registChipPos, Vec2<float>(MAPCHIP_POS.x + rightX, MAPCHIP_POS.y)))
			{
				break;
			}
			else
			{
				registChipPos.push_back(Vec2<float>(MAPCHIP_POS.x + rightX, MAPCHIP_POS.y));
				registDirChipPos[RIGHT].push_back(Vec2<float>(MAPCHIP_POS.x + rightX, MAPCHIP_POS.y));
			}
		}
	}
	//�㉺�ɐL�΂������o�����炱���̕������΂�-----------------------
#pragma endregion

#pragma region �u���b�N�̒ǉ�

	//�ǂ̕����ɐL�т���-----------------------
	//���E
	Vec2<float> leftUp, rightDown;
	sideOrUpDownFlag = false;
	if (registDirChipPos[LEFT].size() || registDirChipPos[RIGHT].size())
	{
		int leftSize = registDirChipPos[LEFT].size() - 1;
		int rightSize = registDirChipPos[RIGHT].size() - 1;

		if (leftSize <= 0)
		{
			leftSize = 0;
		}
		if (rightSize <= 0)
		{
			rightSize = 0;
		}

		//�オ0�Ȃ�
		if (registDirChipPos[LEFT].size() == 0)
		{
			leftUp = { MAPCHIP_POS.x * 50.0f,MAPCHIP_POS.y * 50.0f };
		}
		else
		{
			leftUp = { registDirChipPos[LEFT][leftSize].x * 50.0f,registDirChipPos[LEFT][leftSize].y * 50.0f };
		}
		//����0�Ȃ�
		if (registDirChipPos[RIGHT].size() == 0)
		{
			rightDown = { MAPCHIP_POS.x * 50.0f,MAPCHIP_POS.y * 50.0f };
		}
		else
		{
			rightDown = { registDirChipPos[RIGHT][rightSize].x * 50.0f,registDirChipPos[RIGHT][rightSize].y * 50.0f };
		}

		sideOrUpDownFlag = true;
	}
	//�㉺
	if (registDirChipPos[UP].size() || registDirChipPos[DOWN].size())
	{
		int upSize = registDirChipPos[UP].size() - 1;
		int downSize = registDirChipPos[DOWN].size() - 1;

		if (upSize <= 0)
		{
			upSize = 0;
		}
		if (downSize <= 0)
		{
			downSize = 0;
		}

		//�オ0�Ȃ�
		if (registDirChipPos[UP].size() == 0)
		{
			leftUp = { MAPCHIP_POS.x * 50.0f,MAPCHIP_POS.y * 50.0f };
		}
		else
		{
			leftUp = { registDirChipPos[UP][upSize].x * 50.0f,registDirChipPos[UP][upSize].y * 50.0f };
		}
		//����0�Ȃ�
		if (registDirChipPos[DOWN].size() == 0)
		{
			rightDown = { MAPCHIP_POS.x * 50.0f,MAPCHIP_POS.y * 50.0f };
		}
		else
		{
			rightDown = { registDirChipPos[DOWN][downSize].x * 50.0f,registDirChipPos[DOWN][downSize].y * 50.0f };
		}
		sideOrUpDownFlag = false;
	}

	//�u���b�N�T�C�Y�ɍ����悤�ɒ�������
	leftUp.x -= 25.0f;
	leftUp.y -= 25.0f;
	rightDown.x += 25.0f;
	rightDown.y += 25.0f;
	//�ǂ̕����ɐL�т���-----------------------



	//�L�т������I�[���ɓn��
	leftUpPos = leftUp;
	rightDownPos = rightDown;
#pragma endregion

	return true;
}

const Vec2<float> &MassChip::GetLeftUpPos()
{
	return leftUpPos;
}

const Vec2<float> &MassChip::GetRightDownPos()
{
	return rightDownPos;
}
