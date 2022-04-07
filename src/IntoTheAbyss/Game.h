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
#include "Boss.h"

#include<memory>
class TextureBuffer;

#include"DrawMap.h"
#include"LightManager.h"
class RenderTarget;
#include"MassChip.h"
#include"DoorBlock.h"

#include"ThornBlock.h"
#include"HomeBase.h"

struct MassChipData
{
	Vec2<float>leftUpPos;
	Vec2<float>rightDownPos;
	bool sideOrUpDownFlag;

	MassChipData(const Vec2<float>& LEFT_UP_POS, const Vec2<float>& RIGHT_DOWN_POS, const bool& SIDE_OR_UPDOWN_FLAG) :
		leftUpPos(LEFT_UP_POS), rightDownPos(RIGHT_DOWN_POS), sideOrUpDownFlag(SIDE_OR_UPDOWN_FLAG)
	{
	}
};


//元ソリューションのmain処理をまとめたもの
class Game
{
	std::vector<std::unique_ptr<MassChipData>> AddData(RoomMapChipArray MAPCHIP_DATA, const int& CHIP_NUM);
	void DrawMapChip(const vector<vector<int>>& mapChipData, vector<vector<MapChipDrawData>>& mapChipDrawData, const int& mapBlockGraph, const int& stageNum, const int& roomNum);
	const int& GetChipNum(const vector<vector<int>>& MAPCHIP_DATA, const int& MAPCHIP_NUM, int* COUNT_CHIP_NUM, Vec2<float>* POS);


	int mapBlockGraph;
	// 動的ブロックの画像。
	int movingBlockGraph;



	// 敵
	Enemy bossEnemy;
	static const int SMALL_ENEMY = 100;
	array<Enemy, SMALL_ENEMY> smallEnemy;
	static const int NOMOVEMENT_ENEMY = 100;
	array<Enemy, NOMOVEMENT_ENEMY> noMovementEnemy;
	int enemyGenerateTimer;
	const int SMALL_GENERATE_TIMER = 60;
	int nomoveMentTimer;
	const int NOMOVEMENT_GENERATE_TIMER = 30;

	// ボス
	Boss boss;

	// ボスorプレイヤーが引っかかっているかのフラグ
	bool isCatchMapChipBoss;
	bool isCatchMapChipPlayer;

	// ボスプレイヤー間の線
	float lineLengthPlayer;				// プレイヤーの紐の長さ
	float lineLengthBoss;				// ボスの紐の長さ
	float addLineLengthPlayer;			// プレイヤーが引っかかって伸びた量
	float addLineLengthBoss;			// ボスが引っかかって伸びた量
	const float LINE_LENGTH = 150.0f;	// 紐のデフォルトの長さの半分
	Vec2<float> lineCenterPos;			// 紐の中心点
	Vec2<float> prevLineCenterPos;		// 前フレームの紐の中心点

	// マップチップのデータ
	vector<vector<int>> mapData;

	vector<DossunBlock> dossunBlock;
	vector<Bubble> bubbleBlock;


	int oldStageNum = -1;
	int oldRoomNum = -1;

	vector<std::unique_ptr<AuraBlock>> auraBlock;
	vector<vector<MapChipDrawData>> mapChipDrawData;
	std::vector<std::unique_ptr<DoorBlock>> doorBlocks;
	std::vector<std::unique_ptr<ThornBlock>> thornBlocks;

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
	Vec2<float> restartPos;
	int doorNumber;
	int giveDoorNumber;
	int doorArrayNumber;
	int sceneChangeHandle;//シーン遷移用の画像
	E_DOOR_DIR door;		//どの方向で出るか
	bool initJumpFlag;
	float gravity;
	E_DOOR_DIR doorDir;
	bool sceneChangeDeadFlag;//プレイヤが死んでいたらフラグを立てシーン遷移中に特殊な処理を入れる
	bool initDeadFlag;

	int restartTimer;//シーン遷移後から何秒経過しているか

	std::vector<std::unique_ptr<MassChip>> massChipData;
	std::vector<std::unique_ptr<EventBlock>>eventBlocks;


	std::vector<DrawMap>drawMap;

	//ライト情報
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


	//試合関連
	int countRound;						//ラウンド数
	int countPlayerWin, countEnemyWin;	//勝利数

	//試合遷移
	bool roundFinishFlag;		//ラウンド終了時の演出開始用のフラグ
	bool readyToStartRoundFlag; //ラウンド開始時の演出開始用のフラグ
	bool gameStartFlag;			//ゲーム開始中のフラグ

	void InitGame(const int& STAGE_NUM, const int& ROOM_NUM);

public:
	std::unique_ptr<HomeBase> playerHomeBase, enemyHomeBase;

	// プレイヤー
	Player player;
	array<int, 2> debugStageData = { 0,0 };//デバック用のステージと部屋番号
	int nowSelectNum = 0;

	Game();
	void Init();
	void Update();
	void Draw(std::weak_ptr<RenderTarget>EmissiveMap);
	void Scramble();

	LightManager& GetLigManager() { return ligMgr; }


};

