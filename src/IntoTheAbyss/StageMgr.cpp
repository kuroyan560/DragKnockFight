#include "StageMgr.h"
#include <assert.h>
#include <fstream>
#include <iostream>
#include <sstream>

#include"KuroEngine.h"
#include"TexHandleMgr.h"

StageMgr::StageMgr()
{
	int allStageNum = 1;
	int allRoomNum = 10;
	int nowStage = 0;

	loadGimmickData = std::make_unique<GimmickLoader>(allStageNum);

	allMapChipData.resize(allStageNum);
	relationRoomData.resize(allStageNum);
	allMapChipDrawData.resize(allStageNum);

	mapChipMemoryData[MAPCHIP_TYPE_STATIC_BLOCK] = { 0,9 };
	mapChipMemoryData[MAPCHIP_TYPE_MOVING_BLOCK] = { 10,19 };
	mapChipMemoryData[MAPCHIP_TYPE_DOOR] = { 20,29 };
	mapChipMemoryData[MAPCHIP_TYPE_TOCH] = { 30,39 };
	mapChipMemoryData[MAPCHIP_TYPE_THOWNP] = { 50,79 };
	mapChipMemoryData[MAPCHIP_TYPE_BUBBLE] = { 80,89 };


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

	mapChipGraphHandle[MAPCHIP_DRAW_ERROR] = TexHandleMgr::LoadGraph("resource/IntoTheAbyss/Error.png");
	mapChipGraphHandle[MAPCHIP_DRAW_WALL_RIGHTDOWN_1PIXEL] = TexHandleMgr::LoadGraph("resource/IntoTheAbyss/Chip_rightDown1Pixel.png");
	mapChipGraphHandle[MAPCHIP_DRAW_WALL_LEFTDOWN_1PIXEL] = TexHandleMgr::LoadGraph("resource/IntoTheAbyss/Chip_leftDown1Pixel.png");



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

			loadGimmickData->LoadData(stageNum, roomNum, filePass + fileName);
		}
	}

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
			//�G���A���ς�邲�ƂɃh�A�ԍ��Ǘ��̕ϐ�������������
			vector<Vec2<float>>doorMapChip;
			doorCount[0] = 0;
			doorCount[1] = 0;
			for (int i = 0; i < doorCallCount.size(); ++i)
			{
				doorCallCount[i] = 0;
			}
	

			for (int y = 0; y < allMapChipDrawData[stageNum][roomNum].size(); ++y)
			{
				for (int x = 0; x < allMapChipDrawData[stageNum][roomNum][y].size(); ++x)
				{
					//�����̎���8�}�X�̃}�b�v�`�b�v�ԍ�����ɓ����
					int now = GetMapChipBlock(stageNum, roomNum, { (float)x,	 (float)y });
					int Up = GetMapChipBlock(stageNum, roomNum, { (float)x,		 (float)y - 1 });
					int Down = GetMapChipBlock(stageNum, roomNum, { (float)x,	 (float)y + 1 });
					int left = GetMapChipBlock(stageNum, roomNum, { (float)x - 1,(float)y });
					int right = GetMapChipBlock(stageNum, roomNum, { (float)x + 1,(float)y });
					int LeftUp = GetMapChipBlock(stageNum, roomNum, { (float)x - 1,(float)y - 1 });
					int LeftDown = GetMapChipBlock(stageNum, roomNum, { (float)x - 1,(float)y + 1 });
					int rightUp = GetMapChipBlock(stageNum, roomNum, { (float)x + 1,(float)y - 1 });
					int rightDown = GetMapChipBlock(stageNum, roomNum, { (float)x + 1,(float)y + 1 });



					bool skipFlag = false;		//�h�A����T���������ǂ����������t���O
					for (int doorNumber = mapChipMemoryData[MAPCHIP_TYPE_DOOR].min; doorNumber < mapChipMemoryData[MAPCHIP_TYPE_DOOR].max; ++doorNumber)
					{
						int arrayNum = doorNumber - mapChipMemoryData[MAPCHIP_TYPE_DOOR].min;//�h�A�ԍ�����z��̓Y�����p�ɐ��l�𒲐�

						if (now == doorNumber)
						{
							bool correctFlag = false;
							for (int i = 0; i < doorMapChip.size(); ++i)
							{
								if (doorMapChip[i].x == x && doorMapChip[i].y == y)
								{
									correctFlag = true;
								}
							}
							if (!correctFlag)
							{
								//������������W��o�^����
								doorMapChip.push_back(Vec2<float>(x, y));
								doorSideCheckFlags[arrayNum] = CheckDoor(&doorMapChip, stageNum, roomNum, { (float)x,(float)y }, doorNumber);
								++doorCount[arrayNum];
							}

							if (2 <= doorCount[arrayNum])
							{
								DoorError(stageNum, roomNum, doorNumber);
							}
						}

						if (now == 22)
						{
							bool debug = false;
						}


						//�h�A�����
						if (now == doorNumber && doorSideCheckFlags[arrayNum] && Up == 0)
						{
							MapChipDrawData tmp;

							tmp.animationFlag = true;
							tmp.handle = 0;
							tmp.radian = Radian(0.0f);
							tmp.offset.x = 2.0f * doorCallCount[arrayNum];
							tmp.offset.y = -50.0f;
							allMapChipDrawData[stageNum][roomNum][y][x] = tmp;
							skipFlag = true;

							++doorCallCount[arrayNum];
						}
						//�h�A������
						if (now == doorNumber && doorSideCheckFlags[arrayNum] && Down == 0)
						{
							MapChipDrawData tmp;

							tmp.animationFlag = true;
							tmp.handle = 0;
							tmp.radian = Radian(180.0f);
							tmp.offset.x = 2.0f * doorCallCount[arrayNum];
							tmp.offset.y = 50.0f;
							allMapChipDrawData[stageNum][roomNum][y][x] = tmp;
							skipFlag = true;

							++doorCallCount[arrayNum];
						}
						//�h�A������
						if (now == doorNumber && !doorSideCheckFlags[arrayNum] && left == 0)
						{
							MapChipDrawData tmp;

							tmp.animationFlag = true;
							tmp.handle = 0;
							tmp.radian = Radian(270.0f);
							tmp.offset.x = -50.0f;
							tmp.offset.y = 2.0f * doorCallCount[arrayNum];
							allMapChipDrawData[stageNum][roomNum][y][x] = tmp;
							skipFlag = true;

							++doorCallCount[arrayNum];
						}
						//�h�A�E����
						if (now == doorNumber && !doorSideCheckFlags[arrayNum] && right == 0)
						{
							MapChipDrawData tmp;

							tmp.animationFlag = true;
							tmp.handle = 0;
							tmp.radian = Radian(90.0f);
							tmp.offset.x = 50.0f;
							tmp.offset.y = 2.0f * doorCallCount[arrayNum];
							allMapChipDrawData[stageNum][roomNum][y][x] = tmp;
							skipFlag = true;

							++doorCallCount[arrayNum];
						}
					}




					if (now == MAPCHIP_BLOCK_START)
					{
						startChip.push_back(roomNum);
						++startChipCount;
					}
					if (2 <= startChipCount)
					{
						vector<string> roomNumber;
						for (int i = 0; i < startChip.size(); ++i)
						{
							roomNumber.push_back(string("Room�ԍ�" + std::to_string(startChip[i])));
						}
						string result = roomNumber[0] + "��" + roomNumber[1] + "�ɃX�^�[�g�p�̃}�b�v�`�b�v�����ߍ��܂�Ă��܂��B\n�X�^�[�g�p�̃`�b�v�̓X�e�[�W���Ɉ�ɂ��ĉ�����";
						MessageBox(NULL, KuroFunc::GetWideStrFromStr(result).c_str(), TEXT("�X�^�[�g�ԍ����"), MB_OK);
						assert(0);
					}


					//�h�b�X���̏ꍇ
					if (mapChipMemoryData[MAPCHIP_TYPE_THOWNP].min <= now && now <= mapChipMemoryData[MAPCHIP_TYPE_THOWNP].max)
					{
						//���̔ԍ����J�n�ʒu���I���ʒu�����ׂ�
						int gimmickNumber = now - mapChipMemoryData[MAPCHIP_TYPE_THOWNP].min;

						//�����Ȃ�J�n�ʒu�A��Ȃ�I���ʒu�Ƃ���
						if (gimmickNumber % 2 == 0)
						{
							loadGimmickData->SetThwompStartPos(stageNum, roomNum, gimmickNumber, Vec2<float>(x * 50.0f, y * 50.0f));
						}
						else
						{
							loadGimmickData->SetThwompEndPos(stageNum, roomNum, gimmickNumber, Vec2<float>(x * 50.0f, y * 50.0f));
						}
						skipFlag = true;
					}

					//�o�u���̏ꍇ
					if (mapChipMemoryData[MAPCHIP_TYPE_BUBBLE].min <= now && now <= mapChipMemoryData[MAPCHIP_TYPE_BUBBLE].max)
					{
						int gimmickNumber = now - mapChipMemoryData[MAPCHIP_TYPE_BUBBLE].min;
						loadGimmickData->PushBubbleData(stageNum, roomNum, gimmickNumber, Vec2<float>(x * 50.0f, y * 50.0f));
						skipFlag = true;
					}



					//�}�b�v�`�b�v�̒l��0��1�ɍi��
					AlimentSpaceNumber(&Up);
					AlimentSpaceNumber(&Down);
					AlimentSpaceNumber(&left);
					AlimentSpaceNumber(&right);
					AlimentSpaceNumber(&LeftUp);
					AlimentSpaceNumber(&LeftDown);
					AlimentSpaceNumber(&rightUp);
					AlimentSpaceNumber(&rightDown);
					AlimentWallNumber(&Up);
					AlimentWallNumber(&Down);
					AlimentWallNumber(&left);
					AlimentWallNumber(&right);
					AlimentWallNumber(&LeftUp);
					AlimentWallNumber(&LeftDown);
					AlimentWallNumber(&rightUp);
					AlimentWallNumber(&rightDown);


					for (int wallChipIndex = 1; wallChipIndex < 10; ++wallChipIndex)
					{

						//�h�A��������X�L�b�v
						if (skipFlag)
						{
							continue;
						}


						//�E�����ǂ�����
						{
							bool checkUpSideFlag =
								Up == 0 && Down == 0 && left == 0 && right == wallChipIndex;
							if (checkUpSideFlag)
							{
								MapChipDrawData tmp;
								tmp.handle = mapChipGraphHandle[MAPCHIP_DRAW_WALL_TOP];
								tmp.radian = Radian(270.0f);
								allMapChipDrawData[stageNum][roomNum][y][x] = tmp;
							}
						}
						//�������ǂ�����
						{
							bool checkUpSideFlag =
								Up == 0 && Down == 0 && left == wallChipIndex && right == 0;
							if (checkUpSideFlag)
							{
								MapChipDrawData tmp;
								tmp.handle = mapChipGraphHandle[MAPCHIP_DRAW_WALL_TOP];
								tmp.radian = Radian(90.0f);
								allMapChipDrawData[stageNum][roomNum][y][x] = tmp;
							}
						}


						//�󒆏�
						if (Up == 0 && Down == 0 && left == 0 && right == 0 && now != 2)
						{
							MapChipDrawData tmp;
							tmp.handle = mapChipGraphHandle[MAPCHIP_DRAW_WALL_IN];
							//allMapChipDrawData[stageNum][roomNum][y][x] = tmp;
						}

						//�ʏ��
						{
							bool checkUpSideFlag =
								Up == 0 && Down == 0 && left == wallChipIndex && right == wallChipIndex;
							if (checkUpSideFlag)
							{
								MapChipDrawData tmp;
								tmp.handle = mapChipGraphHandle[MAPCHIP_DRAW_WALL_CENTRAL];
								tmp.radian = Radian(90.0f);
								allMapChipDrawData[stageNum][roomNum][y][x] = tmp;
							}
						}


						{
							bool checkUpSideFlag =
								Up == wallChipIndex && Down == 0 && left == wallChipIndex && right == wallChipIndex;
							bool checkObliaueFlag =
								LeftUp == wallChipIndex && rightUp == wallChipIndex;
							//���
							if (checkUpSideFlag && checkObliaueFlag)
							{
								MapChipDrawData tmp;
								tmp.handle = mapChipGraphHandle[MAPCHIP_DRAW_WALL_DOWN];
								allMapChipDrawData[stageNum][roomNum][y][x] = tmp;
							}
						}

						{
							bool checkUpSideFlag =
								Up == 0 && Down == wallChipIndex && left == wallChipIndex && right == wallChipIndex;
							bool checkObliaueFlag =
								LeftDown == wallChipIndex && rightDown == wallChipIndex;
							//����
							if (checkUpSideFlag && checkObliaueFlag)
							{
								MapChipDrawData tmp;
								tmp.handle = mapChipGraphHandle[MAPCHIP_DRAW_FLOOR];
								allMapChipDrawData[stageNum][roomNum][y][x] = tmp;
							}
						}

						{
							bool checkUpSideFlag =
								Up == wallChipIndex && Down == wallChipIndex && left == wallChipIndex && right == 0;
							bool checkObliaueFlag =
								LeftUp == wallChipIndex && LeftDown == wallChipIndex;

							//����
							if (checkUpSideFlag && checkObliaueFlag)
							{
								MapChipDrawData tmp;
								tmp.handle = mapChipGraphHandle[MAPCHIP_DRAW_WALL_RIGHT];
								allMapChipDrawData[stageNum][roomNum][y][x] = tmp;
							}
						}

						{
							bool checkUpSideFlag =
								Up == wallChipIndex && Down == wallChipIndex && left == 0 && right == wallChipIndex;
							bool checkObliaueFlag =
								rightUp == wallChipIndex && rightDown == wallChipIndex;

							//�E��
							if (checkUpSideFlag && checkObliaueFlag)
							{
								MapChipDrawData tmp;
								tmp.handle = mapChipGraphHandle[MAPCHIP_DRAW_WALL_LEFT];
								allMapChipDrawData[stageNum][roomNum][y][x] = tmp;
							}
						}


						{
							bool checkUpSideFlag =
								Up == 0 && Down == wallChipIndex && left == 0 && right == 0;
							bool checkObliaueFlag =
								LeftUp == 0 && LeftDown == 0 && rightUp == 0 && rightDown == 0;

							//�オ��Ԃ̕�
							if (checkUpSideFlag && checkObliaueFlag)
							{
								MapChipDrawData tmp;
								tmp.handle = mapChipGraphHandle[MAPCHIP_DRAW_WALL_TOP];
								allMapChipDrawData[stageNum][roomNum][y][x] = tmp;
							}
						}


						{
							bool checkUpSideFlag =
								Up == wallChipIndex && Down == 0 && left == 0 && right == 0;
							bool checkObliaueFlag =
								LeftUp == 0 && LeftDown == 0 && rightUp == 0 && rightDown == 0;

							//�オ��Ԃ̕�
							if (checkUpSideFlag && checkObliaueFlag)
							{
								MapChipDrawData tmp;
								tmp.handle = mapChipGraphHandle[MAPCHIP_DRAW_WALL_BUTTOM];
								tmp.radian = Radian(90.0f);
								allMapChipDrawData[stageNum][roomNum][y][x] = tmp;
							}
						}

						{
							bool checkUpSideFlag =
								Up == 0 && Down == wallChipIndex && left == wallChipIndex && right == 0;
							bool checkObliaueFlag =
								LeftDown == wallChipIndex;
							//�E���
							if (checkUpSideFlag && checkObliaueFlag)
							{
								MapChipDrawData tmp;
								tmp.handle = mapChipGraphHandle[MAPCHIP_DRAW_WALL_RIGHT_UP];
								allMapChipDrawData[stageNum][roomNum][y][x] = tmp;
							}
						}

						{
							bool checkUpSideFlag =
								Up == wallChipIndex && Down == 0 && left == wallChipIndex && right == 0;
							bool checkObliaueFlag =
								LeftUp == wallChipIndex;
							//�E����
							if (checkUpSideFlag && checkObliaueFlag)
							{
								MapChipDrawData tmp;
								tmp.handle = mapChipGraphHandle[MAPCHIP_DRAW_WALL_RIGHT_DOWN];
								allMapChipDrawData[stageNum][roomNum][y][x] = tmp;
							}
						}

						{
							bool checkUpSideFlag =
								Up == 0 && Down == wallChipIndex && left == 0 && right == wallChipIndex;
							//�����
							if (checkUpSideFlag)
							{
								MapChipDrawData tmp;
								tmp.handle = mapChipGraphHandle[MAPCHIP_DRAW_WALL_LEFT_UP];
								allMapChipDrawData[stageNum][roomNum][y][x] = tmp;
							}
						}

						{
							bool checkUpSideFlag =
								Up == wallChipIndex && Down == 0 && left == 0 && right == wallChipIndex;
							bool checkObliaueFlag =
								rightUp == wallChipIndex;
							//������
							if (checkUpSideFlag)
							{
								MapChipDrawData tmp;
								tmp.handle = mapChipGraphHandle[MAPCHIP_DRAW_WALL_LEFT_DOWN];
								allMapChipDrawData[stageNum][roomNum][y][x] = tmp;
							}
						}

						{
							bool checkUpSideFlag =
								Up == wallChipIndex && Down == wallChipIndex && left == wallChipIndex && right == wallChipIndex;
							bool checkObliaueFlag =
								LeftUp == wallChipIndex && LeftDown == wallChipIndex && rightUp == wallChipIndex && rightDown == wallChipIndex;
							bool outSideFlag = Up == -1 && Down == -1 && left == -1 && right == -1 &&
								LeftUp == -1 && LeftDown == -1 && rightUp == -1 && rightDown == -1;

							//�ǂ̒�
							if ((checkUpSideFlag && checkObliaueFlag) || outSideFlag)
							{
								MapChipDrawData tmp;
								tmp.handle = mapChipGraphHandle[MAPCHIP_DRAW_WALL_IN];
								allMapChipDrawData[stageNum][roomNum][y][x] = tmp;
							}
						}


						//�E��
						{
							bool checkUpSideFlag =
								Up == wallChipIndex && Down == wallChipIndex && left == wallChipIndex && right == wallChipIndex;
							bool checkObliaueFlag =
								LeftUp == wallChipIndex && LeftDown == 0 && rightUp == wallChipIndex && rightDown == wallChipIndex;
							bool outSideFlag = Up == -1 && Down == wallChipIndex && left == wallChipIndex && right == -1 &&
								LeftUp == -1 && LeftDown == 0 && rightUp == -1 && rightDown == -1;

							if ((checkUpSideFlag && checkObliaueFlag) || outSideFlag)
							{
								MapChipDrawData tmp;
								tmp.handle = mapChipGraphHandle[MAPCHIP_DRAW_WALL_IN_LEFT_DOWN];
								allMapChipDrawData[stageNum][roomNum][y][x] = tmp;
							}
						}
						//�E��
						{
							bool checkUpSideFlag =
								Up == wallChipIndex && Down == wallChipIndex && left == wallChipIndex && right == wallChipIndex;
							bool checkObliaueFlag =
								LeftUp == 0 && LeftDown == wallChipIndex && rightUp == wallChipIndex && rightDown == wallChipIndex;
							bool outSideFlag = Up == wallChipIndex && Down == -1 && left == wallChipIndex && right == -1 &&
								LeftUp == 0 && LeftDown == -1 && rightUp == -1 && rightDown == -1;


							if ((checkUpSideFlag && checkObliaueFlag) || outSideFlag)
							{
								MapChipDrawData tmp;
								tmp.handle = mapChipGraphHandle[MAPCHIP_DRAW_WALL_IN_LEFT_UP];
								allMapChipDrawData[stageNum][roomNum][y][x] = tmp;
							}
						}
						//����
						{
							bool checkUpSideFlag =
								Up == wallChipIndex && Down == wallChipIndex && left == wallChipIndex && right == wallChipIndex;
							bool checkObliaueFlag =
								LeftUp == wallChipIndex && LeftDown == wallChipIndex && rightUp == wallChipIndex && rightDown == 0;
							bool outSideFlag = Up == -1 && Down == wallChipIndex && left == -1 && right == wallChipIndex &&
								LeftUp == -1 && LeftDown == -1 && rightUp == -1 && rightDown == 0;


							if ((checkUpSideFlag && checkObliaueFlag) || outSideFlag)
							{
								MapChipDrawData tmp;
								tmp.handle = mapChipGraphHandle[MAPCHIP_DRAW_WALL_IN_RIGHT_DOWN];
								allMapChipDrawData[stageNum][roomNum][y][x] = tmp;
							}
						}
						//����
						{
							bool checkUpSideFlag =
								Up == wallChipIndex && Down == wallChipIndex && left == wallChipIndex && right == wallChipIndex;
							bool checkObliaueFlag =
								LeftUp == wallChipIndex && LeftDown == wallChipIndex && rightUp == 0 && rightDown == wallChipIndex;
							bool outSideFlag = Up == wallChipIndex && Down == -1 && left == -1 && right == wallChipIndex &&
								LeftUp == -1 && LeftDown == -1 && rightUp == 0 && rightDown == -1;

							if ((checkUpSideFlag && checkObliaueFlag) || outSideFlag)
							{
								MapChipDrawData tmp;
								tmp.handle = mapChipGraphHandle[MAPCHIP_DRAW_WALL_IN_RIGHT_UP];
								allMapChipDrawData[stageNum][roomNum][y][x] = tmp;
							}
						}



						//��p�̏���
						//���E�ǁA��Ƀh�A�A���͋󒆂̏ꍇ�ɉ��ǂ�`�悷��
						if (Up == 3 && Down == 0 && left != 0 && right != 0)
						{
							MapChipDrawData tmp;
							tmp.handle = mapChipGraphHandle[MAPCHIP_DRAW_FLOOR];
							tmp.radian = 0;
							allMapChipDrawData[stageNum][roomNum][y][x] = tmp;
						}


						//��T�H�n�Ȃ�ǒ��̉摜���g��
						{
							bool checkUpSideFlag =
								Up == wallChipIndex && Down == 0 && left == wallChipIndex && right == wallChipIndex;
							bool checkObliaueFlag =
								LeftUp == 0 && rightUp == 0;

							if (checkUpSideFlag && checkObliaueFlag)
							{
								MapChipDrawData tmp;
								tmp.handle = mapChipGraphHandle[MAPCHIP_DRAW_WALL_TBLOCK];
								tmp.radian = Radian(180.0f);
								allMapChipDrawData[stageNum][roomNum][y][x] = tmp;
							}
						}

						{
							bool checkUpSideFlag =
								Up == wallChipIndex && Down == wallChipIndex && left == wallChipIndex && right == wallChipIndex;
							bool checkObliaueFlag =
								LeftUp == wallChipIndex && LeftDown == 0 && rightUp == wallChipIndex && rightDown == 0;
							bool outSideFlag =
								Up == -1 && Down == wallChipIndex && left == wallChipIndex && right == wallChipIndex &&
								LeftUp == -1 && LeftDown == 0 && rightUp == -1 && rightDown == 0;

							//��T�H�n�Ȃ�ǒ��̉摜���g��
							if ((checkUpSideFlag && checkObliaueFlag) || outSideFlag)
							{
								MapChipDrawData tmp;
								tmp.handle = mapChipGraphHandle[MAPCHIP_DRAW_WALL_TBLOCK];
								tmp.radian = 0.0f;
								allMapChipDrawData[stageNum][roomNum][y][x] = tmp;
							}
						}

						{
							bool checkUpSideFlag =
								Up == wallChipIndex && Down == wallChipIndex && left == wallChipIndex && right == wallChipIndex;
							bool checkObliaueFlag =
								LeftUp == 0 && LeftDown == 0 && rightUp == wallChipIndex && rightDown == wallChipIndex;
							bool outSideFlag =
								Up == wallChipIndex && Down == wallChipIndex && left == wallChipIndex && right == -1 &&
								LeftUp == 0 && LeftDown == 0 && rightUp == -1 && rightDown == -1;

							//�ET�H�n�Ȃ�ǒ��̉摜���g��
							if ((checkUpSideFlag && checkObliaueFlag) || outSideFlag)
							{
								MapChipDrawData tmp;
								tmp.handle = mapChipGraphHandle[MAPCHIP_DRAW_WALL_TBLOCK];
								tmp.radian = Radian(270.0f);
								allMapChipDrawData[stageNum][roomNum][y][x] = tmp;
							}
						}

						{
							bool checkUpSideFlag =
								Up == wallChipIndex && Down == wallChipIndex && left == wallChipIndex && right == wallChipIndex;
							bool checkObliaueFlag =
								LeftUp == wallChipIndex && LeftDown == wallChipIndex && rightUp == 0 && rightDown == 0;
							bool outSideFlag =
								Up == wallChipIndex && Down == wallChipIndex && left == -1 && right == wallChipIndex &&
								LeftUp == -1 && LeftDown == -1 && rightUp == 0 && rightDown == 0;

							//��T�H�n�Ȃ�ǒ��̉摜���g��
							if ((checkUpSideFlag && checkObliaueFlag) || outSideFlag)
							{
								MapChipDrawData tmp;
								tmp.handle = mapChipGraphHandle[MAPCHIP_DRAW_WALL_TBLOCK];
								tmp.radian = Radian(90.0f);
								allMapChipDrawData[stageNum][roomNum][y][x] = tmp;
							}
						}


						{
							bool checkUpSideFlag =
								Up == 0 && Down == wallChipIndex && left == wallChipIndex && right == wallChipIndex;
							bool checkObliaueFlag =
								LeftUp == 0 && LeftDown == 0 && rightUp == 0 && rightDown == wallChipIndex;
							if (checkUpSideFlag && checkObliaueFlag)
							{
								MapChipDrawData tmp;
								tmp.handle = mapChipGraphHandle[MAPCHIP_DRAW_WALL_LEFTDOWN_1PIXEL];
								allMapChipDrawData[stageNum][roomNum][y][x] = tmp;
							}
						}

						{
							bool checkUpSideFlag =
								Up == wallChipIndex && Down == 0 && left == wallChipIndex && right == wallChipIndex;
							bool checkObliaueFlag =
								LeftUp == wallChipIndex && LeftDown == 0 && rightUp == 0 && rightDown == 0;
							if (checkUpSideFlag && checkObliaueFlag)
							{
								MapChipDrawData tmp;
								tmp.handle = mapChipGraphHandle[MAPCHIP_DRAW_WALL_LEFTDOWN_1PIXEL];
								tmp.radian = Radian(180.0f);
								allMapChipDrawData[stageNum][roomNum][y][x] = tmp;
							}
						}



						if (Up == 1 && Down == 1 && left == -1 && right == 1)
						{
							MapChipDrawData tmp;
							tmp.handle = mapChipGraphHandle[MAPCHIP_DRAW_WALL_TBLOCK2];
							tmp.radian = Radian(0.0f);
							//allMapChipDrawData[stageNum][roomNum][y][x] = tmp;
						}


						if (Up == 40 && Down == 1 && left == 1 && right == 0)
						{
							MapChipDrawData tmp;
							tmp.handle = mapChipGraphHandle[MAPCHIP_DRAW_WALL_CORNER];
							tmp.radian = Radian(180.0f);
							//allMapChipDrawData[stageNum][roomNum][y][x] = tmp;
						}

						//�A�j���[�V�����e�X�g�p
						if (false)
						{
							MapChipDrawData tmp;
							tmp.animationFlag = true;
							tmp.handle = 0;
							allMapChipDrawData[stageNum][roomNum][y][x] = tmp;
						}

						if (allMapChipDrawData[stageNum][roomNum][y][x].handle == -1)
						//if (!allMapChipDrawData[stageNum][roomNum][y][x].handle)
						{
							//�G���[
							MapChipDrawData tmp;
							tmp.handle = mapChipGraphHandle[MAPCHIP_DRAW_ERROR];
							allMapChipDrawData[stageNum][roomNum][y][x] = tmp;
						}

					}
				}
			}
		}
	}
	//�������Ƃ̕`������i�[����-----------------------




	bool debug = false;
	debug = true;
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


RoomMapChipDrawArray StageMgr::GetMapChipDrawBlock(const int &STAGE_NUMBER, const int &ROOM_NUMBER, const Vec2<float> &MAPCHIP_POS)
{
	RoomMapChipDrawArray tmp = allMapChipDrawData[STAGE_NUMBER][ROOM_NUMBER];
	return tmp;
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
