#include "SelectStage.h"

SelectStage::SelectStage()
{
	stageNum = 0;
	resetStageFlag = false;
}

const int &SelectStage::GetStageNum()
{
	return stageNum;
}
