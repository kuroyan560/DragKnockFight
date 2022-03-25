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
		DOOR_NONE,		//ドア無し
		DOOR_MAX		
	};

	bool CheckUsedData(vector<Vec2<float>> DATA, Vec2<float> DATA2);
	void DrawMapChip(const vector<vector<int>>& mapChipData, vector<vector<MapChipDrawData>>& mapChipDrawData, const int& mapBlockGraph, const int& stageNum, const int& roomNum);
	Vec2<float> GetPlayerResponePos(const int &STAGE_NUMBER, const int &ROOM_NUMBER, const int &DOOR_NUMBER, Vec2<float> DOOR_MAPCHIP_POS, E_DOOR_DIR *DIR, const bool &ONLY_GET_DOOR_DIR = false);
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

	//ライト情報
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
	int stageNum = 0;//ステージ番号
	int roomNum = 0; //部屋番号

	array<int, 2> debugStageData = { 0,0 };//デバック用のステージと部屋番号


	Game();
	void Init();
	void Update();
	void Draw(std::weak_ptr<RenderTarget>EmissiveMap);

	LightManager& GetLigManager() { return ligMgr; }
};

