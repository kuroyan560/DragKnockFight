#include "StageMgr.h"
#include <assert.h>
#include <fstream>
#include <iostream>
#include <sstream>

#include"KuroEngine.h"
#include"TexHandleMgr.h"

StageMgr::StageMgr()
{
	std::string rootFilePass = "resource/ChainCombat/MapChipData/";
	std::string stageFilePass = "Stage";
	std::string roomFileName = "Room_";

	int allStageNum = 0;
	while (KuroFunc::ExistDir(rootFilePass + stageFilePass + std::to_string(allStageNum)))
	{
		allStageNum++;
	}

	//int allStageNum = 4;
	//int allRoomNum = 10;
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


	gimmcikGraphHandle[GMMICK_RED] = TexHandleMgr::LoadGraph("resource/IntoTheAbyss/chip_item_red.png");
	gimmcikGraphHandle[GMMICK_GREEN] = TexHandleMgr::LoadGraph("resource/IntoTheAbyss/chip_item_green.png");
	gimmcikGraphHandle[GMMICK_ELEC_OFF] = TexHandleMgr::LoadGraph("resource/IntoTheAbyss/chip_spark_off.png");
	TexHandleMgr::LoadDivGraph("resource/IntoTheAbyss/chip_spark.png", 12, { 12,1 }, sparkGraphHandle.data());


	//���̉ӏ�-----------------------

	//��X�͂��̖��ɗ������������{���������܂�
	TexHandleMgr::LoadDivGraph("resource/IntoTheAbyss/EventBlocks.png", 10, { 10,1 }, eventChipHandle.data());

	//���̉ӏ�-----------------------




	//vector<int>tmp(6);
	//TexHandleMgr::LoadDivGraph("resource/IntoTheAbyss/gate.png", 6, { 6,1 }, tmp.data());
	////�A�j���[�V��������ǂݍ���
	//animationData.push_back(std::make_shared<MapChipAnimationData>(tmp, 10));

	vector<int>tmp2(12);
	TexHandleMgr::LoadDivGraph("resource/IntoTheAbyss/chip_spark.png", 12, { 12,1 }, tmp2.data());
	vector<int>tmp3(1);
	tmp3[0] = tmp2[0];
	animationData.push_back(std::make_shared<MapChipAnimationData>(tmp3, 10));

	//std::string rootFilePass = "Resource/MapChipData/";

	//�M�~�b�N���ʂ̐ݒ�
	//GimmickLoader::Instance()->LoadData(rootFilePass + "GimmickCommonData.txt");


	//�X�e�[�W���̏������ǂݍ���-----------------------
	for (int stageNum = 0; stageNum < allStageNum; ++stageNum)
	{
		std::string stageDir = rootFilePass + "Stage" + std::to_string(stageNum);
		int allRoomNum = 0;
		while (KuroFunc::ExistFile(stageDir + "/" + roomFileName + std::to_string(allRoomNum) + ".csv"))
		{
			allRoomNum++;
		}

		for (int roomNum = 0; roomNum < allRoomNum; ++roomNum)
		{
			RoomMapChipArray data;	//�������̓ǂݍ���
			std::string filePass = stageDir + "/" + roomFileName + std::to_string(roomNum) + ".csv";	//Stage�܂ł̃t�@�C���p�X
			//Stage�܂ł̃t�@�C���p�X+�������̃t�@�C���p�X
			loder.CSVLoad(&data, filePass);
			//�f�[�^�̒ǉ�
			allMapChipData[stageNum].push_back(data);
		}
	}
	//�X�e�[�W���̏������ǂݍ���-----------------------

	swingCount.resize(allStageNum);
	for (int stageNum = 0; stageNum < allStageNum; ++stageNum)
	{
		//�t�@�C���p�X
		std::string filePass =
			rootFilePass + stageFilePass + std::to_string(stageNum) + "/";
		//�t�@�C����
		std::string fileName = filePass + "swingCount.txt";

		const int thisStageRoomCount = allMapChipData[stageNum].size();
		swingCount[stageNum].resize(thisStageRoomCount, 0);	//�U��񂵉\�񐔂O�Ŗ��߂�

		//�t�@�C�������݂��Ȃ��Ȃ炻�������U��񂵂��o���Ȃ��X�e�[�W
		if (!KuroFunc::ExistFile(fileName))
		{
			continue;
		}

		// �t�@�C���f�[�^
		std::ifstream ifs;
		// �t�@�C�����J���B
		ifs.open(fileName);
		// �t�@�C�����J���������`�F�b�N����B
		//assert(ifs.fail());	//���炩�̗��R�Ŏ��s
		//�t�@�C���������I��
		string line;
		while (getline(ifs, line))
		{
			//��s���̕�������X�g���[���ɕϊ����ĉ�͂��₷���Ȃ�
			istringstream line_stream(line);

			//���p�X�y�[�X��؂�ōs�̐擪��������擾
			string key;//������v��f���̔��f������
			getline(line_stream, key, ' ');

			int num = -1;
			for (int roomCount = 0; roomCount < thisStageRoomCount; ++roomCount)
			{
				std::string roomkey = "room" + std::to_string(roomCount);
				if (key == roomkey)
				{
					line_stream >> num;
					swingCount[stageNum][roomCount] = num;
					break;
				}
			}
		}
		ifs.close();
	}


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
					if (mapChipMemoryData[MAPCHIP_TYPE_STATIC_BLOCK].min <= allMapChipData[stageNum][roomNum][y][x] && allMapChipData[stageNum][roomNum][y][x] <= mapChipMemoryData[MAPCHIP_TYPE_STATIC_BLOCK].max - 5)
					{
						MapChipDrawEnum now = static_cast<MapChipDrawEnum>(allMapChipData[stageNum][roomNum][y][x] - 1);
						allMapChipDrawData[stageNum][roomNum][y][x].handle = mapChipGraphHandle[now];
					}
					else if (MAPCHIP_TYPE_STATIC_CHANGE_AREA <= allMapChipData[stageNum][roomNum][y][x] && allMapChipData[stageNum][roomNum][y][x] <= MAPCHIP_TYPE_STATIC_ELEC_ON_ALLWAYS)
					{
						SetGimmickGraphHandle(stageNum, roomNum, Vec2<int>(x, y));
					}
				}
			}
		}
	}
	//�������Ƃ̕`������i�[����-----------------------

	//�g��Ȃ�
	//GimmickLoader::Instance()->ErrorCheck();
}

void StageMgr::SetLocalMapChipData(const int &STAGE_NUMBER, const int &ROOM_NUMBER)
{
	localRoomMapChipArray = allMapChipData[STAGE_NUMBER][ROOM_NUMBER];
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


void StageMgr::SetLocalMapChipDrawBlock(const int &STAGE_NUMBER, const int &ROOM_NUMBER)
{
	localRoomMapChipDrawArray = allMapChipDrawData[STAGE_NUMBER][ROOM_NUMBER];
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

void StageMgr::WriteMapChipData(const Vec2<int> MAPCHIP_NUM, const int &CHIPNUM, const Vec2<float> &LeftCharaPos, const float &LeftCharaSize, const Vec2<float> &RightCharaPos, const float &RightCharaSize)
{
	//�z��O�Q��
	if (MAPCHIP_NUM.y < 0 || localRoomMapChipArray.size() <= MAPCHIP_NUM.y || MAPCHIP_NUM.x < 0 || localRoomMapChipArray[MAPCHIP_NUM.y].size() <= MAPCHIP_NUM.x)
	{
		return;
	}
	if (localRoomMapChipArray[MAPCHIP_NUM.y][MAPCHIP_NUM.x] == CHIPNUM)return;	//�ω��Ȃ�

	//���Ȃ��u���b�N�͔�΂�
	if (localRoomMapChipArray[MAPCHIP_NUM.y][MAPCHIP_NUM.x] == MAPCHIP_TYPE_STATIC_UNBROKEN_BLOCK)
	{
		return;
	}

	// ���ꍇ�͒ʂ��Ȃ��B
	if (CHIPNUM != 0) {

		// �����̃L�����Ɖ~�`�̓����蔻����s���āA�������Ă�����u���b�N�𐶐����Ȃ��B
		Vec2<float> mapChipPos = Vec2<float>(MAPCHIP_NUM.x * MAP_CHIP_SIZE + MAP_CHIP_HALF_SIZE, MAPCHIP_NUM.y * MAP_CHIP_SIZE + MAP_CHIP_HALF_SIZE);
		if (Vec2<float>(mapChipPos - LeftCharaPos).Length() <= MAP_CHIP_SIZE + MAP_CHIP_SIZE + LeftCharaSize) {
			return;
		}

		// �E���̃L�����Ɖ~�`�̓����蔻����s���āA�������Ă�����u���b�N�𐶐����Ȃ��B
		if (Vec2<float>(mapChipPos - RightCharaPos).Length() <= MAP_CHIP_SIZE + MAP_CHIP_SIZE + RightCharaSize) {
			return;
		}

	}


	localRoomMapChipArray[MAPCHIP_NUM.y][MAPCHIP_NUM.x] = CHIPNUM;
	SetLocalGimmickGraphHandle(MAPCHIP_NUM, CHIPNUM);
	int wallChip = 1;
	if (CHIPNUM == wallChip)
	{
		localRoomMapChipDrawArray[MAPCHIP_NUM.y][MAPCHIP_NUM.x].Reset();
		localRoomMapChipDrawArray[MAPCHIP_NUM.y][MAPCHIP_NUM.x].handle = mapChipGraphHandle[0];
	}
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
	else if (allMapChipData[STAGE_NUM][ROOM_NUM][MAPCHIP_NUM.y][MAPCHIP_NUM.x] == MAPCHIP_TYPE_STATIC_BOUNCE_BLOCK)
	{
		return MAPCHIP_BLOCK_ELEC_ON;
	}
	else if (allMapChipData[STAGE_NUM][ROOM_NUM][MAPCHIP_NUM.y][MAPCHIP_NUM.x] == MAPCHIP_TYPE_STATIC_NON_SCORE_BLOCK)
	{
		return MAPCHIP_BLOCK_ELEC_OFF;
	}
	else if (allMapChipData[STAGE_NUM][ROOM_NUM][MAPCHIP_NUM.y][MAPCHIP_NUM.x] == MAPCHIP_TYPE_STATIC_UNBROKEN_BLOCK)
	{
		return MAPCHIP_BLOCK_COLOR_LEFT;
	}
	else if (allMapChipData[STAGE_NUM][ROOM_NUM][MAPCHIP_NUM.y][MAPCHIP_NUM.x] == MAPCHIP_TYPE_STATIC_RARE_BLOCK)
	{
		return MAPCHIP_BLOCK_COLOR_RIGHT;
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

MapChipType StageMgr::GetLocalMapChipType(const Vec2<int> MAPCHIP_NUM)
{
	if (localRoomMapChipArray.size() <= MAPCHIP_NUM.y && localRoomMapChipArray[MAPCHIP_NUM.y].size() <= MAPCHIP_NUM.x)
	{
		return MAPCHIP_BLOCK_NONE;
	}

	int otherBlockNum = 6;
	int spaceNum = 0;
	if (mapChipMemoryData[MAPCHIP_TYPE_STATIC_BLOCK].min <= localRoomMapChipArray[MAPCHIP_NUM.y][MAPCHIP_NUM.x] &&
		localRoomMapChipArray[MAPCHIP_NUM.y][MAPCHIP_NUM.x] <= mapChipMemoryData[MAPCHIP_TYPE_STATIC_BLOCK].max - otherBlockNum)
	{
		return MAPCHIP_BLOCK_WALL;
	}
	else if (localRoomMapChipArray[MAPCHIP_NUM.y][MAPCHIP_NUM.x] == MAPCHIP_TYPE_STATIC_BOUNCE_BLOCK)
	{
		return MAPCHIP_BLOCK_ELEC_ON;
	}
	else if (localRoomMapChipArray[MAPCHIP_NUM.y][MAPCHIP_NUM.x] == MAPCHIP_TYPE_STATIC_NON_SCORE_BLOCK)
	{
		return MAPCHIP_BLOCK_ELEC_OFF;
	}
	else if (localRoomMapChipArray[MAPCHIP_NUM.y][MAPCHIP_NUM.x] == MAPCHIP_TYPE_STATIC_UNBROKEN_BLOCK)
	{
		return MAPCHIP_BLOCK_COLOR_LEFT;
	}
	else if (localRoomMapChipArray[MAPCHIP_NUM.y][MAPCHIP_NUM.x] == MAPCHIP_TYPE_STATIC_RARE_BLOCK)
	{
		return MAPCHIP_BLOCK_COLOR_RIGHT;
	}
	else if (localRoomMapChipArray[MAPCHIP_NUM.y][MAPCHIP_NUM.x] == MAPCHIP_TYPE_STATIC_ELEC_ON_ALLWAYS)
	{
		return MAPCHIP_BLOCK_ELEC_ON_ALLWAYS;
	}
	else if (localRoomMapChipArray[MAPCHIP_NUM.y][MAPCHIP_NUM.x] == spaceNum)
	{
		return MAPCHIP_BLOCK_SPACE;
	}
	else if (localRoomMapChipArray[MAPCHIP_NUM.y][MAPCHIP_NUM.x] == MAPCHIP_TYPE_STATIC_CHANGE_AREA)
	{
		return MAPCHIP_BLOCK_CHANGE_AREA;
	}
	else
	{
		return MAPCHIP_BLOCK_NONE;
	}
}

RoomMapChipArray *StageMgr::GetLocalMap()
{
	return &localRoomMapChipArray;
}

RoomMapChipDrawArray *StageMgr::GetLocalDrawMap()
{
	return &localRoomMapChipDrawArray;
}

int StageMgr::GetAllLocalWallBlocksNum(int RARE_BLOCK_COUNT)
{
	int count = 0;
	for (int y = 0; y < localRoomMapChipArray.size(); ++y)
	{
		for (int x = 0; x < localRoomMapChipArray[y].size(); ++x)
		{
			if (localRoomMapChipArray[y][x] == MAPCHIP_TYPE_STATIC_NON_SCORE_BLOCK)continue;

			bool isWallFlag = mapChipMemoryData[MAPCHIP_TYPE_STATIC_BLOCK].min <= localRoomMapChipArray[y][x] && localRoomMapChipArray[y][x] <= MAPCHIP_TYPE_STATIC_CHANGE_AREA - 1;
			bool isOutSideWall = y == 0 || x == 0 || y == localRoomMapChipArray.size() - 1 || x == localRoomMapChipArray[y].size() - 1;
			bool isRareFlag = localRoomMapChipArray[y][x] == MAPCHIP_TYPE_STATIC_RARE_BLOCK;

			if (isWallFlag && !isOutSideWall)
			{
				++count;
			}
			else if (isRareFlag)
			{
				count += RARE_BLOCK_COUNT;
			}
		}
	}
	return count;
}

int StageMgr::GetAllRoomWallBlocksNum(int STAGE_NUM, int RARE_BLOCK_COUNT)
{
	int count = 0;

	for (int stageIndex = 0; stageIndex < allMapChipData[STAGE_NUM].size(); ++stageIndex)
	{



	}

	return count;
}

int StageMgr::GetAllWallBlocksNum(int STAGE_NUM, int ROOM_NUM)
{
	int count = 0;
	for (int y = 0; y < allMapChipData.size(); ++y)
	{
		for (int x = 0; x < allMapChipData[y].size(); ++x)
		{
			bool isOutSideWall = y == 0 || x == 0 || y == allMapChipData[STAGE_NUM][ROOM_NUM].size() - 1 || x == allMapChipData[STAGE_NUM][ROOM_NUM][y].size() - 1;
			bool isWallFlag = mapChipMemoryData[MAPCHIP_TYPE_STATIC_BLOCK].min <= allMapChipData[STAGE_NUM][ROOM_NUM][y][x] && allMapChipData[STAGE_NUM][ROOM_NUM][y][x] <= MAPCHIP_TYPE_STATIC_CHANGE_AREA - 1;

			if (isWallFlag && !isOutSideWall)
			{
				++count;
			}
		}
	}
	return count;
}

int StageMgr::GetMaxMapChipNum()
{
	int chipNum = localRoomMapChipArray.size() * localRoomMapChipArray[0].size();
	return chipNum;
}

int StageMgr::GetEnableToUseRoomNumber(int STAGE_NUMBER)
{
	int count = 0;
	for (int i = 0; i < allMapChipData[STAGE_NUMBER].size(); ++i)
	{
		if (allMapChipData[STAGE_NUMBER][i].size() != 0)
		{
			++count;
		}
	}
	return count;
}

int StageMgr::GetEnableToUseStageNumber()
{
	int count = 0;
	for (int i = 0; i < allMapChipData.size(); ++i)
	{
		if (allMapChipData[i].size() != 0)
		{
			++count;
		}
	}
	return count;
}

Vec2<float>StageMgr::GetPlayerResponePos(const int &StageNum, const int &RoomNum)
{
	for (int y = 0; y < allMapChipData[StageNum][RoomNum].size(); ++y)
	{
		for (int x = 0; x < allMapChipData[StageNum][RoomNum][y].size(); ++x)
		{
			if (allMapChipData[StageNum][RoomNum][y][x] == MAPCHIP_TYPE_STATIC_RESPONE_PLAYER)
			{
				return Vec2<float>(x * MAP_CHIP_SIZE, y * MAP_CHIP_SIZE);
			}
		}
	}
	return Vec2<float>(0.0f, 0.0f);
}

Vec2<float>StageMgr::GetBossResponePos(const int &StageNum, const int &RoomNum)
{
	for (int y = 0; y < allMapChipData[StageNum][RoomNum].size(); ++y)
	{
		for (int x = 0; x < allMapChipData[StageNum][RoomNum][y].size(); ++x)
		{
			if (allMapChipData[StageNum][RoomNum][y][x] == MAPCHIP_TYPE_STATIC_RESPONE_BOSS)
			{
				return Vec2<float>(x * MAP_CHIP_SIZE, y * MAP_CHIP_SIZE);
			}
		}
	}
	return Vec2<float>(0.0f, 0.0f);
}

int StageMgr::GetSwingCount(int STAGE_NUM, int ROOM_NUM)
{
	return swingCount[STAGE_NUM][ROOM_NUM];
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

void StageMgr::SetGimmickGraphHandle(const int &STAGE_NUM, const int &ROOM_NUM, const Vec2<int> &MAPCHIP_NUM)
{
	//�`��̏�������
	if (GetMapChipType(STAGE_NUM, ROOM_NUM, MAPCHIP_NUM) == MAPCHIP_BLOCK_COLOR_RIGHT)
	{
		allMapChipDrawData[STAGE_NUM][ROOM_NUM][MAPCHIP_NUM.y][MAPCHIP_NUM.x].handle = gimmcikGraphHandle[GMMICK_RED];
	}
	else if (GetMapChipType(STAGE_NUM, ROOM_NUM, MAPCHIP_NUM) == MAPCHIP_BLOCK_COLOR_LEFT)
	{
		allMapChipDrawData[STAGE_NUM][ROOM_NUM][MAPCHIP_NUM.y][MAPCHIP_NUM.x].handle = gimmcikGraphHandle[GMMICK_GREEN];
	}
	else if (GetMapChipType(STAGE_NUM, ROOM_NUM, MAPCHIP_NUM) == MAPCHIP_BLOCK_ELEC_ON)
	{
		allMapChipDrawData[STAGE_NUM][ROOM_NUM][MAPCHIP_NUM.y][MAPCHIP_NUM.x].handle = 0;
		allMapChipDrawData[STAGE_NUM][ROOM_NUM][MAPCHIP_NUM.y][MAPCHIP_NUM.x].animationFlag = true;
		allMapChipDrawData[STAGE_NUM][ROOM_NUM][MAPCHIP_NUM.y][MAPCHIP_NUM.x].interval = 5;
	}
	else if (GetMapChipType(STAGE_NUM, ROOM_NUM, MAPCHIP_NUM) == MAPCHIP_BLOCK_ELEC_OFF)
	{
		allMapChipDrawData[STAGE_NUM][ROOM_NUM][MAPCHIP_NUM.y][MAPCHIP_NUM.x].handle = gimmcikGraphHandle[GMMICK_ELEC_OFF];
	}
	else if (GetMapChipType(STAGE_NUM, ROOM_NUM, MAPCHIP_NUM) == MAPCHIP_BLOCK_ELEC_ON_ALLWAYS)
	{
		allMapChipDrawData[STAGE_NUM][ROOM_NUM][MAPCHIP_NUM.y][MAPCHIP_NUM.x].handle = 0;
		allMapChipDrawData[STAGE_NUM][ROOM_NUM][MAPCHIP_NUM.y][MAPCHIP_NUM.x].animationFlag = true;
		allMapChipDrawData[STAGE_NUM][ROOM_NUM][MAPCHIP_NUM.y][MAPCHIP_NUM.x].interval = 5;
	}
	else
	{
		allMapChipDrawData[STAGE_NUM][ROOM_NUM][MAPCHIP_NUM.y][MAPCHIP_NUM.x].Reset();
	}
}
