#pragma once
#include"../KuroEngine.h"
#include"../Common/Singleton.h"
#include"../IntoTheAbyss/StageMgr.h"
#include"DrawMap.h"
#include<vector>
#include<memory>
#include"LocalScrollMgr.h"
#include"LocalCamera.h"

class DrawMapChipForSceneChange
{
public:
	DrawMapChipForSceneChange();

	void Init(int STAGE_NUM, bool SCENE_CHANGE_FLAG);
	void Finalize();
	void Update();
	void Draw();

	std::shared_ptr<RenderTarget> mapBuffer;
private:
	void DrawMapChip(const vector<vector<int>> &mapChipData, vector<vector<MapChipDrawData>> &mapChipDrawData, const int &stageNum, const int &roomNum);

	int stageNum;
	std::vector<DrawMap>drawMap;
	bool isSS;
	Vec2<float>centralPos,playerPos,bossPos;


	//ã÷ífÇÃãZÅAéûíZÇÃà◊égóp
	LocalScrollMgr scroll;
	std::shared_ptr<LocalCamera> camera;

	bool sceneChageFlag;

	RoomMapChipArray mapChip;
	RoomMapChipDrawArray mapChipDraw;

};
