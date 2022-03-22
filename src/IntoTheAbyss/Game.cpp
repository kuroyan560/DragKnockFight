#include "Game.h"
#include"MapChipCollider.h"
#include"ShakeMgr.h"
#include "BulletMgr.h"
#include"ScrollMgr.h"
#include"BulletParticleMgr.h"
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

bool Game::CheckUsedData(std::vector<Vec2<float>> DATA, Vec2<float> DATA2)
{
	for (int i = 0; i < DATA.size(); ++i)
	{
		if (DATA[i] == DATA2)
		{
			return true;
		}
	}
	return false;
}

#include<map>
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
		drawMap[i].Draw(TexHandleMgr::GetTexBuffer(itr->first));
		i++;
	}
}

Vec2<float> Game::GetPlayerResponePos(const int &STAGE_NUMBER, const int &ROOM_NUMBER, const int &DOOR_NUMBER, Vec2<float> DOOR_MAPCHIP_POS, E_DOOR_DIR *DIR)
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


	string result = "次につながるドアが見つかりません。\nRalationファイルを確認するか、担当の大石に連絡をください。";
	MessageBox(NULL, KuroFunc::GetWideStrFromStr(result).c_str(), TEXT("ドアが見つかりません"), MB_OK);
	assert(0);
	//失敗
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

Game::Game()
{
	mapBlockGraph = TexHandleMgr::LoadGraph("resource/IntoTheAbyss/Block.png");
	//mapBlockGraph = D3D12App::Instance()->GenerateTextureBuffer("resource/IntoTheAbyss/Block.png");

	// 弾管理クラスを初期化。
	BulletMgr::Instance()->Setting();

	// スクロール量を設定。
	const float WIN_WIDTH_HALF = WinApp::Instance()->GetWinCenter().x;
	const float WIN_HEIGHT_HALF = WinApp::Instance()->GetWinCenter().y;
	ScrollMgr::Instance()->scrollAmount = { -WIN_WIDTH_HALF, -WIN_HEIGHT_HALF };
	ScrollMgr::Instance()->honraiScrollAmount = { -WIN_WIDTH_HALF, -WIN_HEIGHT_HALF };

	// マップチップのデータをロード
	mapData = StageMgr::Instance()->GetMapChipData(0, 0);

	movingBlockGraph = TexHandleMgr::LoadGraph("resource/IntoTheAbyss/MovingBlock.png");
	//sceneChangeHandle = TexHandleMgr::LoadGraph("resource/IntoTheAbyss/SceneChange.png");
	//movingBlockGraph = D3D12App::Instance()->GenerateTextureBuffer("resource/IntoTheAbyss/MovingBlock.png");

	// スクロールマネージャーを初期化。
	ScrollMgr::Instance()->Init(&mapData);

	// シェイク量を設定。
	ShakeMgr::Instance()->Init();

	// 弾パーティクルをセッティング。
	BulletParticleMgr::Instance()->Setting();

	//マップ開始時の場所にスポーンさせる
	for (int y = 0; y < mapData.size(); ++y)
	{
		for (int x = 0; x < mapData[y].size(); ++x)
		{
			if (mapData[y][x] == MAPCHIP_BLOCK_START)
			{
				player.centerPos = { (float)x * 50.0f,(float)y * 50.0f };
				break;
			}
		}
	}

	StageMgr::Instance()->loadGimmickData->GetThowpeData(0, 0);

	//オーラブロック生成
	int auraChipNum = 40;//オーラブロックのチップ番号
	vector<Vec2<float>>usedNum;	//どのマップチップ番号が使われたか
	for (int y = 0; y < mapData.size(); ++y)
	{
		for (int x = 0; x < mapData[y].size(); ++x)
		{
			if (mapData[y][x] == auraChipNum)
			{
				//重複したら処理を飛ばす-----------------------
				bool usedFlag = false;
				for (int i = 0; i < usedNum.size(); ++i)
				{
					if (usedNum[i].x == x && usedNum[i].y == y)
					{
						usedFlag = true;
					}
				}
				//重複したら処理を飛ばす-----------------------

				if (!usedFlag)
				{
#pragma region 上下左右の探索
					vector<Vec2<float>>left, right, up, down;
					usedNum.push_back(Vec2<float>(x, y));
					//上にどれくらい伸びるか
					for (int upY = 1; 1; ++upY)
					{
						int Y = StageMgr::Instance()->GetMapChipBlock(stageNum, roomNum, Vec2<float>(x, y - upY));

						if (Y != auraChipNum || CheckUsedData(usedNum, Vec2<float>(x, y - upY)))
						{
							break;
						}
						else
						{
							usedNum.push_back(Vec2<float>(x, y - upY));
							up.push_back(Vec2<float>(x, y - upY));
						}
					}
					//下にどれくらい伸びるか
					for (int downY = 1; 1; ++downY)
					{
						int Y = StageMgr::Instance()->GetMapChipBlock(stageNum, roomNum, Vec2<float>(x, y + downY));
						if (Y != auraChipNum || CheckUsedData(usedNum, Vec2<float>(x, y + downY)))
						{
							break;
						}
						else
						{
							usedNum.push_back(Vec2<float>(x, y + downY));
							down.push_back(Vec2<float>(x, y + downY));
						}
					}

					//上下に伸ばす事が出来たらここの部分を飛ばす-----------------------
					if (down.size() == 0 && up.size() == 0)
					{
						//左にどれくらい伸びるか
						for (int leftX = 1; 1; ++leftX)
						{
							int X = StageMgr::Instance()->GetMapChipBlock(stageNum, roomNum, Vec2<float>(x - leftX, y));
							if (X != auraChipNum || CheckUsedData(usedNum, Vec2<float>(x - leftX, y)))
							{
								break;
							}
							else
							{
								usedNum.push_back(Vec2<float>(x - leftX, y));
								left.push_back(Vec2<float>(x - leftX, y));
							}
						}
						//右にどれくらい伸びるか
						for (int rightX = 1; 1; ++rightX)
						{
							int X = StageMgr::Instance()->GetMapChipBlock(stageNum, roomNum, Vec2<float>(x + rightX, y));
							if (X != auraChipNum || CheckUsedData(usedNum, Vec2<float>(x + rightX, y)))
							{
								break;
							}
							else
							{
								usedNum.push_back(Vec2<float>(x + rightX, y));
								right.push_back(Vec2<float>(x + rightX, y));
							}
						}
					}
					//上下に伸ばす事が出来たらここの部分を飛ばす-----------------------
#pragma endregion

#pragma region ブロックの追加

					//どの方向に伸びたか-----------------------
					//左右
					Vec2<float> leftUp, rightDown;
					bool sideOrUpDownFlag = false;
					if (left.size() || right.size())
					{
						int leftSize = left.size() - 1;
						int rightSize = right.size() - 1;

						if (leftSize <= 0)
						{
							leftSize = 0;
						}
						if (rightSize <= 0)
						{
							rightSize = 0;
						}

						//上が0なら
						if (left.size() == 0)
						{
							leftUp = { x * 50.0f,y * 50.0f };
						}
						else
						{
							leftUp = { left[leftSize].x * 50.0f,left[leftSize].y * 50.0f };
						}
						//下が0なら
						if (right.size() == 0)
						{
							rightDown = { x * 50.0f,y * 50.0f };
						}
						else
						{
							rightDown = { right[rightSize].x * 50.0f,right[rightSize].y * 50.0f };
						}

						sideOrUpDownFlag = true;
					}
					//上下
					if (up.size() || down.size())
					{
						int upSize = up.size() - 1;
						int downSize = down.size() - 1;

						if (upSize <= 0)
						{
							upSize = 0;
						}
						if (downSize <= 0)
						{
							downSize = 0;
						}

						//上が0なら
						if (up.size() == 0)
						{
							leftUp = { x * 50.0f,y * 50.0f };
						}
						else
						{
							leftUp = { up[upSize].x * 50.0f,up[upSize].y * 50.0f };
						}
						//下が0なら
						if (down.size() == 0)
						{
							rightDown = { x * 50.0f,y * 50.0f };
						}
						else
						{
							rightDown = { down[downSize].x * 50.0f,down[downSize].y * 50.0f };
						}
						sideOrUpDownFlag = false;
					}

					//ブロックサイズに合うように調整する
					leftUp.x -= 25.0f;
					leftUp.y -= 25.0f;
					rightDown.x += 25.0f;
					rightDown.y += 25.0f;
					//どの方向に伸びたか-----------------------

					//伸びた情報をオーラに渡す
					auraBlock.push_back(std::make_unique<AuraBlock>());
					if (sideOrUpDownFlag)
					{
						auraBlock[auraBlock.size() - 1]->Init(leftUp, rightDown, AURA_DIR_LEFTORRIGHT);
					}
					else
					{
						auraBlock[auraBlock.size() - 1]->Init(leftUp, rightDown, AURA_DIR_UPORDOWN);
					}
#pragma endregion
				}
			}
		}
	}

	ViewPort::Instance()->Init(player.centerPos, { 800.0f,500.0f });

	mapChipDrawData = StageMgr::Instance()->GetMapChipDrawBlock(stageNum, roomNum);
	alphaValue = 0;
}

void Game::Update()
{
	ScrollMgr::Instance()->zoom = ViewPort::Instance()->zoomRate;



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
		stageNum = debugStageData[0];
		roomNum = debugStageData[1];
		mapData = StageMgr::Instance()->GetMapChipData(stageNum, roomNum);

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
		player.centerPos = door * Vec2<float>(50.0f, 50.0f);
		ScrollMgr::Instance()->WarpScroll(player.centerPos);
	}
#pragma endregion
	//ImGui::Begin("Stage");
	//ImGui::Text("StageNumber:%d", debugStageData[0]);
	//ImGui::Text("RoomNumber:%d", debugStageData[1]);
	//ImGui::End();







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

	/*===== 更新処理 =====*/

	//ドア判定-----------------------
	SizeData chipMemorySize = StageMgr::Instance()->GetMapChipSizeData(MAPCHIP_TYPE_DOOR);
	for (int i = chipMemorySize.min; i < chipMemorySize.max; ++i)
	{
		//奥扉判定-----------------------
		Vec2<float> playerChip((player.centerPos.x + 25.0f) / 50.0f, (player.centerPos.y + 25.0f) / 50.0f);
		Vec2<float> playerChip2((player.centerPos.x + 25.0f) / 50.0f, (player.centerPos.y + 75.0f) / 50.0f);
		Vec2<float> rightCehck(playerChip.x + 1, playerChip.y);
		int rightChip = StageMgr::Instance()->GetMapChipBlock(stageNum, roomNum, rightCehck);
		Vec2<float> leftCehck(playerChip.x - 1, playerChip.y);
		int leftChip = StageMgr::Instance()->GetMapChipBlock(stageNum, roomNum, leftCehck);
		bool zDoorFlag = rightChip == 0 && leftChip == 0;

		int num = 1;
		bool hitDoorFlag;
		//奥扉判定-----------------------


		int nowChip = StageMgr::Instance()->GetMapChipBlock(stageNum, roomNum, playerChip);
		int nowChip2 = StageMgr::Instance()->GetMapChipBlock(stageNum, roomNum, playerChip2);
		int prevChip = StageMgr::Instance()->GetMapChipBlock(stageNum, roomNum, prevPlayerChipPos);

		bool goToNextStageFlag = (nowChip == -1 && nowChip2 == -1);

		//どのドアに触れたか
		if (nowChip == i)
		{
			giveDoorNumber = i;
		}
		bool tochDoorFlag = chipMemorySize.min <= giveDoorNumber && giveDoorNumber <= chipMemorySize.max;


		//ドアとの判定
		//最後に通ったドア番号を保存、シーン遷移開始、プレイヤーのカーソル描画無効
		if (goToNextStageFlag && !zDoorFlag && !sceneBlackFlag && !sceneLightFlag && tochDoorFlag)
		{
			sceneBlackFlag = true;
			doorNumber = giveDoorNumber;
			player.drawCursorFlag = false;
		}
		else if (goToNextStageFlag && zDoorFlag && UsersInput::Instance()->OnTrigger(XBOX_BUTTON::B) && !sceneBlackFlag && !sceneLightFlag && tochDoorFlag)
		{
			sceneBlackFlag = true;
			doorNumber = giveDoorNumber;
			player.drawCursorFlag = false;
		}
		prevPlayerChipPos = playerChip;
	}
	//ドア判定-----------------------



	//シーン遷移-----------------------
	//暗転中
	if (sceneBlackFlag)
	{
		alphaValue += 10;

		//暗転フラグを下げ、明転準備に入る
		if (255 <= alphaValue)
		{
			//プレイヤーの動きを止める
			player.vel = { 0.0f,0.0f };
			player.gravity = 0.0f;
			alphaValue = 355;
			++timer;

			if (10 <= timer)
			{
				if (!player.isDead)
				{
					//マップ情報更新
					int localRoomNum = StageMgr::Instance()->GetRelationData(stageNum, roomNum, doorNumber - chipMemorySize.min);
					mapData = StageMgr::Instance()->GetMapChipData(stageNum, localRoomNum);
					//ドア座標を入手
					Vec2<float>doorPos = GetDoorPos(doorNumber, mapData);
					//プレイヤーがリスポーンする座標を入手
					responePos = GetPlayerResponePos(stageNum, roomNum, doorNumber, doorPos, &door);


					sceneChangingFlag = true;
					//画面外から登場させる
					switch (door)
					{
					case Game::DOOR_UP_GORIGHT:
						ScrollMgr::Instance()->WarpScroll(responePos);
						initJumpFlag = false;
						player.centerPos = { responePos.x + 50.0f , responePos.y - 50.0f * 2.0f };
						break;

					case Game::DOOR_UP_GOLEFT:
						ScrollMgr::Instance()->WarpScroll(responePos);
						initJumpFlag = false;
						player.centerPos = { responePos.x + 50.0f , responePos.y - 50.0f * 2.0f };
						break;

					case Game::DOOR_DOWN:
						player.centerPos = { responePos.x, responePos.y - 50.0f * 2.0f };
						ScrollMgr::Instance()->WarpScroll(player.centerPos);
						break;

					case Game::DOOR_LEFT:
						ScrollMgr::Instance()->WarpScroll(responePos);
						player.centerPos = { responePos.x + 50.0f * 3.0f, responePos.y };
						break;

					case Game::DOOR_RIGHT:
						ScrollMgr::Instance()->WarpScroll(responePos);
						player.centerPos = { responePos.x - 50.0f * 3.0f, responePos.y };
						break;

					case Game::DOOR_Z:
						break;

					default:
						break;
					}
				}
				//暗転開始
				sceneBlackFlag = false;
				sceneLightFlag = true;
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
			if (!player.isDead)
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
			}
			else
			{
				// プレイヤーを死んでない判定にする。
				if (player.isDead)
				{
					//プレイヤーをリスポーンさせる
					switch (door)
					{
					case Game::DOOR_UP_GORIGHT:
						player.centerPos = { responePos.x + 50.0f * 2,responePos.y };
						break;

					case Game::DOOR_UP_GOLEFT:
						player.centerPos = { responePos.x - 50.0f * 2,responePos.y };
						break;

					case Game::DOOR_DOWN:
						player.centerPos = responePos;
						break;

					case Game::DOOR_LEFT:
						player.centerPos = responePos;
						break;

					case Game::DOOR_RIGHT:
						player.centerPos = responePos;
						break;
					case Game::DOOR_Z:
						break;
					default:
						break;
					}
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


	if (UsersInput::Instance()->OnTrigger(DIK_U))
	{
		player.isDead = true;
		sceneBlackFlag = true;
	}



	//ステージ毎の切り替え判定
	//部屋の初期化
	if ((roomNum != oldRoomNum || stageNum != oldStageNum) || SelectStage::Instance()->resetStageFlag)
	{
		debugStageData[0] = stageNum;
		debugStageData[1] = roomNum;

		mapChipDrawData = StageMgr::Instance()->GetMapChipDrawBlock(stageNum, roomNum);

		// ドッスンブロックを生成。
		vector<shared_ptr<ThownpeData>> dossunData;

		// ドッスンブロックデータを取得
		dossunData = GimmickLoader::Instance()->GetThowpeData(stageNum, roomNum);

		const int dossunCount = dossunData.size();

		// 照準の判定に使用するデータを保存。
		SightCollisionStorage::Instance()->data.clear();

		// ドッスンブロックを初期化。
		dossunBlock.clear();
		dossunBlock.push_back({});
		dossunBlock[0].Generate(player.centerPos + Vec2<float>(100, 0), player.centerPos + Vec2<float>(1000, 0), Vec2<float>(MAP_CHIP_SIZE, MAP_CHIP_SIZE), GIMMICK_DOSSN_ON_LOW);
		SightCollisionStorage::Instance()->data.push_back(dossunBlock[dossunBlock.size() - 1].sightData);

		// ドッスンを生成。
		for (int index = 0; index < dossunCount; ++index) {

			DossunBlock dossunBuff;
			//dossunBuff.Generate(dossunData[index]->startPos, dossunData[index]->endPos, dossunData[index]->size, dossunData[index]->type);

			// データを追加。
			//dossunBlock.push_back(dossunBuff);

			SightCollisionStorage::Instance()->data.push_back(dossunBlock[dossunBlock.size() - 1].sightData);

		}



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

		player.isDead = false;

		if (SelectStage::Instance()->resetStageFlag)
		{
			bool responeFlag = false;
			//マップ開始時の場所にスポーンさせる
			for (int y = 0; y < mapData.size(); ++y)
			{
				for (int x = 0; x < mapData[y].size(); ++x)
				{
					if (mapData[y][x] == MAPCHIP_BLOCK_START)
					{
						player.centerPos = { (float)x * 50.0f,(float)y * 50.0f };
						responeFlag = true;
						break;
					}
				}
			}

			//マップ開始用の場所からリスポーンしたらこの処理を通さない
			if (!responeFlag)
			{
				for (int y = 0; y < mapData.size(); ++y)
				{
					for (int x = 0; x < mapData[y].size(); ++x)
					{
						if (mapData[y][x] == MAPCHIP_BLOCK_DEBUG_START)
						{
							player.centerPos = { (float)x * 50.0f,(float)y * 50.0f };
							ScrollMgr::Instance()->WarpScroll(player.centerPos);
							break;
						}
					}
				}
			}
		}
		SelectStage::Instance()->resetStageFlag = false;
	}
	oldRoomNum = roomNum;
	oldStageNum = stageNum;



	// R or Aが押されたらプレイヤーの位置を初期化する。
	//if (Input::isKeyTrigger(KEY_INPUT_R) || Input::isJoyBottomTrigger(XINPUT_BUTTON_A))
	if (UsersInput::Instance()->OnTrigger(DIK_R) || UsersInput::Instance()->OnTrigger(XBOX_BUTTON::A))
	{
		player.centerPos = player.GetGeneratePos();
		ScrollMgr::Instance()->scrollAmount = { -3.0f, 295.0f };
		ScrollMgr::Instance()->honraiScrollAmount = { 0, 295.0f };
	}

	ScrollMgr::Instance()->DetectMapChipForScroll(player.centerPos);





	// プレイヤーの更新処理
	player.Update(mapData);

	//if (Input::isKey(KEY_INPUT_RIGHT)) player.centerPos.x += 1.0f;
	if (UsersInput::Instance()->OnTrigger(DIK_RIGHT)) player.centerPos.x += 1.0f;
	//if (Input::isKey(KEY_INPUT_P)) player.centerPos.x += 100.0f;
	if (UsersInput::Instance()->OnTrigger(DIK_P)) player.centerPos.x += 100.0f;
	//if (Input::isKey(KEY_INPUT_LEFT)) player.centerPos.x -= 1.0f;
	if (UsersInput::Instance()->OnTrigger(DIK_LEFT)) player.centerPos.x -= 1.0f;
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
	BulletParticleMgr::Instance()->Update();

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

	//if (Input::isKey(KEY_INPUT_UP)) ViewPort::Instance()->zoomRate += 0.01f;
	//if (UsersInput::Instance()->OnTrigger(DIK_UP))  ViewPort::Instance()->zoomRate += 0.01f;
	//if (Input::isKey(KEY_INPUT_DOWN)) ViewPort::Instance()->zoomRate -= 0.01f;
	//if (UsersInput::Instance()->OnTrigger(DIK_DOWN)) ViewPort::Instance()->zoomRate -= 0.01f;

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
	BulletMgr::Instance()->CheckHit(mapData);

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
			BulletParticleMgr::Instance()->Generate(BulletMgr::Instance()->GetBullet(i)->pos, BulletMgr::Instance()->GetBullet(i)->forwardVec);
			BulletParticleMgr::Instance()->Generate(BulletMgr::Instance()->GetBullet(i)->pos, BulletMgr::Instance()->GetBullet(i)->forwardVec);

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


}

void Game::Draw()
{
	/*===== 描画処理 =====*/

	DrawMapChip(mapData, mapChipDrawData, mapBlockGraph, 0, roomNum);

	MovingBlockMgr::Instance()->Draw(movingBlockGraph);

	// 弾パーティクルの描画処理
	BulletParticleMgr::Instance()->Draw();

	// ドッスンブロックの描画処理
	const int DOSSUN_COUNT = dossunBlock.size();
	for (int index = 0; index < DOSSUN_COUNT; ++index) {
		dossunBlock[index].Draw();
	}

	player.Draw();

	// シャボン玉ブロックの描画処理
	{
		const int BUBBLE_COUNT = bubbleBlock.size();
		for (int index = 0; index < BUBBLE_COUNT; ++index) {
			if (bubbleBlock[index].isBreak) continue;
			bubbleBlock[index].Draw();
		}
	}

	//ViewPort::Instance()->Draw();


	for (int i = 0; i < auraBlock.size(); ++i)
	{
		auraBlock[i]->Draw();
	}

	if (sceneBlackFlag || sceneLightFlag)
	{
		DrawFunc::DrawBox2D(Vec2<float>(0.0f, 0.0f), Vec2<float>(1280.0f, 720.0f), Color(0, 0, 0, alphaValue), true, AlphaBlendMode_Trans);
	}
}
