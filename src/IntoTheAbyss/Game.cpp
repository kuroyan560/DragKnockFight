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

				//�L�т������I�[���ɓn��
				data.push_back(std::make_unique<MassChipData>(checkData.GetLeftUpPos(), checkData.GetRightDownPos(), checkData.sideOrUpDownFlag));
			}
		}
	}

	return data;
}

void Game::DrawMapChip(const vector<vector<int>> &mapChipData, vector<vector<MapChipDrawData>> &mapChipDrawData, const int &stageNum, const int &roomNum)
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


				vector<MapChipAnimationData *>tmpAnimation = StageMgr::Instance()->animationData;
				int handle = -1;
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

	//�h�A���q�����Ă��邩�m�F
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
			string result = "���ɂȂ���h�A��������܂���B\nRalation�t�@�C�����m�F���邩�A�S���̑�΂ɘA�������������B";
			MessageBox(NULL, KuroFunc::GetWideStrFromStr(result).c_str(), TEXT("�h�A��������܂���"), MB_OK);
			stageNum = oldStageNum;
			roomNum = oldRoomNum;
			SelectStage::Instance()->SelectStageNum(oldStageNum);
			SelectStage::Instance()->SelectRoomNum(oldRoomNum);
			sceneLightFlag = false;
		}
	}


	mapData = StageMgr::Instance()->GetMapChipData(stageNum, roomNum);
	mapChipDrawData = StageMgr::Instance()->GetMapChipDrawBlock(stageNum, roomNum);

	// �V�F�C�N�ʂ�ݒ�B
	ShakeMgr::Instance()->Init();
	ParticleMgr::Instance()->Init();

	//�I�[���u���b�N�ǉ�
	{
		const int auraChipNum = MAPCHIP_BLOCK_AURABLOCK;//�I�[���u���b�N�̃`�b�v�ԍ�
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

	//�h�A�̒ǉ�
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

#pragma region �C�x���g�`�b�v����
	//�C�x���g�u���b�N����
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

#pragma region �h�b�X������
	// �h�b�X���u���b�N�𐶐��B
	vector<shared_ptr<ThownpeData>> dossunData;

	// �h�b�X���u���b�N�f�[�^���擾
	dossunData = GimmickLoader::Instance()->GetThowpeData(stageNum, roomNum);

	const int dossunCount = dossunData.size();

	// �Ə��̔���Ɏg�p����f�[�^��ۑ��B
	SightCollisionStorage::Instance()->data.clear();

	// �h�b�X���u���b�N���������B
	dossunBlock.clear();

	// �h�b�X���𐶐��B
	for (int index = 0; index < dossunCount; ++index) {
		//�n�_�ƏI�_���ꏏ�Ȃ�
		if (dossunData[index]->startPos != dossunData[index]->endPos)
		{
			DossunBlock dossunBuff;
			dossunBuff.Generate(dossunData[index]->startPos, dossunData[index]->endPos, dossunData[index]->size, dossunData[index]->type);
			// �f�[�^��ǉ��B
			dossunBlock.push_back(dossunBuff);
			if (&dossunBlock[dossunBlock.size() - 1].sightData == nullptr) assert(0); //�h�b�X���u���b�N�̃f�[�^�����̂�Nullptr�ł��I
			SightCollisionStorage::Instance()->data.push_back(&dossunBlock[dossunBlock.size() - 1].sightData);
		}
	}
#pragma endregion

#pragma region �V���{����
	// �V���{���ʃu���b�N�̏����擾�B
	vector<shared_ptr<BubbleData>> bubbleData;
	bubbleData = GimmickLoader::Instance()->GetBubbleData(stageNum, roomNum);

	const int bubbleCount = bubbleData.size();

	// �V���{���ʃu���b�N���������B
	bubbleBlock.clear();

	// �V���{���ʃu���b�N�𐶐��B
	for (int index = 0; index < bubbleCount; ++index) {

		Bubble bubbleBuff;
		bubbleBuff.Generate(bubbleData[index]->pos);

		// �f�[�^��ǉ��B
		bubbleBlock.push_back(bubbleBuff);
	}
#pragma endregion

	initDeadFlag = false;
	//giveDoorNumber = 0;
	debugStageData[0] = stageNum;
	debugStageData[1] = roomNum;



	//�h�A�J�ڂ̓v���C���[�����������Ȃ�
	if (!sceneLightFlag)
	{
#pragma region �v���C���[������
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

					// �R�̒��S�_���v�Z
					Vec2<float> bossDir = boss.pos - player.centerPos;
					bossDir.Normalize();
					float playerLineLength = lineLengthPlayer + addLineLengthPlayer;
					lineCenterPos = player.centerPos + bossDir * Vec2<float>(playerLineLength, playerLineLength);
					prevLineCenterPos = lineCenterPos;

					break;
				}
			}
		}

		//�f�o�b�N�J�n�p�̏ꏊ���烊�X�|�[�������炱�̏�����ʂ��Ȃ�
		if (!responeFlag)
		{
			//�}�b�v�J�n���̏ꏊ�ɃX�|�[��������
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
		//�V�[���J�ڌ�̏�����
		for (int i = 0; i < doorBlocks.size(); ++i)
		{
			if (doorBlocks[i]->chipNumber == doorNumber)
			{
				responePos = doorBlocks[i]->responePos;
				restartPos = doorBlocks[i]->restartPos;
				player.Init(responePos);

				// �R�̒��S�_���v�Z
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
	//�{�X�𐶐�
	boss.Generate(responePos + Vec2<float>(300.0f, 0.0f));

	miniMap.CalucurateCurrentPos(lineCenterPos);

	Camera::Instance()->Init();

	//ScrollMgr::Instance()->Init(&mapData);
	//// �X�N���[���ʂ�ݒ�B
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

	//�w�i�ɐ�
	BackGround::Instance()->Init(GetStageSize());

}

Game::Game()
{
	//mapBlockGraph = D3D12App::Instance()->GenerateTextureBuffer("resource/IntoTheAbyss/Block.png");

	// �e�Ǘ��N���X���������B
	BulletMgr::Instance()->Setting();

	movingBlockGraph = TexHandleMgr::LoadGraph("resource/IntoTheAbyss/MovingBlock.png");
	//sceneChangeHandle = TexHandleMgr::LoadGraph("resource/IntoTheAbyss/SceneChange.png");
	//movingBlockGraph = D3D12App::Instance()->GenerateTextureBuffer("resource/IntoTheAbyss/MovingBlock.png");

	// �e�p�[�e�B�N�����Z�b�e�B���O�B
	//BulletParticleMgr::Instance()->Setting();

	bgm = AudioApp::Instance()->LoadAudio("resource/ChainCombat/sound/bgm_1.wav");
	AudioApp::Instance()->ChangeVolume(bgm, 0.1f);

	//���C�g���
	ptLig.SetInfluenceRange(PT_LIG_RANGE);
	ptLig.SetColor(Color(PT_LIG_BRIGHT, PT_LIG_BRIGHT, PT_LIG_BRIGHT, 1.0f));
	spotLig.SetInfluenceRange(SPOT_LIG_RANGE);

	ligMgr.RegisterPointLight(&ptLig);
	ligMgr.RegisterSpotLight(&spotLig);
	ligMgr.RegisterHemiSphereLight(&hemiLig);
	ligMgr.RegisterPointLight(&player.lHand->ptLight);	//�Ə�����
	ligMgr.RegisterPointLight(&player.rHand->ptLight);//�Ə�����



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
	// �X�N���[���}�l�[�W���[���������B
	//ScrollMgr::Instance()->Init(&mapData);
	//// �X�N���[���ʂ�ݒ�B
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


	//�S�[���ɐG�ꂽ�玟�̃X�e�[�W�Ɍ���������
	{
		Vec2<float> playerChip((player.centerPos.x + 25.0f) / 50.0f, (player.centerPos.y + 25.0f) / 50.0f);
		//�S�[���ɐG�ꂽ�玟�̃X�e�[�W�Ɍ�����
		if (StageMgr::Instance()->GetMapChipBlock(SelectStage::Instance()->GetStageNum(), SelectStage::Instance()->GetRoomNum(), playerChip) == MAPCHIP_BLOCK_GOAL)
		{
			SelectStage::Instance()->SelectStageNum(0);
			SelectStage::Instance()->SelectRoomNum(0);

			mapData = StageMgr::Instance()->GetMapChipData(SelectStage::Instance()->GetStageNum(), SelectStage::Instance()->GetRoomNum());
			Vec2<float> door;
			//�S�[���T��
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


#pragma region �h�A�ړ�
	if (!sceneBlackFlag && !sceneLightFlag)
	{
		for (int doorIndex = 0; doorIndex < doorBlocks.size(); ++doorIndex)
		{
			//�����蔻��
			bool hitFlag = doorBlocks[doorIndex]->Collision(player.centerPos, player.PLAYER_HIT_SIZE, player.vel, player.prevFrameCenterPos);

			//�h�A�Ɠ��������琔����n��
			if (hitFlag)
			{
				giveDoorNumber = doorBlocks[doorIndex]->chipNumber;
				doorArrayNumber = doorIndex;
			}

			//�v���C���[�̑̂����S�ɉB�����߂̃I�t�Z�b�g
			Vec2<float>offset;
			if (player.vel.x <= 0.0f)
			{
				offset.x = MAP_CHIP_HALF_SIZE;
			}
			else
			{
				offset.x = -MAP_CHIP_HALF_SIZE;
			}

			//���ݒn�擾
			int nowChip = StageMgr::Instance()->GetMapChipBlock(SelectStage::Instance()->GetStageNum(), SelectStage::Instance()->GetRoomNum(), Vec2<float>((player.centerPos + Vec2<float>(0.0f, -MAP_CHIP_HALF_SIZE) + offset) / Vec2<float>(MAP_CHIP_SIZE, MAP_CHIP_SIZE)));
			int nowChip2 = StageMgr::Instance()->GetMapChipBlock(SelectStage::Instance()->GetStageNum(), SelectStage::Instance()->GetRoomNum(), Vec2<float>((player.centerPos + Vec2<float>(0.0f, MAP_CHIP_HALF_SIZE) + offset) / Vec2<float>(MAP_CHIP_SIZE, MAP_CHIP_SIZE)));

			//�h�A�ړ�����
			bool outSideFlag = nowChip == -1 && nowChip2 == -1;//�v���C���[����ʊO�ɏo������
			if (outSideFlag)
			{
				sceneBlackFlag = true;
				doorNumber = giveDoorNumber;
				player.drawCursorFlag = false;
			}
		}
	}
#pragma endregion



#pragma region �V�[���J��
	//�V�[���J��-----------------------
	//�Ó]��
	if (sceneBlackFlag)
	{
		alphaValue += 10;

		//�v���C���[�̓������~�߂�
		player.vel = { 0.0f,0.0f };
		player.gravity = 0.0f;


		//�Ó]�t���O�������A���]�����ɓ���
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
					//�}�b�v���X�V
					int localRoomNum = StageMgr::Instance()->GetRelationData(SelectStage::Instance()->GetStageNum(), SelectStage::Instance()->GetRoomNum(), doorNumber - chipMemorySize.min);

					RoomMapChipArray tmpMapData = StageMgr::Instance()->GetMapChipData(SelectStage::Instance()->GetStageNum(), localRoomNum);

					SelectStage::Instance()->SelectRoomNum(localRoomNum);
					sceneChangingFlag = true;
					initJumpFlag = false;
				}

				if (!responeErrorFlag)
				{
					//�Ó]�J�n
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

	//���]��
	if (sceneLightFlag)
	{
		player.drawCursorFlag = false;
		alphaValue -= 10;
		bool goFlag = false;

		//�v���C���[�𓮂���
		if (alphaValue <= 250)
		{
			if (!sceneChangeDeadFlag)
			{
				//sceneChangingFlag = false;
				switch (door)
				{
				case DOOR_UP_GORIGHT:
					//�W�����v����
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
					//�W�����v����
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

					// �R�̒��S�_���v�Z
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
		//�Q�[���J�n
		if (alphaValue <= 0 && goFlag)
		{
			sceneLightFlag = false;
			player.drawCursorFlag = true;
			sceneChangeDeadFlag = false;
			initDeadFlag = false;
		}
	}

	//�Ó]�Ɩ��]���̓v���C���[�̓��͂��֎~����
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
	//�V�[���J��-----------------------
#pragma endregion


	const bool resetInput = UsersInput::Instance()->OnTrigger(DIK_SPACE) || UsersInput::Instance()->OnTrigger(BACK);
	if (resetInput)
	{
		SelectStage::Instance()->resetStageFlag = true;
		//player.isDead = true;
		//sceneBlackFlag = true;
		//sceneChangeDeadFlag = player.isDead;
	}


	//�X�e�[�W���̐؂�ւ�����
	//�����̏�����
	if ((SelectStage::Instance()->GetRoomNum() != oldRoomNum || SelectStage::Instance()->GetStageNum() != oldStageNum) || SelectStage::Instance()->resetStageFlag)
	{
		InitGame(SelectStage::Instance()->GetStageNum(), SelectStage::Instance()->GetRoomNum());
		oldRoomNum = SelectStage::Instance()->GetRoomNum();
		oldStageNum = SelectStage::Instance()->GetStageNum();
	}

	//�v���C���[�w�n�ƓG�̔���
	if (playerHomeBase->Collision(boss.areaHitBox) && !roundFinishFlag && false)
	{
		//�v���C���[����
		++countRound;
		++countPlayerWin;
		roundFinishFlag = true;
		gameStartFlag = false;
	}

	//�G�w�n�ƃv���C���[�̔���
	if (enemyHomeBase->Collision(player.areaHitBox) && !roundFinishFlag)
	{
		//�G����
		++countRound;
		++countEnemyWin;
		roundFinishFlag = true;
		gameStartFlag = false;
	}

	//���E���h�I�����o�J�n
	if (roundFinishFlag)
	{
		readyToStartRoundFlag = true;
		roundFinishFlag = false;
	}

	//���E���h�J�n���̉��o�J�n
	if (readyToStartRoundFlag)
	{
		gameStartFlag = true;
		SelectStage::Instance()->resetStageFlag = true;
		readyToStartRoundFlag = false;
	}





	//�C�x���g�u���b�N�Ƃ̔���
	for (int i = 0; i < eventBlocks.size(); ++i)
	{
		eventBlocks[i]->HitBox(player.centerPos, player.PLAYER_HIT_SIZE, player.vel, player.prevFrameCenterPos);
	}


#pragma region ���S����
	//�J�ڒ��ł͂Ȃ�&&�v���C���[�����S���Ă��Ȏ��ɞ��u���b�N�Ƃ̔�������
	if (!player.isDead && !sceneBlackFlag && !sceneLightFlag)
	{
		//���u���b�N�Ƃ̔���
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

	// �v���C���[�̍X�V����
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



	// �{�X�̍X�V����
	boss.Update();

	// �v���C���[�ƃ{�X�̈������荇���̏���
	Scramble();

	//	ScrollManager::Instance()->CalucurateScroll(prevLineCenterPos - lineCenterPos, lineCenterPos);
	ScrollMgr::Instance()->CalucurateScroll(prevLineCenterPos - lineCenterPos, lineCenterPos);



	// �X�N���[���ʂ̍X�V����
	//ScrollManager::Instance()->Update();
	ScrollMgr::Instance()->Update();

	// �V�F�C�N�ʂ̍X�V����
	ShakeMgr::Instance()->Update();

#pragma region �e�M�~�b�N�̍X�V����

	// ���I�u���b�N�̍X�V����
	MovingBlockMgr::Instance()->Update(player.centerPos);

	SuperiorityGauge::Instance()->Update();

	// �e�p�[�e�B�N���̍X�V����
	//BulletParticleMgr::Instance()->Update();

	// �h�b�X���u���b�N�̍X�V����
	const int DOSSUN_COUNT = dossunBlock.size();
	for (int index = 0; index < DOSSUN_COUNT; ++index) {
		dossunBlock[index].Update();
	}

	ViewPort::Instance()->Update(player.centerPos);
	for (int i = 0; i < auraBlock.size(); ++i)
	{
		auraBlock[i]->Update();
	}

	// �V���{���ʃu���b�N�̍X�V����
	{
		const int BUBBLE_COUNT = bubbleBlock.size();
		for (int index = 0; index < BUBBLE_COUNT; ++index) {
			bubbleBlock[index].Update();
		}
	}

#pragma endregion


	/*===== �����蔻�� =====*/

#pragma region �����蔻��

	// �h�b�X���u���b�N�̓����蔻��
	for (int index = 0; index < DOSSUN_COUNT; ++index) {
		dossunBlock[index].CheckHit(mapData);
	}

	// �v���C���[�̓����蔻��
	player.CheckHit(mapData, bubbleBlock, dossunBlock, boss.pos, isCatchMapChipPlayer, lineCenterPos);

	// ���I�u���b�N�̓����蔻��
	MovingBlockMgr::Instance()->CheckHit(mapData);

	// �e�ƃ}�b�v�`�b�v�̓����蔻��
	BulletMgr::Instance()->CheckHit(mapData, bubbleBlock);

	// �{�X�̓����蔻��
	boss.CheckHit(mapData, isCatchMapChipBoss, player.centerPos, lineCenterPos);

	// �r���[�|�[�g���v���C���[��ňړ�������B
	ViewPort::Instance()->SetPlayerPosX(player.centerPos.x);
	ViewPort::Instance()->SetPlayerPosY(player.centerPos.y);

	// �I�[���u���b�N�̃f�[�^�ƃr���[�|�[�g�̔�����s���B
	ViewPort::Instance()->SavePrevFlamePos();

	// �G�ƒe�̓����蔻��
	bossEnemy.CheckHitBullet();
	for (int index = 0; index < SMALL_ENEMY; ++index) {

		if (!smallEnemy[index].isActive) continue;

		smallEnemy[index].CheckHitBullet();

	}
	for (int index = 0; index < NOMOVEMENT_ENEMY; ++index) {

		if (!noMovementEnemy[index].isActive) continue;

		noMovementEnemy[index].CheckHitBullet();

	}

	//�{�X�e�ƃv���C���[�̔���
	for (int index = 0; index < SMALL_ENEMY; ++index)
	{
		std::shared_ptr<SphereCollision> bullet = BossBulletManager::Instance()->GetBullet(index)->bulletHitBox;
		bool hitFlag = BulletCollision::Instance()->CheckSphereAndSphere(*bullet, *player.bulletHitBox);
		bool initFlag = BossBulletManager::Instance()->GetBullet(index)->isActive;

		//����������Ă���&&�v���C���[�Ɣ�����������D���Q�[�W�̕΂肪�ς��A�e�͏����������
		if (hitFlag && initFlag)
		{
			SuperiorityGauge::Instance()->AddEnemyGauge(10.0f);
			BossBulletManager::Instance()->GetBullet(index)->Init();
		}
	}


#pragma endregion

#pragma region �K�v�̂Ȃ������蔻��

	// �e�ƃr���[�|�[�g�̓����蔻��
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

			// �e�p�[�e�B�N���𐶐�����B
			//BulletParticleMgr::Instance()->Generate(BulletMgr::Instance()->GetBullet(i)->pos, BulletMgr::Instance()->GetBullet(i)->forwardVec);
			//BulletParticleMgr::Instance()->Generate(BulletMgr::Instance()->GetBullet(i)->pos, BulletMgr::Instance()->GetBullet(i)->forwardVec);
			ParticleMgr::Instance()->Generate(BulletMgr::Instance()->GetBullet(i)->pos, BulletMgr::Instance()->GetBullet(i)->forwardVec, BULLET);
			ParticleMgr::Instance()->Generate(BulletMgr::Instance()->GetBullet(i)->pos, BulletMgr::Instance()->GetBullet(i)->forwardVec, BULLET);

			BulletMgr::Instance()->GetBullet(i)->Init();
		}
	}

	// �I�[���ƃr���[�|�[�g�̓����蔻��
	{
		// �I�[���u���b�N�̃f�[�^�ƃr���[�|�[�g�̔�����s���B
		vector<ViewPortAuraReturnData> buff;
		// �����
		buff = ViewPortCollider::Instance()->CheckHitRectangleAura(ViewPort::Instance()->pointData[ViewPort::LEFT_UP], ViewPort::Instance()->pointData[ViewPort::RIGHT_UP], CHECK_HIT_TOP, auraBlock);

		// �����߂�
		if (buff.size() > 0 && !ViewPort::Instance()->noHitFlags[ViewPort::Instance()->UP])
		{

			ViewPortAuraReturnData biggestBuff = {};
			// �l����ԑ傫�������߂��ʂ����߂�B
			for (int index = 0; index < buff.size(); ++index) {

				if (fabs(buff[index].pushBackAmount) < fabs(biggestBuff.pushBackAmount)) continue;

				biggestBuff = buff[index];

			}

			// �z�[���h����Ă��Ȃ���Ԃ�������-�����ɉ����߂����̂͂��������̂ŁA�����߂��ʂ�ł������B
			if (!biggestBuff.isHold && biggestBuff.pushBackAmount < 0)
			{
				biggestBuff.pushBackAmount = 0;
			}
			// �z�[���h����Ă��Ȃ��Ԃ�������+�����ɉ����߂����̂͂��������̂ŁA�����߂��ʂ�ł������B
			else if (biggestBuff.isHold && biggestBuff.pushBackAmount > 0) {

				biggestBuff.pushBackAmount = 0;

			}

			// �㑤�ɓ������������ۑ�����B
			ViewPort::Instance()->isHitTop = true;

			// �z�[���h��Ԃ�������
			if (biggestBuff.isHold) {


				ViewPort::Instance()->PushBackAuraHoldUp(biggestBuff.pushBackAmount);

			}
			else {

				// ���S���W�������߂��B
				ViewPort::Instance()->centralPos.y += biggestBuff.pushBackAmount;

				// �r���[�|�[�g�̎l�_�������߂��B
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


		// �E����
		buff = ViewPortCollider::Instance()->CheckHitRectangleAura(ViewPort::Instance()->pointData[ViewPort::RIGHT_UP], ViewPort::Instance()->pointData[ViewPort::RIGHT_DOWN], CHECK_HIT_RIGHT, auraBlock);

		// �����߂�
		if (buff.size() > 0 && !ViewPort::Instance()->noHitFlags[ViewPort::Instance()->RIGHT])
		{

			ViewPortAuraReturnData biggestBuff = {};
			// �l����ԑ傫�������߂��ʂ����߂�B
			for (int index = 0; index < buff.size(); ++index) {

				if (fabs(buff[index].pushBackAmount) < fabs(biggestBuff.pushBackAmount)) continue;

				biggestBuff = buff[index];

			}

			// �z�[���h����Ă��Ȃ���Ԃ�������-�����ɉ����߂����̂͂��������̂ŁA�����߂��ʂ�ł������B
			if (!biggestBuff.isHold && biggestBuff.pushBackAmount > 0)
			{
				biggestBuff.pushBackAmount = 0;
			}
			// �z�[���h����Ă��Ȃ��Ԃ�������+�����ɉ����߂����̂͂��������̂ŁA�����߂��ʂ�ł������B
			else if (biggestBuff.isHold && biggestBuff.pushBackAmount < 0) {

				biggestBuff.pushBackAmount = 0;

			}

			// �E���ɓ������������ۑ�����B
			ViewPort::Instance()->isHitRight = true;

			// �z�[���h��Ԃ�������
			if (biggestBuff.isHold) {


				ViewPort::Instance()->PushBackAuraHoldRight(biggestBuff.pushBackAmount);

			}
			else {

				// ���S���W�������߂��B
				ViewPort::Instance()->centralPos.x += biggestBuff.pushBackAmount;

				// �r���[�|�[�g�̎l�_�������߂��B
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

		// ������
		buff = ViewPortCollider::Instance()->CheckHitRectangleAura(ViewPort::Instance()->pointData[ViewPort::LEFT_DOWN], ViewPort::Instance()->pointData[ViewPort::RIGHT_DOWN], CHECK_HIT_BOTTOM, auraBlock);

		// �����߂�
		if (buff.size() > 0 && !ViewPort::Instance()->noHitFlags[ViewPort::Instance()->DOWN])
		{

			ViewPortAuraReturnData biggestBuff = {};
			// �l����ԑ傫�������߂��ʂ����߂�B
			for (int index = 0; index < buff.size(); ++index) {

				if (fabs(buff[index].pushBackAmount) < fabs(biggestBuff.pushBackAmount)) continue;

				biggestBuff = buff[index];

			}

			// �z�[���h����Ă��Ȃ���Ԃ�������-�����ɉ����߂����̂͂��������̂ŁA�����߂��ʂ�ł������B
			if (!biggestBuff.isHold && biggestBuff.pushBackAmount > 0)
			{
				biggestBuff.pushBackAmount = 0;
			}
			// �z�[���h����Ă��Ȃ��Ԃ�������+�����ɉ����߂����̂͂��������̂ŁA�����߂��ʂ�ł������B
			else if (biggestBuff.isHold && biggestBuff.pushBackAmount < 0) {

				biggestBuff.pushBackAmount = 0;

			}

			// �㑤�ɓ������������ۑ�����B
			ViewPort::Instance()->isHitBottom = true;

			// �z�[���h��Ԃ�������
			if (biggestBuff.isHold) {


				ViewPort::Instance()->PushBackAuraHoldDown(biggestBuff.pushBackAmount);

			}
			else {

				// ���S���W�������߂��B
				ViewPort::Instance()->centralPos.y += biggestBuff.pushBackAmount;

				// �r���[�|�[�g�̎l�_�������߂��B
				ViewPort::Instance()->PushBackAura(Vec2<float>(0, biggestBuff.pushBackAmount));

			}

			if (biggestBuff.isHold) {
				ViewPort::Instance()->holdFlags[ViewPort::Instance()->DOWN] = true;
				//ViewPort::Instance()->PushBackLine(ViewPort::Instance()->LEFT, biggestBuff.pushBackAmount);
			}

			// �X�N���[�����~�߂�B
			++countStopNum;
		}
		else
		{
			ViewPort::Instance()->holdFlags[ViewPort::Instance()->DOWN] = false;
		}


		// ������
		buff = ViewPortCollider::Instance()->CheckHitRectangleAura(ViewPort::Instance()->pointData[ViewPort::LEFT_UP], ViewPort::Instance()->pointData[ViewPort::LEFT_DOWN], CHECK_HIT_LEFT, auraBlock);



		// �����߂�
		if (buff.size() > 0 && !ViewPort::Instance()->noHitFlags[ViewPort::Instance()->LEFT])
		{

			ViewPortAuraReturnData biggestBuff = {};
			// �l����ԑ傫�������߂��ʂ����߂�B
			for (int index = 0; index < buff.size(); ++index) {

				if (fabs(buff[index].pushBackAmount) < fabs(biggestBuff.pushBackAmount)) continue;

				biggestBuff = buff[index];

			}

			// �z�[���h����Ă��Ȃ���Ԃ�������-�����ɉ����߂����̂͂��������̂ŁA�����߂��ʂ�ł������B
			if (!biggestBuff.isHold && biggestBuff.pushBackAmount < 0)
			{
				biggestBuff.pushBackAmount = 0;
			}
			// �z�[���h����Ă��Ȃ��Ԃ�������+�����ɉ����߂����̂͂��������̂ŁA�����߂��ʂ�ł������B
			else if (biggestBuff.isHold && biggestBuff.pushBackAmount > 0) {

				biggestBuff.pushBackAmount = 0;

			}

			//biggestBuff.pushBackAmount /= 2.0f;

			// �����ɓ������������ۑ�����B
			ViewPort::Instance()->isHitLeft = true;

			// �z�[���h��Ԃ�������
			if (biggestBuff.isHold) {


				ViewPort::Instance()->PushBackAuraHoldLeft(biggestBuff.pushBackAmount);

			}
			else {

				// ���S���W�������߂��B
				ViewPort::Instance()->centralPos.x += biggestBuff.pushBackAmount;

				// �r���[�|�[�g�̎l�_�������߂��B
				ViewPort::Instance()->PushBackAura(Vec2<float>(biggestBuff.pushBackAmount, 0));

			}

			if (biggestBuff.isHold) {
				ViewPort::Instance()->holdFlags[ViewPort::Instance()->LEFT] = true;
				//ViewPort::Instance()->PushBackLine(ViewPort::Instance()->LEFT, biggestBuff.pushBackAmount);
			}

			// �X�N���[�����~�߂�B
			++countStopNum;
		}
		else
		{
			ViewPort::Instance()->holdFlags[ViewPort::Instance()->LEFT] = false;
		}


	}

#pragma endregion

	ViewPort::Instance()->playerPos = player.centerPos;
	//���C�g�X�V
	auto pos = ScrollMgr::Instance()->Affect(player.centerPos);
	ptLig.SetPos({ pos.x,pos.y,PT_LIG_Z });

	spotLig.SetTarget({ pos.x,pos.y + SPOT_LIG_TARGET_OFFSET_Y,0.0f });
	spotLig.SetPos({ pos.x,pos.y + SPOT_LIG_TARGET_OFFSET_Y,SPOT_LIG_Z });

	//�p�[�e�B�N���X�V
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

	/*===== �`�揈�� =====*/
	BackGround::Instance()->Draw();

	mapChipDrawData = StageMgr::Instance()->GetMapChipDrawBlock(stageNum, roomNum);
	DrawMapChip(mapData, mapChipDrawData, stageNum, roomNum);

	MovingBlockMgr::Instance()->Draw(movingBlockGraph);

	// �e�p�[�e�B�N���̕`�揈��
	//BulletParticleMgr::Instance()->Draw();

	// �h�b�X���u���b�N�̕`�揈��
	const int DOSSUN_COUNT = dossunBlock.size();
	for (int index = 0; index < DOSSUN_COUNT; ++index) {
		dossunBlock[index].Draw();
	}

	// �V���{���ʃu���b�N�̕`�揈��
	{
		const int BUBBLE_COUNT = bubbleBlock.size();
		for (int index = 0; index < BUBBLE_COUNT; ++index) {
			if (bubbleBlock[index].isBreak) continue;
			bubbleBlock[index].Draw();
		}
	}

	//ViewPort::Instance()->Draw();

	//�I�[���u���b�N�̕`��
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
	// �v���C���[�ƃ{�X�Ԃɐ���`��
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

		// �����̒��S�ɉ~��`��
		static int LINE_CENTER_GRAPH = TexHandleMgr::LoadGraph("resource/ChainCombat/line_center.png");
		DrawFunc::DrawRotaGraph2D(ScrollMgr::Instance()->Affect(lineCenterPos), { ScrollMgr::Instance()->zoom,ScrollMgr::Instance()->zoom }, 0.0f, TexHandleMgr::GetTexBuffer(LINE_CENTER_GRAPH));
		//DrawFunc::DrawCircle2D(playerDefLength + playerBossDir * lineLengthPlayer - scrollShakeAmount, 10, Color());
	}

	playerHomeBase->Draw();
	enemyHomeBase->Draw();

	player.Draw(ligMgr);
	ParticleMgr::Instance()->Draw(ligMgr);

	// �{�X��`��
	boss.Draw();

	// �v���C���[�ƃ{�X�Ԃɐ���`��
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
	/*===== �������荇���̏��� =====*/

	// �O�t���[���̐��̒��S���W��ۑ�
	prevLineCenterPos = lineCenterPos;

	// �ړ��ʂ��擾�B �D���Q�[�W�͂����ōX�V�B
	double playerVel = player.vel.Length();
	Vec2<float> playerVelGauge = player.vel * SuperiorityGauge::Instance()->GetPlayerGaugeData()->gaugeDivValue;
	double bossVel = boss.vel.Length();
	Vec2<float> bossVelGauge = boss.vel * SuperiorityGauge::Instance()->GetEnemyGaugeData()->gaugeDivValue;
	double subVel = fabs(fabs(playerVel) - fabs(bossVel));

	player.centerPos += playerVelGauge;
	boss.pos += bossVelGauge;


	// �����̒���
	float line = 0;
	float LINE = (lineLengthBoss + lineLengthPlayer) + (addLineLengthBoss + addLineLengthPlayer);

	// �C�ɂ��Ȃ��ł��������I
	bool isBoss = false;
	bool isPlayer = false;

	// �ǂ���̈ړ��ʂ����������擾�B�ǂ���������ꍇ�͏������΂��B
	if (playerVelGauge.Length() < bossVelGauge.Length()) {

		// �{�X�̈ړ��ʂ̂ق����傫��������

		// ���������߂�B
		line = Vec2<float>(player.centerPos).Distance(boss.pos);

		// �v���C���[���{�X�̕��Ɉړ�������B
		if (LINE < line) {

			// �����߂���
			float moveLength = line - LINE;

			// �����߂�����
			Vec2<float> moveDir = Vec2<float>(boss.pos - player.centerPos);
			moveDir.Normalize();

			// �����߂��B
			player.centerPos += moveDir * Vec2<float>(moveLength, moveLength);

			// ���������蔻�肾������
			if (isCatchMapChipPlayer) {

				addLineLengthPlayer += moveLength;

			}

			isBoss = true;

		}

	}
	else if (bossVelGauge.Length() < playerVelGauge.Length()) {

		// �v���C���[�̈ړ��ʂ̂ق����傫��������

		// ���������߂�B
		line = Vec2<float>(player.centerPos).Distance(boss.pos);

		// �{�X���v���C���[�̕��Ɉړ�������B
		if (LINE < line) {

			// �����߂���
			float moveLength = line - LINE;

			// �����߂�����
			Vec2<float> moveDir = Vec2<float>(player.centerPos - boss.pos);
			moveDir.Normalize();

			// �����߂��B
			boss.pos += moveDir * Vec2<float>(moveLength, moveLength);

			if (boss.pos.x < boss.prevPos.x) {
				boss.vel.x -= boss.prevPos.x - boss.pos.x;
			}

			// �{�X�̈ړ��ʂ�0�������Ȃ��悤�ɂ���B
			if (boss.vel.x < 0) {

				boss.vel.x = 0;

			}

			// ���������蔻�肾������
			if (isCatchMapChipBoss) {

				addLineLengthBoss += moveLength;

			}

			isPlayer = true;

		}

	}
	else {

		return;

	}

	// ���������蔻�肶��Ȃ������炾�񂾂�Z������B
	if (isBoss || (!isCatchMapChipBoss && 0 < addLineLengthBoss)) {

		addLineLengthBoss -= 5.0f;

		// �E�B���h�E�ɋ��܂�����
		if (0 < boss.stuckWindowTimer) {

			addLineLengthBoss -= 20.0f;

		}

		if (addLineLengthBoss < 0) addLineLengthBoss = 0;

	}
	if (isPlayer || (!isCatchMapChipPlayer && 0 < addLineLengthPlayer)) {

		addLineLengthPlayer -= 5.0f;

		// �E�B���h�E�ɋ��܂�����
		if (0 < player.stuckWindowTimer) {

			addLineLengthPlayer -= 20.0f;

		}

		if (addLineLengthPlayer < 0) addLineLengthPlayer = 0;

	}

	// �R�̒��S�_���v�Z
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
