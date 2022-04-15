#include "Game.h"
#include"MapChipCollider.h"
#include"ShakeMgr.h"
#include "BulletMgr.h"
#include"ScrollMgr.h"
//#include"BulletParticleMgr.h"
#include "StunEffect.h"
#include "SlowMgr.h"
#include"Bullet.h"
#include"Collider.h"
#include"SightCollisionStorage.h"
#include"SelectStage.h"


#include"KuroFunc.h"
#include"KuroEngine.h"
#include"TexHandleMgr.h"
#include"DrawFunc.h"
#include"ParticleMgr.h"

#include"SuperiorityGauge.h"
#include"BackGround.h"
#include"Camera.h"
#include"GameTimer.h"
#include"ScoreManager.h"
#include"FaceIcon.h"
#include"WinCounter.h"

#include"BossBulletManager.h"

#include"BulletCollision.h"

#include"CrashMgr.h"

#include"ResultTransfer.h"
#include "Player.h"
#include "Boss.h"

#include<map>
std::vector<std::unique_ptr<MassChipData>> Game::AddData(RoomMapChipArray MAPCHIP_DATA, const int& CHIP_NUM)
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

void Game::DrawMapChip(const vector<vector<int>>& mapChipData, vector<vector<MapChipDrawData>>& mapChipDrawData, const int& stageNum, const int& roomNum)
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


				vector<MapChipAnimationData*>tmpAnimation = StageMgr::Instance()->animationData;
				int handle = -1;
				if (height < 0 || mapChipDrawData.size() <= height) continue;
				if (width < 0 || mapChipDrawData[height].size() <= width) continue;
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
		drawMap[i].Draw(TexHandleMgr::GetTexBuffer(itr->first), nullptr, nullptr);
		i++;
	}
}

const int& Game::GetChipNum(const vector<vector<int>>& MAPCHIP_DATA, const int& MAPCHIP_NUM, int* COUNT_CHIP_NUM, Vec2<float>* POS)
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
void Game::InitGame(const int& STAGE_NUM, const int& ROOM_NUM)
{
	CrashMgr::Instance()->Init();

	AudioApp::Instance()->PlayWave(bgm, true);

	int stageNum = STAGE_NUM;
	int roomNum = ROOM_NUM;

	SuperiorityGauge::Instance()->Init();

	FaceIcon::Instance()->Init();

	mapData = StageMgr::Instance()->GetMapChipData(stageNum, roomNum);
	mapChipDrawData = StageMgr::Instance()->GetMapChipDrawBlock(stageNum, roomNum);

	// シェイク量を設定。
	ShakeMgr::Instance()->Init();
	ParticleMgr::Instance()->Init();

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

	initDeadFlag = false;
	//giveDoorNumber = 0;
	debugStageData[0] = stageNum;
	debugStageData[1] = roomNum;

	//ScrollMgr::Instance()->honraiScrollAmount = { -1060.0f,-490.0f };
	alphaValue = 0;

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
		playerHomeBase->Init(playerLeftUpPos - chipPos, playerRightDownPos + chipPos, true);
		enemyHomeBase->Init(enemyLeftUpPos - chipPos, enemyRightDownPos + chipPos, false);

		{
			float size = (mapData[0].size() * MAP_CHIP_SIZE) - 400.0f;
			miniMap.Init(size);
		}


		lineCenterPos = {};
		prevLineCenterPos = {};

		isCatchMapChipBoss = false;
		isCatchMapChipPlayer = false;
	}


	Vec2<float> responePos((mapData[0].size() * MAP_CHIP_SIZE) * 0.5f, (mapData.size() * MAP_CHIP_SIZE) * 0.5f);
	responePos -= 100;

	lineCenterPos = responePos;
	leftCharacter->Init(responePos - Vec2<float>(150.0f, 0.0f));
	rightCharacter->Init(responePos + Vec2<float>(150.0f, 0.0f));

	miniMap.CalucurateCurrentPos(lineCenterPos);

	Camera::Instance()->Init();

	GameTimer::Instance()->Init({}, 120, {}, {});
	GameTimer::Instance()->Start();


	ScoreManager::Instance()->Init();
	firstLoadFlag = false;
	lineExtendScale = lineExtendMaxScale;

}

Game::Game()
{
	// 弾管理クラスを初期化。
	BulletMgr::Instance()->Setting();

	bgm = AudioApp::Instance()->LoadAudio("resource/ChainCombat/sound/bgm_1.wav");
	AudioApp::Instance()->ChangeVolume(bgm, 0.07f);

	playerHomeBase = std::make_unique<HomeBase>();
	enemyHomeBase = std::make_unique<HomeBase>();

	playerHomeBase->Init({ 0.0f,0.0f }, { 0.0f,0.0f }, true);
	enemyHomeBase->Init({ 0.0f,0.0f }, { 800.0f,1000.0f }, false);
	//enemyHomeBase->Init({ 0.0f,0.0f }, { 0.0f,0.0f });

	cameraBasePos = { 0.0f,-40.0f };

	roundChangeEffect.Init();

	mapData = StageMgr::Instance()->GetMapChipData(0, 0);
	mapChipDrawData = StageMgr::Instance()->GetMapChipDrawBlock(0, 0);

	Vec2<float> responePos((mapData[0].size() * MAP_CHIP_SIZE) * 0.5f, (mapData.size() * MAP_CHIP_SIZE) * 0.5f);
	responePos -= 100;
	lineCenterPos = responePos;
	ScrollMgr::Instance()->Init(lineCenterPos, Vec2<float>(mapData[0].size() * MAP_CHIP_SIZE, mapData.size() * MAP_CHIP_SIZE), cameraBasePos);

	Camera::Instance()->Init();
	SuperiorityGauge::Instance()->Init();

	readyToStartRoundFlag = true;

	//背景に星
	BackGround::Instance()->Init(GetStageSize());

	GameTimer::Instance()->Init({}, 120, {}, {});
	ScoreManager::Instance()->Init();


}

void Game::Init()
{
	WinCounter::Instance()->Reset();

	turnResultScene = false;
	
	leftCharacter = std::make_shared<Player>();
	rightCharacter = std::make_shared<Boss>();

	rightCharacter->RegisterSetPartner(leftCharacter);
	leftCharacter->RegisterSetPartner(rightCharacter);

	InitGame(0, 0);
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

	const bool resetInput = UsersInput::Instance()->OnTrigger(DIK_SPACE) || UsersInput::Instance()->OnTrigger(BACK);
	if (resetInput)
	{
		SelectStage::Instance()->resetStageFlag = true;
		//player.isDead = true;
		//sceneBlackFlag = true;
		//sceneChangeDeadFlag = player.isDead;
	}


	//プレイヤー陣地と敵の判定
	if (playerHomeBase->Collision(rightCharacter->GetAreaHitBox()) && !roundFinishFlag && !readyToStartRoundFlag)
	{
		//プレイヤー勝利
		WinCounter::Instance()->RoundFinish(lineCenterPos, true);
		roundFinishFlag = true;
		playerOrEnemeyWinFlag = true;
		gameStartFlag = false;
	}

	//敵陣地とプレイヤーの判定
	if (enemyHomeBase->Collision(leftCharacter->GetAreaHitBox()) && !roundFinishFlag && !readyToStartRoundFlag)
	{
		//敵勝利
		WinCounter::Instance()->RoundFinish(lineCenterPos, false);
		roundFinishFlag = true;
		playerOrEnemeyWinFlag = false;
		gameStartFlag = false;
	}

	//ラウンド終了演出開始
	if (roundFinishFlag)
	{
		BossBulletManager::Instance()->Init();


		//勝利数カウント演出
		if (!WinCounter::Instance()->GetNowAnimation())
		{
			//どちらかが３勝とったらゲーム終了
			if (WinCounter::Instance()->GetGameFinish())
			{
				//とりあえずリセットしとく
				//WinCounter::Instance()->Reset();
				ResultTransfer::Instance()->resultScore = ScoreManager::Instance()->GetScore();
				turnResultScene = true;
			}
			//次のラウンドへ
			else
			{
				++roundTimer;

				if (60 <= roundTimer)
				{
					readyToStartRoundFlag = true;
					roundFinishFlag = false;
					SuperiorityGauge::Instance()->Init();
				}
			}
		}
	}

	//ラウンド開始時の演出開始
	if (readyToStartRoundFlag)
	{
		roundChangeEffect.Start(WinCounter::Instance()->GetNowRound(), playerOrEnemeyWinFlag);
		Vec2<float>winSize;
		winSize.x = static_cast<float>(WinApp::Instance()->GetWinSize().x);
		winSize.y = static_cast<float>(WinApp::Instance()->GetWinSize().y);

		Vec2<float> responePos = Vec2<float>(((mapData[0].size() * MAP_CHIP_SIZE) / 2.0f) - 100.0f, (mapData.size() * MAP_CHIP_SIZE) / 2.0f);
		responePos.y -= -cameraBasePos.y * 2.0f;
		ScrollMgr::Instance()->Warp(responePos);

		//プレイヤーと敵の座標初期化
		if (roundChangeEffect.readyToInitFlag && !roundChangeEffect.initFlag)
		{
			InitGame(0, 0);
			roundChangeEffect.initFlag = true;
		}

		//登場演出
		if (roundChangeEffect.initFlag)
		{
			bool leftAppear = leftCharacter->Appear();
			bool rightApperar = rightCharacter->Appear();
			if (leftAppear && rightApperar)	//どちらのキャラも登場演出完了
			{
				readyToStartRoundFlag = false;
				gameStartFlag = true;
				roundTimer = 0;
			}
		}
		//gameStartFlag = true;
		//SelectStage::Instance()->resetStageFlag = true;
		//readyToStartRoundFlag = false;

		float size = (mapData[0].size() * MAP_CHIP_SIZE) - 400.0f;
		miniMap.Init(size);
	}

	if (!readyToStartRoundFlag)
	{
		miniMap.CalucurateCurrentPos(lineCenterPos);

		// プレイヤーの更新処理
		leftCharacter->Update(mapData, lineCenterPos);

		// ボスの更新処理
		rightCharacter->Update(mapData, lineCenterPos);

		// 弾を更新
		BulletMgr::Instance()->Update();

		// プレイヤーとボスの引っ張り合いの処理
		Scramble();
	}


	miniMap.Update();

	roundChangeEffect.Update();

	GameTimer::Instance()->Update();
	ScoreManager::Instance()->Update();

	//	ScrollManager::Instance()->CalucurateScroll(prevLineCenterPos - lineCenterPos, lineCenterPos);
	ScrollMgr::Instance()->CalucurateScroll(prevLineCenterPos - lineCenterPos, lineCenterPos);

	// スクロール量の更新処理
	//ScrollManager::Instance()->Update();
	ScrollMgr::Instance()->Update();

	// シェイク量の更新処理
	ShakeMgr::Instance()->Update();

	// 振り回し管理クラスの更新処理
	//SwingMgr::Instance()->Update(player.centerPos, boss.pos, lineLengthBoss + lineLengthPlayer + addLineLengthBoss + addLineLengthPlayer);

#pragma region 各ギミックの更新処理

	SuperiorityGauge::Instance()->Update();

	// スタン演出クラスの更新処理
	StunEffect::Instance()->Update();

#pragma endregion


	/*===== 当たり判定 =====*/

#pragma region 当たり判定
	// 弾とマップチップの当たり判定
	BulletMgr::Instance()->CheckHit(mapData);

	//ボス弾とプレイヤーの判定
	for (int index = 0; index < BossBulletManager::Instance()->bullets.size(); ++index)
	{
		std::shared_ptr<SphereCollision> bullet = BossBulletManager::Instance()->GetBullet(index)->bulletHitBox;
		bool hitFlag = BulletCollision::Instance()->CheckSphereAndSphere(*bullet, leftCharacter->GetBulletHitSphere());
		bool initFlag = BossBulletManager::Instance()->GetBullet(index)->isActive;

		//初期化されている&&プレイヤーと判定を取ったら優勢ゲージの偏りが変わり、弾は初期化される
		if (hitFlag && initFlag)
		{
			SuperiorityGauge::Instance()->AddEnemyGauge(1.0f);
			BossBulletManager::Instance()->GetBullet(index)->Init();
		}
	}

	//プレイヤー弾とボスの判定
	for (int index = 0; index < BulletMgr::Instance()->bullets.size(); ++index)
	{
		std::shared_ptr<SphereCollision> bullet = BulletMgr::Instance()->GetBullet(index)->bulletHitBox;
		bool hitFlag = BulletCollision::Instance()->CheckSphereAndSphere(*bullet, rightCharacter->GetBulletHitSphere());
		bool initFlag = BulletMgr::Instance()->GetBullet(index)->isActive;

		//初期化されている&&プレイヤーと判定を取ったら優勢ゲージの偏りが変わり、弾は初期化される
		if (hitFlag && initFlag)
		{
			SuperiorityGauge::Instance()->AddPlayerGauge(1.0f);
			BulletMgr::Instance()->GetBullet(index)->Init();
		}
	}


#pragma endregion

	//パーティクル更新
	ParticleMgr::Instance()->Update();

	BackGround::Instance()->Update();
	Camera::Instance()->Update();
	FaceIcon::Instance()->Update();
	WinCounter::Instance()->Update();

	// 優勢ゲージが振り切ったトリガー判定のときにスタン演出を有効化する。
	if (SuperiorityGauge::Instance()->GetEnemyGaugeData()->overGaugeFlag && !SuperiorityGauge::Instance()->GetEnemyGaugeData()->prevOverGaugeFlag) {
		// 敵の優勢ゲージが振り切ったということは、プレイヤーの優勢ゲージが0だということ。
		StunEffect::Instance()->Activate(leftCharacter->pos, Vec2<float>(0, 0), false);
		ResultTransfer::Instance()->leftBreakCount++;
	}
	if (SuperiorityGauge::Instance()->GetPlayerGaugeData()->overGaugeFlag && !SuperiorityGauge::Instance()->GetPlayerGaugeData()->prevOverGaugeFlag) {
		// プレイヤーの優勢ゲージが振り切ったということは、敵の優勢ゲージが0だということ。
		StunEffect::Instance()->Activate(rightCharacter->pos, Vec2<float>(1200, 0), true);
		ResultTransfer::Instance()->rightBreakCount++;
	}

	CrashMgr::Instance()->Update();
}

void Game::Draw(std::weak_ptr<RenderTarget>EmissiveMap)
{
	int stageNum = SelectStage::Instance()->GetStageNum();
	int roomNum = SelectStage::Instance()->GetRoomNum();

	/*===== 描画処理 =====*/
	BackGround::Instance()->Draw();


	mapChipDrawData = StageMgr::Instance()->GetMapChipDrawBlock(stageNum, roomNum);
	DrawMapChip(mapData, mapChipDrawData, stageNum, roomNum);

	for (int i = 0; i < thornBlocks.size(); ++i)
	{
		thornBlocks[i]->Draw();
	}

	playerHomeBase->Draw();
	enemyHomeBase->Draw();

	static int CHAIN_GRAPH = TexHandleMgr::LoadGraph("resource/ChainCombat/chain.png");
	static const int CHAIN_THICKNESS = 4;
	// プレイヤーとボス間に線を描画
	{
		Vec2<float> playerBossDir = rightCharacter->pos - leftCharacter->pos;
		playerBossDir.Normalize();
		Vec2<float> playerDefLength = leftCharacter->pos + playerBossDir * leftCharacter->addLineLength;

		DrawFunc::DrawLine2DGraph(ScrollMgr::Instance()->Affect(leftCharacter->pos), ScrollMgr::Instance()->Affect(playerDefLength + playerBossDir * leftCharacter->addLineLength),
			TexHandleMgr::GetTexBuffer(CHAIN_GRAPH), CHAIN_THICKNESS * ScrollMgr::Instance()->zoom);


		Vec2<float> bossPlayerDir = leftCharacter->pos - rightCharacter->pos;
		bossPlayerDir.Normalize();
		Vec2<float> bossDefLength = rightCharacter->pos + bossPlayerDir * rightCharacter->addLineLength;

		float time = 30.0f;
		if (1.0f < lineExtendScale)
		{
			lineExtendScale -= lineExtendMaxScale / time;
		}
		else
		{
			lineExtendScale = 1.0f;
		}

		//DrawFunc::DrawLine2D(boss.pos - scrollShakeAmount, bossDefLength - scrollShakeAmount, Color(255, 0, 0, 255));
		//DrawFunc::DrawLine2D(bossDefLength - scrollShakeAmount, bossDefLength + bossPlayerDir * lineLengthBoss - scrollShakeAmount, Color(255, 255, 255, 255));
		DrawFunc::DrawLine2DGraph(ScrollMgr::Instance()->Affect(rightCharacter->pos), ScrollMgr::Instance()->Affect(bossDefLength + bossPlayerDir * rightCharacter->addLineLength),
			TexHandleMgr::GetTexBuffer(CHAIN_GRAPH), CHAIN_THICKNESS * ScrollMgr::Instance()->zoom * lineExtendScale);

		// 線分の中心に円を描画
		if (roundChangeEffect.drawFightFlag)
		{
			static int LINE_CENTER_GRAPH = TexHandleMgr::LoadGraph("resource/ChainCombat/line_center.png");
			DrawFunc::DrawRotaGraph2D(ScrollMgr::Instance()->Affect(lineCenterPos), Vec2<float>(ScrollMgr::Instance()->zoom, ScrollMgr::Instance()->zoom) * lineExtendScale, 0.0f, TexHandleMgr::GetTexBuffer(LINE_CENTER_GRAPH));
		}
		//DrawFunc::DrawCircle2D(playerDefLength + playerBossDir * lineLengthPlayer - scrollShakeAmount, 10, Color());
	}

	roundChangeEffect.Draw();

	if (roundChangeEffect.initFlag)
	{
		leftCharacter->Draw();
		rightCharacter->Draw();
	}

	BossBulletManager::Instance()->Draw();

	ParticleMgr::Instance()->Draw();
	CrashMgr::Instance()->Draw();

	GameTimer::Instance()->Draw();
	ScoreManager::Instance()->Draw();

	// プレイヤーとボス間に線を描画
	//DrawFunc::DrawLine2D(ScrollMgr::Instance()->Affect(player.centerPos), ScrollMgr::Instance()->Affect(boss.pos), Color());

	SuperiorityGauge::Instance()->Draw();

	miniMap.Draw();

	FaceIcon::Instance()->Draw();

	WinCounter::Instance()->Draw();

	StunEffect::Instance()->Draw();
}

void Game::Scramble()
{
	/*===== 引っ張り合いの処理 =====*/

	// 前フレームの線の中心座標を保存
	prevLineCenterPos = lineCenterPos;

	Vec2<float> playerVelGauge;
	Vec2<float> bossVelGauge;

	// 移動量を取得。 優勢ゲージはここで更新。
	double playerVel = leftCharacter->vel.Length() * SlowMgr::Instance()->slowAmount;
	playerVelGauge = (leftCharacter->vel * SuperiorityGauge::Instance()->GetPlayerGaugeData()->gaugeDivValue) * SlowMgr::Instance()->slowAmount;
	double bossVel = rightCharacter->vel.Length() * SlowMgr::Instance()->slowAmount;
	bossVelGauge = (rightCharacter->vel * SuperiorityGauge::Instance()->GetEnemyGaugeData()->gaugeDivValue) * SlowMgr::Instance()->slowAmount;
	double subVel = fabs(fabs(playerVel) - fabs(bossVel));

	// [振り回し状態のとき] [スタン演出中] は移動させない。
	if (!(rightCharacter->GetNowSwing() || leftCharacter->GetNowSwing() || StunEffect::Instance()->isActive)) {
		leftCharacter->pos += playerVelGauge;
		rightCharacter->pos += bossVelGauge;
	}


	// 線分の長さ
	float line = 0;
	float LINE = CharacterInterFace::LINE_LENGTH * 2 + (leftCharacter->addLineLength + rightCharacter->addLineLength);

	// 気にしないでください！
	bool isBoss = false;
	bool isPlayer = false;

	// どちらの移動量が多いかを取得。どちらも同じ場合は処理を飛ばす。
	if (playerVelGauge.Length() < bossVelGauge.Length()) {

		// ボスの移動量のほうが大きかったら

		// 距離を求める。
		line = Vec2<float>(leftCharacter->pos).Distance(rightCharacter->pos);

		// プレイヤーをボスの方に移動させる。
		if (LINE < line) {

			// 押し戻し量
			float moveLength = line - LINE;

			// 押し戻し方向
			Vec2<float> moveDir = Vec2<float>(rightCharacter->pos - leftCharacter->pos);
			moveDir.Normalize();

			// 押し戻す。
			leftCharacter->pos += moveDir * Vec2<float>(moveLength, moveLength);

			// 引っかかり判定だったら
			if (isCatchMapChipPlayer) {

				leftCharacter->addLineLength += moveLength;

			}

			isBoss = true;

		}

	}
	else if (bossVelGauge.Length() < playerVelGauge.Length()) {

		// プレイヤーの移動量のほうが大きかったら

		// 距離を求める。
		line = Vec2<float>(leftCharacter->pos).Distance(rightCharacter->pos);

		// ボスをプレイヤーの方に移動させる。
		if (LINE < line) {

			// 押し戻し量
			float moveLength = line - LINE;

			// 押し戻し方向
			Vec2<float> moveDir = Vec2<float>(leftCharacter->pos - rightCharacter->pos);
			moveDir.Normalize();

			// 押し戻す。
			rightCharacter->pos += moveDir * Vec2<float>(moveLength, moveLength);

			//if (rightCharacter->GetPos().x < rightCharacter->GetPrevPos().x) {
			//	rightCharacter->AddVel({ -(rightCharacter->GetPrevPos().x - rightCharacter->GetPos().x),0.0f });
			//}

			// ボスの移動量が0を下回らないようにする。
			//if (rightCharacter->GetVel().x < 0) {

			//	boss.vel.x = 0;

			//}

			// 引っかかり判定だったら
			if (isCatchMapChipBoss) {

				rightCharacter->addLineLength += moveLength;

			}

			isPlayer = true;

		}

	}
	else {

		return;

	}

	// 引っかかり判定じゃなかったらだんだん短くする。
	if (isBoss || (!isCatchMapChipBoss && 0 < rightCharacter->addLineLength)) {

		rightCharacter->addLineLength -= 5.0f;

		// ウィンドウに挟まったら
		if (0 < rightCharacter->GetStackWinTimer()) {

			rightCharacter->addLineLength -= 20.0f;

		}

		if (rightCharacter->addLineLength < 0) rightCharacter->addLineLength = 0;

	}
	if (isPlayer || (!isCatchMapChipPlayer && 0 < leftCharacter->addLineLength)) {

		leftCharacter->addLineLength -= 5.0f;

		// ウィンドウに挟まったら
		if (leftCharacter->GetNowStuckWin()) {

			leftCharacter->addLineLength -= 20.0f;

		}

		if (leftCharacter->addLineLength < 0) leftCharacter->addLineLength = 0;

	}

	// 紐の中心点を計算
	{
		Vec2<float> bossDir = rightCharacter->pos - leftCharacter->pos;
		bossDir.Normalize();
		float playerLineLength = CharacterInterFace::LINE_LENGTH + leftCharacter->addLineLength;
		lineCenterPos = leftCharacter->pos + bossDir * Vec2<float>(playerLineLength, playerLineLength);
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
