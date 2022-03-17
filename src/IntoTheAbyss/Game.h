#pragma once
#include"Vec.h"
#include<vector>
#include "StageMgr.h"
using namespace std;
#include "Player.h"
#include "Enemy.h"
#include"AuraBlock.h"
#include "DossunBlock.h"

#include<memory>
class TextureBuffer;

//���\�����[�V������main�������܂Ƃ߂�����
class Game
{
	bool CheckUsedData(vector<Vec2<float>> DATA, Vec2<float> DATA2);
	void DrawMapChip(const vector<vector<int>>& mapChipData, vector<vector<MapChipDrawData>>& mapChipDrawData, const int & mapBlockGraph, const int& stageNum, const int& roomNum);
	Vec2<float> GetPlayerResponePos(const int& STAGE_NUMBER, const int& ROOM_NUMBER, const int& DOOR_NUMBER, Vec2<float> DOOR_MAPCHIP_POS);
	Vec2<float> GetPlayerPos(const int& STAGE_NUMBER, int* ROOM_NUMBER, const int& DOOR_NUMBER, const SizeData& SIZE_DATA, vector<vector<int>>* MAPCHIP_DATA);

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

	DossunBlock testDossunBlock;

	int roomNum = 0;


	int nowSelectNum = 0;
	array<int, 2> debugStageData = { 0,0 };

	int stageNum = 0;//�X�e�[�W�ԍ�

	int oldStageNum = -1;
	int oldRoomNum = -1;

	vector<std::unique_ptr<AuraBlock>> auraBlock;

	vector<vector<MapChipDrawData>> mapChipDrawData;

	int countStopNum = 0;
	int countHitNum = 0;

public:
	Game();
	void Update();
	void Draw();
};

