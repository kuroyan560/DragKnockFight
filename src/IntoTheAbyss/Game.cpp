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

	AudioApp::Instance()->PlayWave(bgm, true);

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
	// �e�Ǘ��N���X���������B
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

	//�w�i�ɐ�
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

#pragma region �X�e�[�W�̐؂�ւ�
	const bool enableToSelectStageFlag = 0 < debugStageData[0];
	const bool enableToSelectStageFlag2 = debugStageData[0] < StageMgr::Instance()->GetMaxStageNumber() - 1;
	//�}�b�v�̐؂�ւ�
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

	const bool done = UsersInput::Instance()->OnTrigger(DIK_RETURN) || UsersInput::Instance()->OnTrigger(A);
	if (done)
	{
		SelectStage::Instance()->SelectStageNum(debugStageData[0]);
		SelectStage::Instance()->SelectRoomNum(debugStageData[1]);
		SelectStage::Instance()->resetStageFlag = true;
		//mapData = StageMgr::Instance()->GetMapChipData(debugStageData[0], debugStageData[1]);

		//Vec2<float> door;
		////�f�o�b�N�p�̃}�b�v�`�b�v�ԍ�����X�^�[�g����
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


	//�v���C���[�w�n�ƓG�̔���
	if (playerHomeBase->Collision(rightCharacter->GetAreaHitBox()) && !roundFinishFlag && !readyToStartRoundFlag)
	{
		//�v���C���[����
		WinCounter::Instance()->RoundFinish(lineCenterPos, true);
		roundFinishFlag = true;
		playerOrEnemeyWinFlag = true;
		gameStartFlag = false;
	}

	//�G�w�n�ƃv���C���[�̔���
	if (enemyHomeBase->Collision(leftCharacter->GetAreaHitBox()) && !roundFinishFlag && !readyToStartRoundFlag)
	{
		//�G����
		WinCounter::Instance()->RoundFinish(lineCenterPos, false);
		roundFinishFlag = true;
		playerOrEnemeyWinFlag = false;
		gameStartFlag = false;
	}

	//���E���h�I�����o�J�n
	if (roundFinishFlag)
	{
		BossBulletManager::Instance()->Init();


		//�������J�E���g���o
		if (!WinCounter::Instance()->GetNowAnimation())
		{
			//�ǂ��炩���R���Ƃ�����Q�[���I��
			if (WinCounter::Instance()->GetGameFinish())
			{
				//�Ƃ肠�������Z�b�g���Ƃ�
				//WinCounter::Instance()->Reset();
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
					SuperiorityGauge::Instance()->Init();
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
		if (roundChangeEffect.readyToInitFlag && !roundChangeEffect.initFlag)
		{
			InitGame(0, 0);
			roundChangeEffect.initFlag = true;
		}

		//�o�ꉉ�o
		if (roundChangeEffect.initFlag)
		{
			bool leftAppear = leftCharacter->Appear();
			bool rightApperar = rightCharacter->Appear();
			if (leftAppear && rightApperar)	//�ǂ���̃L�������o�ꉉ�o����
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

		// �v���C���[�̍X�V����
		leftCharacter->Update(mapData, lineCenterPos);

		// �{�X�̍X�V����
		rightCharacter->Update(mapData, lineCenterPos);

		// �e���X�V
		BulletMgr::Instance()->Update();

		// �v���C���[�ƃ{�X�̈������荇���̏���
		Scramble();
	}


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
	// �e�ƃ}�b�v�`�b�v�̓����蔻��
	BulletMgr::Instance()->CheckHit(mapData);

	//�{�X�e�ƃv���C���[�̔���
	for (int index = 0; index < BossBulletManager::Instance()->bullets.size(); ++index)
	{
		std::shared_ptr<SphereCollision> bullet = BossBulletManager::Instance()->GetBullet(index)->bulletHitBox;
		bool hitFlag = BulletCollision::Instance()->CheckSphereAndSphere(*bullet, leftCharacter->GetBulletHitSphere());
		bool initFlag = BossBulletManager::Instance()->GetBullet(index)->isActive;

		//����������Ă���&&�v���C���[�Ɣ�����������D���Q�[�W�̕΂肪�ς��A�e�͏����������
		if (hitFlag && initFlag)
		{
			SuperiorityGauge::Instance()->AddEnemyGauge(1.0f);
			BossBulletManager::Instance()->GetBullet(index)->Init();
		}
	}

	//�v���C���[�e�ƃ{�X�̔���
	for (int index = 0; index < BulletMgr::Instance()->bullets.size(); ++index)
	{
		std::shared_ptr<SphereCollision> bullet = BulletMgr::Instance()->GetBullet(index)->bulletHitBox;
		bool hitFlag = BulletCollision::Instance()->CheckSphereAndSphere(*bullet, rightCharacter->GetBulletHitSphere());
		bool initFlag = BulletMgr::Instance()->GetBullet(index)->isActive;

		//����������Ă���&&�v���C���[�Ɣ�����������D���Q�[�W�̕΂肪�ς��A�e�͏����������
		if (hitFlag && initFlag)
		{
			SuperiorityGauge::Instance()->AddPlayerGauge(1.0f);
			BulletMgr::Instance()->GetBullet(index)->Init();
		}
	}


#pragma endregion

	//�p�[�e�B�N���X�V
	ParticleMgr::Instance()->Update();

	BackGround::Instance()->Update();
	Camera::Instance()->Update();
	FaceIcon::Instance()->Update();
	WinCounter::Instance()->Update();

	// �D���Q�[�W���U��؂����g���K�[����̂Ƃ��ɃX�^�����o��L��������B
	if (SuperiorityGauge::Instance()->GetEnemyGaugeData()->overGaugeFlag && !SuperiorityGauge::Instance()->GetEnemyGaugeData()->prevOverGaugeFlag) {
		// �G�̗D���Q�[�W���U��؂����Ƃ������Ƃ́A�v���C���[�̗D���Q�[�W��0���Ƃ������ƁB
		StunEffect::Instance()->Activate(leftCharacter->pos, Vec2<float>(0, 0), false);
		ResultTransfer::Instance()->leftBreakCount++;
	}
	if (SuperiorityGauge::Instance()->GetPlayerGaugeData()->overGaugeFlag && !SuperiorityGauge::Instance()->GetPlayerGaugeData()->prevOverGaugeFlag) {
		// �v���C���[�̗D���Q�[�W���U��؂����Ƃ������Ƃ́A�G�̗D���Q�[�W��0���Ƃ������ƁB
		StunEffect::Instance()->Activate(rightCharacter->pos, Vec2<float>(1200, 0), true);
		ResultTransfer::Instance()->rightBreakCount++;
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

	static int CHAIN_GRAPH = TexHandleMgr::LoadGraph("resource/ChainCombat/chain.png");
	static const int CHAIN_THICKNESS = 4;
	// �v���C���[�ƃ{�X�Ԃɐ���`��
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

		// �����̒��S�ɉ~��`��
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

	// �v���C���[�ƃ{�X�Ԃɐ���`��
	//DrawFunc::DrawLine2D(ScrollMgr::Instance()->Affect(player.centerPos), ScrollMgr::Instance()->Affect(boss.pos), Color());

	SuperiorityGauge::Instance()->Draw();

	miniMap.Draw();

	FaceIcon::Instance()->Draw();

	WinCounter::Instance()->Draw();

	StunEffect::Instance()->Draw();
}

void Game::Scramble()
{
	/*===== �������荇���̏��� =====*/

	// �O�t���[���̐��̒��S���W��ۑ�
	prevLineCenterPos = lineCenterPos;

	Vec2<float> playerVelGauge;
	Vec2<float> bossVelGauge;

	// �ړ��ʂ��擾�B �D���Q�[�W�͂����ōX�V�B
	double playerVel = leftCharacter->vel.Length() * SlowMgr::Instance()->slowAmount;
	playerVelGauge = (leftCharacter->vel * SuperiorityGauge::Instance()->GetPlayerGaugeData()->gaugeDivValue) * SlowMgr::Instance()->slowAmount;
	double bossVel = rightCharacter->vel.Length() * SlowMgr::Instance()->slowAmount;
	bossVelGauge = (rightCharacter->vel * SuperiorityGauge::Instance()->GetEnemyGaugeData()->gaugeDivValue) * SlowMgr::Instance()->slowAmount;
	double subVel = fabs(fabs(playerVel) - fabs(bossVel));

	// [�U��񂵏�Ԃ̂Ƃ�] [�X�^�����o��] �͈ړ������Ȃ��B
	if (!(rightCharacter->GetNowSwing() || leftCharacter->GetNowSwing() || StunEffect::Instance()->isActive)) {
		leftCharacter->pos += playerVelGauge;
		rightCharacter->pos += bossVelGauge;
	}


	// �����̒���
	float line = 0;
	float LINE = CharacterInterFace::LINE_LENGTH * 2 + (leftCharacter->addLineLength + rightCharacter->addLineLength);

	// �C�ɂ��Ȃ��ł��������I
	bool isBoss = false;
	bool isPlayer = false;

	// �ǂ���̈ړ��ʂ����������擾�B�ǂ���������ꍇ�͏������΂��B
	if (playerVelGauge.Length() < bossVelGauge.Length()) {

		// �{�X�̈ړ��ʂ̂ق����傫��������

		// ���������߂�B
		line = Vec2<float>(leftCharacter->pos).Distance(rightCharacter->pos);

		// �v���C���[���{�X�̕��Ɉړ�������B
		if (LINE < line) {

			// �����߂���
			float moveLength = line - LINE;

			// �����߂�����
			Vec2<float> moveDir = Vec2<float>(rightCharacter->pos - leftCharacter->pos);
			moveDir.Normalize();

			// �����߂��B
			leftCharacter->pos += moveDir * Vec2<float>(moveLength, moveLength);

			// ���������蔻�肾������
			if (isCatchMapChipPlayer) {

				leftCharacter->addLineLength += moveLength;

			}

			isBoss = true;

		}

	}
	else if (bossVelGauge.Length() < playerVelGauge.Length()) {

		// �v���C���[�̈ړ��ʂ̂ق����傫��������

		// ���������߂�B
		line = Vec2<float>(leftCharacter->pos).Distance(rightCharacter->pos);

		// �{�X���v���C���[�̕��Ɉړ�������B
		if (LINE < line) {

			// �����߂���
			float moveLength = line - LINE;

			// �����߂�����
			Vec2<float> moveDir = Vec2<float>(leftCharacter->pos - rightCharacter->pos);
			moveDir.Normalize();

			// �����߂��B
			rightCharacter->pos += moveDir * Vec2<float>(moveLength, moveLength);

			//if (rightCharacter->GetPos().x < rightCharacter->GetPrevPos().x) {
			//	rightCharacter->AddVel({ -(rightCharacter->GetPrevPos().x - rightCharacter->GetPos().x),0.0f });
			//}

			// �{�X�̈ړ��ʂ�0�������Ȃ��悤�ɂ���B
			//if (rightCharacter->GetVel().x < 0) {

			//	boss.vel.x = 0;

			//}

			// ���������蔻�肾������
			if (isCatchMapChipBoss) {

				rightCharacter->addLineLength += moveLength;

			}

			isPlayer = true;

		}

	}
	else {

		return;

	}

	// ���������蔻�肶��Ȃ������炾�񂾂�Z������B
	if (isBoss || (!isCatchMapChipBoss && 0 < rightCharacter->addLineLength)) {

		rightCharacter->addLineLength -= 5.0f;

		// �E�B���h�E�ɋ��܂�����
		if (0 < rightCharacter->GetStackWinTimer()) {

			rightCharacter->addLineLength -= 20.0f;

		}

		if (rightCharacter->addLineLength < 0) rightCharacter->addLineLength = 0;

	}
	if (isPlayer || (!isCatchMapChipPlayer && 0 < leftCharacter->addLineLength)) {

		leftCharacter->addLineLength -= 5.0f;

		// �E�B���h�E�ɋ��܂�����
		if (leftCharacter->GetNowStuckWin()) {

			leftCharacter->addLineLength -= 20.0f;

		}

		if (leftCharacter->addLineLength < 0) leftCharacter->addLineLength = 0;

	}

	// �R�̒��S�_���v�Z
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
