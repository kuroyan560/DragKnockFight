#pragma once
#include"../Common/Singleton.h"

class SelectStage:public Singleton<SelectStage>
{
public:
	SelectStage();

	void SelectStageNum(const int &STAGE_NUM)
	{
		stageNum = STAGE_NUM;
	};
	void SelectRoomNum(const int &ROOM_NUM)
	{
		roomNum = ROOM_NUM;
	};

	const int &GetStageNum();
	const int &GetRoomNum();
	bool HaveNextLap();	//次ラップがあるか

	bool resetStageFlag;//ステージをリセットする処理
private:
	int stageNum;	//ステージ番号
	int roomNum;	//エリア番号
};

