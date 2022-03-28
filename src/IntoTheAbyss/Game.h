#pragma once
#include"Vec.h"
#include<vector>
#include "StageMgr.h"
using namespace std;
#include "Player.h"
#include "Enemy.h"
#include"AuraBlock.h"
#include "DossunBlock.h"
#include "Bubble.h"
#include"GimmickLoader.h"
#include"EventBlock.h"

#include<memory>
class TextureBuffer;

#include"DrawMap.h"
#include"LightManager.h"
class RenderTarget;
#include"MassChip.h"
#include"DoorBlock.h"

#include"ThornBlock.h"

struct MassChipData
{
	Vec2<float>leftUpPos;
	Vec2<float>rightDownPos;
	bool sideOrUpDownFlag;

	MassChipData(const Vec2<float> &LEFT_UP_POS, const Vec2<float> &RIGHT_DOWN_POS, const bool &SIDE_OR_UPDOWN_FLAG) :
		leftUpPos(LEFT_UP_POS), rightDownPos(RIGHT_DOWN_POS), sideOrUpDownFlag(SIDE_OR_UPDOWN_FLAG)
	{
	}
};


//���\�����[�V������main�������܂Ƃ߂�����
class Game
{
	enum E_DOOR_DIR
	{
		DOOR_UP_GORIGHT,		//��ɏo�ĉE�ɍs���h�A
		DOOR_UP_GOLEFT,	//��ɏo�č��ɍs���h�A
		DOOR_DOWN,	//���ɏo��h�A
		DOOR_LEFT,	//���ɏo��h�A
		DOOR_RIGHT,	//�E�ɏo��h�A
		DOOR_Z,		//�^�񒆂���o��h�A
		DOOR_NONE,		//�h�A����
		DOOR_MAX
	};

	std::vector<std::unique_ptr<MassChipData>> AddData(RoomMapChipArray MAPCHIP_DATA, const int &CHIP_NUM);
	void DrawMapChip(const vector<vector<int>> &mapChipData, vector<vector<MapChipDrawData>> &mapChipDrawData, const int &mapBlockGraph, const int &stageNum, const int &roomNum);
	Vec2<float> GetPlayerResponePos(const int &STAGE_NUMBER, const int &ROOM_NUMBER, const int &DOOR_NUMBER, Vec2<float> DOOR_MAPCHIP_POS, E_DOOR_DIR *DIR, const bool &ONLY_GET_DOOR_DIR = false);
	Vec2<float> GetDoorPos(const int &DOOR_NUMBER, const vector<vector<int>> &MAPCHIP_DATA);
	const int &GetChipNum(const vector<vector<int>> &MAPCHIP_DATA, const int &MAPCHIP_NUM, int *COUNT_CHIP_NUM, Vec2<float> *POS);


	int mapBlockGraph;
	// ���I�u���b�N�̉摜�B
	int movingBlockGraph;

	// �v���C���[
	Player player;

	// �G
	Enemy enemy;

	// �}�b�v�`�b�v�̃f�[�^
	vector<vector<int>> mapData;

	vector<DossunBlock> dossunBlock;
	vector<Bubble> bubbleBlock;

	int nowSelectNum = 0;


	int oldStageNum = -1;
	int oldRoomNum = -1;

	vector<std::unique_ptr<AuraBlock>> auraBlock;
	vector<vector<MapChipDrawData>> mapChipDrawData;
	std::vector<std::unique_ptr<DoorBlock>> doorBlocks;
	std::vector<std::unique_ptr<ThornBlock>> thornBlocks;

	int countStopNum = 0;
	int countHitNum = 0;

	Vec2<float> prevPlayerChipPos;

	bool sceneBlackFlag;//�t���O�����ƈÓ]����
	bool sceneLightFlag;//�t���O�����Ɩ��]����
	bool sceneUpDownFlag;
	bool sceneChangingFlag;
	int alphaValue;
	int timer;
	Vec2<float> responePos;
	int doorNumber;
	int giveDoorNumber;
	int sceneChangeHandle;//�V�[���J�ڗp�̉摜
	E_DOOR_DIR door;		//�ǂ̕����ŏo�邩
	bool initJumpFlag;
	float gravity;
	E_DOOR_DIR doorDir;
	bool sceneChangeDeadFlag;//�v���C��������ł�����t���O�𗧂ăV�[���J�ڒ��ɓ���ȏ���������
	bool initDeadFlag;

	std::vector<std::unique_ptr<MassChip>> massChipData;
	std::vector<std::unique_ptr<EventBlock>>eventBlocks;


	std::vector<DrawMap>drawMap;

	//���C�g���
	LightManager ligMgr;
	const float PT_LIG_RANGE = 128.0;
	const float PT_LIG_BRIGHT = 0.3f;
	const float PT_LIG_Z = -2.0f;
	Light::Point ptLig;
	const float SPOT_LIG_RANGE = 128.0;
	const float SPOT_LIG_TARGET_OFFSET_Y = -10.0f;
	const float SPOT_LIG_Z = -20.0f;
	Light::Spot spotLig;
	Light::HemiSphere hemiLig;


	void InitGame(const int &STAGE_NUM, const int &ROOM_NUM);

public:

	array<int, 2> debugStageData = { 0,0 };//�f�o�b�N�p�̃X�e�[�W�ƕ����ԍ�


	Game();
	void Init();
	void Update();
	void Draw(std::weak_ptr<RenderTarget>EmissiveMap);

	LightManager &GetLigManager() { return ligMgr; }


};

