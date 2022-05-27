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

#include"StageMgr.h"
bool SelectStage::HaveNextLap()
{
	return StageMgr::Instance()->HaveMap(SelectStage::Instance()->GetStageNum(), roomNum + 1);
}
