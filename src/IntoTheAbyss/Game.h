#pragma once
#include"Vec.h"
#include<vector>
#include "StageMgr.h"
using namespace std;

#include"GimmickLoader.h"

#include<memory>
class TextureBuffer;

#include"DrawMap.h"
class RenderTarget;
#include"MassChip.h"

#include"ThornBlock.h"
#include"HomeBase.h"
#include"MapChipCollider.h"

#include"MiniMap.h"

#include"RoundChangeEffect.h"
#include"ScreenEdgeEffect.h"

#include"NavigationAI.h"
#include"CharacterAI.h"

#include"BossHandMgr.h"

#include "Barrages.h"

class CharacterInterFace;

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
	void DrawMapChip(const vector<vector<int>>& mapChipData, vector<vector<MapChipDrawData>>& mapChipDrawData, const int& stageNum, const int& roomNum);
	const int& GetChipNum(const vector<vector<int>>& MAPCHIP_DATA, const int& MAPCHIP_NUM, int* COUNT_CHIP_NUM, Vec2<float>* POS);

	// ボスorプレイヤーが引っかかっているかのフラグ
	bool isCatchMapChipBoss;
	bool isCatchMapChipPlayer;

	// ボスプレイヤー間の線
	Vec2<float> lineCenterPos;			// 紐の中心点
	Vec2<float> prevLineCenterPos;		// 前フレームの紐の中心点

	// マップチップのデータ
	vector<vector<int>>* mapData;

	int oldStageNum = -1;
	int oldRoomNum = -1;

	vector<vector<MapChipDrawData>>* mapChipDrawData;
	int prevDrawChipStageNum;
	int prevDrawChipRoomNum;

	int countStopNum = 0;
	int countHitNum = 0;

	Vec2<float> prevPlayerChipPos;

	bool sceneUpDownFlag;
	bool sceneChangingFlag;
	int alphaValue;
	int timer;
	Vec2<float> responePos;
	Vec2<float> restartPos;
	int sceneChangeHandle;//シーン遷移用の画像
	bool sceneChangeDeadFlag;//プレイヤが死んでいたらフラグを立てシーン遷移中に特殊な処理を入れる
	bool initDeadFlag;

	int restartTimer;//シーン遷移後から何秒経過しているか

	std::vector<std::unique_ptr<MassChip>> massChipData;


	std::vector<DrawMap>drawMap;

	//試合遷移
	bool roundFinishFlag;		//ラウンド終了時の演出開始用のフラグ
	bool readyToStartRoundFlag; //ラウンド開始時の演出開始用のフラグ
	bool gameStartFlag;			//ゲーム開始中のフラグ

	float lineExtendScale;
	const float lineExtendMaxScale = 10.0f;


	int roundTimer;
	int bgm;

	bool firstLoadFlag;
	void InitGame(const int& STAGE_NUM, const int& ROOM_NUM);

	MiniMap miniMap;
	Vec2<float>cameraBasePos;

	RoundChangeEffect roundChangeEffect;
	bool playerOrEnemeyWinFlag;

	bool turnResultScene = false;


	Color areaHitColor;
	Color playerHitColor;

	ScreenEdgeEffect screenEdgeEffect;

	Vec2<float>responeScrollPos;

	//登場演出を行うかのフラグ
	bool practiceMode = true;

	std::unique_ptr<BossHandMgr> bossHandMgr;
	std::unique_ptr<BossHandMgr> playerHandMgr;
public:
	HomeBase playerHomeBase, enemyHomeBase;

	array<int, 2> debugStageData = { 0,0 };//デバック用のステージと部屋番号
	int nowSelectNum = 0;

	Game();
	void Init(const bool& PractiveMode = false);
	void Update(const bool& Loop = false);
	void Draw();
	void Scramble();
	void CalCenterPos();

	Vec2<float>GetStageSize();
	const bool& TurnResultScene() { return turnResultScene; }


	// ステージ切り替え関数
	void SwitchingStage();

	// 陣地の判定
	void DeterminationOfThePosition();

	// ラウンド開始時演出
	void RoundStartEffect(const bool& Loop, const RoomMapChipArray& tmpMapData);

	// ラウンド終了時演出
	void RoundFinishEffect(const bool& Loop);

};

