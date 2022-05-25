#include "Game.h"
#include"MapChipCollider.h"
#include"ShakeMgr.h"
#include"ScrollMgr.h"
//#include"BulletParticleMgr.h"
#include "StunEffect.h"
#include "SlowMgr.h"
#include"Bullet.h"
#include"Collider.h"
#include"SightCollisionStorage.h"
#include"SelectStage.h"
#include"AfterImage.h"
#include"CrashEffectMgr.h"
#include"Stamina.h"

#include"KuroFunc.h"
#include"KuroEngine.h"
#include"TexHandleMgr.h"
#include"DrawFunc.h"
#include"ParticleMgr.h"

#include"SuperiorityGauge.h"
//#include"BackGround.h"
#include"Camera.h"
#include"GameTimer.h"
#include"ScoreManager.h"
#include"FaceIcon.h"
#include"WinCounter.h"

#include"BulletCollision.h"

#include"CrashMgr.h"

#include"ResultTransfer.h"
#include "Player.h"
#include "Boss.h"

#include "CharacterInterFace.h"

#include<map>

#include"DebugParameter.h"

#include"DebugKeyManager.h"

#include"CharacterManager.h"
#include "StaminaItemMgr.h"

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
	SizeData wallChipMemorySize = StageMgr::Instance()->GetMapChipSizeData(MAPCHIP_TYPE_STATIC_BLOCK);


	// マップチップの縦の要素数を取得。
	const int HEIGHT = mapChipData.size();
	for (int height = 0; height < HEIGHT; ++height) {

		// マップチップの横の要素数を取得。
		const int WIDTH = mapChipData[height].size();
		for (int width = 0; width < WIDTH; ++width) {

			if (mapChipDrawData[height][width].shocked)mapChipDrawData[height][width].shocked -= 0.02f;
			if (mapChipDrawData[height][width].expEaseRate < 1.0f)mapChipDrawData[height][width].expEaseRate += 0.005f;

			// ブロック以外だったら処理を飛ばす。
			bool blockFlag = (mapChipData[height][width] >= wallChipMemorySize.min && mapChipData[height][width] <= wallChipMemorySize.max);
			if (blockFlag)
			{
				// スクロール量から描画する位置を求める。
				const Vec2<float> drawPos = ScrollMgr::Instance()->Affect({ width * MAP_CHIP_SIZE,height * MAP_CHIP_SIZE });

				// 画面外だったら描画しない。
				if (drawPos.x < -DRAW_MAP_CHIP_SIZE || drawPos.x > WinApp::Instance()->GetWinSize().x + DRAW_MAP_CHIP_SIZE) continue;
				if (drawPos.y < -DRAW_MAP_CHIP_SIZE || drawPos.y > WinApp::Instance()->GetWinSize().y + DRAW_MAP_CHIP_SIZE) continue;


				vector<std::shared_ptr<MapChipAnimationData>>tmpAnimation = StageMgr::Instance()->animationData;
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

				//mapChipDrawData[height][width].shocked = KuroMath::Lerp(mapChipDrawData[height][width].shocked, 0.0f, 0.8f);

				Vec2<float> pos = drawPos;
				pos += mapChipDrawData[height][width].offset;
				if (0 <= handle)
				{
					ChipData chipData;
					chipData.pos = pos;
					chipData.radian = mapChipDrawData[height][width].radian;
					chipData.shocked = mapChipDrawData[height][width].shocked;
					chipData.expEaseRate = mapChipDrawData[height][width].expEaseRate;
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
			drawMap[i].AddChip(itr->second[chipIdx]);
		}
		drawMap[i].Draw(TexHandleMgr::GetTexBuffer(itr->first));
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
	DebugParameter::Instance()->totalCombo = 0;
	DebugParameter::Instance()->timer = 0;

	CrashMgr::Instance()->Init();

	int stageNum = STAGE_NUM;
	int roomNum = ROOM_NUM;

	SuperiorityGauge::Instance()->Init();

	bossHandMgr->Init(false);
	playerHandMgr->Init(false);

	FaceIcon::Instance()->Init(CharacterManager::Instance()->Left()->GetCharacterName(), CharacterManager::Instance()->Right()->GetCharacterName());


	StageMgr::Instance()->SetLocalMapChipData(stageNum, roomNum);
	StageMgr::Instance()->SetLocalMapChipDrawBlock(stageNum, roomNum);
	mapData = StageMgr::Instance()->GetLocalMap();
	mapChipDrawData = StageMgr::Instance()->GetLocalDrawMap();

	RoomMapChipArray tmp = *mapData;

	// シェイク量を設定。
	ShakeMgr::Instance()->Init();
	ParticleMgr::Instance()->Init();

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


		for (int y = 0; y < tmp.size(); ++y)
		{
			for (int x = 0; x < tmp[y].size(); ++x)
			{
				if (tmp[y][x] == 33)
				{
					playerLeftUpPos = Vec2<float>(x * MAP_CHIP_SIZE, y * MAP_CHIP_SIZE);
				}
				if (tmp[y][x] == 34)
				{
					playerRightDownPos = Vec2<float>(x * MAP_CHIP_SIZE, y * MAP_CHIP_SIZE);
				}
				if (tmp[y][x] == 35)
				{
					enemyLeftUpPos = Vec2<float>(x * MAP_CHIP_SIZE, y * MAP_CHIP_SIZE);
				}
				if (tmp[y][x] == 36)
				{
					enemyRightDownPos = Vec2<float>(x * MAP_CHIP_SIZE, y * MAP_CHIP_SIZE);
				}
			}
		}

		Vec2<float>chipPos(MAP_CHIP_HALF_SIZE, MAP_CHIP_HALF_SIZE);
		playerHomeBase.Init(playerLeftUpPos - chipPos, playerRightDownPos + chipPos, true);
		enemyHomeBase.Init(enemyLeftUpPos - chipPos, enemyRightDownPos + chipPos, false);

		{
			float size = (tmp[0].size() * MAP_CHIP_SIZE) - 400.0f;
			miniMap.Init(size);
		}


		lineCenterPos = {};
		prevLineCenterPos = {};

		isCatchMapChipBoss = false;
		isCatchMapChipPlayer = false;
	}

	StaminaItemMgr::Instance()->Init();

	Vec2<float> responePos((tmp[0].size() * MAP_CHIP_SIZE) * 0.5f, (tmp.size() * MAP_CHIP_SIZE) * 0.5f);

	//スクロールを上にずらす用
	//responePos.x -= 100;
	//responePos.y += 50;
	lineCenterPos = responePos - cameraBasePos;
	CharacterManager::Instance()->CharactersInit(lineCenterPos, !practiceMode);

	miniMap.CalucurateCurrentPos(lineCenterPos);

	Camera::Instance()->Init();
	GameTimer::Instance()->Init(120);
	GameTimer::Instance()->Start();
	ScoreManager::Instance()->Init();

	firstLoadFlag = false;
	lineExtendScale = lineExtendMaxScale;

	gameStartFlag = false;

	screenEdgeEffect.Init();

	readyToStartRoundFlag = !practiceMode;

	if (practiceMode)
	{
		//ゲームスタート
		gameStartFlag = true;
		roundTimer = 0;
		CharacterManager::Instance()->Left()->SetCanMove(true);
		CharacterManager::Instance()->Right()->SetCanMove(true);
		CharacterManager::Instance()->Left()->SetHitCheck(true);
		CharacterManager::Instance()->Right()->SetHitCheck(true);
		GameTimer::Instance()->SetInterruput(false);
		roundChangeEffect.initGameFlag = true;
		roundChangeEffect.drawFightFlag = true;
	}

	mapChipGenerator[DebugParameter::Instance()->generator]->Init();
}

Game::Game()
{
	bgm = AudioApp::Instance()->LoadAudio("resource/ChainCombat/sound/bgm_1.wav");

	playerHomeBase.Init({ 0.0f,0.0f }, { 0.0f,0.0f }, true);
	enemyHomeBase.Init({ 0.0f,0.0f }, { 800.0f,1000.0f }, false);
	//enemyHomeBase->Init({ 0.0f,0.0f }, { 0.0f,0.0f });

	cameraBasePos = { 0.0f,-40.0f };

	roundChangeEffect.Init();
	StageMgr::Instance()->SetLocalMapChipData(SelectStage::Instance()->GetStageNum(), SelectStage::Instance()->GetRoomNum());
	StageMgr::Instance()->SetLocalMapChipDrawBlock(SelectStage::Instance()->GetStageNum(), SelectStage::Instance()->GetRoomNum());
	mapData = StageMgr::Instance()->GetLocalMap();
	RoomMapChipArray tmp = *mapData;

	mapChipDrawData = StageMgr::Instance()->GetLocalDrawMap();

	Vec2<float> responePos((tmp[0].size() * MAP_CHIP_SIZE) * 0.5f, (tmp.size() * MAP_CHIP_SIZE) * 0.5f);
	//スクロールを上にずらす用
	responePos.x -= 25;
	responePos.y += 50;
	responeScrollPos = responePos;
	ScrollMgr::Instance()->Init(responeScrollPos, Vec2<float>(tmp[0].size() * MAP_CHIP_SIZE, tmp.size() * MAP_CHIP_SIZE), cameraBasePos);

	Camera::Instance()->Init();
	SuperiorityGauge::Instance()->Init();

	readyToStartRoundFlag = true;
	//背景に星
	//BackGround::Instance()->Init(GetStageSize());

	GameTimer::Instance()->Init(120);
	ScoreManager::Instance()->Init();


	StageMgr::Instance()->GetMapChipType(0, 0, Vec2<int>(20, 20));
	StageMgr::Instance()->WriteMapChipData(Vec2<int>(20, 20), 0, CharacterManager::Instance()->Left()->pos, CharacterManager::Instance()->Left()->size.x, CharacterManager::Instance()->Right()->pos, CharacterManager::Instance()->Right()->size.x);

	{
		std::string bossFilePass = "resource/ChainCombat/boss/0/arm/";
		int dL = TexHandleMgr::LoadGraph(bossFilePass + "default/L.png");
		int dR = TexHandleMgr::LoadGraph(bossFilePass + "default/R.png");
		int hL = TexHandleMgr::LoadGraph(bossFilePass + "hold/L.png");
		int hR = TexHandleMgr::LoadGraph(bossFilePass + "hold/R.png");
		bossHandMgr = std::make_unique<BossHandMgr>(dL, dR, hL, hR, true);
	}

	{
		std::string bossFilePass = "resource/ChainCombat/player/luna/arm/";
		int dL = TexHandleMgr::LoadGraph(bossFilePass + "default/L.png");
		int dR = TexHandleMgr::LoadGraph(bossFilePass + "default/R.png");
		int hL = TexHandleMgr::LoadGraph(bossFilePass + "hold/L.png");
		int hR = TexHandleMgr::LoadGraph(bossFilePass + "hold/R.png");
		playerHandMgr = std::make_unique<BossHandMgr>(dL, dR, hL, hR, false);
	}

	mapChipGenerator[SPLINE_ORBIT] = std::make_shared<MapChipGenerator_SplineOrbit>();
	mapChipGenerator[RAND_PATTERN] = std::make_shared<MapChipGenerator_RandPattern>();

}

void Game::Init(const bool& PracticeMode)
{
	practiceMode = PracticeMode;

	WinCounter::Instance()->Reset();

	turnResultScene = false;

	CharacterManager::Instance()->CharactersGenerate();

	InitGame(SelectStage::Instance()->GetStageNum(), SelectStage::Instance()->GetRoomNum());
	ScrollMgr::Instance()->Reset();
	roundChangeEffect.Init();
	CrashEffectMgr::Instance()->Init();

	StaminaItemMgr::Instance()->SetArea(playerHomeBase.hitBox.center->x - playerHomeBase.hitBox.size.x, enemyHomeBase.hitBox.center->x + enemyHomeBase.hitBox.size.x);

}

void Game::Update(const bool& Loop)
{

	if (UsersInput::Instance()->KeyOnTrigger(DIK_R))
	{
		int stageNum = SelectStage::Instance()->GetStageNum();
		int roomNum = SelectStage::Instance()->GetRoomNum();
		StageMgr::Instance()->SetLocalMapChipData(stageNum, roomNum);
		StageMgr::Instance()->SetLocalMapChipDrawBlock(stageNum, roomNum);
	}

	if (DebugParameter::Instance()->changeGenerator)
	{
		mapChipGenerator[DebugParameter::Instance()->generator]->Init();
	}

	//ScrollMgr::Instance()->zoom = ViewPort::Instance()->zoomRate;
	RoomMapChipArray tmpMapData = *mapData;

	// ステージの切り替え
	SwitchingStage();

	if (UsersInput::Instance()->KeyOnTrigger(DIK_R)) {

		CharacterManager::Instance()->Left()->swingDestroyCounter.Increment();

	}
	else if (UsersInput::Instance()->KeyOnTrigger(DIK_T)) {

		CharacterManager::Instance()->Left()->swingDestroyCounter.Init();

	}


	const bool resetInput = UsersInput::Instance()->KeyOnTrigger(DIK_SPACE) || UsersInput::Instance()->ControllerOnTrigger(0, BACK);
	if (resetInput)
	{
		SelectStage::Instance()->resetStageFlag = true;
		//player.isDead = true;
		//sceneBlackFlag = true;
		//sceneChangeDeadFlag = player.isDead;
	}

	// 陣地の判定
	//DeterminationOfThePosition();

	//ラウンド終了演出開始
	RoundFinishEffect(Loop);

	//ラウンド開始時の演出開始
	RoundStartEffect(Loop, tmpMapData);

	miniMap.CalucurateCurrentPos(lineCenterPos);

	screenEdgeEffect.CheckPos(miniMap.nowValue);


	if (roundChangeEffect.readyFlag)
	{
		{
			Vec2<float> sub = CharacterManager::Instance()->Left()->pos - CharacterManager::Instance()->Right()->pos;
			bossHandMgr->Hold(-sub.GetNormal(), CharacterAIOrder::Instance()->prevSwingFlag);
			bossHandMgr->Update(CharacterManager::Instance()->Right()->pos);
		}
		{
			Vec2<float> sub = CharacterManager::Instance()->Right()->pos - CharacterManager::Instance()->Left()->pos;
			playerHandMgr->Hold(-sub.GetNormal(), CharacterManager::Instance()->Left()->prevSwingFlag || CharacterManager::Instance()->Left()->GetNowSwing());
			playerHandMgr->Update(CharacterManager::Instance()->Left()->pos);
		}
	}
	// プレイヤーの更新処理
	if (!roundFinishFlag)
	{
		if (roundChangeEffect.initGameFlag)
		{
			DebugParameter::Instance()->timer++;
			mapChipGenerator[DebugParameter::Instance()->generator]->Update();
		}

		// 座標を保存。
		CharacterManager::Instance()->Left()->SavePrevFramePos();
		CharacterManager::Instance()->Right()->SavePrevFramePos();

		CharacterManager::Instance()->Left()->Update(tmpMapData, lineCenterPos, readyToStartRoundFlag);

		// ボスの更新処理
		CharacterManager::Instance()->Right()->Update(tmpMapData, lineCenterPos, readyToStartRoundFlag);
	}

	CharacterAIData::Instance()->nowPos = CharacterManager::Instance()->Right()->pos;
	// プレイヤーとボスの引っ張り合いの処理
	Scramble();

	// プレイヤーとボスの当たり判定処理
	CharacterManager::Instance()->Left()->CheckHit(tmpMapData, lineCenterPos);
	CharacterManager::Instance()->Right()->CheckHit(tmpMapData, lineCenterPos);
	CharacterAIData::Instance()->prevPos = CharacterManager::Instance()->Right()->pos;

	miniMap.Update();
	screenEdgeEffect.Update();

	roundChangeEffect.Update();

	GameTimer::Instance()->Update();
	ScoreManager::Instance()->Update();

	// シェイク量の更新処理
	ShakeMgr::Instance()->Update();

	SuperiorityGauge::Instance()->Update();

	// スタン演出クラスの更新処理
	StunEffect::Instance()->Update();


	/*===== 当たり判定 =====*/

#pragma region 当たり判定

	//左弾と右プレイヤーの判定
	for (int index = 0; index < CharacterManager::Instance()->Left()->GetBulletMgr().bullets.size(); ++index)
	{
		if (!CharacterManager::Instance()->Left()->GetBulletMgr().bullets[index].isActive)continue;

		std::shared_ptr<SphereCollision> bulCol = CharacterManager::Instance()->Left()->GetBulletMgr().bullets[index].bulletHitBox;
		bool hitFlag = BulletCollision::Instance()->CheckSphereAndSphere(*bulCol, CharacterManager::Instance()->Right()->GetBulletHitSphere());

		//初期化されている&&プレイヤーと判定を取ったら優勢ゲージの偏りが変わり、弾は初期化される
		if (hitFlag)
		{
			//SuperiorityGauge::Instance()->AddGauge(LEFT_TEAM, DebugParameter::Instance()->gaugeData->playerBulletAddGuaugeValue);
			CharacterManager::Instance()->Left()->GetBulletMgr().bullets[index].Init();
			CharacterManager::Instance()->Right()->Damage();
		}
	}

	//右弾と左プレイヤーの判定
	static int DAMAGED_SE = AudioApp::Instance()->LoadAudio("resource/ChainCombat/sound/damaged.wav", 0.7f);
	for (int index = 0; index < CharacterManager::Instance()->Right()->GetBulletMgr().bullets.size(); ++index)
	{
		if (!CharacterManager::Instance()->Right()->GetBulletMgr().bullets[index].isActive)continue;

		std::shared_ptr<SphereCollision> bulCol = CharacterManager::Instance()->Right()->GetBulletMgr().bullets[index].bulletHitBox;
		bool hitFlag = BulletCollision::Instance()->CheckSphereAndSphere(*bulCol, CharacterManager::Instance()->Left()->GetBulletHitSphere());

		//初期化されている&&プレイヤーと判定を取ったら優勢ゲージの偏りが変わり、弾は初期化される
		if (hitFlag)
		{
			//SuperiorityGauge::Instance()->AddGauge(RIGHT_TEAM, DebugParameter::Instance()->gaugeData->enemyBulletAddGuaugeValue);
			CharacterManager::Instance()->Right()->GetBulletMgr().bullets[index].Init();
			CharacterManager::Instance()->Left()->Damage();
			//AudioApp::Instance()->PlayWave(DAMAGED_SE);
		}
	}

#pragma endregion


	// 中心点を計算。
	CalCenterPos();

	//	ScrollManager::Instance()->CalucurateScroll(prevLineCenterPos - lineCenterPos, lineCenterPos);
	ScrollMgr::Instance()->CalucurateScroll(prevLineCenterPos - lineCenterPos, lineCenterPos);

	// スクロール量の更新処理
	//ScrollManager::Instance()->Update();
	Vec2<float> distance = CharacterManager::Instance()->Left()->pos - CharacterManager::Instance()->Right()->pos;
	Vec2<float>cPos = CharacterManager::Instance()->Left()->pos - distance / 2.0f;
	ScrollMgr::Instance()->Update(cPos);

	//パーティクル更新
	ParticleMgr::Instance()->Update();

	//BackGround::Instance()->Update();
	Camera::Instance()->Update();
	FaceIcon::Instance()->Update();
	WinCounter::Instance()->Update();


	if (DebugKeyManager::Instance()->DebugKeyTrigger(DIK_Q, "AddLeftSuperiorityGauge", TO_STRING(DIK_Q)))
	{
		SuperiorityGauge::Instance()->AddGauge(LEFT_TEAM, 10.0f);
	}
	if (DebugKeyManager::Instance()->DebugKeyTrigger(DIK_W, "AddRightSuperiorityGauge", TO_STRING(DIK_W)))
	{
		SuperiorityGauge::Instance()->AddGauge(RIGHT_TEAM, 10.0f);
	}



	// 優勢ゲージが振り切ったトリガー判定のときにスタン演出を有効化する。
	if (SuperiorityGauge::Instance()->GetGaugeData(RIGHT_TEAM).overGaugeFlag && !SuperiorityGauge::Instance()->GetGaugeData(RIGHT_TEAM).prevOverGaugeFlag) {
		// 敵の優勢ゲージが振り切ったということは、プレイヤーの優勢ゲージが0だということ。
		/*StunEffect::Instance()->Activate(CharacterManager::Instance()->Left()->pos, Vec2<float>(0, 0), LEFT_TEAM);
		ResultTransfer::Instance()->leftBreakCount++;
		CharacterManager::Instance()->Left()->Break();
		CharacterManager::Instance()->Left()->InitVel();*/
		WinCounter::Instance()->RoundFinish(lineCenterPos, false, CharacterManager::Instance()->Left()->pos);
		CharacterManager::Instance()->Left()->OnKnockOut();
		roundFinishFlag = true;
		playerOrEnemeyWinFlag = false;
		gameStartFlag = false;

		areaHitColor = Color(255, 0, 0, 255);
		playerHitColor = Color(255, 0, 0, 255);

		screenEdgeEffect.RightPlayerWin(120);

		// 両キャラの予測線を消す。
		CharacterManager::Instance()->Right()->InitSwingLineSegmetn();
		CharacterManager::Instance()->Left()->InitSwingLineSegmetn();
	}
	if (SuperiorityGauge::Instance()->GetGaugeData(LEFT_TEAM).overGaugeFlag && !SuperiorityGauge::Instance()->GetGaugeData(LEFT_TEAM).prevOverGaugeFlag) {
		// プレイヤーの優勢ゲージが振り切ったということは、敵の優勢ゲージが0だということ。
		/*StunEffect::Instance()->Activate(CharacterManager::Instance()->Right()->pos, Vec2<float>(1200, 0), RIGHT_TEAM);
		ResultTransfer::Instance()->rightBreakCount++;
		CharacterManager::Instance()->Right()->Break();
		CharacterManager::Instance()->Right()->InitVel();*/
		//プレイヤー勝利
		WinCounter::Instance()->RoundFinish(lineCenterPos, true, CharacterManager::Instance()->Right()->pos);
		CharacterManager::Instance()->Right()->OnKnockOut();
		roundFinishFlag = true;
		playerOrEnemeyWinFlag = true;
		gameStartFlag = false;

		screenEdgeEffect.LeftPlayerWin(120);

		// 両キャラの予測線を消す。
		CharacterManager::Instance()->Right()->InitSwingLineSegmetn();
		CharacterManager::Instance()->Left()->InitSwingLineSegmetn();
	}

	CrashMgr::Instance()->Update();

	// 残像を更新。
	AfterImageMgr::Instance()->Update();

	// クラッシュ時の演出の更新処理。
	CrashEffectMgr::Instance()->Update();

	countBlock.Update();

	// スタミナアイテムの更新処理
	if (!readyToStartRoundFlag) {
		StaminaItemMgr::Instance()->Update(playerHomeBase.GetRightUpPos(), enemyHomeBase.GetLeftDownPos());
	}

	// スタミナアイテムの当たり判定処理
	int healAmount = StaminaItemMgr::Instance()->CheckHit(&CharacterManager::Instance()->Left()->pos, 30, 70, StaminaItem::CHARA_ID::LEFT, CharacterManager::Instance()->Left()->GetPilotPosPtr());
	CharacterManager::Instance()->Left()->HealStamina(healAmount);
	healAmount = StaminaItemMgr::Instance()->CheckHit(&CharacterManager::Instance()->Right()->pos, 90, 70, StaminaItem::CHARA_ID::RIGHT, CharacterManager::Instance()->Right()->GetPilotPosPtr());
	CharacterManager::Instance()->Right()->HealStamina(healAmount);


	if (!Camera::Instance()->Active()) {

		//互いの距離でカメラのズーム率を変える。
		float distance = CharacterManager::Instance()->Left()->pos.Distance(CharacterManager::Instance()->Right()->pos);

		//最大距離
		const float MAX_ADD_ZOOM = 3000.0f;

		float zoomRate = 1.0f;
		float deadLine = 1200.0f;//この距離以下はズームしない

		// 限界より伸びていたら。
		if (MAX_ADD_ZOOM < distance)
		{
			zoomRate = 1.0f;
		}
		else if (deadLine <= distance)
		{
			zoomRate = (distance - deadLine) / MAX_ADD_ZOOM;
		}
		else
		{
			zoomRate = 0.0f;
		}
		static const float ZOOM_OFFSET = -0.01f;		// デフォルトで少しだけカメラを引き気味にする。
		Camera::Instance()->zoom = 0.5f - zoomRate + ZOOM_OFFSET;

		// カメラのズームが0.27f未満にならないようにする。
		float minZoomValue = 0.27f;
		if (Camera::Instance()->zoom < minZoomValue)
		{
			Camera::Instance()->zoom = minZoomValue;
		}
	}
	else {

		ScrollMgr::Instance()->lineCenterOffset = {};

	}


	// 敵キャラがプレイヤーにある程度近付いたら反対側に吹っ飛ばす機能。
	const float BOUNCE_DISTANCE = 300.0f; // ある程度の距離
	if (Vec2<float>(CharacterManager::Instance()->Left()->pos - CharacterManager::Instance()->Right()->pos).Length() <= BOUNCE_DISTANCE) {

		Vec2<float> charaDir = Vec2<float>(CharacterManager::Instance()->Right()->pos - CharacterManager::Instance()->Left()->pos).GetNormal();

		// 吹っ飛ばす量
		const float BOUNCE_POWER = 50.0f;
		CharacterManager::Instance()->Right()->bounceVel = charaDir * BOUNCE_POWER;

	}

}

void Game::Draw()
{
	int stageNum = SelectStage::Instance()->GetStageNum();
	int roomNum = SelectStage::Instance()->GetRoomNum();

	/*===== 描画処理 =====*/
	//BackGround::Instance()->Draw();

	if (stageNum != prevDrawChipStageNum || roomNum != prevDrawChipRoomNum)
	{
	}

	mapChipDrawData = StageMgr::Instance()->GetLocalDrawMap();
	prevDrawChipStageNum = stageNum;
	prevDrawChipRoomNum = roomNum;
	DrawMapChip(*mapData, *mapChipDrawData, stageNum, roomNum);



	if (roundChangeEffect.readyFlag)
	{
		bossHandMgr->Draw();
		playerHandMgr->Draw();
	}

	//playerHomeBase.Draw();
	//enemyHomeBase.Draw();

	// スタミナアイテムの描画処理
	StaminaItemMgr::Instance()->Draw();

	static int CENTER_CHAIN_GRAPH = TexHandleMgr::LoadGraph("resource/ChainCombat/chain.png");
	static int PLAYER_CHAIN_GRAPH = TexHandleMgr::LoadGraph("resource/ChainCombat/chain_player.png");
	static int ENEMY_CHAIN_GRAPH = TexHandleMgr::LoadGraph("resource/ChainCombat/chain_enemy.png");
	static const int CHAIN_THICKNESS = 4;
	// プレイヤーとボス間に線を描画
	if (roundChangeEffect.initGameFlag)
	{
		//左プレイヤー～中央のチェイン
		auto& left = CharacterManager::Instance()->Left();
		Vec2<float>leftLineCenterDir = (lineCenterPos - left->pos).GetNormal();
		Vec2<float>leftChainBorderPos = left->pos + leftLineCenterDir * left->addLineLength;	//中央チェインと左プレイヤーチェインとの変わり目
		if (0.0f < left->addLineLength)
		{
			DrawFunc::DrawLine2DGraph(ScrollMgr::Instance()->Affect(left->pos), ScrollMgr::Instance()->Affect(leftChainBorderPos),
				TexHandleMgr::GetTexBuffer(PLAYER_CHAIN_GRAPH), CHAIN_THICKNESS * ScrollMgr::Instance()->zoom);
		}

		//右プレイヤー～中央のチェイン
		auto& right = CharacterManager::Instance()->Right();
		Vec2<float>rightLineCenterDir = (lineCenterPos - right->pos).GetNormal();
		Vec2<float>rightChainBorderPos = right->pos + rightLineCenterDir * right->addLineLength;	//中央チェインと右プレイヤーチェインとの変わり目
		if (0.0f < right->addLineLength)
		{
			DrawFunc::DrawLine2DGraph(ScrollMgr::Instance()->Affect(right->pos), ScrollMgr::Instance()->Affect(rightChainBorderPos),
				TexHandleMgr::GetTexBuffer(ENEMY_CHAIN_GRAPH), CHAIN_THICKNESS * ScrollMgr::Instance()->zoom);
		}

		float charaDistance = (CharacterManager::Instance()->Left()->pos - CharacterManager::Instance()->Right()->pos).Length();
		//中央チェイン
		if (charaDistance < CharacterManager::Instance()->Left()->LINE_LENGTH * 2.0f) {

			// 既定値より短かったら。
			DrawFunc::DrawLine2DGraph(ScrollMgr::Instance()->Affect(CharacterManager::Instance()->Left()->pos), ScrollMgr::Instance()->Affect(CharacterManager::Instance()->Right()->pos),
				TexHandleMgr::GetTexBuffer(CENTER_CHAIN_GRAPH), CHAIN_THICKNESS * ScrollMgr::Instance()->zoom);

		}
		else {

			DrawFunc::DrawLine2DGraph(ScrollMgr::Instance()->Affect(leftChainBorderPos), ScrollMgr::Instance()->Affect(rightChainBorderPos),
				TexHandleMgr::GetTexBuffer(CENTER_CHAIN_GRAPH), CHAIN_THICKNESS * ScrollMgr::Instance()->zoom);

		}

		// 線分の中心に円を描画
		if (roundChangeEffect.drawFightFlag)
		{
			static int LINE_CENTER_GRAPH = TexHandleMgr::LoadGraph("resource/ChainCombat/line_center.png");
			DrawFunc::DrawRotaGraph2D(ScrollMgr::Instance()->Affect(lineCenterPos), Vec2<float>(ScrollMgr::Instance()->zoom, ScrollMgr::Instance()->zoom), 0.0f, TexHandleMgr::GetTexBuffer(LINE_CENTER_GRAPH));
		}
		//DrawFunc::DrawCircle2D(playerDefLength + playerBossDir * lineLengthPlayer - scrollShakeAmount, 10, Color());

		mapChipGenerator[DebugParameter::Instance()->generator]->Draw();
	}

	roundChangeEffect.Draw();

	if (roundChangeEffect.initGameFlag)
	{
		// 残像を描画
		AfterImageMgr::Instance()->Draw();
		CharacterManager::Instance()->Left()->Draw(readyToStartRoundFlag);
		CharacterManager::Instance()->Right()->Draw(readyToStartRoundFlag);
	}

	// クラッシュ時の演出の描画処理。
	CrashEffectMgr::Instance()->Draw();

	ParticleMgr::Instance()->Draw();

	CrashMgr::Instance()->Draw();

	//screenEdgeEffect.Draw();

	if (roundChangeEffect.initGameFlag && !roundFinishFlag)
	{
		CharacterManager::Instance()->Left()->DrawUI();
		CharacterManager::Instance()->Right()->DrawUI();
	}

	GameTimer::Instance()->Draw();
	ScoreManager::Instance()->Draw();

	countBlock.Draw();

	// プレイヤーとボス間に線を描画
	//DrawFunc::DrawLine2D(ScrollMgr::Instance()->Affect(player.centerPos), ScrollMgr::Instance()->Affect(boss.pos), Color());

	//SuperiorityGauge::Instance()->Draw();

	//miniMap.Draw();

	//FaceIcon::Instance()->Draw();

	WinCounter::Instance()->Draw();

	StunEffect::Instance()->Draw();

	{
		Vec2<float>leftUpPos = *CharacterManager::Instance()->Right()->GetAreaHitBox().center - CharacterManager::Instance()->Right()->GetAreaHitBox().size / 2.0f;
		Vec2<float>rightDownPos = *CharacterManager::Instance()->Right()->GetAreaHitBox().center + CharacterManager::Instance()->Right()->GetAreaHitBox().size / 2.0f;
		//DrawFunc::DrawBox2D(ScrollMgr::Instance()->Affect(leftUpPos), ScrollMgr::Instance()->Affect(rightDownPos), Color(255, 255, 255, 255), DXGI_FORMAT_R8G8B8A8_UNORM);
	}

	{
		Vec2<float>leftUpPos = *CharacterManager::Instance()->Left()->GetAreaHitBox().center - CharacterManager::Instance()->Left()->GetAreaHitBox().size / 2.0f;
		Vec2<float>rightDownPos = *CharacterManager::Instance()->Left()->GetAreaHitBox().center + CharacterManager::Instance()->Left()->GetAreaHitBox().size / 2.0f;
		//DrawFunc::DrawBox2D(ScrollMgr::Instance()->Affect(leftUpPos), ScrollMgr::Instance()->Affect(rightDownPos), playerHitColor, DXGI_FORMAT_R8G8B8A8_UNORM);
	}

	{
		Vec2<float>leftUpPos = *enemyHomeBase.hitBox.center - enemyHomeBase.hitBox.size / 2.0f;
		Vec2<float>rightDownPos = *enemyHomeBase.hitBox.center + enemyHomeBase.hitBox.size / 2.0f;
		//DrawFunc::DrawBox2D(ScrollMgr::Instance()->Affect(leftUpPos), ScrollMgr::Instance()->Affect(rightDownPos), areaHitColor, DXGI_FORMAT_R8G8B8A8_UNORM);
	}

}

void Game::Scramble()
{
	/*===== 引っ張り合いの処理 =====*/
	// 前フレームの線の中心座標を保存
	prevLineCenterPos = lineCenterPos;

	//どちらも動けないとき何もしない
	if (!(CharacterManager::Instance()->Left()->GetCanMove() || CharacterManager::Instance()->Right()->GetCanMove()))return;

	Vec2<float> leftVelGauge;
	Vec2<float> rightVelGauge;

	// 右側のキャラが吹っ飛ぶマップチップに当たった際の移動量の処理を行う。
	CharacterManager::Instance()->Right()->pos += CharacterManager::Instance()->Right()->bounceVel;
	CharacterManager::Instance()->Right()->addLineLength += CharacterManager::Instance()->Right()->bounceVel.Length();

	// 移動量を取得。 優勢ゲージはここで更新。
	double leftVel = CharacterManager::Instance()->Left()->vel.Length() * SlowMgr::Instance()->slowAmount;
	leftVelGauge = CharacterManager::Instance()->Left()->vel * SlowMgr::Instance()->slowAmount;
	double rightVel = CharacterManager::Instance()->Right()->vel.Length() * SlowMgr::Instance()->slowAmount;
	rightVelGauge = CharacterManager::Instance()->Right()->vel * SlowMgr::Instance()->slowAmount;
	double subVel = fabs(fabs(leftVel) - fabs(rightVel));

	// [スタン演出中] は移動させない。 踏ん張り中の場合は、どちらにせよ移動量が限りなく0に近いので移動させても問題がない。
	if (!(StunEffect::Instance()->isActive)) {
		// 振り回され中じゃなかったら移動させる。
		if (!CharacterManager::Instance()->Right()->GetNowSwing()) {
			CharacterManager::Instance()->Left()->pos += leftVelGauge;
		}
		// 振り回され中じゃなかったら移動させる。
		if (!CharacterManager::Instance()->Left()->GetNowSwing() && !CharacterManager::Instance()->Left()->isStopPartner) {
			CharacterManager::Instance()->Right()->pos += rightVelGauge;
		}
		else if (CharacterManager::Instance()->Left()->isStopPartner) {
			CharacterManager::Instance()->Right()->pos += leftVelGauge;
		}
	}

	// 線分の長さ
	float charaLength = 0;
	float LINE = CharacterInterFace::LINE_LENGTH * 2 + (CharacterManager::Instance()->Left()->addLineLength + CharacterManager::Instance()->Right()->addLineLength);

	// どちらかが踏ん張っているか。
	bool isHoldNow = CharacterManager::Instance()->Left()->isHold || CharacterManager::Instance()->Right()->isHold;

	// どちらかが振り回しているか。
	bool isSwingNow = CharacterManager::Instance()->Left()->GetNowSwing() || CharacterManager::Instance()->Right()->GetNowSwing();


	// どちらの移動量が多いかを取得。どちらも同じ場合は処理を飛ばす。
	if (leftVelGauge.Length() < rightVelGauge.Length()) {

		// 右側のほうが移動量が大きかったら。
		// 右側のキャラを中心とした位置に左側のキャラを持ってくる。

		// 距離を求める。
		charaLength = Vec2<float>(CharacterManager::Instance()->Left()->pos).Distance(CharacterManager::Instance()->Right()->pos);

		// 左側のキャラを右側のキャラの方に移動させる。
		if (LINE < charaLength) {

			// 押し戻し量
			float moveLength = charaLength - LINE;

			// 押し戻し方向
			Vec2<float> moveDir = Vec2<float>(CharacterManager::Instance()->Right()->pos - CharacterManager::Instance()->Left()->pos);
			moveDir.Normalize();

			// 押し戻す。
			CharacterManager::Instance()->Left()->pos += moveDir * Vec2<float>(moveLength, moveLength);

			// 引っかかり判定だったら
			if (CharacterManager::Instance()->Left()->GetStackFlag()) {

				CharacterManager::Instance()->Left()->addLineLength += moveLength;
				//CharacterManager::Instance()->Right()->pos += -moveDir * Vec2<float>(moveLength, moveLength);

				// 引っかかっている場合は更に押し戻す。(壁ズリを表現するため)
				if (0 < CharacterManager::Instance()->Left()->addLineLength) {
					CharacterManager::Instance()->Left()->pos += moveDir * Vec2<float>(moveLength, moveLength);
				}

			}

		}

	}
	else if (rightVelGauge.Length() < leftVelGauge.Length()) {

		// 左側のほうが移動量が大きかったら。
		// 左側のキャラを中心とした位置に右側のキャラを持ってくる。

		// 距離を求める。
		charaLength = Vec2<float>(CharacterManager::Instance()->Left()->pos).Distance(CharacterManager::Instance()->Right()->pos);

		// ボスをプレイヤーの方に移動させる。
		if (LINE < charaLength) {

			// 押し戻し量
			float moveLength = charaLength - LINE;

			// 押し戻し方向
			Vec2<float> moveDir = Vec2<float>(CharacterManager::Instance()->Left()->pos - CharacterManager::Instance()->Right()->pos);
			moveDir.Normalize();

			// 押し戻す。
			CharacterManager::Instance()->Right()->pos += moveDir * Vec2<float>(moveLength, moveLength);


			// 引っかかり判定だったら
			if (CharacterManager::Instance()->Right()->GetStackFlag()) {

				CharacterManager::Instance()->Right()->addLineLength += moveLength;
				//CharacterManager::Instance()->Left()->pos += -moveDir * Vec2<float>(moveLength, moveLength);

				// 引っかかっている場合は更に押し戻す。(壁ズリを表現するため)
				if (0 < CharacterManager::Instance()->Right()->addLineLength) {
					CharacterManager::Instance()->Right()->pos += moveDir * Vec2<float>(moveLength, moveLength);
				}

			}

		}

	}

	const float ADD_LINE_LENGTH_SUB_AMOUNT = 5.0f;

	// どちらのキャラも引っかかっているか
	bool isBothStuck = CharacterManager::Instance()->Right()->GetStackFlag() && CharacterManager::Instance()->Left()->GetStackFlag();
	// 引っかかり判定じゃなかったらだんだん短くする。
	Vec2<float> movedVel = (CharacterManager::Instance()->Right()->pos - CharacterManager::Instance()->Right()->prevPos);
	movedVel += (CharacterManager::Instance()->Left()->pos - CharacterManager::Instance()->Left()->prevPos);
	// 右側の紐の処理
	if (!isBothStuck && 0 < CharacterManager::Instance()->Right()->addLineLength && !isSwingNow) {

		// 引く量 0未満にならないようにするため。
		float subAmount = ADD_LINE_LENGTH_SUB_AMOUNT;
		if (CharacterManager::Instance()->Right()->addLineLength < ADD_LINE_LENGTH_SUB_AMOUNT) {
			subAmount = CharacterManager::Instance()->Right()->addLineLength;
		}

		// 移動中は引かない。
		if (ADD_LINE_LENGTH_SUB_AMOUNT < CharacterManager::Instance()->Right()->vel.Length()) {
			subAmount = 0;
		}

		// 紐加算量をへらす。
		CharacterManager::Instance()->Right()->addLineLength -= subAmount;

		// キャラクターを紐加算量が減った方向に移動させる。
		Vec2<float> charaDir = (CharacterManager::Instance()->Left()->pos - CharacterManager::Instance()->Right()->pos).GetNormal();
		CharacterManager::Instance()->Right()->vel += charaDir * subAmount / 10.0f;

		// 引いた分移動させる。
		float charaLength = (CharacterManager::Instance()->Left()->pos - CharacterManager::Instance()->Right()->pos).Length();
		// [今の長さ] が [初期長さ * 2] + [左の紐の長さ] 以上だったら処理を行う。
		if (CharacterInterFace::LINE_LENGTH * 2.0f + CharacterManager::Instance()->Left()->addLineLength < charaLength) {

			// 動いていたら
			if (1.0f < movedVel.Length()) {
			}
			// 右側が引っかかっていたら。
			else if (CharacterManager::Instance()->Right()->GetStackFlag()) {
				// 右側が引っかかっているときは代わりに左側を動かす。
				CharacterManager::Instance()->Left()->pos += (CharacterManager::Instance()->Right()->pos - CharacterManager::Instance()->Left()->pos).GetNormal() * subAmount;
			}
			else {
				CharacterManager::Instance()->Right()->pos += (CharacterManager::Instance()->Left()->pos - CharacterManager::Instance()->Right()->pos).GetNormal() * subAmount;
			}
		}

	}

	if (CharacterManager::Instance()->Right()->addLineLength < 0) CharacterManager::Instance()->Right()->addLineLength = 0;


	// 左側の紐の処理
	if (!isBothStuck && 0 < CharacterManager::Instance()->Left()->addLineLength && !isSwingNow) {

		// 引く量 0未満にならないようにするため。
		float subAmount = ADD_LINE_LENGTH_SUB_AMOUNT;
		if (CharacterManager::Instance()->Left()->addLineLength < ADD_LINE_LENGTH_SUB_AMOUNT) {
			subAmount = CharacterManager::Instance()->Left()->addLineLength;
		}

		// 移動中は引かない。
		if (ADD_LINE_LENGTH_SUB_AMOUNT < CharacterManager::Instance()->Left()->vel.Length()) {
			subAmount = 0;
		}


		// 紐加算量をへらす。
		CharacterManager::Instance()->Left()->addLineLength -= subAmount;

		// キャラクターを紐加算量が減った方向に移動させる。
		Vec2<float> charaDir = (CharacterManager::Instance()->Right()->pos - CharacterManager::Instance()->Left()->pos).GetNormal();
		CharacterManager::Instance()->Left()->vel += charaDir * subAmount / 10.0f;

		// 引いた分移動させる。
		float charaLength = (CharacterManager::Instance()->Left()->pos - CharacterManager::Instance()->Right()->pos).Length();
		// [今の長さ] が [初期長さ * 2] + [左の紐の長さ] 以上だったら処理を行う。
		if (CharacterInterFace::LINE_LENGTH * 2.0f + CharacterManager::Instance()->Right()->addLineLength < charaLength) {

			// 動いていたら
			if (1.0f < movedVel.Length()) {
			}
			// 左側が引っかかっていたら。
			else if (CharacterManager::Instance()->Left()->GetStackFlag()) {
				// 左側が引っかかっているときは代わりに右側を動かす。
				CharacterManager::Instance()->Right()->pos += (CharacterManager::Instance()->Left()->pos - CharacterManager::Instance()->Right()->pos).GetNormal() * subAmount;
			}
			else {
				CharacterManager::Instance()->Left()->pos += (CharacterManager::Instance()->Right()->pos - CharacterManager::Instance()->Left()->pos).GetNormal() * subAmount;
			}
		}

	}

	if (CharacterManager::Instance()->Right()->addLineLength < 0) CharacterManager::Instance()->Right()->addLineLength = 0;
	if (CharacterManager::Instance()->Left()->addLineLength < 0) CharacterManager::Instance()->Left()->addLineLength = 0;

	isCatchMapChipBoss = false;
	isCatchMapChipPlayer = false;

}

void Game::CalCenterPos()
{

	/*===== 中心点を求める処理 =====*/

	// 本当はScrambleの一番うしろに入れていた処理なんですが、押し戻しをした後に呼ぶ必要が出てきたので関数で分けました。

	auto& left = CharacterManager::Instance()->Left();
	auto& right = CharacterManager::Instance()->Right();

	// 移動量に応じて本来あるべき長さにする。
	Vec2<float> prevSubPos = CharacterManager::Instance()->Left()->pos - CharacterManager::Instance()->Left()->prevPos;
	float horaiAddLineLength = (prevSubPos.Length() / CharacterManager::Instance()->Left()->MOVE_SPEED_PLAYER);
	horaiAddLineLength *= CharacterManager::Instance()->Left()->ADD_LINE_LENGTH_VEL;
	if (CharacterManager::Instance()->Left()->addLineLength < horaiAddLineLength && 1.0f < CharacterManager::Instance()->Left()->vel.Length() && 1.0f < prevSubPos.Length()) {
		CharacterManager::Instance()->Left()->addLineLength = horaiAddLineLength;
	}
	prevSubPos = CharacterManager::Instance()->Right()->pos - CharacterManager::Instance()->Right()->prevPos;
	horaiAddLineLength = (prevSubPos.Length() / CharacterManager::Instance()->Right()->MOVE_SPEED_PLAYER);
	horaiAddLineLength *= CharacterManager::Instance()->Right()->ADD_LINE_LENGTH_VEL;
	if (CharacterManager::Instance()->Right()->addLineLength < horaiAddLineLength && 1.0f < CharacterManager::Instance()->Right()->vel.Length() && 1.0f < prevSubPos.Length()) {
		CharacterManager::Instance()->Right()->addLineLength = horaiAddLineLength;
	}

	// 現在の距離が加算されている紐の長さより短かったら、addLineValueを短くする。
	float nowLength = (left->pos - right->pos).Length();
	if (nowLength < CharacterInterFace::LINE_LENGTH * 2.0f + left->addLineLength + right->addLineLength) {

		// オーバーしている量を求める。
		float overLength = (CharacterInterFace::LINE_LENGTH * 2.0f + left->addLineLength + right->addLineLength) - nowLength;
		// オーバーしている量の半分を求める。
		float halfOverLength = overLength / 2.0f;

		// まずは左側から短くする。

		left->addLineLength -= halfOverLength;

		// 0以下になったら、その量を保存しておく。
		float leftOverLength = 0;
		if (left->addLineLength < 0) {

			leftOverLength = fabs(left->addLineLength);

			// 紐の長さが0未満にならないようにする。
			left->addLineLength = 0;

		}

		// 次に右側を短くする。

		right->addLineLength -= halfOverLength + leftOverLength;

		// 紐の長さが0未満にならないようにする。
		if (right->addLineLength < 0) {

			right->addLineLength = 0;

		}
	}

	// 紐の中心点を計算
	{
		float distance = (CharacterManager::Instance()->Right()->pos - CharacterManager::Instance()->Left()->pos).Length();
		Vec2<float> bossDir = CharacterManager::Instance()->Right()->pos - CharacterManager::Instance()->Left()->pos;
		bossDir.Normalize();

		// ボスとプレイヤー間の距離が規定値以下だったら
		//if (distance < CharacterInterFace::LINE_LENGTH + CharacterInterFace::LINE_LENGTH) {
		//	// 既定値以下だったら団子化減少を防ぐために、二点間の真ん中の座標にする。
		//	lineCenterPos = CharacterManager::Instance()->Left()->pos + bossDir * Vec2<float>(distance / 2.0f, distance / 2.0f);
		//}
		//else {
			// 規定値以上だったら普通に場所を求める。

		auto& right = CharacterManager::Instance()->Right();
		auto& left = CharacterManager::Instance()->Left();

		Vec2<float> rightPos = right->pos;
		rightPos += (left->pos - right->pos).GetNormal() * right->addLineLength;

		Vec2<float> leftPos = left->pos;
		leftPos += (right->pos - left->pos).GetNormal() * left->addLineLength;

		float length = (leftPos - rightPos).Length();
		length /= 2.0f;
		Vec2<float> dir = (leftPos - rightPos).GetNormal();

		lineCenterPos = rightPos + dir * length;

		//float playerLineLength = CharacterManager::Instance()->Left()->LINE_LENGTH + CharacterManager::Instance()->Left()->addLineLength;
		//lineCenterPos = CharacterManager::Instance()->Left()->pos + bossDir * Vec2<float>(playerLineLength, playerLineLength);
	//}
	}

}

Vec2<float> Game::GetStageSize()
{
	static const float CHIP_SIZE = 64;
	int sizeX = mapData[0].size();
	int sizeY = mapData->size();
	return Vec2<float>(CHIP_SIZE * sizeX, CHIP_SIZE * sizeY);
}

void Game::SwitchingStage()
{
	const bool enableToSelectStageFlag = 0 < debugStageData[0];
	const bool enableToSelectStageFlag2 = debugStageData[0] < StageMgr::Instance()->GetMaxStageNumber() - 1;
	//マップの切り替え
	const bool left = UsersInput::Instance()->KeyOnTrigger(DIK_LEFT) || UsersInput::Instance()->ControllerOnTrigger(0, DPAD_LEFT);
	const bool right = UsersInput::Instance()->KeyOnTrigger(DIK_RIGHT) || UsersInput::Instance()->ControllerOnTrigger(0, DPAD_RIGHT);

	const bool enableToSelectRoomFlag = 0 < debugStageData[1];
	const bool enableToSelectRoomFlag2 = debugStageData[1] < StageMgr::Instance()->GetMaxRoomNumber(debugStageData[0]) - 1;

	//部屋か番号に切り替え
	if (left && 0 < nowSelectNum)
	{
		--nowSelectNum;
		--debugStageData[1];
	}
	if (right && nowSelectNum < StageMgr::Instance()->GetMaxRoomNumber(SelectStage::Instance()->GetStageNum()))
	{
		++nowSelectNum;
		++debugStageData[1];
	}

	if (debugStageData[1] <= 0)
	{
		debugStageData[1] = 0;
		nowSelectNum = 0;
	}
	if (StageMgr::Instance()->GetMaxRoomNumber(SelectStage::Instance()->GetStageNum()) <= debugStageData[1])
	{
		debugStageData[1] = StageMgr::Instance()->GetMaxRoomNumber(SelectStage::Instance()->GetStageNum());
		nowSelectNum = StageMgr::Instance()->GetMaxRoomNumber(SelectStage::Instance()->GetStageNum());
	}

	if (StageMgr::Instance()->GetMaxRoomNumber(SelectStage::Instance()->GetStageNum()) <= nowSelectNum)
	{
		debugStageData[1] = StageMgr::Instance()->GetMaxRoomNumber(SelectStage::Instance()->GetStageNum()) - 1;
	}


	const bool done = UsersInput::Instance()->KeyOnTrigger(DIK_RETURN)
		|| (UsersInput::Instance()->ControllerInput(0, A) && UsersInput::Instance()->ControllerOnTrigger(0, B))
		|| (UsersInput::Instance()->ControllerOnTrigger(0, A) && UsersInput::Instance()->ControllerInput(0, B));
	if (done)
	{
		SelectStage::Instance()->SelectStageNum(debugStageData[0]);
		SelectStage::Instance()->SelectRoomNum(debugStageData[1]);
		SelectStage::Instance()->resetStageFlag = true;

		InitGame(SelectStage::Instance()->GetStageNum(), SelectStage::Instance()->GetRoomNum());
	}

}

void Game::DeterminationOfThePosition()
{

	//プレイヤー陣地と敵の判定
	if (playerHomeBase.Collision(CharacterManager::Instance()->Right()->GetAreaHitBox()) && !roundFinishFlag && !readyToStartRoundFlag)
	{
		//プレイヤー勝利
		WinCounter::Instance()->RoundFinish(lineCenterPos, true, CharacterManager::Instance()->Right()->pos);
		CharacterManager::Instance()->Right()->OnKnockOut();
		roundFinishFlag = true;
		playerOrEnemeyWinFlag = true;
		gameStartFlag = false;

		screenEdgeEffect.LeftPlayerWin(120);

		// 両キャラの予測線を消す。
		CharacterManager::Instance()->Right()->InitSwingLineSegmetn();
		CharacterManager::Instance()->Left()->InitSwingLineSegmetn();

	}

	//敵陣地とプレイヤーの判定
	if (enemyHomeBase.Collision(CharacterManager::Instance()->Left()->GetAreaHitBox()) && !roundFinishFlag && !readyToStartRoundFlag)
	{
		//敵勝利
		WinCounter::Instance()->RoundFinish(lineCenterPos, false, CharacterManager::Instance()->Left()->pos);
		CharacterManager::Instance()->Left()->OnKnockOut();
		roundFinishFlag = true;
		playerOrEnemeyWinFlag = false;
		gameStartFlag = false;

		areaHitColor = Color(255, 0, 0, 255);
		playerHitColor = Color(255, 0, 0, 255);

		screenEdgeEffect.RightPlayerWin(120);

		// 両キャラの予測線を消す。
		CharacterManager::Instance()->Right()->InitSwingLineSegmetn();
		CharacterManager::Instance()->Left()->InitSwingLineSegmetn();
	}

}

void Game::RoundStartEffect(const bool& Loop, const RoomMapChipArray& tmpMapData)
{

	//ラウンド開始時の演出開始
	if (readyToStartRoundFlag)
	{
		roundChangeEffect.Start(WinCounter::Instance()->GetNowRound(), playerOrEnemeyWinFlag);
		Vec2<float>winSize;
		winSize.x = static_cast<float>(WinApp::Instance()->GetWinSize().x);
		winSize.y = static_cast<float>(WinApp::Instance()->GetWinSize().y);

		//Vec2<float> responePos = Vec2<float>(((mapData[0].size() * MAP_CHIP_SIZE) / 2.0f) - 100.0f, (mapData.size() * MAP_CHIP_SIZE) / 2.0f);
		//responePos.y -= -cameraBasePos.y * 2.0f;
		ScrollMgr::Instance()->Warp(responeScrollPos);

		//プレイヤーと敵の座標初期化
		if (roundChangeEffect.readyToInitFlag && !roundChangeEffect.initGameFlag)
		{
			roundChangeEffect.initGameFlag = true;
			if (!AudioApp::Instance()->NowPlay(bgm))
			{
				AudioApp::Instance()->ChangeVolume(bgm, 0.1f);
				//AudioApp::Instance()->PlayWave(bgm, true);
			}
		}

		//登場演出
		if (roundChangeEffect.initGameFlag)
		{
			CharacterManager::Instance()->Left()->Appear();
			CharacterManager::Instance()->Right()->Appear();
			if (CharacterManager::Instance()->Left()->CompleteAppear() && CharacterManager::Instance()->Right()->CompleteAppear())	//どちらのキャラも登場演出完了
			{
				//ゲームスタート
				readyToStartRoundFlag = false;
				gameStartFlag = true;
				roundTimer = 0;
				CharacterManager::Instance()->Left()->SetCanMove(true);
				CharacterManager::Instance()->Right()->SetCanMove(true);
				CharacterManager::Instance()->Left()->SetHitCheck(true);
				CharacterManager::Instance()->Right()->SetHitCheck(true);
				GameTimer::Instance()->SetInterruput(false);
			}
		}
		//gameStartFlag = true;
		//SelectStage::Instance()->resetStageFlag = true;
		//readyToStartRoundFlag = false;
		float size = (tmpMapData[0].size() * MAP_CHIP_SIZE) - 400.0f;
		miniMap.Init(size);
	}

}

void Game::RoundFinishEffect(const bool& Loop)
{

	//ラウンド終了演出開始
	if (roundFinishFlag)
	{
		//動けなくする
		CharacterManager::Instance()->Left()->SetCanMove(false);
		CharacterManager::Instance()->Right()->SetCanMove(false);
		CharacterManager::Instance()->Left()->SetHitCheck(false);
		CharacterManager::Instance()->Right()->SetHitCheck(false);

		//時間計測ストップ
		GameTimer::Instance()->SetInterruput(true);

		//勝利数カウント演出
		if (!WinCounter::Instance()->GetNowAnimation())
		{
			//どちらかが３勝とったらゲーム終了
			if (WinCounter::Instance()->GetGameFinish() && !Loop)
			{
				ResultTransfer::Instance()->resultScore = ScoreManager::Instance()->GetScore();
				if (WinCounter::Instance()->Winner() == LEFT_TEAM)ResultTransfer::Instance()->winner = CharacterManager::Instance()->Left()->GetCharacterName();
				else ResultTransfer::Instance()->winner = CharacterManager::Instance()->Right()->GetCharacterName();
				turnResultScene = true;
			}
			//次のラウンドへ
			else
			{
				//練習モード
				if (Loop)WinCounter::Instance()->Reset();

				++roundTimer;

				if (60 <= roundTimer)
				{
					readyToStartRoundFlag = true;
					roundFinishFlag = false;
					InitGame(SelectStage::Instance()->GetStageNum(), SelectStage::Instance()->GetRoomNum());
				}
			}
		}
	}

}
