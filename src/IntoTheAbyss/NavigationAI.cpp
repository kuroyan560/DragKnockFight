#include "NavigationAI.h"
#include"../Engine/DrawFunc.h"
#include"ScrollMgr.h"

void NavigationAI::Init(const RoomMapChipArray &MAP_DATA)
{
	const Vec2<float> MAPCHIP_SIZE = { 50.0f,50.0f };

	SizeData wallMemorySize = StageMgr::Instance()->GetMapChipSizeData(MAPCHIP_TYPE_STATIC_BLOCK);

	//座標の設定--------------------------
	int xNum = (MAP_DATA[0].size() * MAPCHIP_SIZE.x) / WAYPOINT_MAX_Y;
	int yNum = (MAP_DATA.size() * MAPCHIP_SIZE.y) / WAYPOINT_MAX_Y;

	for (int y = 0; y < WAYPOINT_MAX_Y; ++y)
	{
		for (int x = 0; x < WAYPOINT_MAX_X; ++x)
		{
			//一度ワールド座標に変換してからマップチップ座標に変換する
			const Vec2<float> worldPos(xNum * x, yNum * y);
			const Vec2<int> mapChipPos(GetMapChipNum(worldPos / MAPCHIP_SIZE));
			if (MAP_DATA[0].size() <= mapChipPos.x || MAP_DATA.size() <= mapChipPos.y)
			{
				continue;
			}

			float radisu = 3.0f;
			//32が真ん中
			if (mapChipPos.y == 25 && mapChipPos.x == 32)
			{
				radisu = 10.0f;
			}


			bool wallFlag = false;
			//壁のある場所にポイントを設置しない
			for (int wallIndex = wallMemorySize.min; wallIndex < wallMemorySize.max; ++wallIndex)
			{
				if (MAP_DATA[mapChipPos.y][mapChipPos.x] == wallIndex)
				{
					wallFlag = true;
					break;
				}
			}



			if (!wallFlag)
			{
				//等間隔で開ける
				wayPoints[y][x].pos = worldPos;
				wayPoints[y][x].radius = radisu;
				wayPoints[y][x].handle = { x,y };
			}
		}
	}
	//座標の設定--------------------------


	//近くにあるウェイポイントへの繋ぎ--------------------------
	for (int y = 0; y < WAYPOINT_MAX_Y; ++y)
	{
		for (int x = 0; x < WAYPOINT_MAX_X; ++x)
		{
			
		}
	}
	//近くにあるウェイポイントへの繋ぎ--------------------------

	//ゴール地点への探索--------------------------
	//ゴール地点への探索--------------------------
}

void NavigationAI::Update(const Vec2<float> &POS)
{

}

void NavigationAI::Draw()
{
	//デバックの時のみ表示
#ifdef  _DEBUG
	//ウェイポイントの描画
	for (int y = 0; y < wayPoints.size(); ++y)
	{
		for (int x = 0; x < wayPoints[y].size(); ++x)
		{
			DrawFunc::DrawCircle2D(ScrollMgr::Instance()->Affect(wayPoints[y][x].pos), wayPoints[y][x].radius, Color(255, 255, 255, 255));
		}
	}
#endif
}

inline const Vec2<int> &NavigationAI::GetMapChipNum(const Vec2<float> &WORLD_POS)
{
	//浮動小数点を切り下げる処理
	Vec2<float> num =
	{
		floor(WORLD_POS.x),
		floor(WORLD_POS.y)
	};
	//引数の値と切り下げた値分の差分を求め、小数点を得る
	Vec2<float>checkNumebr = WORLD_POS - num;

	//小数点が0.5未満なら切り下げ、それ以外は切り上げる
	Vec2<int> result;
	if (checkNumebr.x < 0.5f)
	{
		result =
		{
			static_cast<int>(floor(WORLD_POS.x)),
			static_cast<int>(floor(WORLD_POS.y))
		};
	}
	else
	{
		result =
		{
			static_cast<int>(ceil(WORLD_POS.x)),
			static_cast<int>(ceil(WORLD_POS.y))
		};
	}

	return result;
}
