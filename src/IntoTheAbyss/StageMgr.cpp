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


	//問題の箇所-----------------------

	//我々はこの問題に立ち向かう橋本を応援します
	TexHandleMgr::LoadDivGraph("resource/IntoTheAbyss/EventBlocks.png", 10, { 10,1 }, eventChipHandle.data());

	//問題の箇所-----------------------




	//vector<int>tmp(6);
	//TexHandleMgr::LoadDivGraph("resource/IntoTheAbyss/gate.png", 6, { 6,1 }, tmp.data());
	////アニメーション情報を読み込み
	//animationData.push_back(std::make_shared<MapChipAnimationData>(tmp, 10));

	vector<int>tmp2(12);
	TexHandleMgr::LoadDivGraph("resource/IntoTheAbyss/chip_spark.png", 12, { 12,1 }, tmp2.data());
	vector<int>tmp3(1);
	tmp3[0] = tmp2[0];
	animationData.push_back(std::make_shared<MapChipAnimationData>(tmp3, 10));

	//std::string rootFilePass = "Resource/MapChipData/";

	//ギミック共通の設定
	//GimmickLoader::Instance()->LoadData(rootFilePass + "GimmickCommonData.txt");


	//ステージ毎の小部屋読み込み-----------------------
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
			RoomMapChipArray data;	//小部屋の読み込み
			std::string filePass = stageDir + "/" + roomFileName + std::to_string(roomNum) + ".csv";	//Stageまでのファイルパス
			//Stageまでのファイルパス+小部屋のファイルパス
			loder.CSVLoad(&data, filePass);
			//データの追加
			allMapChipData[stageNum].push_back(data);
		}
	}
	//ステージ毎の小部屋読み込み-----------------------

	swingCount.resize(allStageNum);
	for (int stageNum = 0; stageNum < allStageNum; ++stageNum)
	{
		//ファイルパス
		std::string filePass =
			rootFilePass + stageFilePass + std::to_string(stageNum) + "/";
		//ファイル名
		std::string fileName = filePass + "swingCount.txt";

		const int thisStageRoomCount = allMapChipData[stageNum].size();
		swingCount[stageNum].resize(thisStageRoomCount, 0);	//振り回し可能回数０で埋める

		//ファイルが存在しないならそもそも振り回しが出来ないステージ
		if (!KuroFunc::ExistFile(fileName))
		{
			continue;
		}

		// ファイルデータ
		std::ifstream ifs;
		// ファイルを開く。
		ifs.open(fileName);
		// ファイルが開けたかをチェックする。
		//assert(ifs.fail());	//何らかの理由で失敗
		//ファイルから情報を選択
		string line;
		while (getline(ifs, line))
		{
			//一行分の文字列をストリームに変換して解析しやすくなる
			istringstream line_stream(line);

			//半角スペース区切りで行の先頭文字列を取得
			string key;//ここでvかf等の判断をする
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


	//小部屋同士の繋がりを読み込み-----------------------
	for (int stageNum = 0; stageNum < allStageNum; ++stageNum)
	{
		std::string filePass = rootFilePass + stageFilePass + std::to_string(stageNum) + "/";	//Stageまでのファイルパス
		// ファイルデータ
		std::ifstream ifs;
		// ファイルを開く。
		ifs.open(filePass + "Relation.txt");
		// ファイルが開けたかをチェックする。
		if (ifs.fail())
		{
			//失敗
			continue;
			return;
		}

		//ファイルから情報を選択
		string line;

		int roomNum = 0;
		while (getline(ifs, line))
		{
			//一行分の文字列をストリームに変換して解析しやすくなる
			istringstream line_stream(line);

			//半角スペース区切りで行の先頭文字列を取得
			string key;//ここでvかf等の判断をする
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
	//小部屋同士の繋がりを読み込み-----------------------




	//部屋ごとの描画情報を格納する-----------------------
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

	array<int, 10>doorCount;//ドア番号の重複をみる
	vector<int>startChip;//どの部屋にスタート用のチップが呼ばれているか見る
	int startChipCount = 0;//スタート用のチップが呼び出された回数を数えます
	array<int, 10>doorCallCount = {};//それぞれのドア番号で何回呼び出されたか
	array<bool, 10>doorSideCheckFlags = {};//それぞれのドアがどの方向に向いているか

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
	//部屋ごとの描画情報を格納する-----------------------

	//使わない
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
		//どの扉なのか探索して、どの小部屋に行くかを返す
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
	bool checkStageFlag = 0 <= STAGE_NUMBER && STAGE_NUMBER < allMapChipDrawData.size();//ステージ番号が配列内にあるか確認

	if (checkStageFlag)
	{
		bool checkRoomFlag = 0 <= ROOM_NUMBER && ROOM_NUMBER < allMapChipDrawData[STAGE_NUMBER].size();//部屋番号が配列内にあるか確認
		bool checkRoomHaveMapChipFlag = allMapChipDrawData[STAGE_NUMBER][ROOM_NUMBER].size() != 0;				   //部屋にマップチップがあるかどうか確認
		if (checkRoomFlag && checkRoomHaveMapChipFlag)
		{
			return true;
		}
	}

	return false;
}

void StageMgr::WriteMapChipData(const Vec2<int> MAPCHIP_NUM, const int &CHIPNUM, const Vec2<float> &LeftCharaPos, const float &LeftCharaSize, const Vec2<float> &RightCharaPos, const float &RightCharaSize)
{
	//配列外参照
	if (MAPCHIP_NUM.y < 0 || localRoomMapChipArray.size() <= MAPCHIP_NUM.y || MAPCHIP_NUM.x < 0 || localRoomMapChipArray[MAPCHIP_NUM.y].size() <= MAPCHIP_NUM.x)
	{
		return;
	}
	if (localRoomMapChipArray[MAPCHIP_NUM.y][MAPCHIP_NUM.x] == CHIPNUM)return;	//変化なし

	//壊れないブロックは飛ばす
	if (localRoomMapChipArray[MAPCHIP_NUM.y][MAPCHIP_NUM.x] == MAPCHIP_TYPE_STATIC_UNBROKEN_BLOCK)
	{
		return;
	}

	// 削る場合は通さない。
	if (CHIPNUM != 0) {

		// 左側のキャラと円形の当たり判定を行って、当たっていたらブロックを生成しない。
		Vec2<float> mapChipPos = Vec2<float>(MAPCHIP_NUM.x * MAP_CHIP_SIZE + MAP_CHIP_HALF_SIZE, MAPCHIP_NUM.y * MAP_CHIP_SIZE + MAP_CHIP_HALF_SIZE);
		if (Vec2<float>(mapChipPos - LeftCharaPos).Length() <= MAP_CHIP_SIZE + MAP_CHIP_SIZE + LeftCharaSize) {
			return;
		}

		// 右側のキャラと円形の当たり判定を行って、当たっていたらブロックを生成しない。
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

	//上どれくらい伸びるか
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
	//下どれくらい伸びるか
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
	//左
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
	//右
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
	//描画の書き換え
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
