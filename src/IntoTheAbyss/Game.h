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

//元ソリューションのmain処理をまとめたもの
class Game
{
	enum E_DOOR_DIR
	{
		DOOR_UP_GORIGHT,		//上に出て右に行くドア
		DOOR_UP_GOLEFT,	//上に出て左に行くドア
		DOOR_DOWN,	//下に出るドア
		DOOR_LEFT,	//左に出るドア
		DOOR_RIGHT,	//右に出るドア
		DOOR_Z,		//真ん中から出るドア
	};

	bool CheckUsedData(vector<Vec2<float>> DATA, Vec2<float> DATA2);
	void DrawMapChip(const vector<vector<int>>& mapChipData, vector<vector<MapChipDrawData>>& mapChipDrawData, const int& mapBlockGraph, const int& stageNum, const int& roomNum);
	Vec2<float> GetPlayerResponePos(const int &STAGE_NUMBER, const int &ROOM_NUMBER, const int &DOOR_NUMBER, Vec2<float> DOOR_MAPCHIP_POS, E_DOOR_DIR *DIR);
	Vec2<float> GetDoorPos(const int &DOOR_NUMBER, const vector<vector<int>> &MAPCHIP_DATA);

	int mapBlockGraph;
	// 動的ブロックの画像。
	int movingBlockGraph;

	// プレイヤー
	Player player;

	// 敵
	Enemy enemy;

	// マップチップのデータ
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

	bool sceneBlackFlag;//フラグが立つと暗転する
	bool sceneLightFlag;//フラグが立つと明転する
	bool sceneUpDownFlag;
	bool sceneChangingFlag;
	int alphaValue;
	int timer;
	Vec2<float> responePos;
	int doorNumber;
	int giveDoorNumber;
	int sceneChangeHandle;//シーン遷移用の画像
	E_DOOR_DIR door;		//どの方向で出るか
	bool initJumpFlag;
	float gravity;
	E_DOOR_DIR doorDir;

	std::vector<DrawMap>drawMap;

public:
	int stageNum = 0;//ステージ番号
	int roomNum = 0; //部屋番号

	array<int, 2> debugStageData = { 0,0 };//デバック用のステージと部屋番号


	Game();
	void Update();
	void Draw();
};

