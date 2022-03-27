#include "Game.h"
#include"MapChipCollider.h"
#include"ShakeMgr.h"
#include "BulletMgr.h"
#include"ScrollMgr.h"
//#include"BulletParticleMgr.h"
#include"AuraBlock.h"
#include"ViewPort.h"
#include"MovingBlockMgr.h"
#include"Bullet.h"
#include"Collider.h"
#include"SightCollisionStorage.h"
#include"SelectStage.h"


#include"KuroFunc.h"
#include"KuroEngine.h"
#include"TexHandleMgr.h"
#include"DrawFunc.h"
#include"ParticleMgr.h"


#include<map>
std::vector<std::unique_ptr<MassChipData>> Game::AddData(RoomMapChipArray MAPCHIP_DATA, const int &CHIP_NUM)
{
	MassChip checkData;
	std::vector<std::unique_ptr<MassChipData>> data;

	for (int y = 0; y < MAPCHIP_DATA.size(); ++y)
	{
		for (int x = 0; x < MAPCHIP_DATA[y].size(); ++x)
		{
			if (MAPCHIP_DATA[y][x] == CHIP_NUM)
			{
				bool sucseedFlag = checkData.Check(Vec2<int>(x, y), CHIP_NUM);
				if (!sucseedFlag)
				{
					continue;
				}

				//伸びた情報をオーラに渡す
				data.push_back(std::make_unique<MassChipData>(checkData.GetLeftUpPos(), checkData.GetRightDownPos(), checkData.sideOrUpDownFlag));
			}
		}
	}

	return data;
}

void Game::DrawMapChip(const vector<vector<int>> &mapChipData, vector<vector<MapChipDrawData>> &mapChipDrawData, const int &mapBlockGraph, const int &stageNum, const int &roomNum)
{
	std::map<int, std::vector<ChipData>>datas;

	// 描画するチップのサイズ
	const float DRAW_MAP_CHIP_SIZE = MAP_CHIP_SIZE * ScrollMgr::Instance()->zoom;

	// マップチップの縦の要素数を取得。
	const int HEIGHT = mapChipData.size();
	for (int height = 0; height < HEIGHT; ++height) {

		// マップチップの横の要素数を取得。
		const int WIDTH = mapChipData[height].size();
		for (int width = 0; width < WIDTH; ++width) {

			// ブロック以外だったら処理を飛ばす。
			bool blockFlag = (mapChipData[height][width] >= 1 && mapChipData[height][width] <= 9);
			bool doorFlag = (mapChipData[height][width] >= 20 && mapChipData[height][width] <= 29);
			if (blockFlag || doorFlag)
			{
				// スクロール量から描画する位置を求める。
				const float centerX = width * DRAW_MAP_CHIP_SIZE - ScrollMgr::Instance()->scrollAmount.x * ScrollMgr::Instance()->zoom;
				const float centerY = height * DRAW_MAP_CHIP_SIZE - ScrollMgr::Instance()->scrollAmount.y * ScrollMgr::Instance()->zoom;

				// 画面外だったら描画しない。
				if (centerX < -DRAW_MAP_CHIP_SIZE || centerX > WinApp::Instance()->GetWinSize().x + DRAW_MAP_CHIP_SIZE) continue;
				if (centerY < -DRAW_MAP_CHIP_SIZE || centerY > WinApp::Instance()->GetWinSize().y + DRAW_MAP_CHIP_SIZE) continue;


				vector<MapChipAnimationData *>tmpAnimation = StageMgr::Instance()->animationData;
				int handle = -1;
				//アニメーションフラグが有効ならアニメーション用の情報を行う
				if (mapChipDrawData[height][width].animationFlag)
				{
					int arrayHandle = mapChipDrawData[height][width].handle;
					++mapChipDrawData[height][width].interval;
					//アニメーションの間隔
					if (mapChipDrawData[height][width].interval % tmpAnimation[arrayHandle]->maxInterval == 0)
					{
						++mapChipDrawData[height][width].animationNum;
						mapChipDrawData[height][width].interval = 0;
					}
					//アニメーション画像の総数に達したら最初に戻る
					if (tmpAnimation[arrayHandle]->handle.size() <= mapChipDrawData[height][width].animationNum)
					{
						mapChipDrawData[height][width].animationNum = 0;
					}
					//分割したアニメーションの画像から渡す
					handle = tmpAnimation[arrayHandle]->handle[mapChipDrawData[height][width].animationNum];
				}
				else
				{
					handle = mapChipDrawData[height][width].handle;
				}

				Vec2<float> pos(centerX + ShakeMgr::Instance()->shakeAmount.x * ScrollMgr::Instance()->zoom, centerY + ShakeMgr::Instance()->shakeAmount.y * ScrollMgr::Instance()->zoom);
				pos += mapChipDrawData[height][width].offset;
				if (0 <= handle)
				{
					ChipData chipData;
					chipData.pos = pos;
					chipData.radian = mapChipDrawData[height][width].radian;
					datas[handle].emplace_back(chipData);
					//DrawFunc::DrawRotaGraph2D({ pos.x, pos.y }, 1.6f * ScrollMgr::Instance()->zoom, mapChipDrawData[height][width].radian, TexHandleMgr::GetTexBuffer(handle));
				}
			}
		}
	}

	while (drawMap.size() < datas.size())
	{
		drawMap.emplace_back();
	}

	int i = 0;
	for (auto itr = datas.begin(); itr != datas.end(); ++itr)
	{
		for (int chipIdx = 0; chipIdx < itr->second.size(); ++chipIdx)
		{
			drawMap[i].AddChip(itr->second[chipIdx].pos, itr->second[chipIdx].radian);
		}
		drawMap[i].Draw(ligMgr, TexHandleMgr::GetTexBuffer(itr->first), nullptr, nullptr);
		i++;
	}
}

Vec2<float> Game::GetPlayerResponePos(const int &STAGE_NUMBER, const int &ROOM_NUMBER, const int &DOOR_NUMBER, Vec2<float> DOOR_MAPCHIP_POS, E_DOOR_DIR *DIR, const bool &ONLY_GET_DOOR_DIR)
{
	Vec2<float> doorPos;
	int roopCount = 0;
	//扉の一番下の座標を探索する
	for (int y = 0; 1; ++y)
	{
		Vec2<float> chipData = { DOOR_MAPCHIP_POS.x,DOOR_MAPCHIP_POS.y + y };
		int chipNum = StageMgr::Instance()->GetMapChipBlock(STAGE_NUMBER, ROOM_NUMBER, chipData);

		//どんどん下を探索してドアじゃない場所に出たら、一個前が一番下の扉
		if (chipNum != DOOR_NUMBER)
		{
			if (1 <= y)
			{
				doorPos = { DOOR_MAPCHIP_POS.x,(DOOR_MAPCHIP_POS.y + y) - 1 };
			}
			else
			{
				doorPos = { DOOR_MAPCHIP_POS.x,(DOOR_MAPCHIP_POS.y + y) };
			}
			break;
		}
		//無限ループに入ったらアサートする
		++roopCount;
		if (1000 <= roopCount)
		{
			assert(0);
		}
	}


	//左右どちらかの扉から、もしくは奥扉からリスポーンさせる場合-----------------------
	array<Vec2<float>, 2> checkWall;
	checkWall[0] = { doorPos.x + 1,doorPos.y };
	checkWall[1] = { doorPos.x - 1,doorPos.y };
	int rightChip = StageMgr::Instance()->GetMapChipBlock(STAGE_NUMBER, ROOM_NUMBER, checkWall[0]);
	int leftChip = StageMgr::Instance()->GetMapChipBlock(STAGE_NUMBER, ROOM_NUMBER, checkWall[1]);

	//壁以外は空白として判定を出す
	if (leftChip <= -2 || 9 <= leftChip)
	{
		leftChip = 0;
	}
	if (rightChip <= -2 || 9 <= rightChip)
	{
		rightChip = 0;
	}


	//右に壁がある場合
	if ((rightChip == 1 && leftChip == 0) || (rightChip == -1 && leftChip == 0))
	{
		*DIR = DOOR_LEFT;
		//左に2ブロック離れた所にリスポーンさせる
		return Vec2<float>((doorPos.x - 2) * 50.0f, doorPos.y * 50.0f);
	}
	//左に壁がある場合
	else if ((rightChip == 0 && leftChip == 1) || (rightChip == 0 && leftChip == -1))
	{
		*DIR = DOOR_RIGHT;
		//右に2ブロック離れた所にリスポーンさせる
		return Vec2<float>((doorPos.x + 2) * 50.0f, doorPos.y * 50.0f);
	}
	//左右どちらとも壁が無かった場合
	else if (rightChip == 0 && leftChip == 0)
	{
		*DIR = DOOR_Z;
		//扉座標にリスポーンさせる
		return Vec2<float>(doorPos.x * 50.0f, doorPos.y * 50.0f);
	}
	//左右どちらかの扉から、もしくは奥扉からリスポーンさせる場合-----------------------


	//上下どちらかの扉からリスポーンさせる場合-----------------------
	//一番左と一番右の扉ブロックを探索する
	vector<Vec2<float>>doorBlock;//ドアブロックの塊
	Vec2<float> rightDoor, leftDoor;//一番端の扉座標を探索する

	roopCount = 0;
	//一番左の扉座標探索
	for (int i = 0; 1; ++i)
	{
		Vec2<float> chipData = { DOOR_MAPCHIP_POS.x - i,DOOR_MAPCHIP_POS.y };
		int chipNum = StageMgr::Instance()->GetMapChipBlock(STAGE_NUMBER, ROOM_NUMBER, chipData);
		if (chipNum != DOOR_NUMBER)
		{
			leftDoor = { DOOR_MAPCHIP_POS.x - i + 1,DOOR_MAPCHIP_POS.y };
			break;
		}

		//無限ループに入ったらアサートする
		++roopCount;
		if (1000 <= roopCount)
		{
			assert(0);
		}
	}

	roopCount = 0;
	//一番右の扉座標探索
	for (int i = 0; 1; ++i)
	{
		Vec2<float> chipData = { DOOR_MAPCHIP_POS.x + i,DOOR_MAPCHIP_POS.y };
		int chipNum = StageMgr::Instance()->GetMapChipBlock(STAGE_NUMBER, ROOM_NUMBER, chipData);
		if (chipNum != DOOR_NUMBER)
		{
			rightDoor = { DOOR_MAPCHIP_POS.x + i - 1,DOOR_MAPCHIP_POS.y };
			break;
		}

		//無限ループに入ったらアサートする
		++roopCount;
		if (1000 <= roopCount)
		{
			assert(0);
		}
	}

	checkWall[0] = { rightDoor.x + 1,rightDoor.y };
	checkWall[1] = { leftDoor.x - 1,leftDoor.y };
	rightChip = StageMgr::Instance()->GetMapChipBlock(STAGE_NUMBER, ROOM_NUMBER, checkWall[0]);
	leftChip = StageMgr::Instance()->GetMapChipBlock(STAGE_NUMBER, ROOM_NUMBER, checkWall[1]);

	//上下の壁を調べる
	array<Vec2<float>, 2> checkTopWall;
	checkTopWall[0] = { rightDoor.x,rightDoor.y - 1 };
	checkTopWall[1] = { rightDoor.x,rightDoor.y + 1 };
	int topWall = StageMgr::Instance()->GetMapChipBlock(STAGE_NUMBER, ROOM_NUMBER, checkTopWall[0]);
	int downWall = StageMgr::Instance()->GetMapChipBlock(STAGE_NUMBER, ROOM_NUMBER, checkTopWall[1]);


	//両方とも壁があり、上に空間があるなら
	if (rightChip != 0 && leftChip != 0 && topWall == 0)
	{
		//壁が無い方にリスポーンさせる
		Vec2<float> check(leftDoor.x - 2, leftDoor.y - 1);
		Vec2<float> check2(rightDoor.x + 2, rightDoor.y - 1);

		int num = StageMgr::Instance()->GetMapChipBlock(STAGE_NUMBER, ROOM_NUMBER, check);
		int num2 = StageMgr::Instance()->GetMapChipBlock(STAGE_NUMBER, ROOM_NUMBER, check2);


		//左に行くか右に行くかで変わる
		//bool leftHaveSpaceFlag = num == 0 && 1 == 1;
		//bool rightHaveSpaceFlag = num2 == 0 && 1 == 1;

		////どっちかがfalseなら反対側をtrueにする
		//if (!leftHaveSpaceFlag)
		//{
		//	rightHaveSpaceFlag = true;
		//}
		//if (!rightHaveSpaceFlag)
		//{
		//	leftHaveSpaceFlag = true;
		//}



		//左に壁があった場合、右の壁にリスポーンさせる。
		if (num2 == 0)
		{
			*DIR = DOOR_UP_GORIGHT;
			return Vec2<float>(rightDoor.x * 50.0f, rightDoor.y * 50.0f);
		}
		else
		{
			*DIR = DOOR_UP_GOLEFT;
			return Vec2<float>(leftDoor.x * 50.0f, leftDoor.y * 50.0f);
		}
	}
	//両方とも壁があり、下に空間があるなら
	else if (rightChip != 0 && leftChip != 0 && downWall == 0)
	{
		*DIR = DOOR_DOWN;
		//中心座標にリスポーンさせる	(左右のドアの距離の半分を左ドアチップから足す)
		Vec2<float>centralPos(leftDoor + (Vec2<float>(rightDoor - leftDoor) / 2.0f));
		centralPos *= Vec2<float>(50.0f, 50.0f);

		return centralPos;
	}
	//上下どちらかの扉からリスポーンさせる場合-----------------------

	if (!ONLY_GET_DOOR_DIR)
	{
		string result = "次につながるドアが見つかりません。\nRalationファイルを確認するか、担当の大石に連絡をください。";
		MessageBox(NULL, KuroFunc::GetWideStrFromStr(result).c_str(), TEXT("ドアが見つかりません"), MB_OK);
	}

	//失敗
	*DIR = DOOR_NONE;
	return Vec2<float>(-1, -1);
}

Vec2<float> Game::GetDoorPos(const int &DOOR_NUMBER, const vector<vector<int>> &MAPCHIP_DATA)
{
	Vec2<float> door;
	//次につながるドアを探す
	for (int y = 0; y < MAPCHIP_DATA.size(); ++y)
	{
		for (int x = 0; x < MAPCHIP_DATA[y].size(); ++x)
		{
			if (MAPCHIP_DATA[y][x] == DOOR_NUMBER)
			{
				door = { (float)x,(float)y };
				break;
			}
		}
	}
	return door;
}

const int &Game::GetChipNum(const vector<vector<int>> &MAPCHIP_DATA, const int &MAPCHIP_NUM, int *COUNT_CHIP_NUM, Vec2<float> *POS)
{
	int chipNum = 0;
	for (int y = 0; y < MAPCHIP_DATA.size(); ++y)
	{
		for (int x = 0; x < MAPCHIP_DATA[y].size(); ++x)
		{
			if (MAPCHIP_DATA[y][x] == MAPCHIP_NUM)
			{
				*COUNT_CHIP_NUM += 1;
				*POS = { x * 50.0f,y * 50.0f };
			}
		}
	}
	return chipNum;
}

void Game::InitGame(const int &STAGE_NUM, const int &ROOM_NUM)
{
	int stageNum = STAGE_NUM;
	int roomNum = ROOM_NUM;

	mapData = StageMgr::Instance()->GetMapChipData(stageNum, roomNum);
	mapChipDrawData = StageMgr::Instance()->GetMapChipDrawBlock(stageNum, roomNum);

	// シェイク量を設定。
	ShakeMgr::Instance()->Init();
	ParticleMgr::Instance()->Init();

	//オーラブロック追加
	{
		const int auraChipNum = MAPCHIP_BLOCK_AURABLOCK;//オーラブロックのチップ番号
		auraBlock.clear();
		std::vector<std::unique_ptr<MassChipData>> data = AddData(mapData, auraChipNum);
		for (int i = 0; i < data.size(); ++i)
		{
			auraBlock.push_back(std::make_unique<AuraBlock>());
			int auraBlocksArrayNum = auraBlock.size() - 1;
			if (data[i]->sideOrUpDownFlag)
			{
				auraBlock[auraBlocksArrayNum]->Init(data[i]->leftUpPos, data[i]->rightDownPos, AURA_DIR_LEFTORRIGHT);
			}
			else
			{
				auraBlock[auraBlocksArrayNum]->Init(data[i]->leftUpPos, data[i]->rightDownPos, AURA_DIR_UPORDOWN);
			}
		}
	}

	//ドアの追加
	{
		doorBlocks.clear();
		SizeData chipMemorySize = StageMgr::Instance()->GetMapChipSizeData(MAPCHIP_TYPE_DOOR);
		for (int doorIndex = chipMemorySize.min; doorIndex < chipMemorySize.max; ++doorIndex)
		{
			std::vector<std::unique_ptr<MassChipData>> data = AddData(mapData, doorIndex);
			for (int i = 0; i < data.size(); ++i)
			{
				doorBlocks.push_back(std::make_unique<DoorBlock>());
				int doorBlocksArrayNum = doorBlocks.size() - 1;
				doorBlocks[doorBlocksArrayNum]->Init(data[i]->leftUpPos, data[i]->rightDownPos, doorIndex);
			}
		}
	}

	//棘ブロック
	{
		thornBlocks.clear();
		const int dogeChipNum = MAPCHIP_BLOCK_THOWN;
		std::vector<std::unique_ptr<MassChipData>> data = AddData(mapData, dogeChipNum);
		for (int i = 0; i < data.size(); ++i)
		{
			thornBlocks.push_back(std::make_unique<ThornBlock>());
			int thornBlocksArrayNum = thornBlocks.size() - 1;
			thornBlocks[thornBlocksArrayNum]->Init(data[i]->leftUpPos, data[i]->rightDownPos);
		}
	}

#pragma region イベントチップ生成
	//イベントブロック生成
	{
		eventBlocks.clear();
		SizeData chipMemorySize = StageMgr::Instance()->GetMapChipSizeData(MAPCHIP_TYPE_EVENT);
		for (int eventChipIndex = chipMemorySize.min; eventChipIndex < chipMemorySize.max; ++eventChipIndex)
		{
			std::vector<std::unique_ptr<MassChipData>> data = AddData(mapData, eventChipIndex);
			for (int i = 0; i < data.size(); ++i)
			{
				eventBlocks.push_back(std::make_unique<EventBlock>());
				int doorBlocksArrayNum = eventBlocks.size() - 1;
				eventBlocks[doorBlocksArrayNum]->Init(data[i]->leftUpPos, data[i]->rightDownPos, eventChipIndex);
			}
		}
	}
#pragma endregion

#pragma region ドッスン生成
	// ドッスンブロックを生成。
	vector<shared_ptr<ThownpeData>> dossunData;

	// ドッスンブロックデータを取得
	dossunData = GimmickLoader::Instance()->GetThowpeData(stageNum, roomNum);

	const int dossunCount = dossunData.size();

	// 照準の判定に使用するデータを保存。
	SightCollisionStorage::Instance()->data.clear();

	// ドッスンブロックを初期化。
	dossunBlock.clear();

	// ドッスンを生成。
	for (int index = 0; index < dossunCount; ++index) {
		//始点と終点が一緒なら
		if (dossunData[index]->startPos != dossunData[index]->endPos)
		{
			DossunBlock dossunBuff;
			dossunBuff.Generate(dossunData[index]->startPos, dossunData[index]->endPos, dossunData[index]->size, dossunData[index]->type);
			// データを追加。
			dossunBlock.push_back(dossunBuff);
			if (&dossunBlock[dossunBlock.size() - 1].sightData == nullptr) assert(0); //ドッスンブロックのデータが何故かNullptrです！
			SightCollisionStorage::Instance()->data.push_back(&dossunBlock[dossunBlock.size() - 1].sightData);
		}
	}
#pragma endregion

#pragma region シャボン玉
	// シャボン玉ブロックの情報を取得。
	vector<shared_ptr<BubbleData>> bubbleData;
	bubbleData = GimmickLoader::Instance()->GetBubbleData(stageNum, roomNum);

	const int bubbleCount = bubbleData.size();

	// シャボン玉ブロックを初期化。
	bubbleBlock.clear();

	// シャボン玉ブロックを生成。
	for (int index = 0; index < bubbleCount; ++index) {

		Bubble bubbleBuff;
		bubbleBuff.Generate(bubbleData[index]->pos);

		// データを追加。
		bubbleBlock.push_back(bubbleBuff);
	}
#pragma endregion

	initDeadFlag = false;
	giveDoorNumber = 0;
	debugStageData[0] = stageNum;
	debugStageData[1] = roomNum;

#pragma region プレイヤー初期化
	bool deadFlag = false;
	if (player.isDead)
	{
		deadFlag = true;
	}


	bool responeFlag = false;

	for (int y = 0; y < mapData.size(); ++y)
	{
		for (int x = 0; x < mapData[y].size(); ++x)
		{
			if (mapData[y][x] == MAPCHIP_BLOCK_DEBUG_START)
			{
				player.Init(Vec2<float>(x * 50.0f, y * 50.0f));
				ScrollMgr::Instance()->WarpScroll(player.centerPos);
				responeFlag = true;
				break;
			}
		}
	}

	//デバック開始用の場所からリスポーンしたらこの処理を通さない
	if (!responeFlag)
	{
		//マップ開始時の場所にスポーンさせる
		for (int y = 0; y < mapData.size(); ++y)
		{
			for (int x = 0; x < mapData[y].size(); ++x)
			{
				if (mapData[y][x] == MAPCHIP_BLOCK_START)
				{
					player.Init(Vec2<float>(x * 50.0f, y * 50.0f));
					ScrollMgr::Instance()->WarpScroll(player.centerPos);
					break;
				}
			}
		}
	}



	if (deadFlag)
	{
		ScrollMgr::Instance()->CalucurateScroll(player.prevFrameCenterPos - player.centerPos);
		ScrollMgr::Instance()->AlimentScrollAmount();
		ScrollMgr::Instance()->Restart();
	}
#pragma endregion

	ViewPort::Instance()->Init(player.centerPos, { 800.0f,500.0f });

	sceneBlackFlag = false;
	sceneLightFlag = false;
	SelectStage::Instance()->resetStageFlag = false;
	alphaValue = 0;
}

Game::Game()
{
	mapBlockGraph = TexHandleMgr::LoadGraph("resource/IntoTheAbyss/Block.png");
	//mapBlockGraph = D3D12App::Instance()->GenerateTextureBuffer("resource/IntoTheAbyss/Block.png");

	// 弾管理クラスを初期化。
	BulletMgr::Instance()->Setting();

	movingBlockGraph = TexHandleMgr::LoadGraph("resource/IntoTheAbyss/MovingBlock.png");
	//sceneChangeHandle = TexHandleMgr::LoadGraph("resource/IntoTheAbyss/SceneChange.png");
	//movingBlockGraph = D3D12App::Instance()->GenerateTextureBuffer("resource/IntoTheAbyss/MovingBlock.png");

	// 弾パーティクルをセッティング。
	//BulletParticleMgr::Instance()->Setting();

	//ライト情報
	ptLig.SetInfluenceRange(PT_LIG_RANGE);
	spotLig.SetInfluenceRange(SPOT_LIG_RANGE);

	ligMgr.RegisterPointLight(&ptLig);
	ligMgr.RegisterSpotLight(&spotLig);
	ligMgr.RegisterHemiSphereLight(&hemiLig);

	Init();
}

void Game::Init()
{
	// スクロールマネージャーを初期化。
	ScrollMgr::Instance()->Init(&mapData);
	// スクロール量を設定。
	const float WIN_WIDTH_HALF = WinApp::Instance()->GetWinCenter().x;
	const float WIN_HEIGHT_HALF = WinApp::Instance()->GetWinCenter().y;
	ScrollMgr::Instance()->scrollAmount = { -WIN_WIDTH_HALF, -WIN_HEIGHT_HALF };
	ScrollMgr::Instance()->honraiScrollAmount = { -WIN_WIDTH_HALF, -WIN_HEIGHT_HALF };
}

void Game::Update()
{
	ScrollMgr::Instance()->zoom = ViewPort::Instance()->zoomRate;

	int stageNum = SelectStage::Instance()->GetStageNum();
	int roomNum = SelectStage::Instance()->GetRoomNum();



#pragma region ステージの切り替え
	stageNum = SelectStage::Instance()->GetStageNum();

	bool enableToSelectStageFlag = 0 < debugStageData[0];
	bool enableToSelectStageFlag2 = debugStageData[0] < StageMgr::Instance()->GetMaxStageNumber() - 1;
	//マップの切り替え
	//if (Input::isKeyTrigger(KEY_INPUT_UP) && enableToSelectStageFlag2 && nowSelectNum == 0)
	if (UsersInput::Instance()->OnTrigger(DIK_UP) && enableToSelectStageFlag2 && nowSelectNum == 0)
	{
		++debugStageData[0];
	}
	//if (Input::isKeyTrigger(KEY_INPUT_DOWN) && enableToSelectStageFlag && nowSelectNum == 0)
	if (UsersInput::Instance()->OnTrigger(DIK_DOWN) && enableToSelectStageFlag && nowSelectNum == 0)
	{
		--debugStageData[0];
	}


	bool enableToSelectRoomFlag = 0 < debugStageData[1];
	bool enableToSelectRoomFlag2 = debugStageData[1] < StageMgr::Instance()->GetMaxRoomNumber(debugStageData[0]) - 1;
	//部屋の切り替え
	//if (Input::isKeyTrigger(KEY_INPUT_UP) && enableToSelectRoomFlag2 && nowSelectNum == 1)
	if (UsersInput::Instance()->OnTrigger(DIK_UP) && enableToSelectRoomFlag2 && nowSelectNum == 1)
	{
		++debugStageData[1];
	}
	//if (Input::isKeyTrigger(KEY_INPUT_DOWN) && enableToSelectRoomFlag && nowSelectNum == 1)
	if (UsersInput::Instance()->OnTrigger(DIK_DOWN) && enableToSelectRoomFlag && nowSelectNum == 1)
	{
		--debugStageData[1];
	}

	//部屋か番号に切り替え
	//if (Input::isKeyTrigger(KEY_INPUT_LEFT) && 0 < nowSelectNum)
	if (UsersInput::Instance()->OnTrigger(DIK_LEFT) && 0 < nowSelectNum)
	{
		--nowSelectNum;
		debugStageData[1] = 0;
	}
	if (UsersInput::Instance()->OnTrigger(DIK_RIGHT) && nowSelectNum < 1)
	{
		++nowSelectNum;
		debugStageData[1] = 0;
	}

	if (UsersInput::Instance()->OnTrigger(DIK_RETURN))
	{
		SelectStage::Instance()->SelectStageNum(debugStageData[0]);
		SelectStage::Instance()->SelectRoomNum(debugStageData[1]);
		mapData = StageMgr::Instance()->GetMapChipData(debugStageData[0], debugStageData[1]);

		Vec2<float> door;
		//デバック用のマップチップ番号からスタートする
		for (int y = 0; y < mapData.size(); ++y)
		{
			for (int x = 0; x < mapData[y].size(); ++x)
			{
				if (mapData[y][x] == MAPCHIP_BLOCK_DEBUG_START)
				{
					door = { (float)x,(float)y };
				}
			}
		}
		player.Init(door * Vec2<float>(50.0f, 50.0f));
		ScrollMgr::Instance()->WarpScroll(player.centerPos);
	}
#pragma endregion


	//ゴールに触れたら次のステージに向かう処理
	{
		Vec2<float> playerChip((player.centerPos.x + 25.0f) / 50.0f, (player.centerPos.y + 25.0f) / 50.0f);
		//ゴールに触れたら次のステージに向かう
		if (StageMgr::Instance()->GetMapChipBlock(stageNum, roomNum, playerChip) == MAPCHIP_BLOCK_GOAL)
		{
			++stageNum;
			roomNum = 0;

			mapData = StageMgr::Instance()->GetMapChipData(stageNum, roomNum);
			Vec2<float> door;
			//ゴール探索
			for (int y = 0; y < mapData.size(); ++y)
			{
				for (int x = 0; x < mapData[y].size(); ++x)
				{
					if (mapData[y][x] == MAPCHIP_BLOCK_START)
					{
						door = { (float)x,(float)y };
					}
				}
			}
			Vec2<float> tmp = { door.x * 50.0f,door.y * 50.0f };
			player.centerPos = tmp;
			ScrollMgr::Instance()->WarpScroll(player.centerPos);
		}
	}


#pragma region ドア移動
	for (int doorIndex = 0; doorIndex < doorBlocks.size(); ++doorIndex)
	{
		//当たり判定
		bool hitFlag = doorBlocks[doorIndex]->Collision(player.centerPos, player.PLAYER_HIT_SIZE, player.vel, player.prevFrameCenterPos);

		//ドアと当たったら数字を渡す
		if (hitFlag)
		{
			giveDoorNumber = doorBlocks[doorIndex]->chipNumber;
		}

		//プレイヤーの体を完全に隠すためのオフセット
		Vec2<float>offset;
		if (player.vel.x <= 0.0f)
		{
			offset.x = MAP_CHIP_HALF_SIZE;
		}
		else
		{
			offset.x = -MAP_CHIP_HALF_SIZE;
		}

		//現在地取得
		int nowChip = StageMgr::Instance()->GetMapChipBlock(stageNum, roomNum, Vec2<float>((player.centerPos + Vec2<float>(0.0f, -MAP_CHIP_HALF_SIZE) + offset) / Vec2<float>(MAP_CHIP_SIZE, MAP_CHIP_SIZE)));
		int nowChip2 = StageMgr::Instance()->GetMapChipBlock(stageNum, roomNum, Vec2<float>((player.centerPos + Vec2<float>(0.0f, MAP_CHIP_HALF_SIZE) + offset) / Vec2<float>(MAP_CHIP_SIZE, MAP_CHIP_SIZE)));

		//ドア移動判定
		bool outSideFlag = nowChip == -1 && nowChip2 == -1;//プレイヤーが画面外に出た判定
		if (outSideFlag)
		{
			sceneBlackFlag = true;
			doorNumber = giveDoorNumber;
			player.drawCursorFlag = false;
		}
	}
#pragma endregion



#pragma region シーン遷移
	//シーン遷移-----------------------
	//暗転中
	if (sceneBlackFlag)
	{
		alphaValue += 10;

		//プレイヤーの動きを止める
		player.vel = { 0.0f,0.0f };
		player.gravity = 0.0f;


		//暗転フラグを下げ、明転準備に入る
		if (255 <= alphaValue)
		{
			alphaValue = 355;
			++timer;
			bool responeErrorFlag = false;
			if (10 <= timer)
			{
				if (!sceneChangeDeadFlag)
				{
					SizeData chipMemorySize = StageMgr::Instance()->GetMapChipSizeData(MAPCHIP_TYPE_DOOR);
					//マップ情報更新
					int localRoomNum = StageMgr::Instance()->GetRelationData(stageNum, roomNum, doorNumber - chipMemorySize.min);

					RoomMapChipArray tmpMapData = StageMgr::Instance()->GetMapChipData(stageNum, localRoomNum);
					//ドア座標を入手
					Vec2<float>doorPos = GetDoorPos(doorNumber, mapData);
					//プレイヤーがリスポーンする座標を入手
					responePos = GetPlayerResponePos(stageNum, localRoomNum, doorNumber, doorPos, &door);

					//リスポーンエラーなら転移しない
					if (responePos.x != -1.0f && responePos.y != -1.0f)
					{
						SelectStage::Instance()->SelectRoomNum(localRoomNum);

						sceneChangingFlag = true;
						//画面外から登場させる
						switch (door)
						{
						case Game::DOOR_UP_GORIGHT:
							ScrollMgr::Instance()->WarpScroll(responePos);
							initJumpFlag = false;
							player.Init({ responePos.x + 50.0f , responePos.y - 50.0f * 2.0f });
							break;

						case Game::DOOR_UP_GOLEFT:
							ScrollMgr::Instance()->WarpScroll(responePos);
							initJumpFlag = false;
							player.Init({ responePos.x + 50.0f , responePos.y - 50.0f * 2.0f });
							break;

						case Game::DOOR_DOWN:
							player.Init({ responePos.x, responePos.y - 50.0f * 2.0f });
							ScrollMgr::Instance()->WarpScroll(player.centerPos);
							break;

						case Game::DOOR_LEFT:
							ScrollMgr::Instance()->WarpScroll(responePos);
							player.Init({ responePos.x + 50.0f * 3.0f, responePos.y });
							break;

						case Game::DOOR_RIGHT:
							ScrollMgr::Instance()->WarpScroll(responePos);
							player.Init({ responePos.x - 50.0f * 3.0f, responePos.y });
							break;

						case Game::DOOR_Z:
							break;

						default:
							break;
						}
					}
					else
					{
						responeErrorFlag = true;
					}
				}

				if (!responeErrorFlag)
				{
					//暗転開始
					sceneBlackFlag = false;
					sceneLightFlag = true;
				}
				else
				{
					sceneBlackFlag = false;
					SelectStage::Instance()->resetStageFlag = true;
				}
			}
		}
	}

	//明転中
	if (sceneLightFlag)
	{
		alphaValue -= 10;
		bool goFlag = false;

		//プレイヤーを動かす
		if (alphaValue <= 250)
		{
			if (!sceneChangeDeadFlag)
			{
				sceneChangingFlag = false;
				switch (door)
				{
				case Game::DOOR_UP_GORIGHT:
					//ジャンプする
					if (!initJumpFlag)
					{
						gravity = 1.0f;
						initJumpFlag = true;
					}
					player.centerPos.y -= 15.0f * gravity;
					gravity -= 0.1f;

					if (gravity <= 0.0f)
					{
						gravity = 0.0f;
					}

					if (!player.onGround)
					{
						player.centerPos.x += 5.0f;
					}
					else
					{
						goFlag = true;
					}
					break;


				case Game::DOOR_UP_GOLEFT:
					//ジャンプする
					if (!initJumpFlag)
					{
						gravity = 1.0f;
						initJumpFlag = true;
					}
					player.centerPos.y -= 15.0f * gravity;
					gravity -= 0.1f;

					if (gravity <= 0.0f)
					{
						gravity = 0.0f;
					}

					if (!player.onGround)
					{
						player.centerPos.x -= 6.0f;
					}
					else
					{
						goFlag = true;
					}
					break;

				case Game::DOOR_DOWN:
					goFlag = true;
					break;

				case Game::DOOR_LEFT:
					if (responePos.x <= player.centerPos.x)
					{
						player.centerPos.x -= 5.0f;
					}
					else
					{
						goFlag = true;
					}
					break;

				case Game::DOOR_RIGHT:
					if (player.centerPos.x <= responePos.x)
					{
						player.centerPos.x += 5.0f;
					}
					else
					{
						goFlag = true;
					}
					break;

				case Game::DOOR_Z:
					break;
				default:
					break;
				}

				ScrollMgr::Instance()->WarpScroll(player.centerPos);
				ScrollMgr::Instance()->CalucurateScroll(player.prevFrameCenterPos - player.centerPos);
				ScrollMgr::Instance()->Restart();

			}
			else
			{
				if (!initDeadFlag)
				{
					//ドア座標を入手
					Vec2<float>doorPos(GetDoorPos(doorNumber, mapData));
					//プレイヤーがリスポーンする座標を入手
					responePos = GetPlayerResponePos(stageNum, roomNum, doorNumber, doorPos, &door);

					//プレイヤーをリスポーンさせる
					switch (door)
					{
					case Game::DOOR_UP_GORIGHT:
						player.Init({ responePos.x + 50.0f * 2,responePos.y });
						break;

					case Game::DOOR_UP_GOLEFT:
						player.Init({ responePos.x - 50.0f * 2,responePos.y });
						break;

					case Game::DOOR_DOWN:
						player.Init({ responePos.x,responePos.y - 80.0f });
						break;

					case Game::DOOR_LEFT:
						player.Init(responePos);
						break;

					case Game::DOOR_RIGHT:
						player.Init(responePos);
						break;
					case Game::DOOR_Z:
						break;

					case Game::DOOR_NONE:
						break;
					default:
						break;
					}
					ScrollMgr::Instance()->WarpScroll(player.centerPos);
					ScrollMgr::Instance()->CalucurateScroll(player.prevFrameCenterPos - player.centerPos);
					ScrollMgr::Instance()->AlimentScrollAmount();
					ScrollMgr::Instance()->Restart();
					initDeadFlag = true;
				}
				goFlag = true;
			}
		}
		else
		{
			player.vel = { 0.0f,0.0f };
			player.gravity = 0.0f;
		}
		//ゲーム開始
		if (alphaValue <= 0 && goFlag)
		{
			sceneLightFlag = false;
			player.drawCursorFlag = true;
			sceneChangeDeadFlag = false;
			initDeadFlag = false;
		}
	}

	//暗転と明転中はプレイヤーの入力を禁止する
	if (sceneChangingFlag)
	{
		player.StopDoorLeftRight();
	}

	if (!sceneBlackFlag && !sceneLightFlag)
	{
		alphaValue = 0;
		timer = 0;
		sceneChangingFlag = false;
	}
	//シーン遷移-----------------------
#pragma endregion

	if (UsersInput::Instance()->OnTrigger(DIK_SPACE))
	{
		SelectStage::Instance()->resetStageFlag = true;
		//player.isDead = true;
		//sceneBlackFlag = true;
		//sceneChangeDeadFlag = player.isDead;
	}



	//ステージ毎の切り替え判定
	//部屋の初期化
	if ((roomNum != oldRoomNum || stageNum != oldStageNum) || SelectStage::Instance()->resetStageFlag)
	{
		InitGame(SelectStage::Instance()->GetStageNum(), SelectStage::Instance()->GetRoomNum());
	}
	oldRoomNum = roomNum;
	oldStageNum = stageNum;

	//イベントブロックとの判定
	for (int i = 0; i < eventBlocks.size(); ++i)
	{
		eventBlocks[i]->HitBox(player.centerPos, player.PLAYER_HIT_SIZE, player.vel, player.prevFrameCenterPos);
	}

	//棘ブロックとの判定
	for (int i = 0; i < thornBlocks.size(); ++i)
	{
		bool hitFlag = thornBlocks[i]->HitBox(player.centerPos, player.PLAYER_HIT_SIZE, player.vel, player.prevFrameCenterPos);

		if (hitFlag)
		{
			player.isDead = true;
		}
	}

	ScrollMgr::Instance()->DetectMapChipForScroll(player.centerPos);



	if (UsersInput::Instance()->Input(DIK_U))
	{
		thornBlocks[0]->adjValue = { 10.0f,10.0f };
	}
	else if (UsersInput::Instance()->Input(DIK_J))
	{
		thornBlocks[0]->adjValue = { -10.0f,-10.0f };
	}
	else if (UsersInput::Instance()->Input(DIK_N))
	{
		thornBlocks[0]->adjValue = { 0.0f,0.0f };
	}



	// プレイヤーの更新処理
	player.Update(mapData);

	//if (Input::isKey(KEY_INPUT_RIGHT)) player.centerPos.x += 1.0f;
	if (UsersInput::Instance()->Input(DIK_RIGHT)) player.centerPos.x += 1.0f;
	//if (Input::isKey(KEY_INPUT_P)) player.centerPos.x += 100.0f;
	if (UsersInput::Instance()->OnTrigger(DIK_P)) player.centerPos.x += 100.0f;
	//if (Input::isKey(KEY_INPUT_LEFT)) player.centerPos.x -= 1.0f;
	if (UsersInput::Instance()->Input(DIK_LEFT)) player.centerPos.x -= 1.0f;
	//if (Input::isKey(KEY_INPUT_O)) player.centerPos.x -= 100.0f;
	if (UsersInput::Instance()->OnTrigger(DIK_O)) player.centerPos.x -= 100.0f;

	// 敵の更新処理
	enemy.Update();

	// スクロール量の更新処理
	ScrollMgr::Instance()->Update();

	// シェイク量の更新処理
	ShakeMgr::Instance()->Update();

	// 動的ブロックの更新処理
	MovingBlockMgr::Instance()->Update(player.centerPos);

	// 弾パーティクルの更新処理
	//BulletParticleMgr::Instance()->Update();

	// ドッスンブロックの更新処理
	const int DOSSUN_COUNT = dossunBlock.size();
	for (int index = 0; index < DOSSUN_COUNT; ++index) {
		dossunBlock[index].Update();
	}

	ViewPort::Instance()->Update(player.centerPos);
	for (int i = 0; i < auraBlock.size(); ++i)
	{
		auraBlock[i]->Update();
	}

	// シャボン玉ブロックの更新処理
	{
		const int BUBBLE_COUNT = bubbleBlock.size();
		for (int index = 0; index < BUBBLE_COUNT; ++index) {
			bubbleBlock[index].Update();
		}
	}


	/*===== 当たり判定 =====*/

	// ドッスンブロックの当たり判定
	for (int index = 0; index < DOSSUN_COUNT; ++index) {
		dossunBlock[index].CheckHit(mapData);
	}

	// プレイヤーの当たり判定
	player.CheckHit(mapData, bubbleBlock, dossunBlock);

	// 動的ブロックの当たり判定
	MovingBlockMgr::Instance()->CheckHit(mapData);

	// 弾とマップチップの当たり判定
	BulletMgr::Instance()->CheckHit(mapData, bubbleBlock);

	// ビューポートをプレイヤー基準で移動させる。
	ViewPort::Instance()->SetPlayerPosX(player.centerPos.x);
	ViewPort::Instance()->SetPlayerPosY(player.centerPos.y);

	// オーラブロックのデータとビューポートの判定を行う。
	ViewPort::Instance()->SavePrevFlamePos();

	// 弾とビューポートの当たり判定
	for (int i = 0; i < BulletMgr::Instance()->bullets.size(); ++i)
	{
		int hitArea = ViewPortCollider::Instance()->CheckHitRectanglePoint
		(
			ViewPort::Instance()->centralPos,
			ViewPort::Instance()->windowSize.y / 2.0f + ViewPort::Instance()->addLineValue[ViewPort::Instance()->UP],
			ViewPort::Instance()->windowSize.x / 2.0f + ViewPort::Instance()->addLineValue[ViewPort::Instance()->RIGHT],
			ViewPort::Instance()->windowSize.y / 2.0f + ViewPort::Instance()->addLineValue[ViewPort::Instance()->DOWN],
			ViewPort::Instance()->windowSize.x / 2.0f + ViewPort::Instance()->addLineValue[ViewPort::Instance()->LEFT],
			BulletMgr::Instance()->GetBullet(i)->pos,
			BulletMgr::Instance()->GetBullet(i)->MAX_RADIUS
		);

		//bool hitFlag =
		//	BulletMgr::Instance()->GetBullet(i)->GetIsActive() &&
		//	hitArea != ViewPortCollider::Instance()->HIT_AREA_NONE;
		auto hitFlag = false;

		if (hitFlag)
		{
			//ViewPort::Instance()->MoveLine(hitArea, 0.01f);

			// 弾パーティクルを生成する。
			//BulletParticleMgr::Instance()->Generate(BulletMgr::Instance()->GetBullet(i)->pos, BulletMgr::Instance()->GetBullet(i)->forwardVec);
			//BulletParticleMgr::Instance()->Generate(BulletMgr::Instance()->GetBullet(i)->pos, BulletMgr::Instance()->GetBullet(i)->forwardVec);
			ParticleMgr::Instance()->Generate(BulletMgr::Instance()->GetBullet(i)->pos, BulletMgr::Instance()->GetBullet(i)->forwardVec, BULLET);
			ParticleMgr::Instance()->Generate(BulletMgr::Instance()->GetBullet(i)->pos, BulletMgr::Instance()->GetBullet(i)->forwardVec, BULLET);

			BulletMgr::Instance()->GetBullet(i)->Init();
		}
	}

	// オーラとビューポートの当たり判定
	{
		// オーラブロックのデータとビューポートの判定を行う。
		vector<ViewPortAuraReturnData> buff;
		// 上方向
		buff = ViewPortCollider::Instance()->CheckHitRectangleAura(ViewPort::Instance()->pointData[ViewPort::LEFT_UP], ViewPort::Instance()->pointData[ViewPort::RIGHT_UP], CHECK_HIT_TOP, auraBlock);

		// 押し戻す
		if (buff.size() > 0 && !ViewPort::Instance()->noHitFlags[ViewPort::Instance()->UP])
		{

			ViewPortAuraReturnData biggestBuff = {};
			// 値が一番大きい押し戻し量を求める。
			for (int index = 0; index < buff.size(); ++index) {

				if (fabs(buff[index].pushBackAmount) < fabs(biggestBuff.pushBackAmount)) continue;

				biggestBuff = buff[index];

			}

			// ホールドされていない状態だったら-方向に押し戻されるのはおかしいので、押し戻し量を打ち消す。
			if (!biggestBuff.isHold && biggestBuff.pushBackAmount < 0)
			{
				biggestBuff.pushBackAmount = 0;
			}
			// ホールドされていなる状態だったら+方向に押し戻されるのはおかしいので、押し戻し量を打ち消す。
			else if (biggestBuff.isHold && biggestBuff.pushBackAmount > 0) {

				biggestBuff.pushBackAmount = 0;

			}

			// 上側に当たった判定を保存する。
			ViewPort::Instance()->isHitTop = true;

			// ホールド状態だったら
			if (biggestBuff.isHold) {


				ViewPort::Instance()->PushBackAuraHoldUp(biggestBuff.pushBackAmount);

			}
			else {

				// 中心座標を押し戻す。
				ViewPort::Instance()->centralPos.y += biggestBuff.pushBackAmount;

				// ビューポートの四点を押し戻す。
				ViewPort::Instance()->PushBackAura(Vec2<float>(0, biggestBuff.pushBackAmount));

			}

			if (biggestBuff.isHold) {
				ViewPort::Instance()->holdFlags[ViewPort::Instance()->UP] = true;
				//ViewPort::Instance()->PushBackLine(ViewPort::Instance()->LEFT, biggestBuff.pushBackAmount);
			}

			// スクロールを止める。
			ScrollMgr::Instance()->StopScroll(0);
			++countStopNum;

		}
		else
		{
			ViewPort::Instance()->holdFlags[ViewPort::Instance()->UP] = false;
		}


		// 右方向
		buff = ViewPortCollider::Instance()->CheckHitRectangleAura(ViewPort::Instance()->pointData[ViewPort::RIGHT_UP], ViewPort::Instance()->pointData[ViewPort::RIGHT_DOWN], CHECK_HIT_RIGHT, auraBlock);

		// 押し戻す
		if (buff.size() > 0 && !ViewPort::Instance()->noHitFlags[ViewPort::Instance()->RIGHT])
		{

			ViewPortAuraReturnData biggestBuff = {};
			// 値が一番大きい押し戻し量を求める。
			for (int index = 0; index < buff.size(); ++index) {

				if (fabs(buff[index].pushBackAmount) < fabs(biggestBuff.pushBackAmount)) continue;

				biggestBuff = buff[index];

			}

			// ホールドされていない状態だったら-方向に押し戻されるのはおかしいので、押し戻し量を打ち消す。
			if (!biggestBuff.isHold && biggestBuff.pushBackAmount > 0)
			{
				biggestBuff.pushBackAmount = 0;
			}
			// ホールドされていなる状態だったら+方向に押し戻されるのはおかしいので、押し戻し量を打ち消す。
			else if (biggestBuff.isHold && biggestBuff.pushBackAmount < 0) {

				biggestBuff.pushBackAmount = 0;

			}

			// 右側に当たった判定を保存する。
			ViewPort::Instance()->isHitRight = true;

			// ホールド状態だったら
			if (biggestBuff.isHold) {


				ViewPort::Instance()->PushBackAuraHoldRight(biggestBuff.pushBackAmount);

			}
			else {

				// 中心座標を押し戻す。
				ViewPort::Instance()->centralPos.x += biggestBuff.pushBackAmount;

				// ビューポートの四点を押し戻す。
				ViewPort::Instance()->PushBackAura(Vec2<float>(biggestBuff.pushBackAmount, 0));

			}

			if (biggestBuff.isHold) {
				ViewPort::Instance()->holdFlags[ViewPort::Instance()->RIGHT] = true;
				//ViewPort::Instance()->PushBackLine(ViewPort::Instance()->LEFT, biggestBuff.pushBackAmount);
			}

		}
		else
		{
			ViewPort::Instance()->holdFlags[ViewPort::Instance()->RIGHT] = false;
			if (countStopNum % 2 != 0)
			{
				ScrollMgr::Instance()->StopScroll(0);
				countHitNum = 0;
				countStopNum = 0;
			}
			++countHitNum;

			if (2 <= countHitNum)
			{
				countStopNum = 0;
			}

		}

		// 下方向
		buff = ViewPortCollider::Instance()->CheckHitRectangleAura(ViewPort::Instance()->pointData[ViewPort::LEFT_DOWN], ViewPort::Instance()->pointData[ViewPort::RIGHT_DOWN], CHECK_HIT_BOTTOM, auraBlock);

		// 押し戻す
		if (buff.size() > 0 && !ViewPort::Instance()->noHitFlags[ViewPort::Instance()->DOWN])
		{

			ViewPortAuraReturnData biggestBuff = {};
			// 値が一番大きい押し戻し量を求める。
			for (int index = 0; index < buff.size(); ++index) {

				if (fabs(buff[index].pushBackAmount) < fabs(biggestBuff.pushBackAmount)) continue;

				biggestBuff = buff[index];

			}

			// ホールドされていない状態だったら-方向に押し戻されるのはおかしいので、押し戻し量を打ち消す。
			if (!biggestBuff.isHold && biggestBuff.pushBackAmount > 0)
			{
				biggestBuff.pushBackAmount = 0;
			}
			// ホールドされていなる状態だったら+方向に押し戻されるのはおかしいので、押し戻し量を打ち消す。
			else if (biggestBuff.isHold && biggestBuff.pushBackAmount < 0) {

				biggestBuff.pushBackAmount = 0;

			}

			// 上側に当たった判定を保存する。
			ViewPort::Instance()->isHitBottom = true;

			// ホールド状態だったら
			if (biggestBuff.isHold) {


				ViewPort::Instance()->PushBackAuraHoldDown(biggestBuff.pushBackAmount);

			}
			else {

				// 中心座標を押し戻す。
				ViewPort::Instance()->centralPos.y += biggestBuff.pushBackAmount;

				// ビューポートの四点を押し戻す。
				ViewPort::Instance()->PushBackAura(Vec2<float>(0, biggestBuff.pushBackAmount));

			}

			if (biggestBuff.isHold) {
				ViewPort::Instance()->holdFlags[ViewPort::Instance()->DOWN] = true;
				//ViewPort::Instance()->PushBackLine(ViewPort::Instance()->LEFT, biggestBuff.pushBackAmount);
			}

			// スクロールを止める。
			ScrollMgr::Instance()->StopScroll(0);
			++countStopNum;
		}
		else
		{
			ViewPort::Instance()->holdFlags[ViewPort::Instance()->DOWN] = false;
		}


		// 左方向
		buff = ViewPortCollider::Instance()->CheckHitRectangleAura(ViewPort::Instance()->pointData[ViewPort::LEFT_UP], ViewPort::Instance()->pointData[ViewPort::LEFT_DOWN], CHECK_HIT_LEFT, auraBlock);



		// 押し戻す
		if (buff.size() > 0 && !ViewPort::Instance()->noHitFlags[ViewPort::Instance()->LEFT])
		{

			ViewPortAuraReturnData biggestBuff = {};
			// 値が一番大きい押し戻し量を求める。
			for (int index = 0; index < buff.size(); ++index) {

				if (fabs(buff[index].pushBackAmount) < fabs(biggestBuff.pushBackAmount)) continue;

				biggestBuff = buff[index];

			}

			// ホールドされていない状態だったら-方向に押し戻されるのはおかしいので、押し戻し量を打ち消す。
			if (!biggestBuff.isHold && biggestBuff.pushBackAmount < 0)
			{
				biggestBuff.pushBackAmount = 0;
			}
			// ホールドされていなる状態だったら+方向に押し戻されるのはおかしいので、押し戻し量を打ち消す。
			else if (biggestBuff.isHold && biggestBuff.pushBackAmount > 0) {

				biggestBuff.pushBackAmount = 0;

			}

			//biggestBuff.pushBackAmount /= 2.0f;

			// 左側に当たった判定を保存する。
			ViewPort::Instance()->isHitLeft = true;

			// ホールド状態だったら
			if (biggestBuff.isHold) {


				ViewPort::Instance()->PushBackAuraHoldLeft(biggestBuff.pushBackAmount);

			}
			else {

				// 中心座標を押し戻す。
				ViewPort::Instance()->centralPos.x += biggestBuff.pushBackAmount;

				// ビューポートの四点を押し戻す。
				ViewPort::Instance()->PushBackAura(Vec2<float>(biggestBuff.pushBackAmount, 0));

			}

			if (biggestBuff.isHold) {
				ViewPort::Instance()->holdFlags[ViewPort::Instance()->LEFT] = true;
				//ViewPort::Instance()->PushBackLine(ViewPort::Instance()->LEFT, biggestBuff.pushBackAmount);
			}

			// スクロールを止める。
			ScrollMgr::Instance()->StopScroll(0);
			++countStopNum;
		}
		else
		{
			ViewPort::Instance()->holdFlags[ViewPort::Instance()->LEFT] = false;
		}


	}
	ViewPort::Instance()->playerPos = player.centerPos;
	//ライト更新
	auto pos = player.GetCenterDrawPos();
	ptLig.SetPos({ pos.x,pos.y,PT_LIG_Z });

	spotLig.SetTarget({ pos.x,pos.y + SPOT_LIG_TARGET_OFFSET_Y,0.0f });
	spotLig.SetPos({ pos.x,pos.y + SPOT_LIG_TARGET_OFFSET_Y,SPOT_LIG_Z });

	//パーティクル更新
	ParticleMgr::Instance()->Update();
}

void Game::Draw(std::weak_ptr<RenderTarget>EmissiveMap)
{
	int stageNum = SelectStage::Instance()->GetStageNum();
	int roomNum = SelectStage::Instance()->GetRoomNum();

	/*===== 描画処理 =====*/
	mapChipDrawData = StageMgr::Instance()->GetMapChipDrawBlock(stageNum, roomNum);
	DrawMapChip(mapData, mapChipDrawData, mapBlockGraph, stageNum, roomNum);

	MovingBlockMgr::Instance()->Draw(movingBlockGraph);

	// 弾パーティクルの描画処理
	//BulletParticleMgr::Instance()->Draw();

	// ドッスンブロックの描画処理
	const int DOSSUN_COUNT = dossunBlock.size();
	for (int index = 0; index < DOSSUN_COUNT; ++index) {
		dossunBlock[index].Draw();
	}

	// シャボン玉ブロックの描画処理
	{
		const int BUBBLE_COUNT = bubbleBlock.size();
		for (int index = 0; index < BUBBLE_COUNT; ++index) {
			if (bubbleBlock[index].isBreak) continue;
			bubbleBlock[index].Draw();
		}
	}

	//ViewPort::Instance()->Draw();

	//オーラブロックの描画
	for (int i = 0; i < auraBlock.size(); ++i)
	{
		auraBlock[i]->Draw();
	}

	for (int i = 0; i < thornBlocks.size(); ++i)
	{
		thornBlocks[i]->Draw();
	}


	player.Draw(ligMgr);
	ParticleMgr::Instance()->Draw(ligMgr);
	if (sceneBlackFlag || sceneLightFlag)
	{
		DrawFunc::DrawBox2D(Vec2<float>(0.0f, 0.0f), Vec2<float>(1280.0f, 720.0f), Color(0, 0, 0, alphaValue), D3D12App::Instance()->GetBackBuffFormat(), true, AlphaBlendMode_Trans);

		KuroEngine::Instance().Graphics().SetRenderTargets({ EmissiveMap.lock() });

		DrawFunc::DrawBox2D(Vec2<float>(0.0f, 0.0f), Vec2<float>(1280.0f, 720.0f), Color(0, 0, 0, alphaValue), EmissiveMap.lock()->GetDesc().Format, true, AlphaBlendMode_Trans);

		KuroEngine::Instance().Graphics().SetRenderTargets({ D3D12App::Instance()->GetBackBuffRenderTarget(),EmissiveMap.lock() });
	}
}
