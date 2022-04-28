#pragma once
#include"../KuroEngine.h"
#include<vector>
#include<array>
#include"StageMgr.h"

static const int GOAL_MAX_NUM = 5;

/// <summary>
/// ウェイポイントが持つ情報
/// </summary>
struct WayPointData
{
	Vec2<int> handle;						//ハンドル
	Vec2<float> pos;						//座標
	float radius;							//大きさ
	std::vector<Vec2<int>> wayPointHandles;		//どの方向に行けるかハンドル取ったもの
	std::array<int, GOAL_MAX_NUM> passNum;	//目標地点までのパス数

	WayPointData() :handle(Vec2<int>(-1, -1))
	{
	}

	void RegistHandle(const Vec2<int> &HANDLE)
	{
		for (int i = 0; i < wayPointHandles.size(); ++i)
		{
			if (wayPointHandles[i].x == HANDLE.x && wayPointHandles[i].x == HANDLE.y)
			{
				return;
			}
		}
		wayPointHandles.push_back(HANDLE);
	}
};

class NavigationAI
{
public:
	/// <summary>
	/// ポイントの生成
	/// </summary>
	/// <param name="MAP_DATA">ステージのCSV</param>
	void Init(const RoomMapChipArray &MAP_DATA);

	void Update(const Vec2<float> &POS);

	void Draw();
private:

	static const int WAYPOINT_MAX_X = 10;
	static const int WAYPOINT_MAX_Y = 10;

	std::array<std::array<WayPointData, WAYPOINT_MAX_Y>, WAYPOINT_MAX_X> wayPoints;//ウェイポイントの配列

	inline const Vec2<int> &GetMapChipNum(const Vec2<float> &WORLD_POS);

};

