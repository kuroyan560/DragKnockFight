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
	//èdï°ÇµÇΩÇÁèàóùÇîÚÇŒÇ∑-----------------------

	int stageNum = SelectStage::Instance()->GetStageNum();
	int roomNum = SelectStage::Instance()->GetRoomNum();

#pragma region è„â∫ç∂âEÇÃíTçı
	registChipPos.push_back(Vec2<float>(MAPCHIP_POS.x, MAPCHIP_POS.y));
	//è„Ç…Ç«ÇÍÇ≠ÇÁÇ¢êLÇ—ÇÈÇ©
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
	//â∫Ç…Ç«ÇÍÇ≠ÇÁÇ¢êLÇ—ÇÈÇ©
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

	//è„â∫Ç…êLÇŒÇ∑éñÇ™èoóàÇΩÇÁÇ±Ç±ÇÃïîï™ÇîÚÇŒÇ∑-----------------------
	if (registDirChipPos[DOWN].size() == 0 && registDirChipPos[UP].size() == 0)
	{
		//ç∂Ç…Ç«ÇÍÇ≠ÇÁÇ¢êLÇ—ÇÈÇ©
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
		//âEÇ…Ç«ÇÍÇ≠ÇÁÇ¢êLÇ—ÇÈÇ©
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
	//è„â∫Ç…êLÇŒÇ∑éñÇ™èoóàÇΩÇÁÇ±Ç±ÇÃïîï™ÇîÚÇŒÇ∑-----------------------
#pragma endregion

#pragma region ÉuÉçÉbÉNÇÃí«â¡

	//Ç«ÇÃï˚å¸Ç…êLÇ—ÇΩÇ©-----------------------
	//ç∂âE
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

		//è„Ç™0Ç»ÇÁ
		if (registDirChipPos[LEFT].size() == 0)
		{
			leftUp = { MAPCHIP_POS.x * 50.0f,MAPCHIP_POS.y * 50.0f };
		}
		else
		{
			leftUp = { registDirChipPos[LEFT][leftSize].x * 50.0f,registDirChipPos[LEFT][leftSize].y * 50.0f };
		}
		//â∫Ç™0Ç»ÇÁ
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
	//è„â∫
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

		//è„Ç™0Ç»ÇÁ
		if (registDirChipPos[UP].size() == 0)
		{
			leftUp = { MAPCHIP_POS.x * 50.0f,MAPCHIP_POS.y * 50.0f };
		}
		else
		{
			leftUp = { registDirChipPos[UP][upSize].x * 50.0f,registDirChipPos[UP][upSize].y * 50.0f };
		}
		//â∫Ç™0Ç»ÇÁ
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

	//ÉuÉçÉbÉNÉTÉCÉYÇ…çáÇ§ÇÊÇ§Ç…í≤êÆÇ∑ÇÈ
	leftUp.x -= 25.0f;
	leftUp.y -= 25.0f;
	rightDown.x += 25.0f;
	rightDown.y += 25.0f;
	//Ç«ÇÃï˚å¸Ç…êLÇ—ÇΩÇ©-----------------------



	//êLÇ—ÇΩèÓïÒÇÉIÅ[ÉâÇ…ìnÇ∑
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
