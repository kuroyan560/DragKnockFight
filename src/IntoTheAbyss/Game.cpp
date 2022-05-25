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

				//�L�т������I�[���ɓn��
				data.push_back(std::make_unique<MassChipData>(checkData.GetLeftUpPos(), checkData.GetRightDownPos(), checkData.sideOrUpDownFlag));
			}
		}
	}

	return data;
}

void Game::DrawMapChip(const vector<vector<int>>& mapChipData, vector<vector<MapChipDrawData>>& mapChipDrawData, const int& stageNum, const int& roomNum)
{
	std::map<int, std::vector<ChipData>>datas;

	// �`�悷��`�b�v�̃T�C�Y
	const float DRAW_MAP_CHIP_SIZE = MAP_CHIP_SIZE * ScrollMgr::Instance()->zoom;
	SizeData wallChipMemorySize = StageMgr::Instance()->GetMapChipSizeData(MAPCHIP_TYPE_STATIC_BLOCK);


	// �}�b�v�`�b�v�̏c�̗v�f�����擾�B
	const int HEIGHT = mapChipData.size();
	for (int height = 0; height < HEIGHT; ++height) {

		// �}�b�v�`�b�v�̉��̗v�f�����擾�B
		const int WIDTH = mapChipData[height].size();
		for (int width = 0; width < WIDTH; ++width) {

			if (mapChipDrawData[height][width].shocked)mapChipDrawData[height][width].shocked -= 0.02f;
			if (mapChipDrawData[height][width].expEaseRate < 1.0f)mapChipDrawData[height][width].expEaseRate += 0.005f;

			// �u���b�N�ȊO�������珈�����΂��B
			bool blockFlag = (mapChipData[height][width] >= wallChipMemorySize.min && mapChipData[height][width] <= wallChipMemorySize.max);
			if (blockFlag)
			{
				// �X�N���[���ʂ���`�悷��ʒu�����߂�B
				const Vec2<float> drawPos = ScrollMgr::Instance()->Affect({ width * MAP_CHIP_SIZE,height * MAP_CHIP_SIZE });

				// ��ʊO��������`�悵�Ȃ��B
				if (drawPos.x < -DRAW_MAP_CHIP_SIZE || drawPos.x > WinApp::Instance()->GetWinSize().x + DRAW_MAP_CHIP_SIZE) continue;
				if (drawPos.y < -DRAW_MAP_CHIP_SIZE || drawPos.y > WinApp::Instance()->GetWinSize().y + DRAW_MAP_CHIP_SIZE) continue;


				vector<std::shared_ptr<MapChipAnimationData>>tmpAnimation = StageMgr::Instance()->animationData;
				int handle = -1;
				if (height < 0 || mapChipDrawData.size() <= height) continue;
				if (width < 0 || mapChipDrawData[height].size() <= width) continue;
				//�A�j���[�V�����t���O���L���Ȃ�A�j���[�V�����p�̏����s��
				if (mapChipDrawData[height][width].animationFlag)
				{
					int arrayHandle = mapChipDrawData[height][width].handle;
					++mapChipDrawData[height][width].interval;
					//�A�j���[�V�����̊Ԋu
					if (mapChipDrawData[height][width].interval % tmpAnimation[arrayHandle]->maxInterval == 0)
					{
						++mapChipDrawData[height][width].animationNum;
						mapChipDrawData[height][width].interval = 0;
					}
					//�A�j���[�V�����摜�̑����ɒB������ŏ��ɖ߂�
					if (tmpAnimation[arrayHandle]->handle.size() <= mapChipDrawData[height][width].animationNum)
					{
						mapChipDrawData[height][width].animationNum = 0;
					}
					//���������A�j���[�V�����̉摜����n��
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

	// �V�F�C�N�ʂ�ݒ�B
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

	//�X�N���[������ɂ��炷�p
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
		//�Q�[���X�^�[�g
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
	//�X�N���[������ɂ��炷�p
	responePos.x -= 25;
	responePos.y += 50;
	responeScrollPos = responePos;
	ScrollMgr::Instance()->Init(responeScrollPos, Vec2<float>(tmp[0].size() * MAP_CHIP_SIZE, tmp.size() * MAP_CHIP_SIZE), cameraBasePos);

	Camera::Instance()->Init();
	SuperiorityGauge::Instance()->Init();

	readyToStartRoundFlag = true;
	//�w�i�ɐ�
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

	// �X�e�[�W�̐؂�ւ�
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

	// �w�n�̔���
	//DeterminationOfThePosition();

	//���E���h�I�����o�J�n
	RoundFinishEffect(Loop);

	//���E���h�J�n���̉��o�J�n
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
	// �v���C���[�̍X�V����
	if (!roundFinishFlag)
	{
		if (roundChangeEffect.initGameFlag)
		{
			DebugParameter::Instance()->timer++;
			mapChipGenerator[DebugParameter::Instance()->generator]->Update();
		}

		// ���W��ۑ��B
		CharacterManager::Instance()->Left()->SavePrevFramePos();
		CharacterManager::Instance()->Right()->SavePrevFramePos();

		CharacterManager::Instance()->Left()->Update(tmpMapData, lineCenterPos, readyToStartRoundFlag);

		// �{�X�̍X�V����
		CharacterManager::Instance()->Right()->Update(tmpMapData, lineCenterPos, readyToStartRoundFlag);
	}

	CharacterAIData::Instance()->nowPos = CharacterManager::Instance()->Right()->pos;
	// �v���C���[�ƃ{�X�̈������荇���̏���
	Scramble();

	// �v���C���[�ƃ{�X�̓����蔻�菈��
	CharacterManager::Instance()->Left()->CheckHit(tmpMapData, lineCenterPos);
	CharacterManager::Instance()->Right()->CheckHit(tmpMapData, lineCenterPos);
	CharacterAIData::Instance()->prevPos = CharacterManager::Instance()->Right()->pos;

	miniMap.Update();
	screenEdgeEffect.Update();

	roundChangeEffect.Update();

	GameTimer::Instance()->Update();
	ScoreManager::Instance()->Update();

	// �V�F�C�N�ʂ̍X�V����
	ShakeMgr::Instance()->Update();

	SuperiorityGauge::Instance()->Update();

	// �X�^�����o�N���X�̍X�V����
	StunEffect::Instance()->Update();


	/*===== �����蔻�� =====*/

#pragma region �����蔻��

	//���e�ƉE�v���C���[�̔���
	for (int index = 0; index < CharacterManager::Instance()->Left()->GetBulletMgr().bullets.size(); ++index)
	{
		if (!CharacterManager::Instance()->Left()->GetBulletMgr().bullets[index].isActive)continue;

		std::shared_ptr<SphereCollision> bulCol = CharacterManager::Instance()->Left()->GetBulletMgr().bullets[index].bulletHitBox;
		bool hitFlag = BulletCollision::Instance()->CheckSphereAndSphere(*bulCol, CharacterManager::Instance()->Right()->GetBulletHitSphere());

		//����������Ă���&&�v���C���[�Ɣ�����������D���Q�[�W�̕΂肪�ς��A�e�͏����������
		if (hitFlag)
		{
			//SuperiorityGauge::Instance()->AddGauge(LEFT_TEAM, DebugParameter::Instance()->gaugeData->playerBulletAddGuaugeValue);
			CharacterManager::Instance()->Left()->GetBulletMgr().bullets[index].Init();
			CharacterManager::Instance()->Right()->Damage();
		}
	}

	//�E�e�ƍ��v���C���[�̔���
	static int DAMAGED_SE = AudioApp::Instance()->LoadAudio("resource/ChainCombat/sound/damaged.wav", 0.7f);
	for (int index = 0; index < CharacterManager::Instance()->Right()->GetBulletMgr().bullets.size(); ++index)
	{
		if (!CharacterManager::Instance()->Right()->GetBulletMgr().bullets[index].isActive)continue;

		std::shared_ptr<SphereCollision> bulCol = CharacterManager::Instance()->Right()->GetBulletMgr().bullets[index].bulletHitBox;
		bool hitFlag = BulletCollision::Instance()->CheckSphereAndSphere(*bulCol, CharacterManager::Instance()->Left()->GetBulletHitSphere());

		//����������Ă���&&�v���C���[�Ɣ�����������D���Q�[�W�̕΂肪�ς��A�e�͏����������
		if (hitFlag)
		{
			//SuperiorityGauge::Instance()->AddGauge(RIGHT_TEAM, DebugParameter::Instance()->gaugeData->enemyBulletAddGuaugeValue);
			CharacterManager::Instance()->Right()->GetBulletMgr().bullets[index].Init();
			CharacterManager::Instance()->Left()->Damage();
			//AudioApp::Instance()->PlayWave(DAMAGED_SE);
		}
	}

#pragma endregion


	// ���S�_���v�Z�B
	CalCenterPos();

	//	ScrollManager::Instance()->CalucurateScroll(prevLineCenterPos - lineCenterPos, lineCenterPos);
	ScrollMgr::Instance()->CalucurateScroll(prevLineCenterPos - lineCenterPos, lineCenterPos);

	// �X�N���[���ʂ̍X�V����
	//ScrollManager::Instance()->Update();
	Vec2<float> distance = CharacterManager::Instance()->Left()->pos - CharacterManager::Instance()->Right()->pos;
	Vec2<float>cPos = CharacterManager::Instance()->Left()->pos - distance / 2.0f;
	ScrollMgr::Instance()->Update(cPos);

	//�p�[�e�B�N���X�V
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



	// �D���Q�[�W���U��؂����g���K�[����̂Ƃ��ɃX�^�����o��L��������B
	if (SuperiorityGauge::Instance()->GetGaugeData(RIGHT_TEAM).overGaugeFlag && !SuperiorityGauge::Instance()->GetGaugeData(RIGHT_TEAM).prevOverGaugeFlag) {
		// �G�̗D���Q�[�W���U��؂����Ƃ������Ƃ́A�v���C���[�̗D���Q�[�W��0���Ƃ������ƁB
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

		// ���L�����̗\�����������B
		CharacterManager::Instance()->Right()->InitSwingLineSegmetn();
		CharacterManager::Instance()->Left()->InitSwingLineSegmetn();
	}
	if (SuperiorityGauge::Instance()->GetGaugeData(LEFT_TEAM).overGaugeFlag && !SuperiorityGauge::Instance()->GetGaugeData(LEFT_TEAM).prevOverGaugeFlag) {
		// �v���C���[�̗D���Q�[�W���U��؂����Ƃ������Ƃ́A�G�̗D���Q�[�W��0���Ƃ������ƁB
		/*StunEffect::Instance()->Activate(CharacterManager::Instance()->Right()->pos, Vec2<float>(1200, 0), RIGHT_TEAM);
		ResultTransfer::Instance()->rightBreakCount++;
		CharacterManager::Instance()->Right()->Break();
		CharacterManager::Instance()->Right()->InitVel();*/
		//�v���C���[����
		WinCounter::Instance()->RoundFinish(lineCenterPos, true, CharacterManager::Instance()->Right()->pos);
		CharacterManager::Instance()->Right()->OnKnockOut();
		roundFinishFlag = true;
		playerOrEnemeyWinFlag = true;
		gameStartFlag = false;

		screenEdgeEffect.LeftPlayerWin(120);

		// ���L�����̗\�����������B
		CharacterManager::Instance()->Right()->InitSwingLineSegmetn();
		CharacterManager::Instance()->Left()->InitSwingLineSegmetn();
	}

	CrashMgr::Instance()->Update();

	// �c�����X�V�B
	AfterImageMgr::Instance()->Update();

	// �N���b�V�����̉��o�̍X�V�����B
	CrashEffectMgr::Instance()->Update();

	countBlock.Update();

	// �X�^�~�i�A�C�e���̍X�V����
	if (!readyToStartRoundFlag) {
		StaminaItemMgr::Instance()->Update(playerHomeBase.GetRightUpPos(), enemyHomeBase.GetLeftDownPos());
	}

	// �X�^�~�i�A�C�e���̓����蔻�菈��
	int healAmount = StaminaItemMgr::Instance()->CheckHit(&CharacterManager::Instance()->Left()->pos, 30, 70, StaminaItem::CHARA_ID::LEFT, CharacterManager::Instance()->Left()->GetPilotPosPtr());
	CharacterManager::Instance()->Left()->HealStamina(healAmount);
	healAmount = StaminaItemMgr::Instance()->CheckHit(&CharacterManager::Instance()->Right()->pos, 90, 70, StaminaItem::CHARA_ID::RIGHT, CharacterManager::Instance()->Right()->GetPilotPosPtr());
	CharacterManager::Instance()->Right()->HealStamina(healAmount);


	if (!Camera::Instance()->Active()) {

		//�݂��̋����ŃJ�����̃Y�[������ς���B
		float distance = CharacterManager::Instance()->Left()->pos.Distance(CharacterManager::Instance()->Right()->pos);

		//�ő勗��
		const float MAX_ADD_ZOOM = 3000.0f;

		float zoomRate = 1.0f;
		float deadLine = 1200.0f;//���̋����ȉ��̓Y�[�����Ȃ�

		// ���E���L�тĂ�����B
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
		static const float ZOOM_OFFSET = -0.01f;		// �f�t�H���g�ŏ��������J�����������C���ɂ���B
		Camera::Instance()->zoom = 0.5f - zoomRate + ZOOM_OFFSET;

		// �J�����̃Y�[����0.27f�����ɂȂ�Ȃ��悤�ɂ���B
		float minZoomValue = 0.27f;
		if (Camera::Instance()->zoom < minZoomValue)
		{
			Camera::Instance()->zoom = minZoomValue;
		}
	}
	else {

		ScrollMgr::Instance()->lineCenterOffset = {};

	}


	// �G�L�������v���C���[�ɂ�����x�ߕt�����甽�Α��ɐ�����΂��@�\�B
	const float BOUNCE_DISTANCE = 300.0f; // ������x�̋���
	if (Vec2<float>(CharacterManager::Instance()->Left()->pos - CharacterManager::Instance()->Right()->pos).Length() <= BOUNCE_DISTANCE) {

		Vec2<float> charaDir = Vec2<float>(CharacterManager::Instance()->Right()->pos - CharacterManager::Instance()->Left()->pos).GetNormal();

		// ������΂���
		const float BOUNCE_POWER = 50.0f;
		CharacterManager::Instance()->Right()->bounceVel = charaDir * BOUNCE_POWER;

	}

}

void Game::Draw()
{
	int stageNum = SelectStage::Instance()->GetStageNum();
	int roomNum = SelectStage::Instance()->GetRoomNum();

	/*===== �`�揈�� =====*/
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

	// �X�^�~�i�A�C�e���̕`�揈��
	StaminaItemMgr::Instance()->Draw();

	static int CENTER_CHAIN_GRAPH = TexHandleMgr::LoadGraph("resource/ChainCombat/chain.png");
	static int PLAYER_CHAIN_GRAPH = TexHandleMgr::LoadGraph("resource/ChainCombat/chain_player.png");
	static int ENEMY_CHAIN_GRAPH = TexHandleMgr::LoadGraph("resource/ChainCombat/chain_enemy.png");
	static const int CHAIN_THICKNESS = 4;
	// �v���C���[�ƃ{�X�Ԃɐ���`��
	if (roundChangeEffect.initGameFlag)
	{
		//���v���C���[�`�����̃`�F�C��
		auto& left = CharacterManager::Instance()->Left();
		Vec2<float>leftLineCenterDir = (lineCenterPos - left->pos).GetNormal();
		Vec2<float>leftChainBorderPos = left->pos + leftLineCenterDir * left->addLineLength;	//�����`�F�C���ƍ��v���C���[�`�F�C���Ƃ̕ς���
		if (0.0f < left->addLineLength)
		{
			DrawFunc::DrawLine2DGraph(ScrollMgr::Instance()->Affect(left->pos), ScrollMgr::Instance()->Affect(leftChainBorderPos),
				TexHandleMgr::GetTexBuffer(PLAYER_CHAIN_GRAPH), CHAIN_THICKNESS * ScrollMgr::Instance()->zoom);
		}

		//�E�v���C���[�`�����̃`�F�C��
		auto& right = CharacterManager::Instance()->Right();
		Vec2<float>rightLineCenterDir = (lineCenterPos - right->pos).GetNormal();
		Vec2<float>rightChainBorderPos = right->pos + rightLineCenterDir * right->addLineLength;	//�����`�F�C���ƉE�v���C���[�`�F�C���Ƃ̕ς���
		if (0.0f < right->addLineLength)
		{
			DrawFunc::DrawLine2DGraph(ScrollMgr::Instance()->Affect(right->pos), ScrollMgr::Instance()->Affect(rightChainBorderPos),
				TexHandleMgr::GetTexBuffer(ENEMY_CHAIN_GRAPH), CHAIN_THICKNESS * ScrollMgr::Instance()->zoom);
		}

		float charaDistance = (CharacterManager::Instance()->Left()->pos - CharacterManager::Instance()->Right()->pos).Length();
		//�����`�F�C��
		if (charaDistance < CharacterManager::Instance()->Left()->LINE_LENGTH * 2.0f) {

			// ����l���Z��������B
			DrawFunc::DrawLine2DGraph(ScrollMgr::Instance()->Affect(CharacterManager::Instance()->Left()->pos), ScrollMgr::Instance()->Affect(CharacterManager::Instance()->Right()->pos),
				TexHandleMgr::GetTexBuffer(CENTER_CHAIN_GRAPH), CHAIN_THICKNESS * ScrollMgr::Instance()->zoom);

		}
		else {

			DrawFunc::DrawLine2DGraph(ScrollMgr::Instance()->Affect(leftChainBorderPos), ScrollMgr::Instance()->Affect(rightChainBorderPos),
				TexHandleMgr::GetTexBuffer(CENTER_CHAIN_GRAPH), CHAIN_THICKNESS * ScrollMgr::Instance()->zoom);

		}

		// �����̒��S�ɉ~��`��
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
		// �c����`��
		AfterImageMgr::Instance()->Draw();
		CharacterManager::Instance()->Left()->Draw(readyToStartRoundFlag);
		CharacterManager::Instance()->Right()->Draw(readyToStartRoundFlag);
	}

	// �N���b�V�����̉��o�̕`�揈���B
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

	// �v���C���[�ƃ{�X�Ԃɐ���`��
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
	/*===== �������荇���̏��� =====*/
	// �O�t���[���̐��̒��S���W��ۑ�
	prevLineCenterPos = lineCenterPos;

	//�ǂ���������Ȃ��Ƃ��������Ȃ�
	if (!(CharacterManager::Instance()->Left()->GetCanMove() || CharacterManager::Instance()->Right()->GetCanMove()))return;

	Vec2<float> leftVelGauge;
	Vec2<float> rightVelGauge;

	// �E���̃L������������ԃ}�b�v�`�b�v�ɓ��������ۂ̈ړ��ʂ̏������s���B
	CharacterManager::Instance()->Right()->pos += CharacterManager::Instance()->Right()->bounceVel;
	CharacterManager::Instance()->Right()->addLineLength += CharacterManager::Instance()->Right()->bounceVel.Length();

	// �ړ��ʂ��擾�B �D���Q�[�W�͂����ōX�V�B
	double leftVel = CharacterManager::Instance()->Left()->vel.Length() * SlowMgr::Instance()->slowAmount;
	leftVelGauge = CharacterManager::Instance()->Left()->vel * SlowMgr::Instance()->slowAmount;
	double rightVel = CharacterManager::Instance()->Right()->vel.Length() * SlowMgr::Instance()->slowAmount;
	rightVelGauge = CharacterManager::Instance()->Right()->vel * SlowMgr::Instance()->slowAmount;
	double subVel = fabs(fabs(leftVel) - fabs(rightVel));

	// [�X�^�����o��] �͈ړ������Ȃ��B ���񒣂蒆�̏ꍇ�́A�ǂ���ɂ���ړ��ʂ�����Ȃ�0�ɋ߂��̂ňړ������Ă���肪�Ȃ��B
	if (!(StunEffect::Instance()->isActive)) {
		// �U��񂳂ꒆ����Ȃ�������ړ�������B
		if (!CharacterManager::Instance()->Right()->GetNowSwing()) {
			CharacterManager::Instance()->Left()->pos += leftVelGauge;
		}
		// �U��񂳂ꒆ����Ȃ�������ړ�������B
		if (!CharacterManager::Instance()->Left()->GetNowSwing() && !CharacterManager::Instance()->Left()->isStopPartner) {
			CharacterManager::Instance()->Right()->pos += rightVelGauge;
		}
		else if (CharacterManager::Instance()->Left()->isStopPartner) {
			CharacterManager::Instance()->Right()->pos += leftVelGauge;
		}
	}

	// �����̒���
	float charaLength = 0;
	float LINE = CharacterInterFace::LINE_LENGTH * 2 + (CharacterManager::Instance()->Left()->addLineLength + CharacterManager::Instance()->Right()->addLineLength);

	// �ǂ��炩�����񒣂��Ă��邩�B
	bool isHoldNow = CharacterManager::Instance()->Left()->isHold || CharacterManager::Instance()->Right()->isHold;

	// �ǂ��炩���U��񂵂Ă��邩�B
	bool isSwingNow = CharacterManager::Instance()->Left()->GetNowSwing() || CharacterManager::Instance()->Right()->GetNowSwing();


	// �ǂ���̈ړ��ʂ����������擾�B�ǂ���������ꍇ�͏������΂��B
	if (leftVelGauge.Length() < rightVelGauge.Length()) {

		// �E���̂ق����ړ��ʂ��傫��������B
		// �E���̃L�����𒆐S�Ƃ����ʒu�ɍ����̃L�����������Ă���B

		// ���������߂�B
		charaLength = Vec2<float>(CharacterManager::Instance()->Left()->pos).Distance(CharacterManager::Instance()->Right()->pos);

		// �����̃L�������E���̃L�����̕��Ɉړ�������B
		if (LINE < charaLength) {

			// �����߂���
			float moveLength = charaLength - LINE;

			// �����߂�����
			Vec2<float> moveDir = Vec2<float>(CharacterManager::Instance()->Right()->pos - CharacterManager::Instance()->Left()->pos);
			moveDir.Normalize();

			// �����߂��B
			CharacterManager::Instance()->Left()->pos += moveDir * Vec2<float>(moveLength, moveLength);

			// ���������蔻�肾������
			if (CharacterManager::Instance()->Left()->GetStackFlag()) {

				CharacterManager::Instance()->Left()->addLineLength += moveLength;
				//CharacterManager::Instance()->Right()->pos += -moveDir * Vec2<float>(moveLength, moveLength);

				// �����������Ă���ꍇ�͍X�ɉ����߂��B(�ǃY����\�����邽��)
				if (0 < CharacterManager::Instance()->Left()->addLineLength) {
					CharacterManager::Instance()->Left()->pos += moveDir * Vec2<float>(moveLength, moveLength);
				}

			}

		}

	}
	else if (rightVelGauge.Length() < leftVelGauge.Length()) {

		// �����̂ق����ړ��ʂ��傫��������B
		// �����̃L�����𒆐S�Ƃ����ʒu�ɉE���̃L�����������Ă���B

		// ���������߂�B
		charaLength = Vec2<float>(CharacterManager::Instance()->Left()->pos).Distance(CharacterManager::Instance()->Right()->pos);

		// �{�X���v���C���[�̕��Ɉړ�������B
		if (LINE < charaLength) {

			// �����߂���
			float moveLength = charaLength - LINE;

			// �����߂�����
			Vec2<float> moveDir = Vec2<float>(CharacterManager::Instance()->Left()->pos - CharacterManager::Instance()->Right()->pos);
			moveDir.Normalize();

			// �����߂��B
			CharacterManager::Instance()->Right()->pos += moveDir * Vec2<float>(moveLength, moveLength);


			// ���������蔻�肾������
			if (CharacterManager::Instance()->Right()->GetStackFlag()) {

				CharacterManager::Instance()->Right()->addLineLength += moveLength;
				//CharacterManager::Instance()->Left()->pos += -moveDir * Vec2<float>(moveLength, moveLength);

				// �����������Ă���ꍇ�͍X�ɉ����߂��B(�ǃY����\�����邽��)
				if (0 < CharacterManager::Instance()->Right()->addLineLength) {
					CharacterManager::Instance()->Right()->pos += moveDir * Vec2<float>(moveLength, moveLength);
				}

			}

		}

	}

	const float ADD_LINE_LENGTH_SUB_AMOUNT = 5.0f;

	// �ǂ���̃L�����������������Ă��邩
	bool isBothStuck = CharacterManager::Instance()->Right()->GetStackFlag() && CharacterManager::Instance()->Left()->GetStackFlag();
	// ���������蔻�肶��Ȃ������炾�񂾂�Z������B
	Vec2<float> movedVel = (CharacterManager::Instance()->Right()->pos - CharacterManager::Instance()->Right()->prevPos);
	movedVel += (CharacterManager::Instance()->Left()->pos - CharacterManager::Instance()->Left()->prevPos);
	// �E���̕R�̏���
	if (!isBothStuck && 0 < CharacterManager::Instance()->Right()->addLineLength && !isSwingNow) {

		// ������ 0�����ɂȂ�Ȃ��悤�ɂ��邽�߁B
		float subAmount = ADD_LINE_LENGTH_SUB_AMOUNT;
		if (CharacterManager::Instance()->Right()->addLineLength < ADD_LINE_LENGTH_SUB_AMOUNT) {
			subAmount = CharacterManager::Instance()->Right()->addLineLength;
		}

		// �ړ����͈����Ȃ��B
		if (ADD_LINE_LENGTH_SUB_AMOUNT < CharacterManager::Instance()->Right()->vel.Length()) {
			subAmount = 0;
		}

		// �R���Z�ʂ��ւ炷�B
		CharacterManager::Instance()->Right()->addLineLength -= subAmount;

		// �L�����N�^�[��R���Z�ʂ������������Ɉړ�������B
		Vec2<float> charaDir = (CharacterManager::Instance()->Left()->pos - CharacterManager::Instance()->Right()->pos).GetNormal();
		CharacterManager::Instance()->Right()->vel += charaDir * subAmount / 10.0f;

		// ���������ړ�������B
		float charaLength = (CharacterManager::Instance()->Left()->pos - CharacterManager::Instance()->Right()->pos).Length();
		// [���̒���] �� [�������� * 2] + [���̕R�̒���] �ȏゾ�����珈�����s���B
		if (CharacterInterFace::LINE_LENGTH * 2.0f + CharacterManager::Instance()->Left()->addLineLength < charaLength) {

			// �����Ă�����
			if (1.0f < movedVel.Length()) {
			}
			// �E���������������Ă�����B
			else if (CharacterManager::Instance()->Right()->GetStackFlag()) {
				// �E���������������Ă���Ƃ��͑���ɍ����𓮂����B
				CharacterManager::Instance()->Left()->pos += (CharacterManager::Instance()->Right()->pos - CharacterManager::Instance()->Left()->pos).GetNormal() * subAmount;
			}
			else {
				CharacterManager::Instance()->Right()->pos += (CharacterManager::Instance()->Left()->pos - CharacterManager::Instance()->Right()->pos).GetNormal() * subAmount;
			}
		}

	}

	if (CharacterManager::Instance()->Right()->addLineLength < 0) CharacterManager::Instance()->Right()->addLineLength = 0;


	// �����̕R�̏���
	if (!isBothStuck && 0 < CharacterManager::Instance()->Left()->addLineLength && !isSwingNow) {

		// ������ 0�����ɂȂ�Ȃ��悤�ɂ��邽�߁B
		float subAmount = ADD_LINE_LENGTH_SUB_AMOUNT;
		if (CharacterManager::Instance()->Left()->addLineLength < ADD_LINE_LENGTH_SUB_AMOUNT) {
			subAmount = CharacterManager::Instance()->Left()->addLineLength;
		}

		// �ړ����͈����Ȃ��B
		if (ADD_LINE_LENGTH_SUB_AMOUNT < CharacterManager::Instance()->Left()->vel.Length()) {
			subAmount = 0;
		}


		// �R���Z�ʂ��ւ炷�B
		CharacterManager::Instance()->Left()->addLineLength -= subAmount;

		// �L�����N�^�[��R���Z�ʂ������������Ɉړ�������B
		Vec2<float> charaDir = (CharacterManager::Instance()->Right()->pos - CharacterManager::Instance()->Left()->pos).GetNormal();
		CharacterManager::Instance()->Left()->vel += charaDir * subAmount / 10.0f;

		// ���������ړ�������B
		float charaLength = (CharacterManager::Instance()->Left()->pos - CharacterManager::Instance()->Right()->pos).Length();
		// [���̒���] �� [�������� * 2] + [���̕R�̒���] �ȏゾ�����珈�����s���B
		if (CharacterInterFace::LINE_LENGTH * 2.0f + CharacterManager::Instance()->Right()->addLineLength < charaLength) {

			// �����Ă�����
			if (1.0f < movedVel.Length()) {
			}
			// �����������������Ă�����B
			else if (CharacterManager::Instance()->Left()->GetStackFlag()) {
				// �����������������Ă���Ƃ��͑���ɉE���𓮂����B
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

	/*===== ���S�_�����߂鏈�� =====*/

	// �{����Scramble�̈�Ԃ�����ɓ���Ă��������Ȃ�ł����A�����߂���������ɌĂԕK�v���o�Ă����̂Ŋ֐��ŕ����܂����B

	auto& left = CharacterManager::Instance()->Left();
	auto& right = CharacterManager::Instance()->Right();

	// �ړ��ʂɉ����Ė{������ׂ������ɂ���B
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

	// ���݂̋��������Z����Ă���R�̒������Z��������AaddLineValue��Z������B
	float nowLength = (left->pos - right->pos).Length();
	if (nowLength < CharacterInterFace::LINE_LENGTH * 2.0f + left->addLineLength + right->addLineLength) {

		// �I�[�o�[���Ă���ʂ����߂�B
		float overLength = (CharacterInterFace::LINE_LENGTH * 2.0f + left->addLineLength + right->addLineLength) - nowLength;
		// �I�[�o�[���Ă���ʂ̔��������߂�B
		float halfOverLength = overLength / 2.0f;

		// �܂��͍�������Z������B

		left->addLineLength -= halfOverLength;

		// 0�ȉ��ɂȂ�����A���̗ʂ�ۑ����Ă����B
		float leftOverLength = 0;
		if (left->addLineLength < 0) {

			leftOverLength = fabs(left->addLineLength);

			// �R�̒�����0�����ɂȂ�Ȃ��悤�ɂ���B
			left->addLineLength = 0;

		}

		// ���ɉE����Z������B

		right->addLineLength -= halfOverLength + leftOverLength;

		// �R�̒�����0�����ɂȂ�Ȃ��悤�ɂ���B
		if (right->addLineLength < 0) {

			right->addLineLength = 0;

		}
	}

	// �R�̒��S�_���v�Z
	{
		float distance = (CharacterManager::Instance()->Right()->pos - CharacterManager::Instance()->Left()->pos).Length();
		Vec2<float> bossDir = CharacterManager::Instance()->Right()->pos - CharacterManager::Instance()->Left()->pos;
		bossDir.Normalize();

		// �{�X�ƃv���C���[�Ԃ̋������K��l�ȉ���������
		//if (distance < CharacterInterFace::LINE_LENGTH + CharacterInterFace::LINE_LENGTH) {
		//	// ����l�ȉ���������c�q��������h�����߂ɁA��_�Ԃ̐^�񒆂̍��W�ɂ���B
		//	lineCenterPos = CharacterManager::Instance()->Left()->pos + bossDir * Vec2<float>(distance / 2.0f, distance / 2.0f);
		//}
		//else {
			// �K��l�ȏゾ�����畁�ʂɏꏊ�����߂�B

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
	//�}�b�v�̐؂�ւ�
	const bool left = UsersInput::Instance()->KeyOnTrigger(DIK_LEFT) || UsersInput::Instance()->ControllerOnTrigger(0, DPAD_LEFT);
	const bool right = UsersInput::Instance()->KeyOnTrigger(DIK_RIGHT) || UsersInput::Instance()->ControllerOnTrigger(0, DPAD_RIGHT);

	const bool enableToSelectRoomFlag = 0 < debugStageData[1];
	const bool enableToSelectRoomFlag2 = debugStageData[1] < StageMgr::Instance()->GetMaxRoomNumber(debugStageData[0]) - 1;

	//�������ԍ��ɐ؂�ւ�
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

	//�v���C���[�w�n�ƓG�̔���
	if (playerHomeBase.Collision(CharacterManager::Instance()->Right()->GetAreaHitBox()) && !roundFinishFlag && !readyToStartRoundFlag)
	{
		//�v���C���[����
		WinCounter::Instance()->RoundFinish(lineCenterPos, true, CharacterManager::Instance()->Right()->pos);
		CharacterManager::Instance()->Right()->OnKnockOut();
		roundFinishFlag = true;
		playerOrEnemeyWinFlag = true;
		gameStartFlag = false;

		screenEdgeEffect.LeftPlayerWin(120);

		// ���L�����̗\�����������B
		CharacterManager::Instance()->Right()->InitSwingLineSegmetn();
		CharacterManager::Instance()->Left()->InitSwingLineSegmetn();

	}

	//�G�w�n�ƃv���C���[�̔���
	if (enemyHomeBase.Collision(CharacterManager::Instance()->Left()->GetAreaHitBox()) && !roundFinishFlag && !readyToStartRoundFlag)
	{
		//�G����
		WinCounter::Instance()->RoundFinish(lineCenterPos, false, CharacterManager::Instance()->Left()->pos);
		CharacterManager::Instance()->Left()->OnKnockOut();
		roundFinishFlag = true;
		playerOrEnemeyWinFlag = false;
		gameStartFlag = false;

		areaHitColor = Color(255, 0, 0, 255);
		playerHitColor = Color(255, 0, 0, 255);

		screenEdgeEffect.RightPlayerWin(120);

		// ���L�����̗\�����������B
		CharacterManager::Instance()->Right()->InitSwingLineSegmetn();
		CharacterManager::Instance()->Left()->InitSwingLineSegmetn();
	}

}

void Game::RoundStartEffect(const bool& Loop, const RoomMapChipArray& tmpMapData)
{

	//���E���h�J�n���̉��o�J�n
	if (readyToStartRoundFlag)
	{
		roundChangeEffect.Start(WinCounter::Instance()->GetNowRound(), playerOrEnemeyWinFlag);
		Vec2<float>winSize;
		winSize.x = static_cast<float>(WinApp::Instance()->GetWinSize().x);
		winSize.y = static_cast<float>(WinApp::Instance()->GetWinSize().y);

		//Vec2<float> responePos = Vec2<float>(((mapData[0].size() * MAP_CHIP_SIZE) / 2.0f) - 100.0f, (mapData.size() * MAP_CHIP_SIZE) / 2.0f);
		//responePos.y -= -cameraBasePos.y * 2.0f;
		ScrollMgr::Instance()->Warp(responeScrollPos);

		//�v���C���[�ƓG�̍��W������
		if (roundChangeEffect.readyToInitFlag && !roundChangeEffect.initGameFlag)
		{
			roundChangeEffect.initGameFlag = true;
			if (!AudioApp::Instance()->NowPlay(bgm))
			{
				AudioApp::Instance()->ChangeVolume(bgm, 0.1f);
				//AudioApp::Instance()->PlayWave(bgm, true);
			}
		}

		//�o�ꉉ�o
		if (roundChangeEffect.initGameFlag)
		{
			CharacterManager::Instance()->Left()->Appear();
			CharacterManager::Instance()->Right()->Appear();
			if (CharacterManager::Instance()->Left()->CompleteAppear() && CharacterManager::Instance()->Right()->CompleteAppear())	//�ǂ���̃L�������o�ꉉ�o����
			{
				//�Q�[���X�^�[�g
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

	//���E���h�I�����o�J�n
	if (roundFinishFlag)
	{
		//�����Ȃ�����
		CharacterManager::Instance()->Left()->SetCanMove(false);
		CharacterManager::Instance()->Right()->SetCanMove(false);
		CharacterManager::Instance()->Left()->SetHitCheck(false);
		CharacterManager::Instance()->Right()->SetHitCheck(false);

		//���Ԍv���X�g�b�v
		GameTimer::Instance()->SetInterruput(true);

		//�������J�E���g���o
		if (!WinCounter::Instance()->GetNowAnimation())
		{
			//�ǂ��炩���R���Ƃ�����Q�[���I��
			if (WinCounter::Instance()->GetGameFinish() && !Loop)
			{
				ResultTransfer::Instance()->resultScore = ScoreManager::Instance()->GetScore();
				if (WinCounter::Instance()->Winner() == LEFT_TEAM)ResultTransfer::Instance()->winner = CharacterManager::Instance()->Left()->GetCharacterName();
				else ResultTransfer::Instance()->winner = CharacterManager::Instance()->Right()->GetCharacterName();
				turnResultScene = true;
			}
			//���̃��E���h��
			else
			{
				//���K���[�h
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
