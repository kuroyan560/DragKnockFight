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
	bool HaveNextLap();	//�����b�v�����邩

	bool resetStageFlag;//�X�e�[�W�����Z�b�g���鏈��
private:
	int stageNum;	//�X�e�[�W�ԍ�
	int roomNum;	//�G���A�ԍ�
};

