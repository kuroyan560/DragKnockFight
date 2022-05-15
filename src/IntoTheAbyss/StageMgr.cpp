#include "StageMgr.h"
#include <assert.h>
#include <fstream>
#include <iostream>
#include <sstream>

#include"KuroEngine.h"
#include"TexHandleMgr.h"

StageMgr::StageMgr()
{
	//int allStageNum = 4;
	int allStageNum = 1;
	//int allRoomNum = 10;
	int allRoomNum = 1;
	int nowStage = 0;

	allMapChipData.resize(allStageNum);
	relationRoomData.resize(allStageNum);
	allMapChipDrawData.resize(allStageNum);
	//1~21
	mapChipMemoryData[MAPCHIP_TYPE_STATIC_BLOCK] = { 1,MAPCHIP_TYPE_STATIC_ELEC_ON_ALLWAYS };


	//LoadDivGraph("Resource/chip_sheet.png", 18, 6, 3, 32, 32, mapChipGraphHandle.data());

	//mapChipGraphHandle[MAPCHIP_DRAW_WALL_CORNER] = LoadGraph("Resource/chip_corner.png");
	//mapChipGraphHandle[MAPCHIP_DRAW_WALL_TBLOCK] = LoadGraph("Resource/chip_TBlock2.png");
	//mapChipGraphHandle[MAPCHIP_DRAW_WALL_TBLOCK2] = LoadGraph("Resource/chip_TBlock.png");

	//mapChipGraphHandle[MAPCHIP_DRAW_ERROR] = LoadGraph("Resource/Error.png");
	//mapChipGraphHandle[MAPCHIP_DRAW_WALL_RIGHTDOWN_1PIXEL] = LoadGraph("Resource/Chip_rightDown1Pixel.png");
	//mapChipGraphHandle[MAPCHIP_DRAW_WALL_LEFTDOWN_1PIXEL] = LoadGraph("Resource/Chip_leftDown1Pixel.png");

	TexHandleMgr::LoadDivGraph("resource/IntoTheAbyss/chip_sheet.png", 18, { 6,3 }, &mapChipGraphHandle[0]);

	mapChipGraphHandle[MAPCHIP_DRAW_WALL_CORNER] = TexHandleMgr::LoadGraph("resource/IntoTheAbyss/chip_corner.png");
	mapChipGraphHandle[MAPCHIP_DRAW_WALL_TBLOCK] = TexHandleMgr::LoadGraph("resource/IntoTheAbyss/chip_TBlock2.png");
	mapChipGraphHandle[MAPCHIP_DRAW_WALL_TBLOCK2] = TexHandleMgr::LoadGraph("resource/IntoTheAbyss/chip_TBlock.png");

	//mapChipGraphHandle[MAPCHIP_DRAW_ERROR] = TexHandleMgr::LoadGraph("resource/IntoTheAbyss/Error.png");
	mapChipGraphHandle[MAPCHIP_DRAW_WALL_RIGHTDOWN_1PIXEL] = TexHandleMgr::LoadGraph("resource/IntoTheAbyss/Chip_rightDown1Pixel.png");
	mapChipGraphHandle[MAPCHIP_DRAW_WALL_LEFTDOWN_1PIXEL] = TexHandleMgr::LoadGraph("resource/IntoTheAbyss/Chip_leftDown1Pixel.png");

	thowGraphHandle = TexHandleMgr::LoadGraph("resource/IntoTheAbyss/Thown.png");




	//���̉ӏ�-----------------------

	//��X�͂��̖��ɗ������������{���������܂�
	TexHandleMgr::LoadDivGraph("resource/IntoTheAbyss/EventBlocks.png", 10, { 10,1 }, eventChipHandle.data());

	//���̉ӏ�-----------------------




	vector<int>tmp(6);
	TexHandleMgr::LoadDivGraph("resource/IntoTheAbyss/gate.png", 6, { 6,1 }, &tmp[0]);
	//�A�j���[�V��������ǂݍ���
	animationData.push_back(new MapChipAnimationData(tmp, 10));


	//std::string rootFilePass = "Resource/MapChipData/";
	std::string rootFilePass = "resource/IntoTheAbyss/MapChipData/";
	std::string stageFilePass = "Stage";
	std::string roomFileName = "Room_";


	for (int stageNum = 0; stageNum < allStageNum; ++stageNum)
	{
		for (int roomNum = 0; roomNum < allRoomNum; ++roomNum)
		{
			//�t�@�C���p�X
			std::string filePass =
				rootFilePass + stageFilePass + std::to_string(stageNum) + "/";
			//�t�@�C����
			std::string fileName =
				roomFileName + std::to_string(roomNum) + "_Gimmick.txt";

			GimmickLoader::Instance()->LoadData(stageNum, roomNum, filePass + fileName);
		}
	}

	//�M�~�b�N���ʂ̐ݒ�
	GimmickLoader::Instance()->LoadData(rootFilePass + "GimmickCommonData.txt");


	//�X�e�[�W���̏������ǂݍ���-----------------------
	for (int stageNum = 0; stageNum < allStageNum; ++stageNum)
	{
		for (int roomNum = 0; roomNum < allRoomNum; ++roomNum)
		{
			RoomMapChipArray data;	//�������̓ǂݍ���
			std::string filePass = rootFilePass + stageFilePass + std::to_string(stageNum) + "/";	//Stage�܂ł̃t�@�C���p�X
			//Stage�܂ł̃t�@�C���p�X+�������̃t�@�C���p�X
			loder.CSVLoad(&data, filePass + roomFileName + std::to_string(roomNum) + ".csv");
			//�f�[�^�̒ǉ�
			allMapChipData[stageNum].push_back(data);
		}
	}
	//�X�e�[�W���̏������ǂݍ���-----------------------


	//���������m�̌q�����ǂݍ���-----------------------
	for (int stageNum = 0; stageNum < allStageNum; ++stageNum)
	{
		std::string filePass = rootFilePass + stageFilePass + std::to_string(stageNum) + "/";	//Stage�܂ł̃t�@�C���p�X
		// �t�@�C���f�[�^
		std::ifstream ifs;
		// �t�@�C�����J���B
		ifs.open(filePass + "Relation.txt");
		// �t�@�C�����J���������`�F�b�N����B
		if (ifs.fail())
		{
			//���s
			continue;
			return;
		}

		//�t�@�C���������I��
		string line;

		int roomNum = 0;
		while (getline(ifs, line))
		{
			//��s���̕�������X�g���[���ɕϊ����ĉ�͂��₷���Ȃ�
			istringstream line_stream(line);

			//���p�X�y�[�X��؂�ōs�̐擪��������擾
			string key;//������v��f���̔��f������
			getline(line_stream, key, ' ');


			if (key == "roomNum")
			{
				relationRoomData[stageNum].push_back({});
				++roomNum;
			}
			if (key == "link")
			{
				RelationData tmp;
				line_stream >> tmp.doorNumber;
				line_stream >> tmp.nextRoomNumber;
				relationRoomData[stageNum][roomNum - 1].push_back(tmp);
			}
		}
		ifs.close();
	}
	//���������m�̌q�����ǂݍ���-----------------------




	//�������Ƃ̕`������i�[����-----------------------
	for (int stageNum = 0; stageNum < allMapChipData.size(); ++stageNum)
	{
		allMapChipDrawData[stageNum].resize(allMapChipData[stageNum].size());
		for (int roomNum = 0; roomNum < allMapChipData[stageNum].size(); ++roomNum)
		{
			allMapChipDrawData[stageNum][roomNum].resize(allMapChipData[stageNum][roomNum].size());
			for (int y = 0; y < allMapChipData[stageNum][roomNum].size(); ++y)
			{
				allMapChipDrawData[stageNum][roomNum][y].resize(allMapChipData[stageNum][roomNum][y].size());
			}
		}
	}

	array<int, 10>doorCount;//�h�A�ԍ��̏d�����݂�
	vector<int>startChip;//�ǂ̕����ɃX�^�[�g�p�̃`�b�v���Ă΂�Ă��邩����
	int startChipCount = 0;//�X�^�[�g�p�̃`�b�v���Ăяo���ꂽ�񐔂𐔂��܂�
	array<int, 10>doorCallCount = {};//���ꂼ��̃h�A�ԍ��ŉ���Ăяo���ꂽ��
	array<bool, 10>doorSideCheckFlags = {};//���ꂼ��̃h�A���ǂ̕����Ɍ����Ă��邩

	for (int stageNum = 0; stageNum < allMapChipDrawData.size(); ++stageNum)
	{
		for (int roomNum = 0; roomNum < allMapChipDrawData[stageNum].size(); ++roomNum)
		{
			for (int y = 0; y < allMapChipDrawData[stageNum][roomNum].size(); ++y)
			{
				for (int x = 0; x < allMapChipDrawData[stageNum][roomNum][y].size(); ++x)
				{
					if (mapChipMemoryData[MAPCHIP_TYPE_STATIC_BLOCK].min <= allMapChipData[stageNum][roomNum][y][x] && allMapChipData[stageNum][roomNum][y][x] <= mapChipMemoryData[MAPCHIP_TYPE_STATIC_BLOCK].max)
					{
						MapChipDrawEnum now = static_cast<MapChipDrawEnum>(allMapChipData[stageNum][roomNum][y][x] - 1);
						allMapChipDrawData[stageNum][roomNum][y][x].handle = mapChipGraphHandle[now];
					}
				}
			}
		}
	}
	//�������Ƃ̕`������i�[����-----------------------

	//�g��Ȃ�
	//GimmickLoader::Instance()->ErrorCheck();
}

const RoomMapChipArray &StageMgr::GetMapChipData(const int &STAGE_NUMBER, const int &ROOM_NUMBER)
{
	return allMapChipData[STAGE_NUMBER][ROOM_NUMBER];
}

const int &StageMgr::GetRelationData(const int &STAGE_NUMBER, const int &ROOM_NUMBER, const int &DOOR_NUMBER)
{
	for (int doorIndex = 0; doorIndex < relationRoomData[STAGE_NUMBER][ROOM_NUMBER].size(); ++doorIndex)
	{
		//�ǂ̔��Ȃ̂��T�����āA�ǂ̏������ɍs������Ԃ�
		if (relationRoomData[STAGE_NUMBER][ROOM_NUMBER][doorIndex].doorNumber == DOOR_NUMBER)
		{
			return relationRoomData[STAGE_NUMBER][ROOM_NUMBER][doorIndex].nextRoomNumber;
		}
	}
}

const SizeData StageMgr::GetMapChipSizeData(MapChipData TYPE)
{
	return mapChipMemoryData[TYPE];
}


RoomMapChipDrawArray StageMgr::GetMapChipDrawBlock(const int &STAGE_NUMBER, const int &ROOM_NUMBER)
{
	RoomMapChipDrawArray tmp = allMapChipDrawData[STAGE_NUMBER][ROOM_NUMBER];
	return tmp;
}

const bool &StageMgr::CheckStageNum(const int &STAGE_NUMBER)
{
	if (0 <= STAGE_NUMBER && STAGE_NUMBER < allMapChipDrawData.size())
	{
		return true;
	}
	else
	{
		return false;
	}
}

const bool &StageMgr::CheckRoomNum(const int &STAGE_NUMBER, const int &ROOM_NUMBER)
{
	bool checkStageFlag = 0 <= STAGE_NUMBER && STAGE_NUMBER < allMapChipDrawData.size();//�X�e�[�W�ԍ����z����ɂ��邩�m�F

	if (checkStageFlag)
	{
		bool checkRoomFlag = 0 <= ROOM_NUMBER && ROOM_NUMBER < allMapChipDrawData[STAGE_NUMBER].size();//�����ԍ����z����ɂ��邩�m�F
		bool checkRoomHaveMapChipFlag = allMapChipDrawData[STAGE_NUMBER][ROOM_NUMBER].size() != 0;				   //�����Ƀ}�b�v�`�b�v�����邩�ǂ����m�F
		if (checkRoomFlag && checkRoomHaveMapChipFlag)
		{
			return true;
		}
	}

	return false;
}

void StageMgr::WriteMapChipData(const int &STAGE_NUM, const int &ROOM_NUM, const Vec2<int> MAPCHIP_NUM, const int &CHIPNUM)
{
	if (allMapChipData[STAGE_NUM][ROOM_NUM].size() <= MAPCHIP_NUM.y && allMapChipData[STAGE_NUM][ROOM_NUM][MAPCHIP_NUM.y].size() <= MAPCHIP_NUM.x)
	{
		//�z��O�Q��
		return;
	}
	allMapChipData[STAGE_NUM][ROOM_NUM][MAPCHIP_NUM.y][MAPCHIP_NUM.x] = CHIPNUM;
}

MapChipType StageMgr::GetMapChipType(const int &STAGE_NUM, const int &ROOM_NUM, const Vec2<int> MAPCHIP_NUM)
{
	if (allMapChipData[STAGE_NUM][ROOM_NUM].size() <= MAPCHIP_NUM.y && allMapChipData[STAGE_NUM][ROOM_NUM][MAPCHIP_NUM.y].size() <= MAPCHIP_NUM.x)
	{
		return MAPCHIP_BLOCK_NONE;
	}

	int otherBlockNum = 6;
	int spaceNum = 0;
	if (mapChipMemoryData[MAPCHIP_TYPE_STATIC_BLOCK].min <= allMapChipData[STAGE_NUM][ROOM_NUM][MAPCHIP_NUM.y][MAPCHIP_NUM.x] &&
		allMapChipData[STAGE_NUM][ROOM_NUM][MAPCHIP_NUM.y][MAPCHIP_NUM.x] <= mapChipMemoryData[MAPCHIP_TYPE_STATIC_BLOCK].max - otherBlockNum)
	{
		return MAPCHIP_BLOCK_WALL;
	}
	else if (allMapChipData[STAGE_NUM][ROOM_NUM][MAPCHIP_NUM.y][MAPCHIP_NUM.x] == MAPCHIP_TYPE_STATIC_ELEC_ON)
	{
		return MAPCHIP_BLOCK_ELEC_ON;
	}
	else if (allMapChipData[STAGE_NUM][ROOM_NUM][MAPCHIP_NUM.y][MAPCHIP_NUM.x] == MAPCHIP_TYPE_STATIC_ELEC_OFF)
	{
		return MAPCHIP_BLOCK_ELEC_OFF;
	}
	else if (allMapChipData[STAGE_NUM][ROOM_NUM][MAPCHIP_NUM.y][MAPCHIP_NUM.x] == MAPCHIP_TYPE_STATIC_COLOR_ON)
	{
		return MAPCHIP_BLOCK_COLOR_ON;
	}
	else if (allMapChipData[STAGE_NUM][ROOM_NUM][MAPCHIP_NUM.y][MAPCHIP_NUM.x] == MAPCHIP_TYPE_STATIC_COLOR_OFF)
	{
		return MAPCHIP_BLOCK_COLOR_OFF;
	}
	else if (allMapChipData[STAGE_NUM][ROOM_NUM][MAPCHIP_NUM.y][MAPCHIP_NUM.x] == MAPCHIP_TYPE_STATIC_ELEC_ON_ALLWAYS)
	{
		return MAPCHIP_BLOCK_ELEC_ON_ALLWAYS;
	}
	else if (allMapChipData[STAGE_NUM][ROOM_NUM][MAPCHIP_NUM.y][MAPCHIP_NUM.x] == spaceNum)
	{
		return MAPCHIP_BLOCK_SPACE;
	}
	else if (allMapChipData[STAGE_NUM][ROOM_NUM][MAPCHIP_NUM.y][MAPCHIP_NUM.x] == MAPCHIP_TYPE_STATIC_CHANGE_AREA)
	{
		return MAPCHIP_BLOCK_CHANGE_AREA;
	}
	else
	{
		return MAPCHIP_BLOCK_NONE;
	}
}

bool StageMgr::CheckDoor(vector<Vec2<float>> *DATA, int STAGE_NUM, int ROOM_NUM, Vec2<float> MAPCHIP, int DOOR_NUM)
{
	bool sideFlag = false;

	//��ǂꂭ�炢�L�т邩
	for (int doorY = 1; 1; ++doorY)
	{
		int chip = GetMapChipBlock(STAGE_NUM, ROOM_NUM, { (float)MAPCHIP.x,MAPCHIP.y - (float)doorY });
		if (chip != DOOR_NUM)
		{
			break;
		}
		else
		{
			DATA->push_back({ (float)MAPCHIP.x,MAPCHIP.y - (float)doorY });
		}
	}
	//���ǂꂭ�炢�L�т邩
	for (int doorY = 1; 1; ++doorY)
	{
		int chip = GetMapChipBlock(STAGE_NUM, ROOM_NUM, { (float)MAPCHIP.x,MAPCHIP.y + (float)doorY });
		if (chip != DOOR_NUM)
		{
			break;
		}
		else
		{
			DATA->push_back({ (float)MAPCHIP.x,MAPCHIP.y + (float)doorY });
		}
	}
	//��
	for (int doorX = 1; 1; ++doorX)
	{
		int chip = GetMapChipBlock(STAGE_NUM, ROOM_NUM, { (float)MAPCHIP.x - doorX,MAPCHIP.y });
		if (chip != DOOR_NUM)
		{
			break;
		}
		else
		{
			sideFlag = true;
			DATA->push_back({ (float)MAPCHIP.x - doorX,(float)MAPCHIP.y });
		}
	}
	//�E
	for (int doorX = 1; 1; ++doorX)
	{
		int chip = GetMapChipBlock(STAGE_NUM, ROOM_NUM, { (float)MAPCHIP.x + doorX,(float)MAPCHIP.y });
		if (chip != DOOR_NUM)
		{
			break;
		}
		else
		{
			sideFlag = true;
			DATA->push_back({ (float)MAPCHIP.x + doorX,(float)MAPCHIP.y });
		}
	}

	return sideFlag;
}
