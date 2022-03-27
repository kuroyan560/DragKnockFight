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

				//�L�т������I�[���ɓn��
				data.push_back(std::make_unique<MassChipData>(checkData.GetLeftUpPos(), checkData.GetRightDownPos(), checkData.sideOrUpDownFlag));
			}
		}
	}

	return data;
}

void Game::DrawMapChip(const vector<vector<int>> &mapChipData, vector<vector<MapChipDrawData>> &mapChipDrawData, const int &mapBlockGraph, const int &stageNum, const int &roomNum)
{
	std::map<int, std::vector<ChipData>>datas;

	// �`�悷��`�b�v�̃T�C�Y
	const float DRAW_MAP_CHIP_SIZE = MAP_CHIP_SIZE * ScrollMgr::Instance()->zoom;

	// �}�b�v�`�b�v�̏c�̗v�f�����擾�B
	const int HEIGHT = mapChipData.size();
	for (int height = 0; height < HEIGHT; ++height) {

		// �}�b�v�`�b�v�̉��̗v�f�����擾�B
		const int WIDTH = mapChipData[height].size();
		for (int width = 0; width < WIDTH; ++width) {

			// �u���b�N�ȊO�������珈�����΂��B
			bool blockFlag = (mapChipData[height][width] >= 1 && mapChipData[height][width] <= 9);
			bool doorFlag = (mapChipData[height][width] >= 20 && mapChipData[height][width] <= 29);
			if (blockFlag || doorFlag)
			{
				// �X�N���[���ʂ���`�悷��ʒu�����߂�B
				const float centerX = width * DRAW_MAP_CHIP_SIZE - ScrollMgr::Instance()->scrollAmount.x * ScrollMgr::Instance()->zoom;
				const float centerY = height * DRAW_MAP_CHIP_SIZE - ScrollMgr::Instance()->scrollAmount.y * ScrollMgr::Instance()->zoom;

				// ��ʊO��������`�悵�Ȃ��B
				if (centerX < -DRAW_MAP_CHIP_SIZE || centerX > WinApp::Instance()->GetWinSize().x + DRAW_MAP_CHIP_SIZE) continue;
				if (centerY < -DRAW_MAP_CHIP_SIZE || centerY > WinApp::Instance()->GetWinSize().y + DRAW_MAP_CHIP_SIZE) continue;


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
	//���̈�ԉ��̍��W��T������
	for (int y = 0; 1; ++y)
	{
		Vec2<float> chipData = { DOOR_MAPCHIP_POS.x,DOOR_MAPCHIP_POS.y + y };
		int chipNum = StageMgr::Instance()->GetMapChipBlock(STAGE_NUMBER, ROOM_NUMBER, chipData);

		//�ǂ�ǂ񉺂�T�����ăh�A����Ȃ��ꏊ�ɏo����A��O����ԉ��̔�
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
		//�������[�v�ɓ�������A�T�[�g����
		++roopCount;
		if (1000 <= roopCount)
		{
			assert(0);
		}
	}


	//���E�ǂ��炩�̔�����A�������͉������烊�X�|�[��������ꍇ-----------------------
	array<Vec2<float>, 2> checkWall;
	checkWall[0] = { doorPos.x + 1,doorPos.y };
	checkWall[1] = { doorPos.x - 1,doorPos.y };
	int rightChip = StageMgr::Instance()->GetMapChipBlock(STAGE_NUMBER, ROOM_NUMBER, checkWall[0]);
	int leftChip = StageMgr::Instance()->GetMapChipBlock(STAGE_NUMBER, ROOM_NUMBER, checkWall[1]);

	//�ǈȊO�͋󔒂Ƃ��Ĕ�����o��
	if (leftChip <= -2 || 9 <= leftChip)
	{
		leftChip = 0;
	}
	if (rightChip <= -2 || 9 <= rightChip)
	{
		rightChip = 0;
	}


	//�E�ɕǂ�����ꍇ
	if ((rightChip == 1 && leftChip == 0) || (rightChip == -1 && leftChip == 0))
	{
		*DIR = DOOR_LEFT;
		//����2�u���b�N���ꂽ���Ƀ��X�|�[��������
		return Vec2<float>((doorPos.x - 2) * 50.0f, doorPos.y * 50.0f);
	}
	//���ɕǂ�����ꍇ
	else if ((rightChip == 0 && leftChip == 1) || (rightChip == 0 && leftChip == -1))
	{
		*DIR = DOOR_RIGHT;
		//�E��2�u���b�N���ꂽ���Ƀ��X�|�[��������
		return Vec2<float>((doorPos.x + 2) * 50.0f, doorPos.y * 50.0f);
	}
	//���E�ǂ���Ƃ��ǂ����������ꍇ
	else if (rightChip == 0 && leftChip == 0)
	{
		*DIR = DOOR_Z;
		//�����W�Ƀ��X�|�[��������
		return Vec2<float>(doorPos.x * 50.0f, doorPos.y * 50.0f);
	}
	//���E�ǂ��炩�̔�����A�������͉������烊�X�|�[��������ꍇ-----------------------


	//�㉺�ǂ��炩�̔����烊�X�|�[��������ꍇ-----------------------
	//��ԍ��ƈ�ԉE�̔��u���b�N��T������
	vector<Vec2<float>>doorBlock;//�h�A�u���b�N�̉�
	Vec2<float> rightDoor, leftDoor;//��Ԓ[�̔����W��T������

	roopCount = 0;
	//��ԍ��̔����W�T��
	for (int i = 0; 1; ++i)
	{
		Vec2<float> chipData = { DOOR_MAPCHIP_POS.x - i,DOOR_MAPCHIP_POS.y };
		int chipNum = StageMgr::Instance()->GetMapChipBlock(STAGE_NUMBER, ROOM_NUMBER, chipData);
		if (chipNum != DOOR_NUMBER)
		{
			leftDoor = { DOOR_MAPCHIP_POS.x - i + 1,DOOR_MAPCHIP_POS.y };
			break;
		}

		//�������[�v�ɓ�������A�T�[�g����
		++roopCount;
		if (1000 <= roopCount)
		{
			assert(0);
		}
	}

	roopCount = 0;
	//��ԉE�̔����W�T��
	for (int i = 0; 1; ++i)
	{
		Vec2<float> chipData = { DOOR_MAPCHIP_POS.x + i,DOOR_MAPCHIP_POS.y };
		int chipNum = StageMgr::Instance()->GetMapChipBlock(STAGE_NUMBER, ROOM_NUMBER, chipData);
		if (chipNum != DOOR_NUMBER)
		{
			rightDoor = { DOOR_MAPCHIP_POS.x + i - 1,DOOR_MAPCHIP_POS.y };
			break;
		}

		//�������[�v�ɓ�������A�T�[�g����
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

	//�㉺�̕ǂ𒲂ׂ�
	array<Vec2<float>, 2> checkTopWall;
	checkTopWall[0] = { rightDoor.x,rightDoor.y - 1 };
	checkTopWall[1] = { rightDoor.x,rightDoor.y + 1 };
	int topWall = StageMgr::Instance()->GetMapChipBlock(STAGE_NUMBER, ROOM_NUMBER, checkTopWall[0]);
	int downWall = StageMgr::Instance()->GetMapChipBlock(STAGE_NUMBER, ROOM_NUMBER, checkTopWall[1]);


	//�����Ƃ��ǂ�����A��ɋ�Ԃ�����Ȃ�
	if (rightChip != 0 && leftChip != 0 && topWall == 0)
	{
		//�ǂ��������Ƀ��X�|�[��������
		Vec2<float> check(leftDoor.x - 2, leftDoor.y - 1);
		Vec2<float> check2(rightDoor.x + 2, rightDoor.y - 1);

		int num = StageMgr::Instance()->GetMapChipBlock(STAGE_NUMBER, ROOM_NUMBER, check);
		int num2 = StageMgr::Instance()->GetMapChipBlock(STAGE_NUMBER, ROOM_NUMBER, check2);


		//���ɍs�����E�ɍs�����ŕς��
		//bool leftHaveSpaceFlag = num == 0 && 1 == 1;
		//bool rightHaveSpaceFlag = num2 == 0 && 1 == 1;

		////�ǂ�������false�Ȃ甽�Α���true�ɂ���
		//if (!leftHaveSpaceFlag)
		//{
		//	rightHaveSpaceFlag = true;
		//}
		//if (!rightHaveSpaceFlag)
		//{
		//	leftHaveSpaceFlag = true;
		//}



		//���ɕǂ��������ꍇ�A�E�̕ǂɃ��X�|�[��������B
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
	//�����Ƃ��ǂ�����A���ɋ�Ԃ�����Ȃ�
	else if (rightChip != 0 && leftChip != 0 && downWall == 0)
	{
		*DIR = DOOR_DOWN;
		//���S���W�Ƀ��X�|�[��������	(���E�̃h�A�̋����̔��������h�A�`�b�v���瑫��)
		Vec2<float>centralPos(leftDoor + (Vec2<float>(rightDoor - leftDoor) / 2.0f));
		centralPos *= Vec2<float>(50.0f, 50.0f);

		return centralPos;
	}
	//�㉺�ǂ��炩�̔����烊�X�|�[��������ꍇ-----------------------

	if (!ONLY_GET_DOOR_DIR)
	{
		string result = "���ɂȂ���h�A��������܂���B\nRalation�t�@�C�����m�F���邩�A�S���̑�΂ɘA�������������B";
		MessageBox(NULL, KuroFunc::GetWideStrFromStr(result).c_str(), TEXT("�h�A��������܂���"), MB_OK);
	}

	//���s
	*DIR = DOOR_NONE;
	return Vec2<float>(-1, -1);
}

Vec2<float> Game::GetDoorPos(const int &DOOR_NUMBER, const vector<vector<int>> &MAPCHIP_DATA)
{
	Vec2<float> door;
	//���ɂȂ���h�A��T��
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
				doorBlocks[doorBlocksArrayNum]->Init(data[i]->leftUpPos, data[i]->rightDownPos, doorIndex);
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
	giveDoorNumber = 0;
	debugStageData[0] = stageNum;
	debugStageData[1] = roomNum;

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
				player.Init(Vec2<float>(x * 50.0f, y * 50.0f));
				ScrollMgr::Instance()->WarpScroll(player.centerPos);
				responeFlag = true;
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

	// �e�Ǘ��N���X���������B
	BulletMgr::Instance()->Setting();

	movingBlockGraph = TexHandleMgr::LoadGraph("resource/IntoTheAbyss/MovingBlock.png");
	//sceneChangeHandle = TexHandleMgr::LoadGraph("resource/IntoTheAbyss/SceneChange.png");
	//movingBlockGraph = D3D12App::Instance()->GenerateTextureBuffer("resource/IntoTheAbyss/MovingBlock.png");

	// �e�p�[�e�B�N�����Z�b�e�B���O�B
	//BulletParticleMgr::Instance()->Setting();

	//���C�g���
	ptLig.SetInfluenceRange(PT_LIG_RANGE);
	spotLig.SetInfluenceRange(SPOT_LIG_RANGE);

	ligMgr.RegisterPointLight(&ptLig);
	ligMgr.RegisterSpotLight(&spotLig);
	ligMgr.RegisterHemiSphereLight(&hemiLig);

	Init();
}

void Game::Init()
{
	// �X�N���[���}�l�[�W���[���������B
	ScrollMgr::Instance()->Init(&mapData);
	// �X�N���[���ʂ�ݒ�B
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



#pragma region �X�e�[�W�̐؂�ւ�
	stageNum = SelectStage::Instance()->GetStageNum();

	bool enableToSelectStageFlag = 0 < debugStageData[0];
	bool enableToSelectStageFlag2 = debugStageData[0] < StageMgr::Instance()->GetMaxStageNumber() - 1;
	//�}�b�v�̐؂�ւ�
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
	//�����̐؂�ւ�
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

	//�������ԍ��ɐ؂�ւ�
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
		//�f�o�b�N�p�̃}�b�v�`�b�v�ԍ�����X�^�[�g����
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


	//�S�[���ɐG�ꂽ�玟�̃X�e�[�W�Ɍ���������
	{
		Vec2<float> playerChip((player.centerPos.x + 25.0f) / 50.0f, (player.centerPos.y + 25.0f) / 50.0f);
		//�S�[���ɐG�ꂽ�玟�̃X�e�[�W�Ɍ�����
		if (StageMgr::Instance()->GetMapChipBlock(stageNum, roomNum, playerChip) == MAPCHIP_BLOCK_GOAL)
		{
			++stageNum;
			roomNum = 0;

			mapData = StageMgr::Instance()->GetMapChipData(stageNum, roomNum);
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
			ScrollMgr::Instance()->WarpScroll(player.centerPos);
		}
	}


#pragma region �h�A�ړ�
	for (int doorIndex = 0; doorIndex < doorBlocks.size(); ++doorIndex)
	{
		//�����蔻��
		bool hitFlag = doorBlocks[doorIndex]->Collision(player.centerPos, player.PLAYER_HIT_SIZE, player.vel, player.prevFrameCenterPos);

		//�h�A�Ɠ��������琔����n��
		if (hitFlag)
		{
			giveDoorNumber = doorBlocks[doorIndex]->chipNumber;
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
		int nowChip = StageMgr::Instance()->GetMapChipBlock(stageNum, roomNum, Vec2<float>((player.centerPos + Vec2<float>(0.0f, -MAP_CHIP_HALF_SIZE) + offset) / Vec2<float>(MAP_CHIP_SIZE, MAP_CHIP_SIZE)));
		int nowChip2 = StageMgr::Instance()->GetMapChipBlock(stageNum, roomNum, Vec2<float>((player.centerPos + Vec2<float>(0.0f, MAP_CHIP_HALF_SIZE) + offset) / Vec2<float>(MAP_CHIP_SIZE, MAP_CHIP_SIZE)));

		//�h�A�ړ�����
		bool outSideFlag = nowChip == -1 && nowChip2 == -1;//�v���C���[����ʊO�ɏo������
		if (outSideFlag)
		{
			sceneBlackFlag = true;
			doorNumber = giveDoorNumber;
			player.drawCursorFlag = false;
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
					int localRoomNum = StageMgr::Instance()->GetRelationData(stageNum, roomNum, doorNumber - chipMemorySize.min);

					RoomMapChipArray tmpMapData = StageMgr::Instance()->GetMapChipData(stageNum, localRoomNum);
					//�h�A���W�����
					Vec2<float>doorPos = GetDoorPos(doorNumber, mapData);
					//�v���C���[�����X�|�[��������W�����
					responePos = GetPlayerResponePos(stageNum, localRoomNum, doorNumber, doorPos, &door);

					//���X�|�[���G���[�Ȃ�]�ڂ��Ȃ�
					if (responePos.x != -1.0f && responePos.y != -1.0f)
					{
						SelectStage::Instance()->SelectRoomNum(localRoomNum);

						sceneChangingFlag = true;
						//��ʊO����o�ꂳ����
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
		alphaValue -= 10;
		bool goFlag = false;

		//�v���C���[�𓮂���
		if (alphaValue <= 250)
		{
			if (!sceneChangeDeadFlag)
			{
				sceneChangingFlag = false;
				switch (door)
				{
				case Game::DOOR_UP_GORIGHT:
					//�W�����v����
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
					//�W�����v����
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
					//�h�A���W�����
					Vec2<float>doorPos(GetDoorPos(doorNumber, mapData));
					//�v���C���[�����X�|�[��������W�����
					responePos = GetPlayerResponePos(stageNum, roomNum, doorNumber, doorPos, &door);

					//�v���C���[�����X�|�[��������
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
	}
	//�V�[���J��-----------------------
#pragma endregion

	if (UsersInput::Instance()->OnTrigger(DIK_SPACE))
	{
		SelectStage::Instance()->resetStageFlag = true;
		//player.isDead = true;
		//sceneBlackFlag = true;
		//sceneChangeDeadFlag = player.isDead;
	}



	//�X�e�[�W���̐؂�ւ�����
	//�����̏�����
	if ((roomNum != oldRoomNum || stageNum != oldStageNum) || SelectStage::Instance()->resetStageFlag)
	{
		InitGame(SelectStage::Instance()->GetStageNum(), SelectStage::Instance()->GetRoomNum());
	}
	oldRoomNum = roomNum;
	oldStageNum = stageNum;

	//�C�x���g�u���b�N�Ƃ̔���
	for (int i = 0; i < eventBlocks.size(); ++i)
	{
		eventBlocks[i]->HitBox(player.centerPos, player.PLAYER_HIT_SIZE, player.vel, player.prevFrameCenterPos);
	}

	//���u���b�N�Ƃ̔���
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



	// �v���C���[�̍X�V����
	player.Update(mapData);

	//if (Input::isKey(KEY_INPUT_RIGHT)) player.centerPos.x += 1.0f;
	if (UsersInput::Instance()->Input(DIK_RIGHT)) player.centerPos.x += 1.0f;
	//if (Input::isKey(KEY_INPUT_P)) player.centerPos.x += 100.0f;
	if (UsersInput::Instance()->OnTrigger(DIK_P)) player.centerPos.x += 100.0f;
	//if (Input::isKey(KEY_INPUT_LEFT)) player.centerPos.x -= 1.0f;
	if (UsersInput::Instance()->Input(DIK_LEFT)) player.centerPos.x -= 1.0f;
	//if (Input::isKey(KEY_INPUT_O)) player.centerPos.x -= 100.0f;
	if (UsersInput::Instance()->OnTrigger(DIK_O)) player.centerPos.x -= 100.0f;

	// �G�̍X�V����
	enemy.Update();

	// �X�N���[���ʂ̍X�V����
	ScrollMgr::Instance()->Update();

	// �V�F�C�N�ʂ̍X�V����
	ShakeMgr::Instance()->Update();

	// ���I�u���b�N�̍X�V����
	MovingBlockMgr::Instance()->Update(player.centerPos);

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


	/*===== �����蔻�� =====*/

	// �h�b�X���u���b�N�̓����蔻��
	for (int index = 0; index < DOSSUN_COUNT; ++index) {
		dossunBlock[index].CheckHit(mapData);
	}

	// �v���C���[�̓����蔻��
	player.CheckHit(mapData, bubbleBlock, dossunBlock);

	// ���I�u���b�N�̓����蔻��
	MovingBlockMgr::Instance()->CheckHit(mapData);

	// �e�ƃ}�b�v�`�b�v�̓����蔻��
	BulletMgr::Instance()->CheckHit(mapData, bubbleBlock);

	// �r���[�|�[�g���v���C���[��ňړ�������B
	ViewPort::Instance()->SetPlayerPosX(player.centerPos.x);
	ViewPort::Instance()->SetPlayerPosY(player.centerPos.y);

	// �I�[���u���b�N�̃f�[�^�ƃr���[�|�[�g�̔�����s���B
	ViewPort::Instance()->SavePrevFlamePos();

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

			// �X�N���[�����~�߂�B
			ScrollMgr::Instance()->StopScroll(0);
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
			ScrollMgr::Instance()->StopScroll(0);
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
			ScrollMgr::Instance()->StopScroll(0);
			++countStopNum;
		}
		else
		{
			ViewPort::Instance()->holdFlags[ViewPort::Instance()->LEFT] = false;
		}


	}
	ViewPort::Instance()->playerPos = player.centerPos;
	//���C�g�X�V
	auto pos = player.GetCenterDrawPos();
	ptLig.SetPos({ pos.x,pos.y,PT_LIG_Z });

	spotLig.SetTarget({ pos.x,pos.y + SPOT_LIG_TARGET_OFFSET_Y,0.0f });
	spotLig.SetPos({ pos.x,pos.y + SPOT_LIG_TARGET_OFFSET_Y,SPOT_LIG_Z });

	//�p�[�e�B�N���X�V
	ParticleMgr::Instance()->Update();
}

void Game::Draw(std::weak_ptr<RenderTarget>EmissiveMap)
{
	int stageNum = SelectStage::Instance()->GetStageNum();
	int roomNum = SelectStage::Instance()->GetRoomNum();

	/*===== �`�揈�� =====*/
	mapChipDrawData = StageMgr::Instance()->GetMapChipDrawBlock(stageNum, roomNum);
	DrawMapChip(mapData, mapChipDrawData, mapBlockGraph, stageNum, roomNum);

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
