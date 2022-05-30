#pragma once
#include"../KuroEngine.h"
#include"../Common/Singleton.h"
#include"../IntoTheAbyss/StageMgr.h"
#include"DrawMap.h"
#include<vector>
#include<memory>
#include"LocalScrollMgr.h"
#include"LocalCamera.h"
#include"CRT.h"

class Sprite;

class DrawMapChipForSceneChange
{
public:
	DrawMapChipForSceneChange();

	void Init(int STAGE_NUM, bool SCENE_CHANGE_FLAG);
	void Finalize();
	void Update();
	void Draw(const float& ChangeRate);

	std::shared_ptr<RenderTarget> mapBuffer;
private:
	void DrawMapChip(const vector<vector<int>> &mapChipData, vector<vector<MapChipDrawData>> &mapChipDrawData, const int &stageNum, const int &roomNum);

	int stageNum;
	std::vector<DrawMap>drawMap;
	bool isSS;
	Vec2<float>centralPos,playerPos,bossPos;


	//ã÷ífÇÃãZÅAéûíZÇÃà◊égóp
	//std::shared_ptr<LocalScrollMgr> scroll;
	//LocalCamera camera;

	bool sceneChageFlag;

	RoomMapChipArray mapChip;
	RoomMapChipDrawArray mapChipDraw;

	CRT crt;

	std::vector<int>stageComment;
	std::shared_ptr<Sprite>commentSprite;
};
