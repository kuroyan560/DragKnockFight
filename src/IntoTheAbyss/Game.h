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

#include<memory>
class TextureBuffer;

#include"DrawMap.h"
#include"LightManager.h"
class RenderTarget;


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

	bool CheckUsedData(vector<Vec2<float>> DATA, Vec2<float> DATA2);
	void DrawMapChip(const vector<vector<int>>& mapChipData, vector<vector<MapChipDrawData>>& mapChipDrawData, const int& mapBlockGraph, const int& stageNum, const int& roomNum);
	Vec2<float> GetPlayerResponePos(const int &STAGE_NUMBER, const int &ROOM_NUMBER, const int &DOOR_NUMBER, Vec2<float> DOOR_MAPCHIP_POS, E_DOOR_DIR *DIR, const bool &ONLY_GET_DOOR_DIR = false);
	Vec2<float> GetDoorPos(const int &DOOR_NUMBER, const vector<vector<int>> &MAPCHIP_DATA);

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


	std::vector<DrawMap>drawMap;

	//���C�g���
	LightManager ligMgr;
	const float PT_LIG_RANGE = 128.0;
	const float PT_LIG_Z = -2.0f;
	Light::Point ptLig;
	const float SPOT_LIG_RANGE = 128.0;
	const float SPOT_LIG_TARGET_OFFSET_Y= -10.0f;
	const float SPOT_LIG_Z = -20.0f;
	Light::Spot spotLig;
	Light::HemiSphere hemiLig;


public:
	int stageNum = 0;//�X�e�[�W�ԍ�
	int roomNum = 0; //�����ԍ�

	array<int, 2> debugStageData = { 0,0 };//�f�o�b�N�p�̃X�e�[�W�ƕ����ԍ�


	Game();
	void Init();
	void Update();
	void Draw(std::weak_ptr<RenderTarget>EmissiveMap);

	LightManager& GetLigManager() { return ligMgr; }
};

