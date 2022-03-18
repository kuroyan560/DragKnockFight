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
	//アニメーション情報を読み込み
	animationData.push_back(new MapChipAnimationData(tmp, 10));


	//std::string rootFilePass = "Resource/MapChipData/";
	std::string rootFilePass = "resource/IntoTheAbyss/MapChipData/";
	std::string stageFilePass = "Stage";
	std::string roomFileName = "Room_";


	for (int stageNum = 0; stageNum < allStageNum; ++stageNum)
	{
		for (int roomNum = 0; roomNum < allRoomNum; ++roomNum)
		{
			//ファイルパス
			std::string filePass =
				rootFilePass + stageFilePass + std::to_string(stageNum) + "/";
			//ファイル名
			std::string fileName =
				roomFileName + std::to_string(roomNum) + "_Gimmick.txt";

			loadGimmickData->LoadData(stageNum, roomNum, filePass + fileName);
		}
	}

	//ステージ毎の小部屋読み込み-----------------------
	for (int stageNum = 0; stageNum < allStageNum; ++stageNum)
	{
		for (int roomNum = 0; roomNum < allRoomNum; ++roomNum)
		{
			RoomMapChipArray data;	//小部屋の読み込み
			std::string filePass = rootFilePass + stageFilePass + std::to_string(stageNum) + "/";	//Stageまでのファイルパス
			//Stageまでのファイルパス+小部屋のファイルパス
			loder.CSVLoad(&data, filePass + roomFileName + std::to_string(roomNum) + ".csv");
			//データの追加
			allMapChipData[stageNum].push_back(data);
		}
	}
	//ステージ毎の小部屋読み込み-----------------------


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
			//エリアが変わるごとにドア番号管理の変数を初期化する
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
					//自分の周り8マスのマップチップ番号を手に入れる
					int now = GetMapChipBlock(stageNum, roomNum, { (float)x,	 (float)y });
					int Up = GetMapChipBlock(stageNum, roomNum, { (float)x,		 (float)y - 1 });
					int Down = GetMapChipBlock(stageNum, roomNum, { (float)x,	 (float)y + 1 });
					int left = GetMapChipBlock(stageNum, roomNum, { (float)x - 1,(float)y });
					int right = GetMapChipBlock(stageNum, roomNum, { (float)x + 1,(float)y });
					int LeftUp = GetMapChipBlock(stageNum, roomNum, { (float)x - 1,(float)y - 1 });
					int LeftDown = GetMapChipBlock(stageNum, roomNum, { (float)x - 1,(float)y + 1 });
					int rightUp = GetMapChipBlock(stageNum, roomNum, { (float)x + 1,(float)y - 1 });
					int rightDown = GetMapChipBlock(stageNum, roomNum, { (float)x + 1,(float)y + 1 });



					bool skipFlag = false;		//ドアをを探索したかどうかを示すフラグ
					for (int doorNumber = mapChipMemoryData[MAPCHIP_TYPE_DOOR].min; doorNumber < mapChipMemoryData[MAPCHIP_TYPE_DOOR].max; ++doorNumber)
					{
						int arrayNum = doorNumber - mapChipMemoryData[MAPCHIP_TYPE_DOOR].min;//ドア番号から配列の添え字用に数値を調整

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
								//自分がいる座標を登録する
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


						//ドア上向き
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
						//ドア下向き
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
						//ドア左向き
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
						//ドア右向き
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
							roomNumber.push_back(string("Room番号" + std::to_string(startChip[i])));
						}
						string result = roomNumber[0] + "と" + roomNumber[1] + "にスタート用のマップチップが埋め込まれています。\nスタート用のチップはステージ毎に一つにして下さい";
						MessageBox(NULL, KuroFunc::GetWideStrFromStr(result).c_str(), TEXT("スタート番号被り"), MB_OK);
						assert(0);
					}


					//ドッスンの場合
					if (mapChipMemoryData[MAPCHIP_TYPE_THOWNP].min <= now && now <= mapChipMemoryData[MAPCHIP_TYPE_THOWNP].max)
					{
						//この番号が開始位置か終了位置か調べる
						int gimmickNumber = now - mapChipMemoryData[MAPCHIP_TYPE_THOWNP].min;

						//偶数なら開始位置、奇数なら終了位置とする
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

					//バブルの場合
					if (mapChipMemoryData[MAPCHIP_TYPE_BUBBLE].min <= now && now <= mapChipMemoryData[MAPCHIP_TYPE_BUBBLE].max)
					{
						int gimmickNumber = now - mapChipMemoryData[MAPCHIP_TYPE_BUBBLE].min;
						loadGimmickData->PushBubbleData(stageNum, roomNum, gimmickNumber, Vec2<float>(x * 50.0f, y * 50.0f));
						skipFlag = true;
					}



					//マップチップの値を0か1に絞る
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

						//ドアだったらスキップ
						if (skipFlag)
						{
							continue;
						}


						//右だけ壁がある
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
						//左だけ壁がある
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


						//空中床
						if (Up == 0 && Down == 0 && left == 0 && right == 0 && now != 2)
						{
							MapChipDrawData tmp;
							tmp.handle = mapChipGraphHandle[MAPCHIP_DRAW_WALL_IN];
							//allMapChipDrawData[stageNum][roomNum][y][x] = tmp;
						}

						//通常壁
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
							//上壁
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
							//下壁
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

							//左壁
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

							//右壁
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

							//上が空間の壁
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

							//上が空間の壁
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
							//右上壁
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
							//右下壁
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
							//左上壁
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
							//左下壁
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

							//壁の中
							if ((checkUpSideFlag && checkObliaueFlag) || outSideFlag)
							{
								MapChipDrawData tmp;
								tmp.handle = mapChipGraphHandle[MAPCHIP_DRAW_WALL_IN];
								allMapChipDrawData[stageNum][roomNum][y][x] = tmp;
							}
						}


						//右上
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
						//右下
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
						//左上
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
						//左下
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



						//専用の処理
						//左右壁、上にドア、下は空中の場合に下壁を描画する
						if (Up == 3 && Down == 0 && left != 0 && right != 0)
						{
							MapChipDrawData tmp;
							tmp.handle = mapChipGraphHandle[MAPCHIP_DRAW_FLOOR];
							tmp.radian = 0;
							allMapChipDrawData[stageNum][roomNum][y][x] = tmp;
						}


						//上T路地なら壁中の画像を使う
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

							//下T路地なら壁中の画像を使う
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

							//右T路地なら壁中の画像を使う
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

							//左T路地なら壁中の画像を使う
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

						//アニメーションテスト用
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
							//エラー
							MapChipDrawData tmp;
							tmp.handle = mapChipGraphHandle[MAPCHIP_DRAW_ERROR];
							allMapChipDrawData[stageNum][roomNum][y][x] = tmp;
						}

					}
				}
			}
		}
	}
	//部屋ごとの描画情報を格納する-----------------------




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


RoomMapChipDrawArray StageMgr::GetMapChipDrawBlock(const int &STAGE_NUMBER, const int &ROOM_NUMBER, const Vec2<float> &MAPCHIP_POS)
{
	RoomMapChipDrawArray tmp = allMapChipDrawData[STAGE_NUMBER][ROOM_NUMBER];
	return tmp;
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
