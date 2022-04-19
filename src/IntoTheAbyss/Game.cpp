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
	SizeData eventChipMemorySize = StageMgr::Instance()->GetMapChipSizeData(MAPCHIP_TYPE_EVENT);
	SizeData wallChipMemorySize = StageMgr::Instance()->GetMapChipSizeData(MAPCHIP_TYPE_STATIC_BLOCK);
	SizeData doorChipMemorySize = StageMgr::Instance()->GetMapChipSizeData(MAPCHIP_TYPE_DOOR);


	// �}�b�v�`�b�v�̏c�̗v�f�����擾�B
	const int HEIGHT = mapChipData.size();
	for (int height = 0; height < HEIGHT; ++height) {

		// �}�b�v�`�b�v�̉��̗v�f�����擾�B
		const int WIDTH = mapChipData[height].size();
		for (int width = 0; width < WIDTH; ++width) {

			// �u���b�N�ȊO�������珈�����΂��B
			bool blockFlag = (mapChipData[height][width] >= 1 && mapChipData[height][width] <= 9);
			bool doorFlag = (mapChipData[height][width] >= doorChipMemorySize.min && mapChipData[height][width] <= doorChipMemorySize.max);
			bool eventFlag = (mapChipData[height][width] >= eventChipMemorySize.min && mapChipData[height][width] <= eventChipMemorySize.max);
			bool thownFlag = mapChipData[height][width] == 41;


			if (blockFlag || doorFlag || eventFlag || thownFlag)
			{
				// �X�N���[���ʂ���`�悷��ʒu�����߂�B
				const Vec2<float> drawPos = ScrollMgr::Instance()->Affect({ width * MAP_CHIP_SIZE,height * MAP_CHIP_SIZE });

				// ��ʊO��������`�悵�Ȃ��B
				if (drawPos.x < -DRAW_MAP_CHIP_SIZE || drawPos.x > WinApp::Instance()->GetWinSize().x + DRAW_MAP_CHIP_SIZE) continue;
				if (drawPos.y < -DRAW_MAP_CHIP_SIZE || drawPos.y > WinApp::Instance()->GetWinSize().y + DRAW_MAP_CHIP_SIZE) continue;


				vector<MapChipAnimationData*>tmpAnimation = StageMgr::Instance()->animationData;
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

	AudioApp::Instance()->StopWave(bgm);

	int stageNum = STAGE_NUM;
	int roomNum = ROOM_NUM;

	SuperiorityGauge::Instance()->Init();

	FaceIcon::Instance()->Init();

	mapData = StageMgr::Instance()->GetMapChipData(stageNum, roomNum);
	mapChipDrawData = StageMgr::Instance()->GetMapChipDrawBlock(stageNum, roomNum);

	// �V�F�C�N�ʂ�ݒ�B
	ShakeMgr::Instance()->Init();
	ParticleMgr::Instance()->Init();

	//���u���b�N
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
	CharacterManager::Instance()->CharactersInit(responePos);

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

	//�w�i�ɐ�
	BackGround::Instance()->Init(GetStageSize());

	GameTimer::Instance()->Init({}, 120, {}, {});
	ScoreManager::Instance()->Init();


}

void Game::Init()
{
	WinCounter::Instance()->Reset();

	turnResultScene = false;

	CharacterManager::Instance()->CharactersGenerate();

	InitGame(0, 0);
}

void Game::Update()
{
	//ScrollMgr::Instance()->zoom = ViewPort::Instance()->zoomRate;

#pragma region �X�e�[�W�̐؂�ւ�
	const bool enableToSelectStageFlag = 0 < debugStageData[0];
	const bool enableToSelectStageFlag2 = debugStageData[0] < StageMgr::Instance()->GetMaxStageNumber() - 1;
	//�}�b�v�̐؂�ւ�
	//if (Input::isKeyTrigger(KEY_INPUT_UP) && enableToSelectStageFlag2 && nowSelectNum == 0)
	const bool up = UsersInput::Instance()->KeyOnTrigger(DIK_UP) || UsersInput::Instance()->ControllerOnTrigger(0, DPAD_UP);
	const bool down = UsersInput::Instance()->KeyOnTrigger(DIK_DOWN) || UsersInput::Instance()->ControllerOnTrigger(0, DPAD_DOWN);
	const bool left = UsersInput::Instance()->KeyOnTrigger(DIK_LEFT) || UsersInput::Instance()->ControllerOnTrigger(0, DPAD_LEFT);
	const bool right = UsersInput::Instance()->KeyOnTrigger(DIK_RIGHT) || UsersInput::Instance()->ControllerOnTrigger(0, DPAD_RIGHT);

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
	//�����̐؂�ւ�
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

	//�������ԍ��ɐ؂�ւ�
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

	const bool done = UsersInput::Instance()->KeyOnTrigger(DIK_RETURN) || UsersInput::Instance()->ControllerOnTrigger(0, A);
	if (done)
	{
		SelectStage::Instance()->SelectStageNum(debugStageData[0]);
		SelectStage::Instance()->SelectRoomNum(debugStageData[1]);
		SelectStage::Instance()->resetStageFlag = true;
		//mapData = StageMgr::Instance()->GetMapChipData(debugStageData[0], debugStageData[1]);
	}
#pragma endregion

	const bool resetInput = UsersInput::Instance()->KeyOnTrigger(DIK_SPACE) || UsersInput::Instance()->ControllerOnTrigger(0, BACK);
	if (resetInput)
	{
		SelectStage::Instance()->resetStageFlag = true;
		//player.isDead = true;
		//sceneBlackFlag = true;
		//sceneChangeDeadFlag = player.isDead;
	}


	//�v���C���[�w�n�ƓG�̔���
	if (playerHomeBase->Collision(CharacterManager::Instance()->Right()->GetAreaHitBox()) && !roundFinishFlag && !readyToStartRoundFlag)
	{
		//�v���C���[����
		WinCounter::Instance()->RoundFinish(lineCenterPos, true);
		roundFinishFlag = true;
		playerOrEnemeyWinFlag = true;
		gameStartFlag = false;
	}

	//�G�w�n�ƃv���C���[�̔���
	if (enemyHomeBase->Collision(CharacterManager::Instance()->Left()->GetAreaHitBox()) && !roundFinishFlag && !readyToStartRoundFlag)
	{
		//�G����
		WinCounter::Instance()->RoundFinish(lineCenterPos, false);
		roundFinishFlag = true;
		playerOrEnemeyWinFlag = false;
		gameStartFlag = false;

		areaHitColor = Color(255, 0, 0, 255);
		playerHitColor = Color(255, 0, 0, 255);
	}
	else if (!roundFinishFlag)
	{
		areaHitColor = Color(255, 255, 255, 255);
		playerHitColor = Color(255, 255, 255, 255);
	}

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
			if (WinCounter::Instance()->GetGameFinish())
			{
				ResultTransfer::Instance()->resultScore = ScoreManager::Instance()->GetScore();
				turnResultScene = true;
			}
			//���̃��E���h��
			else
			{
				++roundTimer;

				if (60 <= roundTimer)
				{
					readyToStartRoundFlag = true;
					roundFinishFlag = false;
					InitGame(0, 0);
				}
			}
		}
	}

	//���E���h�J�n���̉��o�J�n
	if (readyToStartRoundFlag)
	{
		roundChangeEffect.Start(WinCounter::Instance()->GetNowRound(), playerOrEnemeyWinFlag);
		Vec2<float>winSize;
		winSize.x = static_cast<float>(WinApp::Instance()->GetWinSize().x);
		winSize.y = static_cast<float>(WinApp::Instance()->GetWinSize().y);

		Vec2<float> responePos = Vec2<float>(((mapData[0].size() * MAP_CHIP_SIZE) / 2.0f) - 100.0f, (mapData.size() * MAP_CHIP_SIZE) / 2.0f);
		responePos.y -= -cameraBasePos.y * 2.0f;
		ScrollMgr::Instance()->Warp(responePos);

		//�v���C���[�ƓG�̍��W������
		if (roundChangeEffect.readyToInitFlag && !roundChangeEffect.initGameFlag)
		{
			roundChangeEffect.initGameFlag = true;
			AudioApp::Instance()->PlayWave(bgm, true);
		}

		//�o�ꉉ�o
		if (roundChangeEffect.initGameFlag)
		{
			bool leftAppear = CharacterManager::Instance()->Left()->Appear();
			bool rightApperar = CharacterManager::Instance()->Right()->Appear();
			if (leftAppear && rightApperar)	//�ǂ���̃L�������o�ꉉ�o����
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

		float size = (mapData[0].size() * MAP_CHIP_SIZE) - 400.0f;
		miniMap.Init(size);
	}

	miniMap.CalucurateCurrentPos(lineCenterPos);

	// �v���C���[�̍X�V����
	CharacterManager::Instance()->Left()->Update(mapData, lineCenterPos);

	// �{�X�̍X�V����
	CharacterManager::Instance()->Right()->Update(mapData, lineCenterPos);

	// �v���C���[�ƃ{�X�̈������荇���̏���
	Scramble();

	// �v���C���[�ƃ{�X�̓����蔻�菈��
	CharacterManager::Instance()->Left()->CheckHit(mapData, lineCenterPos);
	CharacterManager::Instance()->Right()->CheckHit(mapData, lineCenterPos);

	miniMap.Update();

	roundChangeEffect.Update();

	GameTimer::Instance()->Update();
	ScoreManager::Instance()->Update();

	//	ScrollManager::Instance()->CalucurateScroll(prevLineCenterPos - lineCenterPos, lineCenterPos);
	ScrollMgr::Instance()->CalucurateScroll(prevLineCenterPos - lineCenterPos, lineCenterPos);

	// �X�N���[���ʂ̍X�V����
	//ScrollManager::Instance()->Update();
	ScrollMgr::Instance()->Update();

	// �V�F�C�N�ʂ̍X�V����
	ShakeMgr::Instance()->Update();

	// �U��񂵊Ǘ��N���X�̍X�V����
	//SwingMgr::Instance()->Update(player.centerPos, boss.pos, lineLengthBoss + lineLengthPlayer + addLineLengthBoss + addLineLengthPlayer);

#pragma region �e�M�~�b�N�̍X�V����

	SuperiorityGauge::Instance()->Update();

	// �X�^�����o�N���X�̍X�V����
	StunEffect::Instance()->Update();

#pragma endregion


	/*===== �����蔻�� =====*/

#pragma region �����蔻��

	//���e�ƉE�v���C���[�̔���
	auto& leftBulMgr = CharacterManager::Instance()->Left()->GetBulletMgr();
	for (int index = 0; index < leftBulMgr.bullets.size(); ++index)
	{
		auto& bul = leftBulMgr.bullets[index];
		if (!bul.isActive)continue;

		std::shared_ptr<SphereCollision> bulCol = bul.bulletHitBox;
		bool hitFlag = BulletCollision::Instance()->CheckSphereAndSphere(*bulCol, CharacterManager::Instance()->Right()->GetBulletHitSphere());

		//����������Ă���&&�v���C���[�Ɣ�����������D���Q�[�W�̕΂肪�ς��A�e�͏����������
		if (hitFlag)
		{
			SuperiorityGauge::Instance()->AddGauge(LEFT_TEAM, DebugParameter::Instance()->gaugeData->playerBulletAddGuaugeValue);
			bul.Init();
			CharacterManager::Instance()->Right()->Damage();
		}
	}

	//�E�e�ƍ��v���C���[�̔���
	auto rightBulMgr = CharacterManager::Instance()->Right()->GetBulletMgr();
	for (int index = 0; index < rightBulMgr.bullets.size(); ++index)
	{
		auto& bul = rightBulMgr.bullets[index];
		if (!bul.isActive)continue;

		std::shared_ptr<SphereCollision> bulCol = bul.bulletHitBox;
		bool hitFlag = BulletCollision::Instance()->CheckSphereAndSphere(*bulCol, CharacterManager::Instance()->Left()->GetBulletHitSphere());

		//����������Ă���&&�v���C���[�Ɣ�����������D���Q�[�W�̕΂肪�ς��A�e�͏����������
		if (hitFlag)
		{
			SuperiorityGauge::Instance()->AddGauge(RIGHT_TEAM, DebugParameter::Instance()->gaugeData->enemyBulletAddGuaugeValue);
			bul.Init();
			CharacterManager::Instance()->Left()->Damage();
		}
	}

#pragma endregion

	//�p�[�e�B�N���X�V
	ParticleMgr::Instance()->Update();

	BackGround::Instance()->Update();
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
	if (SuperiorityGauge::Instance()->GetRightGaugeData().overGaugeFlag && !SuperiorityGauge::Instance()->GetRightGaugeData().prevOverGaugeFlag) {
		// �G�̗D���Q�[�W���U��؂����Ƃ������Ƃ́A�v���C���[�̗D���Q�[�W��0���Ƃ������ƁB
		StunEffect::Instance()->Activate(CharacterManager::Instance()->Left()->pos, Vec2<float>(0, 0), LEFT_TEAM);
		ResultTransfer::Instance()->leftBreakCount++;
		CharacterManager::Instance()->Left()->Break();
	}
	if (SuperiorityGauge::Instance()->GetLeftGaugeData().overGaugeFlag && !SuperiorityGauge::Instance()->GetLeftGaugeData().prevOverGaugeFlag) {
		// �v���C���[�̗D���Q�[�W���U��؂����Ƃ������Ƃ́A�G�̗D���Q�[�W��0���Ƃ������ƁB
		StunEffect::Instance()->Activate(CharacterManager::Instance()->Right()->pos, Vec2<float>(1200, 0), RIGHT_TEAM);
		ResultTransfer::Instance()->rightBreakCount++;
		CharacterManager::Instance()->Right()->Break();
	}

	CrashMgr::Instance()->Update();
}

void Game::Draw(std::weak_ptr<RenderTarget>EmissiveMap)
{
	int stageNum = SelectStage::Instance()->GetStageNum();
	int roomNum = SelectStage::Instance()->GetRoomNum();

	/*===== �`�揈�� =====*/
	BackGround::Instance()->Draw();


	mapChipDrawData = StageMgr::Instance()->GetMapChipDrawBlock(stageNum, roomNum);
	DrawMapChip(mapData, mapChipDrawData, stageNum, roomNum);

	for (int i = 0; i < thornBlocks.size(); ++i)
	{
		thornBlocks[i]->Draw();
	}

	playerHomeBase->Draw();
	enemyHomeBase->Draw();

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

		//�����`�F�C��
		DrawFunc::DrawLine2DGraph(ScrollMgr::Instance()->Affect(leftChainBorderPos), ScrollMgr::Instance()->Affect(rightChainBorderPos),
			TexHandleMgr::GetTexBuffer(CENTER_CHAIN_GRAPH), CHAIN_THICKNESS * ScrollMgr::Instance()->zoom);

		Vec2<float> bossPlayerDir = CharacterManager::Instance()->Left()->pos - CharacterManager::Instance()->Right()->pos;
		bossPlayerDir.Normalize();
		Vec2<float> bossDefLength = CharacterManager::Instance()->Right()->pos + bossPlayerDir * CharacterManager::Instance()->Right()->addLineLength;

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
		//DrawFunc::DrawLine2DGraph(ScrollMgr::Instance()->Affect(CharacterManager::Instance()->Right()->pos), ScrollMgr::Instance()->Affect(lineCenterPos),
		//	TexHandleMgr::GetTexBuffer(CENTER_CHAIN_GRAPH), CHAIN_THICKNESS * ScrollMgr::Instance()->zoom * lineExtendScale);

		// �����̒��S�ɉ~��`��
		if (roundChangeEffect.drawFightFlag)
		{
			static int LINE_CENTER_GRAPH = TexHandleMgr::LoadGraph("resource/ChainCombat/line_center.png");
			DrawFunc::DrawRotaGraph2D(ScrollMgr::Instance()->Affect(lineCenterPos), Vec2<float>(ScrollMgr::Instance()->zoom, ScrollMgr::Instance()->zoom), 0.0f, TexHandleMgr::GetTexBuffer(LINE_CENTER_GRAPH));
		}
		//DrawFunc::DrawCircle2D(playerDefLength + playerBossDir * lineLengthPlayer - scrollShakeAmount, 10, Color());
	}

	roundChangeEffect.Draw();

	if (roundChangeEffect.initGameFlag)
	{
		CharacterManager::Instance()->Left()->Draw();
		CharacterManager::Instance()->Right()->Draw();
		CharacterManager::Instance()->Left()->DrawUI();
		CharacterManager::Instance()->Right()->DrawUI();
	}

	ParticleMgr::Instance()->Draw();
	CrashMgr::Instance()->Draw();

	GameTimer::Instance()->Draw();
	ScoreManager::Instance()->Draw();

	// �v���C���[�ƃ{�X�Ԃɐ���`��
	//DrawFunc::DrawLine2D(ScrollMgr::Instance()->Affect(player.centerPos), ScrollMgr::Instance()->Affect(boss.pos), Color());

	SuperiorityGauge::Instance()->Draw();

	miniMap.Draw();

	FaceIcon::Instance()->Draw();

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
		Vec2<float>leftUpPos = *enemyHomeBase->hitBox.center - enemyHomeBase->hitBox.size / 2.0f;
		Vec2<float>rightDownPos = *enemyHomeBase->hitBox.center + enemyHomeBase->hitBox.size / 2.0f;
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

	// �ړ��ʂ��擾�B �D���Q�[�W�͂����ōX�V�B
	double leftVel = CharacterManager::Instance()->Left()->vel.Length() * SlowMgr::Instance()->slowAmount;
	leftVelGauge = (CharacterManager::Instance()->Left()->vel * SuperiorityGauge::Instance()->GetLeftGaugeData().gaugeDivValue) * SlowMgr::Instance()->slowAmount;
	double rightVel = CharacterManager::Instance()->Right()->vel.Length() * SlowMgr::Instance()->slowAmount;
	rightVelGauge = (CharacterManager::Instance()->Right()->vel * SuperiorityGauge::Instance()->GetRightGaugeData().gaugeDivValue) * SlowMgr::Instance()->slowAmount;
	double subVel = fabs(fabs(leftVel) - fabs(rightVel));

	// [�U��񂵏�Ԃ̂Ƃ�] [�X�^�����o��] �͈ړ������Ȃ��B ���񒣂蒆�̏ꍇ�́A�ǂ���ɂ���ړ��ʂ�����Ȃ�0�ɋ߂��̂ňړ������Ă���肪�Ȃ��B
	if (!(CharacterManager::Instance()->Right()->GetNowSwing() || CharacterManager::Instance()->Left()->GetNowSwing() || StunEffect::Instance()->isActive)) {
		CharacterManager::Instance()->Left()->pos += leftVelGauge;
		CharacterManager::Instance()->Right()->pos += rightVelGauge;
	}


	// �����̒���
	float line = 0;
	float LINE = CharacterInterFace::LINE_LENGTH * 2 + (CharacterManager::Instance()->Left()->addLineLength + CharacterManager::Instance()->Right()->addLineLength);

	// �ǂ��炩�����񒣂��Ă��邩�B
	bool isHoldNow = CharacterManager::Instance()->Left()->isHold || CharacterManager::Instance()->Right()->isHold;


	// �ǂ��炩�����񒣂��Ă�����B
	if (isHoldNow) {

		// �E���̃L���������񒣂��Ă�����B
		if (CharacterManager::Instance()->Left()->isHold) {

			// ���������߂�B
			line = Vec2<float>(CharacterManager::Instance()->Left()->pos).Distance(CharacterManager::Instance()->Right()->pos);

			// �{�X���v���C���[�̕��Ɉړ�������B
			if (LINE < line) {

				// �����߂���
				float moveLength = line - LINE;

				// �����߂�����
				Vec2<float> moveDir = Vec2<float>(CharacterManager::Instance()->Left()->pos - CharacterManager::Instance()->Right()->pos);
				moveDir.Normalize();

				// �����߂��B
				CharacterManager::Instance()->Right()->pos += moveDir * Vec2<float>(moveLength, moveLength);

				// ���������蔻�肾������
				if (CharacterManager::Instance()->Right()->GetStackFlag()) {

					CharacterManager::Instance()->Right()->addLineLength += moveLength;

				}

			}

		}
		// �����̃L���������񒣂��Ă�����B
		else {

			// ���������߂�B
			line = Vec2<float>(CharacterManager::Instance()->Left()->pos).Distance(CharacterManager::Instance()->Right()->pos);

			// �v���C���[���{�X�̕��Ɉړ�������B
			if (LINE < line) {

				// �����߂���
				float moveLength = line - LINE;

				// �����߂�����
				Vec2<float> moveDir = Vec2<float>(CharacterManager::Instance()->Right()->pos - CharacterManager::Instance()->Left()->pos);
				moveDir.Normalize();

				// �����߂��B
				CharacterManager::Instance()->Left()->pos += moveDir * Vec2<float>(moveLength, moveLength);

				// ���������蔻�肾������
				if (CharacterManager::Instance()->Left()->GetStackFlag()) {

					CharacterManager::Instance()->Left()->addLineLength += moveLength;

				}

			}

		}


	}

	// �ǂ���̈ړ��ʂ����������擾�B�ǂ���������ꍇ�͏������΂��B
	else if (leftVelGauge.Length() < rightVelGauge.Length()) {

		// �E���̂ق����ړ��ʂ��傫��������B
		// �E���̃L�����𒆐S�Ƃ����ʒu�ɍ����̃L�����������Ă���B

		// ���������߂�B
		line = Vec2<float>(CharacterManager::Instance()->Left()->pos).Distance(CharacterManager::Instance()->Right()->pos);

		// �v���C���[���{�X�̕��Ɉړ�������B
		if (LINE < line) {

			// �����߂���
			float moveLength = line - LINE;

			// �����߂�����
			Vec2<float> moveDir = Vec2<float>(CharacterManager::Instance()->Right()->pos - CharacterManager::Instance()->Left()->pos);
			moveDir.Normalize();

			// �����߂��B
			CharacterManager::Instance()->Left()->pos += moveDir * Vec2<float>(moveLength, moveLength);

			// ���������蔻�肾������
			if (CharacterManager::Instance()->Left()->GetStackFlag()) {

				CharacterManager::Instance()->Left()->addLineLength += moveLength;

			}

		}

	}
	else if (rightVelGauge.Length() < leftVelGauge.Length()) {

		// �����̂ق����ړ��ʂ��傫��������B
		// �����̃L�����𒆐S�Ƃ����ʒu�ɉE���̃L�����������Ă���B

		// ���������߂�B
		line = Vec2<float>(CharacterManager::Instance()->Left()->pos).Distance(CharacterManager::Instance()->Right()->pos);

		// �{�X���v���C���[�̕��Ɉړ�������B
		if (LINE < line) {

			// �����߂���
			float moveLength = line - LINE;

			// �����߂�����
			Vec2<float> moveDir = Vec2<float>(CharacterManager::Instance()->Left()->pos - CharacterManager::Instance()->Right()->pos);
			moveDir.Normalize();

			// �����߂��B
			CharacterManager::Instance()->Right()->pos += moveDir * Vec2<float>(moveLength, moveLength);

			//if (CharacterManager::Instance()->Right()->GetPos().x < CharacterManager::Instance()->Right()->GetPrevPos().x) {
			//	CharacterManager::Instance()->Right()->AddVel({ -(CharacterManager::Instance()->Right()->GetPrevPos().x - CharacterManager::Instance()->Right()->GetPos().x),0.0f });
			//}

			// �{�X�̈ړ��ʂ�0�������Ȃ��悤�ɂ���B
			//if (CharacterManager::Instance()->Right()->GetVel().x < 0) {

			//	boss.vel.x = 0;

			//}

			// ���������蔻�肾������
			if (CharacterManager::Instance()->Right()->GetStackFlag()) {

				CharacterManager::Instance()->Right()->addLineLength += moveLength;

			}

		}

	}
	else {

		//return;

	}

	// ���������蔻�肶��Ȃ������炾�񂾂�Z������B
	if (!isCatchMapChipBoss && 0 < CharacterManager::Instance()->Right()->addLineLength) {

		CharacterManager::Instance()->Right()->addLineLength -= 5.0f;

		// �E�B���h�E�ɋ��܂�����
		if (0 < CharacterManager::Instance()->Right()->GetStackWinTimer()) {

			CharacterManager::Instance()->Right()->addLineLength -= 20.0f;

		}

		if (CharacterManager::Instance()->Right()->addLineLength < 0) CharacterManager::Instance()->Right()->addLineLength = 0;

	}
	if (!isCatchMapChipPlayer && 0 < CharacterManager::Instance()->Left()->addLineLength) {

		CharacterManager::Instance()->Left()->addLineLength -= 5.0f;

		// �E�B���h�E�ɋ��܂�����
		if (CharacterManager::Instance()->Left()->GetNowStuckWin()) {

			CharacterManager::Instance()->Left()->addLineLength -= 20.0f;

		}

		if (CharacterManager::Instance()->Left()->addLineLength < 0) CharacterManager::Instance()->Left()->addLineLength = 0;

	}

	isCatchMapChipBoss = false;
	isCatchMapChipPlayer = false;


	// �ړ��ʂɉ����Ė{������ׂ������ɂ���B
	float horaiAddLineLength = (CharacterManager::Instance()->Left()->vel.Length() / CharacterManager::Instance()->Left()->MOVE_SPEED_PLAYER);
	horaiAddLineLength *= CharacterManager::Instance()->Left()->ADD_LINE_LENGTH_VEL;
	if (CharacterManager::Instance()->Left()->addLineLength < horaiAddLineLength) {
		CharacterManager::Instance()->Left()->addLineLength = horaiAddLineLength;
	}
	horaiAddLineLength = (CharacterManager::Instance()->Right()->vel.Length() / CharacterManager::Instance()->Right()->MOVE_SPEED_PLAYER);
	horaiAddLineLength *= CharacterManager::Instance()->Right()->ADD_LINE_LENGTH_VEL;
	if (CharacterManager::Instance()->Right()->addLineLength < horaiAddLineLength) {
		CharacterManager::Instance()->Right()->addLineLength = horaiAddLineLength;
	}

	// �R�̒��S�_���v�Z
	{
		float distance = (CharacterManager::Instance()->Right()->pos - CharacterManager::Instance()->Left()->pos).Length();
		Vec2<float> bossDir = CharacterManager::Instance()->Right()->pos - CharacterManager::Instance()->Left()->pos;
		bossDir.Normalize();

		// �{�X�ƃv���C���[�Ԃ̋������K��l�ȉ���������
		if (distance < CharacterInterFace::LINE_LENGTH + CharacterInterFace::LINE_LENGTH) {
			// ����l�ȉ���������c�q��������h�����߂ɁA��_�Ԃ̐^�񒆂̍��W�ɂ���B
			lineCenterPos = CharacterManager::Instance()->Left()->pos + bossDir * Vec2<float>(distance / 2.0f, distance / 2.0f);
		}
		else {
			// �K��l�ȏゾ�����畁�ʂɏꏊ�����߂�B
			float playerLineLength = CharacterManager::Instance()->Left()->LINE_LENGTH + CharacterManager::Instance()->Left()->addLineLength;
			lineCenterPos = CharacterManager::Instance()->Left()->pos + bossDir * Vec2<float>(playerLineLength, playerLineLength);
		}
	}

}

Vec2<float> Game::GetStageSize()
{
	static const float CHIP_SIZE = 64;
	int sizeX = mapData[0].size();
	int sizeY = mapData.size();
	return Vec2<float>(CHIP_SIZE * sizeX, CHIP_SIZE * sizeY);
}