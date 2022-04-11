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
#include"GUI.h"

#include"SuperiorityGauge.h"
#include"BackGround.h"
#include"Camera.h"
#include"GameTimer.h"
#include"ScoreManager.h"

#include"BossBulletManager.h"

#include"BulletCollision.h"


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

void Game::DrawMapChip(const vector<vector<int>> &mapChipData, vector<vector<MapChipDrawData>> &mapChipDrawData, const int &stageNum, const int &roomNum)
{
	std::map<int, std::vector<ChipData>>datas;

	// 描画するチップのサイズ
	const float DRAW_MAP_CHIP_SIZE = MAP_CHIP_SIZE * ScrollMgr::Instance()->zoom;
	SizeData eventChipMemorySize = StageMgr::Instance()->GetMapChipSizeData(MAPCHIP_TYPE_EVENT);
	SizeData wallChipMemorySize = StageMgr::Instance()->GetMapChipSizeData(MAPCHIP_TYPE_STATIC_BLOCK);
	SizeData doorChipMemorySize = StageMgr::Instance()->GetMapChipSizeData(MAPCHIP_TYPE_DOOR);


	// マップチップの縦の要素数を取得。
	const int HEIGHT = mapChipData.size();
	for (int height = 0; height < HEIGHT; ++height) {

		// マップチップの横の要素数を取得。
		const int WIDTH = mapChipData[height].size();
		for (int width = 0; width < WIDTH; ++width) {

			// ブロック以外だったら処理を飛ばす。
			bool blockFlag = (mapChipData[height][width] >= 1 && mapChipData[height][width] <= 9);
			bool doorFlag = (mapChipData[height][width] >= doorChipMemorySize.min && mapChipData[height][width] <= doorChipMemorySize.max);
			bool eventFlag = (mapChipData[height][width] >= eventChipMemorySize.min && mapChipData[height][width] <= eventChipMemorySize.max);
			bool thownFlag = mapChipData[height][width] == 41;


			if (blockFlag || doorFlag || eventFlag || thownFlag)
			{
				// スクロール量から描画する位置を求める。
				const Vec2<float> drawPos = ScrollMgr::Instance()->Affect({ width * MAP_CHIP_SIZE,height * MAP_CHIP_SIZE });

				// 画面外だったら描画しない。
				if (drawPos.x < -DRAW_MAP_CHIP_SIZE || drawPos.x > WinApp::Instance()->GetWinSize().x + DRAW_MAP_CHIP_SIZE) continue;
				if (drawPos.y < -DRAW_MAP_CHIP_SIZE || drawPos.y > WinApp::Instance()->GetWinSize().y + DRAW_MAP_CHIP_SIZE) continue;


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

				Vec2<float> pos = drawPos;
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

#include"PlayerHand.h"
void Game::InitGame(const int &STAGE_NUM, const int &ROOM_NUM)
{
	//AudioApp::Instance()->PlayWave(bgm, true);

	int stageNum = STAGE_NUM;
	int roomNum = ROOM_NUM;

	SuperiorityGauge::Instance()->Init();

	//ドアが繋がっているか確認
	if (!SelectStage::Instance()->resetStageFlag)
	{
		RoomMapChipArray tmpMap = StageMgr::Instance()->GetMapChipData(stageNum, roomNum);
		std::vector<std::unique_ptr<DoorBlock>> tmpDoor;
		SizeData chipMemorySize = StageMgr::Instance()->GetMapChipSizeData(MAPCHIP_TYPE_DOOR);
		for (int doorIndex = chipMemorySize.min; doorIndex < chipMemorySize.max; ++doorIndex)
		{
			std::vector<std::unique_ptr<MassChipData>> data = AddData(tmpMap, doorIndex);
			for (int i = 0; i < data.size(); ++i)
			{
				tmpDoor.push_back(std::make_unique<DoorBlock>());
				int doorBlocksArrayNum = tmpDoor.size() - 1;
				tmpDoor[doorBlocksArrayNum]->Init(data[i]->leftUpPos, data[i]->rightDownPos, doorIndex, data[i]->sideOrUpDownFlag);
			}
		}

		bool responeErrorFlag = true;
		for (int tmpDoorIndex = 0; tmpDoorIndex < tmpDoor.size(); ++tmpDoorIndex)
		{
			if (tmpDoor[tmpDoorIndex]->chipNumber == doorNumber)
			{
				responeErrorFlag = false;
			}
		}

		if (responeErrorFlag)
		{
			string result = "次につながるドアが見つかりません。\nRalationファイルを確認するか、担当の大石に連絡をください。";
			MessageBox(NULL, KuroFunc::GetWideStrFromStr(result).c_str(), TEXT("ドアが見つかりません"), MB_OK);
			stageNum = oldStageNum;
			roomNum = oldRoomNum;
			SelectStage::Instance()->SelectStageNum(oldStageNum);
			SelectStage::Instance()->SelectRoomNum(oldRoomNum);
			sceneLightFlag = false;
		}
	}


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
				doorBlocks[doorBlocksArrayNum]->Init(data[i]->leftUpPos, data[i]->rightDownPos, doorIndex, data[i]->sideOrUpDownFlag);
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
	//giveDoorNumber = 0;
	debugStageData[0] = stageNum;
	debugStageData[1] = roomNum;



	//ドア遷移はプレイヤーを初期化しない
	if (!sceneLightFlag)
	{
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
					restartPos = Vec2<float>(x * 50.0f, y * 50.0f);
					player.Init(restartPos);
					responeFlag = true;

					// 紐の中心点を計算
					Vec2<float> bossDir = boss.pos - player.centerPos;
					bossDir.Normalize();
					float playerLineLength = lineLengthPlayer + addLineLengthPlayer;
					lineCenterPos = player.centerPos + bossDir * Vec2<float>(playerLineLength, playerLineLength);
					prevLineCenterPos = lineCenterPos;

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
						restartPos = Vec2<float>(x * 50.0f, y * 50.0f);
						player.Init(restartPos);
						break;
					}
				}
			}
		}

		//ScrollMgr::Instance()->honraiScrollAmount = { -1060.0f,-490.0f };
#pragma endregion
		alphaValue = 0;
	}
	else
	{
		//シーン遷移後の初期化
		for (int i = 0; i < doorBlocks.size(); ++i)
		{
			if (doorBlocks[i]->chipNumber == doorNumber)
			{
				responePos = doorBlocks[i]->responePos;
				restartPos = doorBlocks[i]->restartPos;
				player.Init(responePos);

				// 紐の中心点を計算
				Vec2<float> bossDir = boss.pos - player.centerPos;
				bossDir.Normalize();
				float playerLineLength = lineLengthPlayer + addLineLengthPlayer;
				lineCenterPos = player.centerPos + bossDir * Vec2<float>(playerLineLength, playerLineLength);
				prevLineCenterPos = lineCenterPos;

				//ScrollMgr::Instance()->DetectMapChipForScroll(lineCenterPos);
				//ScrollMgr::Instance()->WarpScroll(lineCenterPos);
				door = doorBlocks[i]->doorDir;
				break;
			}
		}
	}

	ViewPort::Instance()->Init(player.centerPos, { 800.0f,500.0f });

	sceneBlackFlag = false;
	//sceneLightFlag = false;
	SelectStage::Instance()->resetStageFlag = false;
	restartTimer = 0.0f;


	{
		Vec2<float>playerLeftUpPos;
		Vec2<float>playerRightDownPos;
		Vec2<float>enemyLeftUpPos;
		Vec2<float>enemyRightDownPos;
		for (int y = 0; y < mapData.size(); ++y)
		{
			for (int x = 0; x < mapData[y].size(); ++x)
			{
				if (mapData[y][x] == 33)
				{
					playerLeftUpPos = Vec2<float>(x * MAP_CHIP_SIZE, y * MAP_CHIP_SIZE);
				}
				if (mapData[y][x] == 34)
				{
					playerRightDownPos = Vec2<float>(x * MAP_CHIP_SIZE, y * MAP_CHIP_SIZE);
				}
				if (mapData[y][x] == 35)
				{
					enemyLeftUpPos = Vec2<float>(x * MAP_CHIP_SIZE, y * MAP_CHIP_SIZE);
				}
				if (mapData[y][x] == 36)
				{
					enemyRightDownPos = Vec2<float>(x * MAP_CHIP_SIZE, y * MAP_CHIP_SIZE);
				}
			}
		}

		Vec2<float>chipPos(MAP_CHIP_HALF_SIZE, MAP_CHIP_HALF_SIZE);
		playerHomeBase->Init(playerLeftUpPos - chipPos, playerRightDownPos + chipPos);
		enemyHomeBase->Init(enemyLeftUpPos - chipPos, enemyRightDownPos + chipPos);

		{
			float size = (mapData[0].size() * MAP_CHIP_SIZE) - 400.0f;
			miniMap.Init(size);
		}


		lineLengthPlayer = LINE_LENGTH;
		lineLengthBoss = LINE_LENGTH;
		addLineLengthPlayer = 0;
		addLineLengthBoss = 0;
		lineCenterPos = {};
		prevLineCenterPos = {};

		isCatchMapChipBoss = false;
		isCatchMapChipPlayer = false;
	}


	Vec2<float> responePos((mapData[0].size() * MAP_CHIP_SIZE) * 0.5f, (mapData.size() * MAP_CHIP_SIZE) * 0.5f);
	responePos -= 100;

	lineCenterPos = responePos;
	player.Init(responePos - Vec2<float>(300.0f, 0.0f));
	//ボスを生成
	boss.Generate(responePos + Vec2<float>(300.0f, 0.0f));

	miniMap.CalucurateCurrentPos(lineCenterPos);

	Camera::Instance()->Init();

	//ScrollMgr::Instance()->Init(&mapData);
	//// スクロール量を設定。
	//const float WIN_WIDTH_HALF = WinApp::Instance()->GetWinCenter().x;
	//const float WIN_HEIGHT_HALF = WinApp::Instance()->GetWinCenter().y;
	//ScrollMgr::Instance()->scrollAmount = { -WIN_WIDTH_HALF, -WIN_HEIGHT_HALF };
	//ScrollMgr::Instance()->honraiScrollAmount = { -WIN_WIDTH_HALF, -WIN_HEIGHT_HALF };

	//ScrollMgr::Instance()->DetectMapChipForScroll(lineCenterPos);
	//ScrollMgr::Instance()->WarpScroll(lineCenterPos);
	//ScrollMgr::Instance()->CalucurateScroll(prevLineCenterPos - lineCenterPos);



	ScrollMgr::Instance()->Init(lineCenterPos, Vec2<float>(mapData[0].size() * MAP_CHIP_SIZE, mapData.size() * MAP_CHIP_SIZE), cameraBasePos);


	GameTimer::Instance()->Init({}, 120, {}, {});
	GameTimer::Instance()->Start();


	ScoreManager::Instance()->Init();
	firstLoadFlag = false;

	//背景に星
	BackGround::Instance()->Init(GetStageSize());

}

Game::Game()
{
	//mapBlockGraph = D3D12App::Instance()->GenerateTextureBuffer("resource/IntoTheAbyss/Block.png");

	// 弾管理クラスを初期化。
	BulletMgr::Instance()->Setting();

	movingBlockGraph = TexHandleMgr::LoadGraph("resource/IntoTheAbyss/MovingBlock.png");
	//sceneChangeHandle = TexHandleMgr::LoadGraph("resource/IntoTheAbyss/SceneChange.png");
	//movingBlockGraph = D3D12App::Instance()->GenerateTextureBuffer("resource/IntoTheAbyss/MovingBlock.png");

	// 弾パーティクルをセッティング。
	//BulletParticleMgr::Instance()->Setting();

	bgm = AudioApp::Instance()->LoadAudio("resource/ChainCombat/sound/bgm_1.wav");
	AudioApp::Instance()->ChangeVolume(bgm, 0.1f);

	//ライト情報
	ptLig.SetInfluenceRange(PT_LIG_RANGE);
	ptLig.SetColor(Color(PT_LIG_BRIGHT, PT_LIG_BRIGHT, PT_LIG_BRIGHT, 1.0f));
	spotLig.SetInfluenceRange(SPOT_LIG_RANGE);

	ligMgr.RegisterPointLight(&ptLig);
	ligMgr.RegisterSpotLight(&spotLig);
	ligMgr.RegisterHemiSphereLight(&hemiLig);
	ligMgr.RegisterPointLight(&player.lHand->ptLight);	//照準光源
	ligMgr.RegisterPointLight(&player.rHand->ptLight);//照準光源



	Init();

	playerHomeBase = std::make_unique<HomeBase>();
	enemyHomeBase = std::make_unique<HomeBase>();

	playerHomeBase->Init({ 0.0f,0.0f }, { 0.0f,0.0f });
	enemyHomeBase->Init({ 0.0f,0.0f }, { 800.0f,1000.0f });
	//enemyHomeBase->Init({ 0.0f,0.0f }, { 0.0f,0.0f });

	cameraBasePos = { 0.0f,-40.0f };
}

void Game::Init()
{
	// スクロールマネージャーを初期化。
	//ScrollMgr::Instance()->Init(&mapData);
	//// スクロール量を設定。
	//const float WIN_WIDTH_HALF = WinApp::Instance()->GetWinCenter().x;
	//const float WIN_HEIGHT_HALF = WinApp::Instance()->GetWinCenter().y;
	//ScrollMgr::Instance()->scrollAmount = { -WIN_WIDTH_HALF, -WIN_HEIGHT_HALF };
	//ScrollMgr::Instance()->honraiScrollAmount = { -WIN_WIDTH_HALF, -WIN_HEIGHT_HALF };

	//bossEnemy.Generate(ENEMY_BOSS, mapData);
	for (int index = 0; index < SMALL_ENEMY; ++index) {

		smallEnemy[index].Init();

	}
	for (int index = 0; index < NOMOVEMENT_ENEMY; ++index) {

		noMovementEnemy[index].Init();

	}
	enemyGenerateTimer = 0;
	nomoveMentTimer = 0;

	//ScrollMgr::Instance()->DetectMapChipForScroll(lineCenterPos);
	//ScrollMgr::Instance()->CalucurateScroll(prevLineCenterPos - lineCenterPos);
}

void Game::Update()
{
	//ScrollMgr::Instance()->zoom = ViewPort::Instance()->zoomRate;

#pragma region ステージの切り替え
	const bool enableToSelectStageFlag = 0 < debugStageData[0];
	const bool enableToSelectStageFlag2 = debugStageData[0] < StageMgr::Instance()->GetMaxStageNumber() - 1;
	//マップの切り替え
	//if (Input::isKeyTrigger(KEY_INPUT_UP) && enableToSelectStageFlag2 && nowSelectNum == 0)
	const bool up = UsersInput::Instance()->OnTrigger(DIK_UP) || UsersInput::Instance()->OnTrigger(DPAD_UP);
	const bool down = UsersInput::Instance()->OnTrigger(DIK_DOWN) || UsersInput::Instance()->OnTrigger(DPAD_DOWN);
	const bool left = UsersInput::Instance()->OnTrigger(DIK_LEFT) || UsersInput::Instance()->OnTrigger(DPAD_LEFT);
	const bool right = UsersInput::Instance()->OnTrigger(DIK_RIGHT) || UsersInput::Instance()->OnTrigger(DPAD_RIGHT);

	if (up && enableToSelectStageFlag2 && nowSelectNum == 0)
	{
		++debugStageData[0];
	}
	//if (Input::isKeyTrigger(KEY_INPUT_DOWN) && enableToSelectStageFlag && nowSelectNum == 0)
	if (down && enableToSelectStageFlag && nowSelectNum == 0)
	{
		--debugStageData[0];
	}


	const bool enableToSelectRoomFlag = 0 < debugStageData[1];
	const bool enableToSelectRoomFlag2 = debugStageData[1] < StageMgr::Instance()->GetMaxRoomNumber(debugStageData[0]) - 1;
	//部屋の切り替え
	//if (Input::isKeyTrigger(KEY_INPUT_UP) && enableToSelectRoomFlag2 && nowSelectNum == 1)
	if (up && enableToSelectRoomFlag2 && nowSelectNum == 1)
	{
		++debugStageData[1];
	}
	//if (Input::isKeyTrigger(KEY_INPUT_DOWN) && enableToSelectRoomFlag && nowSelectNum == 1)
	if (down && enableToSelectRoomFlag && nowSelectNum == 1)
	{
		--debugStageData[1];
	}

	//部屋か番号に切り替え
	//if (Input::isKeyTrigger(KEY_INPUT_LEFT) && 0 < nowSelectNum)
	if (left && 0 < nowSelectNum)
	{
		--nowSelectNum;
		debugStageData[1] = 0;
	}
	if (right && nowSelectNum < 1)
	{
		++nowSelectNum;
		debugStageData[1] = 0;
	}

	const bool done = UsersInput::Instance()->OnTrigger(DIK_RETURN) || UsersInput::Instance()->OnTrigger(A);
	if (done)
	{
		SelectStage::Instance()->SelectStageNum(debugStageData[0]);
		SelectStage::Instance()->SelectRoomNum(debugStageData[1]);
		SelectStage::Instance()->resetStageFlag = true;
		//mapData = StageMgr::Instance()->GetMapChipData(debugStageData[0], debugStageData[1]);

		//Vec2<float> door;
		////デバック用のマップチップ番号からスタートする
		//for (int y = 0; y < mapData.size(); ++y)
		//{
		//	for (int x = 0; x < mapData[y].size(); ++x)
		//	{
		//		if (mapData[y][x] == MAPCHIP_BLOCK_DEBUG_START)
		//		{
		//			door = { (float)x,(float)y };
		//		}
		//	}
		//}
		//player.Init(door * Vec2<float>(50.0f, 50.0f));
		//ScrollMgr::Instance()->WarpScroll(player.centerPos);
	}
#pragma endregion


	//ゴールに触れたら次のステージに向かう処理
	{
		Vec2<float> playerChip((player.centerPos.x + 25.0f) / 50.0f, (player.centerPos.y + 25.0f) / 50.0f);
		//ゴールに触れたら次のステージに向かう
		if (StageMgr::Instance()->GetMapChipBlock(SelectStage::Instance()->GetStageNum(), SelectStage::Instance()->GetRoomNum(), playerChip) == MAPCHIP_BLOCK_GOAL)
		{
			SelectStage::Instance()->SelectStageNum(0);
			SelectStage::Instance()->SelectRoomNum(0);

			mapData = StageMgr::Instance()->GetMapChipData(SelectStage::Instance()->GetStageNum(), SelectStage::Instance()->GetRoomNum());
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
		}
	}


#pragma region ドア移動
	if (!sceneBlackFlag && !sceneLightFlag)
	{
		for (int doorIndex = 0; doorIndex < doorBlocks.size(); ++doorIndex)
		{
			//当たり判定
			bool hitFlag = doorBlocks[doorIndex]->Collision(player.centerPos, player.PLAYER_HIT_SIZE, player.vel, player.prevFrameCenterPos);

			//ドアと当たったら数字を渡す
			if (hitFlag)
			{
				giveDoorNumber = doorBlocks[doorIndex]->chipNumber;
				doorArrayNumber = doorIndex;
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
			int nowChip = StageMgr::Instance()->GetMapChipBlock(SelectStage::Instance()->GetStageNum(), SelectStage::Instance()->GetRoomNum(), Vec2<float>((player.centerPos + Vec2<float>(0.0f, -MAP_CHIP_HALF_SIZE) + offset) / Vec2<float>(MAP_CHIP_SIZE, MAP_CHIP_SIZE)));
			int nowChip2 = StageMgr::Instance()->GetMapChipBlock(SelectStage::Instance()->GetStageNum(), SelectStage::Instance()->GetRoomNum(), Vec2<float>((player.centerPos + Vec2<float>(0.0f, MAP_CHIP_HALF_SIZE) + offset) / Vec2<float>(MAP_CHIP_SIZE, MAP_CHIP_SIZE)));

			//ドア移動判定
			bool outSideFlag = nowChip == -1 && nowChip2 == -1;//プレイヤーが画面外に出た判定
			if (outSideFlag)
			{
				sceneBlackFlag = true;
				doorNumber = giveDoorNumber;
				player.drawCursorFlag = false;
			}
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
					int localRoomNum = StageMgr::Instance()->GetRelationData(SelectStage::Instance()->GetStageNum(), SelectStage::Instance()->GetRoomNum(), doorNumber - chipMemorySize.min);

					RoomMapChipArray tmpMapData = StageMgr::Instance()->GetMapChipData(SelectStage::Instance()->GetStageNum(), localRoomNum);

					SelectStage::Instance()->SelectRoomNum(localRoomNum);
					sceneChangingFlag = true;
					initJumpFlag = false;
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
		player.drawCursorFlag = false;
		alphaValue -= 10;
		bool goFlag = false;

		//プレイヤーを動かす
		if (alphaValue <= 250)
		{
			if (!sceneChangeDeadFlag)
			{
				//sceneChangingFlag = false;
				switch (door)
				{
				case DOOR_UP_GORIGHT:
					//ジャンプする
					if (!initJumpFlag)
					{
						gravity = 1.0f;
						initJumpFlag = true;
					}
					player.centerPos.y += -15.0f * gravity;
					gravity -= 0.1f;

					if (!player.onGround)
					{
						player.centerPos.x += 5.0f;
					}
					else
					{
						goFlag = true;
					}
					break;


				case DOOR_UP_GOLEFT:
					//ジャンプする
					if (!initJumpFlag)
					{
						gravity = 1.0f;
						initJumpFlag = true;
					}
					player.centerPos.y += -15.0f * gravity;
					gravity -= 0.1f;

					if (!player.onGround)
					{
						player.centerPos.x -= 5.0f;
					}
					else
					{
						goFlag = true;
					}
					break;

				case DOOR_DOWN:
					goFlag = true;
					break;

				case DOOR_LEFT:
					if (restartPos.x <= player.centerPos.x)
					{
						player.centerPos.x -= 5.0f;
					}
					else
					{
						goFlag = true;
					}
					break;

				case DOOR_RIGHT:
					if (player.centerPos.x <= restartPos.x)
					{
						player.centerPos.x += 5.0f;
					}
					else
					{
						goFlag = true;
					}
					break;

				case DOOR_Z:
					break;
				default:
					break;
				}

				//ScrollMgr::Instance()->WarpScroll(lineCenterPos);
				//ScrollMgr::Instance()->CalucurateScroll(prevLineCenterPos - lineCenterPos);
				//ScrollMgr::Instance()->Restart();

			}
			else
			{
				if (!initDeadFlag)
				{
					player.Init(restartPos);

					// 紐の中心点を計算
					Vec2<float> bossDir = boss.pos - player.centerPos;
					bossDir.Normalize();
					float playerLineLength = lineLengthPlayer + addLineLengthPlayer;
					lineCenterPos = player.centerPos + bossDir * Vec2<float>(playerLineLength, playerLineLength);
					prevLineCenterPos = lineCenterPos;

					//ScrollMgr::Instance()->WarpScroll(lineCenterPos);
					//ScrollMgr::Instance()->CalucurateScroll(prevLineCenterPos - lineCenterPos);
					//ScrollMgr::Instance()->AlimentScrollAmount();
					//ScrollMgr::Instance()->Restart();
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
		player.drawCursorFlag = true;
	}
	//シーン遷移-----------------------
#pragma endregion


	const bool resetInput = UsersInput::Instance()->OnTrigger(DIK_SPACE) || UsersInput::Instance()->OnTrigger(BACK);
	if (resetInput)
	{
		SelectStage::Instance()->resetStageFlag = true;
		//player.isDead = true;
		//sceneBlackFlag = true;
		//sceneChangeDeadFlag = player.isDead;
	}


	//ステージ毎の切り替え判定
	//部屋の初期化
	if ((SelectStage::Instance()->GetRoomNum() != oldRoomNum || SelectStage::Instance()->GetStageNum() != oldStageNum) || SelectStage::Instance()->resetStageFlag)
	{
		InitGame(SelectStage::Instance()->GetStageNum(), SelectStage::Instance()->GetRoomNum());
		oldRoomNum = SelectStage::Instance()->GetRoomNum();
		oldStageNum = SelectStage::Instance()->GetStageNum();
	}

	//プレイヤー陣地と敵の判定
	if (playerHomeBase->Collision(boss.areaHitBox) && !roundFinishFlag && false)
	{
		//プレイヤー勝利
		++countRound;
		++countPlayerWin;
		roundFinishFlag = true;
		gameStartFlag = false;
	}

	//敵陣地とプレイヤーの判定
	if (enemyHomeBase->Collision(player.areaHitBox) && !roundFinishFlag)
	{
		//敵勝利
		++countRound;
		++countEnemyWin;
		roundFinishFlag = true;
		gameStartFlag = false;
	}

	//ラウンド終了演出開始
	if (roundFinishFlag)
	{
		readyToStartRoundFlag = true;
		roundFinishFlag = false;
	}

	//ラウンド開始時の演出開始
	if (readyToStartRoundFlag)
	{
		gameStartFlag = true;
		SelectStage::Instance()->resetStageFlag = true;
		readyToStartRoundFlag = false;
	}





	//イベントブロックとの判定
	for (int i = 0; i < eventBlocks.size(); ++i)
	{
		eventBlocks[i]->HitBox(player.centerPos, player.PLAYER_HIT_SIZE, player.vel, player.prevFrameCenterPos);
	}


#pragma region 死亡判定
	//遷移中ではない&&プレイヤーが死亡していな時に棘ブロックとの判定を取る
	if (!player.isDead && !sceneBlackFlag && !sceneLightFlag)
	{
		//棘ブロックとの判定
		for (int i = 0; i < thornBlocks.size(); ++i)
		{
			bool hitFlag = thornBlocks[i]->HitBox(player.centerPos, player.PLAYER_HIT_SIZE, player.vel, player.prevFrameCenterPos);
			if (hitFlag)
			{
				player.isDead = true;
				sceneBlackFlag = true;
				sceneChangeDeadFlag = player.isDead;
				break;
			}
		}
	}
#pragma endregion



	miniMap.CalucurateCurrentPos(lineCenterPos);

	// プレイヤーの更新処理
	player.Update(mapData, boss.pos);

	miniMap.Update();


	GameTimer::Instance()->Update();
	ScoreManager::Instance()->Update();


	//if (Input::isKey(KEY_INPUT_RIGHT)) player.centerPos.x += 1.0f;
	if (UsersInput::Instance()->Input(DIK_RIGHT)) player.centerPos.x += 1.0f;
	//if (Input::isKey(KEY_INPUT_P)) player.centerPos.x += 100.0f;
	if (UsersInput::Instance()->OnTrigger(DIK_P)) player.centerPos.x += 100.0f;
	//if (Input::isKey(KEY_INPUT_LEFT)) player.centerPos.x -= 1.0f;
	if (UsersInput::Instance()->Input(DIK_LEFT)) player.centerPos.x -= 1.0f;
	//if (Input::isKey(KEY_INPUT_O)) player.centerPos.x -= 100.0f;
	if (UsersInput::Instance()->OnTrigger(DIK_O)) player.centerPos.x -= 100.0f;


	if (UsersInput::Instance()->OnTrigger(DIK_J))
	{
		ScoreManager::Instance()->Add(10000);
	}
	if (UsersInput::Instance()->OnTrigger(DIK_K))
	{
		ScoreManager::Instance()->Sub(100);
	}



	// ボスの更新処理
	boss.Update();

	// プレイヤーとボスの引っ張り合いの処理
	Scramble();

	//	ScrollManager::Instance()->CalucurateScroll(prevLineCenterPos - lineCenterPos, lineCenterPos);
	ScrollMgr::Instance()->CalucurateScroll(prevLineCenterPos - lineCenterPos, lineCenterPos);



	// スクロール量の更新処理
	//ScrollManager::Instance()->Update();
	ScrollMgr::Instance()->Update();

	// シェイク量の更新処理
	ShakeMgr::Instance()->Update();

#pragma region 各ギミックの更新処理

	// 動的ブロックの更新処理
	MovingBlockMgr::Instance()->Update(player.centerPos);

	SuperiorityGauge::Instance()->Update();

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

#pragma endregion


	/*===== 当たり判定 =====*/

#pragma region 当たり判定

	// ドッスンブロックの当たり判定
	for (int index = 0; index < DOSSUN_COUNT; ++index) {
		dossunBlock[index].CheckHit(mapData);
	}

	// プレイヤーの当たり判定
	player.CheckHit(mapData, bubbleBlock, dossunBlock, boss.pos, isCatchMapChipPlayer, lineCenterPos);

	// 動的ブロックの当たり判定
	MovingBlockMgr::Instance()->CheckHit(mapData);

	// 弾とマップチップの当たり判定
	BulletMgr::Instance()->CheckHit(mapData, bubbleBlock);

	// ボスの当たり判定
	boss.CheckHit(mapData, isCatchMapChipBoss, player.centerPos, lineCenterPos);

	// ビューポートをプレイヤー基準で移動させる。
	ViewPort::Instance()->SetPlayerPosX(player.centerPos.x);
	ViewPort::Instance()->SetPlayerPosY(player.centerPos.y);

	// オーラブロックのデータとビューポートの判定を行う。
	ViewPort::Instance()->SavePrevFlamePos();

	// 敵と弾の当たり判定
	bossEnemy.CheckHitBullet();
	for (int index = 0; index < SMALL_ENEMY; ++index) {

		if (!smallEnemy[index].isActive) continue;

		smallEnemy[index].CheckHitBullet();

	}
	for (int index = 0; index < NOMOVEMENT_ENEMY; ++index) {

		if (!noMovementEnemy[index].isActive) continue;

		noMovementEnemy[index].CheckHitBullet();

	}

	//ボス弾とプレイヤーの判定
	for (int index = 0; index < SMALL_ENEMY; ++index)
	{
		std::shared_ptr<SphereCollision> bullet = BossBulletManager::Instance()->GetBullet(index)->bulletHitBox;
		bool hitFlag = BulletCollision::Instance()->CheckSphereAndSphere(*bullet, *player.bulletHitBox);
		bool initFlag = BossBulletManager::Instance()->GetBullet(index)->isActive;

		//初期化されている&&プレイヤーと判定を取ったら優勢ゲージの偏りが変わり、弾は初期化される
		if (hitFlag && initFlag)
		{
			SuperiorityGauge::Instance()->AddEnemyGauge(10.0f);
			BossBulletManager::Instance()->GetBullet(index)->Init();
		}
	}


#pragma endregion

#pragma region 必要のない当たり判定

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
			++countStopNum;
		}
		else
		{
			ViewPort::Instance()->holdFlags[ViewPort::Instance()->LEFT] = false;
		}


	}

#pragma endregion

	ViewPort::Instance()->playerPos = player.centerPos;
	//ライト更新
	auto pos = ScrollMgr::Instance()->Affect(player.centerPos);
	ptLig.SetPos({ pos.x,pos.y,PT_LIG_Z });

	spotLig.SetTarget({ pos.x,pos.y + SPOT_LIG_TARGET_OFFSET_Y,0.0f });
	spotLig.SetPos({ pos.x,pos.y + SPOT_LIG_TARGET_OFFSET_Y,SPOT_LIG_Z });

	//パーティクル更新
	ParticleMgr::Instance()->Update();

	if (UsersInput::Instance()->OnTrigger(DIK_M))
	{
		if (Camera::Instance()->Active())
		{
			Camera::Instance()->Release();
		}
		else
		{
			Camera::Instance()->Focus(player.centerPos, 2.0f);
		}
	}

	BackGround::Instance()->Update();
	Camera::Instance()->Update();

}

void Game::Draw(std::weak_ptr<RenderTarget>EmissiveMap)
{
	int stageNum = SelectStage::Instance()->GetStageNum();
	int roomNum = SelectStage::Instance()->GetRoomNum();

	/*===== 描画処理 =====*/
	BackGround::Instance()->Draw();

	mapChipDrawData = StageMgr::Instance()->GetMapChipDrawBlock(stageNum, roomNum);
	DrawMapChip(mapData, mapChipDrawData, stageNum, roomNum);

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

	bossEnemy.Draw();
	for (int index = 0; index < SMALL_ENEMY; ++index) {

		if (!smallEnemy[index].isActive) continue;

		smallEnemy[index].Draw();

	}
	for (int index = 0; index < NOMOVEMENT_ENEMY; ++index) {

		if (!noMovementEnemy[index].isActive) continue;

		noMovementEnemy[index].Draw();

	}

	GameTimer::Instance()->Draw();
	ScoreManager::Instance()->Draw();

	static int CHAIN_GRAPH = TexHandleMgr::LoadGraph("resource/ChainCombat/chain.png");
	static const int CHAIN_THICKNESS = 4;
	// プレイヤーとボス間に線を描画
	{
		Vec2<float> playerBossDir = boss.pos - player.centerPos;
		playerBossDir.Normalize();
		Vec2<float> playerDefLength = player.centerPos + playerBossDir * addLineLengthPlayer;

		DrawFunc::DrawLine2DGraph(ScrollMgr::Instance()->Affect(player.centerPos), ScrollMgr::Instance()->Affect(playerDefLength + playerBossDir * lineLengthPlayer),
			TexHandleMgr::GetTexBuffer(CHAIN_GRAPH), CHAIN_THICKNESS * ScrollMgr::Instance()->zoom);
		Vec2<float> bossPlayerDir = player.centerPos - boss.pos;
		bossPlayerDir.Normalize();
		Vec2<float> bossDefLength = boss.pos + bossPlayerDir * addLineLengthBoss;

		//DrawFunc::DrawLine2D(boss.pos - scrollShakeAmount, bossDefLength - scrollShakeAmount, Color(255, 0, 0, 255));
		//DrawFunc::DrawLine2D(bossDefLength - scrollShakeAmount, bossDefLength + bossPlayerDir * lineLengthBoss - scrollShakeAmount, Color(255, 255, 255, 255));
		DrawFunc::DrawLine2DGraph(ScrollMgr::Instance()->Affect(boss.pos), ScrollMgr::Instance()->Affect(bossDefLength + bossPlayerDir * lineLengthBoss),
			TexHandleMgr::GetTexBuffer(CHAIN_GRAPH), CHAIN_THICKNESS * ScrollMgr::Instance()->zoom);

		// 線分の中心に円を描画
		static int LINE_CENTER_GRAPH = TexHandleMgr::LoadGraph("resource/ChainCombat/line_center.png");
		DrawFunc::DrawRotaGraph2D(ScrollMgr::Instance()->Affect(lineCenterPos), { ScrollMgr::Instance()->zoom,ScrollMgr::Instance()->zoom }, 0.0f, TexHandleMgr::GetTexBuffer(LINE_CENTER_GRAPH));
		//DrawFunc::DrawCircle2D(playerDefLength + playerBossDir * lineLengthPlayer - scrollShakeAmount, 10, Color());
	}

	playerHomeBase->Draw();
	enemyHomeBase->Draw();

	player.Draw(ligMgr);
	ParticleMgr::Instance()->Draw(ligMgr);

	// ボスを描画
	boss.Draw();

	// プレイヤーとボス間に線を描画
	DrawFunc::DrawLine2D(ScrollMgr::Instance()->Affect(player.centerPos), ScrollMgr::Instance()->Affect(boss.pos), Color());

	SuperiorityGauge::Instance()->Draw();

	GUI::Instance()->Draw();

	miniMap.Draw();

	if (sceneBlackFlag || sceneLightFlag)
	{
		DrawFunc::DrawBox2D(Vec2<float>(0.0f, 0.0f), Vec2<float>(1280.0f, 720.0f), Color(0, 0, 0, alphaValue), D3D12App::Instance()->GetBackBuffFormat(), true, AlphaBlendMode_Trans);

		KuroEngine::Instance().Graphics().SetRenderTargets({ EmissiveMap.lock() });

		DrawFunc::DrawBox2D(Vec2<float>(0.0f, 0.0f), Vec2<float>(1280.0f, 720.0f), Color(0, 0, 0, alphaValue), EmissiveMap.lock()->GetDesc().Format, true, AlphaBlendMode_Trans);

		KuroEngine::Instance().Graphics().SetRenderTargets({ D3D12App::Instance()->GetBackBuffRenderTarget(),EmissiveMap.lock() });
	}
}

void Game::Scramble()
{
	/*===== 引っ張り合いの処理 =====*/

	// 前フレームの線の中心座標を保存
	prevLineCenterPos = lineCenterPos;

	// 移動量を取得。 優勢ゲージはここで更新。
	double playerVel = player.vel.Length();
	Vec2<float> playerVelGauge = player.vel * SuperiorityGauge::Instance()->GetPlayerGaugeData()->gaugeDivValue;
	double bossVel = boss.vel.Length();
	Vec2<float> bossVelGauge = boss.vel * SuperiorityGauge::Instance()->GetEnemyGaugeData()->gaugeDivValue;
	double subVel = fabs(fabs(playerVel) - fabs(bossVel));

	player.centerPos += playerVelGauge;
	boss.pos += bossVelGauge;


	// 線分の長さ
	float line = 0;
	float LINE = (lineLengthBoss + lineLengthPlayer) + (addLineLengthBoss + addLineLengthPlayer);

	// 気にしないでください！
	bool isBoss = false;
	bool isPlayer = false;

	// どちらの移動量が多いかを取得。どちらも同じ場合は処理を飛ばす。
	if (playerVelGauge.Length() < bossVelGauge.Length()) {

		// ボスの移動量のほうが大きかったら

		// 距離を求める。
		line = Vec2<float>(player.centerPos).Distance(boss.pos);

		// プレイヤーをボスの方に移動させる。
		if (LINE < line) {

			// 押し戻し量
			float moveLength = line - LINE;

			// 押し戻し方向
			Vec2<float> moveDir = Vec2<float>(boss.pos - player.centerPos);
			moveDir.Normalize();

			// 押し戻す。
			player.centerPos += moveDir * Vec2<float>(moveLength, moveLength);

			// 引っかかり判定だったら
			if (isCatchMapChipPlayer) {

				addLineLengthPlayer += moveLength;

			}

			isBoss = true;

		}

	}
	else if (bossVelGauge.Length() < playerVelGauge.Length()) {

		// プレイヤーの移動量のほうが大きかったら

		// 距離を求める。
		line = Vec2<float>(player.centerPos).Distance(boss.pos);

		// ボスをプレイヤーの方に移動させる。
		if (LINE < line) {

			// 押し戻し量
			float moveLength = line - LINE;

			// 押し戻し方向
			Vec2<float> moveDir = Vec2<float>(player.centerPos - boss.pos);
			moveDir.Normalize();

			// 押し戻す。
			boss.pos += moveDir * Vec2<float>(moveLength, moveLength);

			if (boss.pos.x < boss.prevPos.x) {
				boss.vel.x -= boss.prevPos.x - boss.pos.x;
			}

			// ボスの移動量が0を下回らないようにする。
			if (boss.vel.x < 0) {

				boss.vel.x = 0;

			}

			// 引っかかり判定だったら
			if (isCatchMapChipBoss) {

				addLineLengthBoss += moveLength;

			}

			isPlayer = true;

		}

	}
	else {

		return;

	}

	// 引っかかり判定じゃなかったらだんだん短くする。
	if (isBoss || (!isCatchMapChipBoss && 0 < addLineLengthBoss)) {

		addLineLengthBoss -= 5.0f;

		// ウィンドウに挟まったら
		if (0 < boss.stuckWindowTimer) {

			addLineLengthBoss -= 20.0f;

		}

		if (addLineLengthBoss < 0) addLineLengthBoss = 0;

	}
	if (isPlayer || (!isCatchMapChipPlayer && 0 < addLineLengthPlayer)) {

		addLineLengthPlayer -= 5.0f;

		// ウィンドウに挟まったら
		if (0 < player.stuckWindowTimer) {

			addLineLengthPlayer -= 20.0f;

		}

		if (addLineLengthPlayer < 0) addLineLengthPlayer = 0;

	}

	// 紐の中心点を計算
	{
		Vec2<float> bossDir = boss.pos - player.centerPos;
		bossDir.Normalize();
		float playerLineLength = lineLengthPlayer + addLineLengthPlayer;
		lineCenterPos = player.centerPos + bossDir * Vec2<float>(playerLineLength, playerLineLength);
	}

	isCatchMapChipBoss = false;
	isCatchMapChipPlayer = false;

}

Vec2<float> Game::GetStageSize()
{
	static const float CHIP_SIZE = 64;
	int sizeX = mapData[0].size();
	int sizeY = mapData.size();
	return Vec2<float>(CHIP_SIZE * sizeX, CHIP_SIZE * sizeY);
}
