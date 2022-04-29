#include "NavigationAI.h"
#include"../Engine/DrawFunc.h"
#include"ScrollMgr.h"
#include"../Engine/UsersInput.h"

const float NavigationAI::SERACH_RADIUS = 200.0f;

void NavigationAI::Init(const RoomMapChipArray &MAP_DATA)
{
	SizeData wallMemorySize = StageMgr::Instance()->GetMapChipSizeData(MAPCHIP_TYPE_STATIC_BLOCK);

	//座標の設定--------------------------
	int xNum = (MAP_DATA[0].size() * MAP_CHIP_SIZE) / WAYPOINT_MAX_Y;
	int yNum = (MAP_DATA.size() * MAP_CHIP_SIZE) / WAYPOINT_MAX_Y;

	for (int y = 0; y < WAYPOINT_MAX_Y; ++y)
	{
		for (int x = 0; x < WAYPOINT_MAX_X; ++x)
		{
			//一度ワールド座標に変換してからマップチップ座標に変換する
			const Vec2<float> worldPos(xNum * x, yNum * y);
			const Vec2<int> mapChipPos(GetMapChipNum(worldPos / MAP_CHIP_SIZE));
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
			if (DontUse(wayPoints[y][x]))
			{
				SphereCollision data;
				data.center = &wayPoints[y][x].pos;
				data.radius = SERACH_RADIUS;
				RegistHandle(data, &wayPoints[y][x]);
			}
		}
	}
	//近くにあるウェイポイントへの繋ぎ--------------------------

	//ゴール地点への探索--------------------------
	//ゴール地点への探索--------------------------

	wayPointFlag = true;
	serachFlag = true;
	lineFlag = true;

	checkingHandle = { -1,-1 };
	checkTimer = 0;
}

void NavigationAI::Update(const Vec2<float> &POS)
{
	Vec2<float>pos = UsersInput::Instance()->GetMousePos();

	for (int y = 0; y < wayPoints.size(); ++y)
	{
		for (int x = 0; x < wayPoints[y].size(); ++x)
		{
			if (DontUse(wayPoints[y][x]))
			{
				SphereCollision data1, data2;

				//スクリーン座標と判定を取る為にスクリーン座標に変換する
				data1.center = &ScrollMgr::Instance()->Affect(wayPoints[y][x].pos);
				data1.radius = wayPoints[y][x].radius;
				data2.center = &pos;
				data2.radius = 10.0f;

				if (BulletCollision::Instance()->CheckSphereAndSphere(data1, data2))
				{
					checkingHandle = wayPoints[y][x].handle;
					checkTimer = 0;
					break;
				}
			}
		}
	}

	++checkTimer;
	if (60 * 5 <= checkTimer)
	{
		checkingHandle = { -1,-1 };
	}
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
			if (DontUse(wayPoints[y][x]))
			{
				if (wayPointFlag)
				{
					//その場所がマウスカーソルと合ったら確認中の場所だと認識する--------------
					bool isCheckingFlag = checkingHandle.x == wayPoints[y][x].handle.x && checkingHandle.y == wayPoints[y][x].handle.y;
					Color color(255, 255, 255, 255);
					if (isCheckingFlag)
					{
						color = Color(255, 0, 0, 255);
					}
					//その場所がマウスカーソルと合ったら確認中の場所だと認識する--------------

					//ウェイポイントの描画
					DrawFunc::DrawCircle2D(ScrollMgr::Instance()->Affect(wayPoints[y][x].pos), wayPoints[y][x].radius, color);
				}
				//探索範囲の描画
				if (serachFlag)
				{
					DrawFunc::DrawCircle2D(ScrollMgr::Instance()->Affect(wayPoints[y][x].pos), SERACH_RADIUS, Color(255, 255, 255, 255));
				}
			}
		}
	}

	//繋がりの描画
	if (lineFlag)
	{
		for (int y = 0; y < wayPoints.size(); ++y)
		{
			for (int x = 0; x < wayPoints[y].size(); ++x)
			{
				if (DontUse(wayPoints[y][x]))
				{
					//登録したハンドルから線を繋げる
					for (int i = 0; i < wayPoints[y][x].wayPointHandles.size(); ++i)
					{
						Vec2<int> endHandle = wayPoints[y][x].wayPointHandles[i];
						DrawFunc::DrawLine2D
						(
							ScrollMgr::Instance()->Affect(wayPoints[y][x].pos),
							ScrollMgr::Instance()->Affect(wayPoints[endHandle.y][endHandle.x].pos),
							Color(0, 155, 0, 255)
						);
					}
				}
			}
		}
	}

	//マウスカーソル
	DrawFunc::DrawCircle2D(UsersInput::Instance()->GetMousePos(), 10.0f, Color(0, 0, 255, 255));
#endif
}

void NavigationAI::ImGuiDraw()
{
	ImGui::Begin("Navigation");
	ImGui::Checkbox("SearchCircle", &serachFlag);
	ImGui::Checkbox("WayPoint", &wayPointFlag);
	ImGui::Checkbox("MoveLine", &lineFlag);
	ImGui::End();

	if (checkingHandle.x != -1 && checkingHandle.y != -1)
	{
		Vec2<int>handle = checkingHandle;
		ImGui::Begin("WayPointData");
		ImGui::Text("Handle:X%d,Y:%d", wayPoints[handle.y][handle.x].handle.x, wayPoints[handle.y][handle.x].handle.y);
		ImGui::Text("Pos:X%f,Y:%f", wayPoints[handle.y][handle.x].pos.x, wayPoints[handle.y][handle.x].pos.y);
		ImGui::Text("Radius:%f", wayPoints[handle.y][handle.x].radius);
		ImGui::Text("RelateHandles");
		for (int i = 0; i < wayPoints[handle.y][handle.x].wayPointHandles.size(); ++i)
		{
			std::string name = "Handle:" + std::to_string(i) + ",X:" +
				std::to_string(wayPoints[handle.y][handle.x].wayPointHandles[i].x) + ",Y:" +
				std::to_string(wayPoints[handle.y][handle.x].wayPointHandles[i].y);
			ImGui::Text(name.c_str());
		}
		ImGui::End();
	}
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

inline bool NavigationAI::DontUse(const WayPointData &DATA)
{
	return DATA.handle.x != -1 && DATA.handle.y != -1;
}

inline void NavigationAI::RegistHandle(const SphereCollision &HANDLE, WayPointData *DATA)
{
	for (int y = 0; y < WAYPOINT_MAX_Y; ++y)
	{
		for (int x = 0; x < WAYPOINT_MAX_X; ++x)
		{
			//使用できるデータから判定を行う
			if (DontUse(wayPoints[y][x]))
			{
				SphereCollision data;
				data.center = &wayPoints[y][x].pos;
				data.radius = SERACH_RADIUS;

				//移動範囲内にウェイポイントがあるか
				bool serachFlag = BulletCollision::Instance()->CheckSphereAndSphere(HANDLE, data);
				//道中壁に当たっているかどうか
				bool canMoveFlag = false;
				if (serachFlag)
				{
					canMoveFlag = !CheckMapChipWallAndRay(*HANDLE.center, *data.center);
				}

				//探索範囲内&&直接行ける場所なら線を繋げる
				if (serachFlag && canMoveFlag)
				{
					wayPoints[y][x].RegistHandle({ x,y });
					DATA->RegistHandle({ x,y });
				}
			}
		}
	}
}
