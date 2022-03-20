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

//���\�����[�V������main�������܂Ƃ߂�����
class Game
{
	enum E_DOOR_DIR
	{
		DOOR_UP,	//��ɏo��h�A
		DOOR_DOWN,	//���ɏo��h�A
		DOOR_LEFT,	//���ɏo��h�A
		DOOR_RIGHT,	//�E�ɏo��h�A
		DOOR_Z,		//�^�񒆂���o��h�A
	};

	bool CheckUsedData(vector<Vec2<float>> DATA, Vec2<float> DATA2);
	void DrawMapChip(const vector<vector<int>>& mapChipData, vector<vector<MapChipDrawData>>& mapChipDrawData, const int& mapBlockGraph, const int& stageNum, const int& roomNum);
	Vec2<float> GetPlayerResponePos(const int &STAGE_NUMBER, const int &ROOM_NUMBER, const int &DOOR_NUMBER, Vec2<float> DOOR_MAPCHIP_POS, E_DOOR_DIR *DIR);
	Vec2<float> GetPlayerPos(const int& STAGE_NUMBER, int* ROOM_NUMBER, const int& DOOR_NUMBER, const SizeData& SIZE_DATA, vector<vector<int>>* MAPCHIP_DATA, E_DOOR_DIR *DIR);

	int mapBlockGraph;
	// ���I�u���b�N�̉摜�B
	int movingBlockGraph;

	// �v���C���[
	Player player;

	// �G
	Enemy enemy;

	// ���Ԓ�~�̒Z���̋������e�X�g����悤�̃u���b�N�B
	TimeStopTestBlock testBlock;

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
	int alphaValue;
	int timer;
	Vec2<float> responePos;
	int doorNumber;
	int sceneChangeHandle;//�V�[���J�ڗp�̉摜
	E_DOOR_DIR door;		//�ǂ̕����ŏo�邩

	std::vector<DrawMap>drawMap;

public:
	int stageNum = 0;//�X�e�[�W�ԍ�
	int roomNum = 0; //�����ԍ�

	array<int, 2> debugStageData = { 0,0 };//�f�o�b�N�p�̃X�e�[�W�ƕ����ԍ�


	Game();
	void Update();
	void Draw();
};

