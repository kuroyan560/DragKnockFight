#include "SelectStage.h"

SelectStage::SelectStage()
{
	stageNum = 0;
	roomNum = 0;
	resetStageFlag = false;
}

const int &SelectStage::GetStageNum()
{
	return stageNum;
}

const int &SelectStage::GetRoomNum()
{
	return roomNum;
}
