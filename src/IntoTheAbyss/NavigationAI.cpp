#include "NavigationAI.h"
#include"../Engine/DrawFunc.h"
#include"ScrollMgr.h"
#include"../Engine/UsersInput.h"
#include"../IntoTheAbyss/DebugKeyManager.h"
#include<queue>

const float NavigationAI::SERACH_RADIUS = 180.0f;
const float NavigationAI::WAYPOINT_RADIUS = 20.0f;

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
			debugColor[y][x] = Color(255, 255, 255, 255);

			//一度ワールド座標に変換してからマップチップ座標に変換する
			const Vec2<float> worldPos(xNum * x, yNum * y);
			const Vec2<int> mapChipPos(GetMapChipNum(worldPos / MAP_CHIP_SIZE));
			if (MAP_DATA[0].size() <= mapChipPos.x || MAP_DATA.size() <= mapChipPos.y)
			{
				continue;
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
				wayPoints[y][x].radius = WAYPOINT_RADIUS;
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
				data2.radius = WAYPOINT_RADIUS;

				bool hitFlag = BulletCollision::Instance()->CheckSphereAndSphere(data1, data2);

				//マウスカーソルと合ったらウェイポイントの情報を参照する
				if (hitFlag)
				{
					checkingHandle = wayPoints[y][x].handle;
					checkTimer = 0;
				}
				//右クリックしたらスタート地点に設定する
				if (hitFlag && DebugKeyManager::Instance()->DebugKeyTrigger(DIK_H, "SetStart", "DIK_H"))
				{
					startPoint = wayPoints[y][x];
				}
				//左クリックしたらゴール地点に設定する
				if (hitFlag && DebugKeyManager::Instance()->DebugKeyTrigger(DIK_N, "SetGoal", "DIK_N"))
				{
					endPoint = wayPoints[y][x];
				}
			}
		}
	}

	++checkTimer;
	if (60 * 5 <= checkTimer)
	{
		checkingHandle = { -1,-1 };
	}

	//スタート地点かゴール地点を変える際は白統一する
	if (startPoint.handle != oldStartPoint.handle || endPoint.handle != oldEndPoint.handle)
	{
		for (int y = 0; y < wayPoints.size(); ++y)
		{
			for (int x = 0; x < wayPoints[y].size(); ++x)
			{
				debugColor[y][x] = Color(255, 255, 255, 255);
			}
		}
	}

	//段階に分けて探索する色を出す
	for (int layer = 0; layer < layerNum; ++layer)
	{
		for (int num = 0; num < searchMap[layer].size(); ++num)
		{
			Vec2<int>handle = searchMap[layer][num].handle;
			debugColor[handle.y][handle.x] = searchMap[layer][num].color;
		}
	}

	//スタートとゴールを設定したらAスターの探索を開始する
	if (startPoint.handle.x != -1 && startPoint.handle.y != -1 &&
		endPoint.handle.x != -1 && endPoint.handle.y != -1)
	{
		AStart(startPoint, endPoint);
	}



	//優先度付きキュー
	std::priority_queue<std::tuple<int, int, float>> hoge;
	std::tuple<int, int, float> data1(0, 0, 10.0f);
	std::tuple<int, int, float> data2(0, 1, 10.0f);
	std::tuple<int, int, float> data3(1, 1, 10.0f);
	hoge.push(data1);
	hoge.push(data2);
	hoge.push(data3);
	std::tuple<int, int, float> data = hoge.top();
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

					bool isCheckingFlag = false;
					Color color = debugColor[y][x];

					//その場所がスタート地点なら色を変える
					isCheckingFlag = startPoint.handle.x == wayPoints[y][x].handle.x && startPoint.handle.y == wayPoints[y][x].handle.y;
					if (isCheckingFlag)
					{
						color = Color(255, 255, 0, 255);
					}

					//その場所がゴール地点なら色を変える
					isCheckingFlag = endPoint.handle.x == wayPoints[y][x].handle.x && endPoint.handle.y == wayPoints[y][x].handle.y;
					if (isCheckingFlag)
					{
						color = Color(0, 255, 255, 255);
					}

					//その場所がマウスカーソルと合ったら確認中の場所だと認識する
					isCheckingFlag = checkingHandle.x == wayPoints[y][x].handle.x && checkingHandle.y == wayPoints[y][x].handle.y;
					if (isCheckingFlag)
					{
						color = Color(255, 0, 0, 255);
					}

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
	DrawFunc::DrawCircle2D(UsersInput::Instance()->GetMousePos(), WAYPOINT_RADIUS, Color(0, 0, 255, 255));



#endif
}

void NavigationAI::ImGuiDraw()
{
	ImGui::Begin("Navigation");
	ImGui::Checkbox("SearchCircle", &serachFlag);
	ImGui::Checkbox("WayPoint", &wayPointFlag);
	ImGui::Checkbox("MoveLine", &lineFlag);
	ImGui::InputInt("LayerNumber", &layerNum);
	ImGui::Text("A*Data");
	ImGui::Text("StartHandle,X:%d,Y:%d", startPoint.handle.x, startPoint.handle.y);
	ImGui::Text("EndHandle,X:%d,Y:%d", endPoint.handle.x, endPoint.handle.y);
	ImGui::Text("SearchHandle");
	if (searchMap.size() <= layerNum)
	{
		layerNum = searchMap.size() - 1;
	}
	if (0 <= layerNum && searchMap[layerNum].size() != 0)
	{
		for (int i = 0; i < searchMap[layerNum].size(); ++i)
		{
			Vec2<int>handle = searchMap[layerNum][i].handle;
			std::string name = "Handle:" + std::to_string(i) + ",X:" +
				std::to_string(handle.x) + ",Y:" +
				std::to_string(handle.y);
			ImGui::Text(name.c_str());
		}
	}
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

void NavigationAI::AStart(const WayPointData &START_POINT, const WayPointData &END_POINT)
{
	std::vector<WayPointData>startPoint;//ウェイポイントの探索開始位置
	std::vector<WayPointData>nextPoint; //次のウェイポイントの探索開始位置
	nextPoint.push_back(START_POINT);
	searchMap.clear();
	queue.clear();

	//次に向かうべき場所を探索する
	while (1)
	{
		//次に探索するウェイポイントのデータをスタート地点とする
		startPoint = nextPoint;
		//次のウェイポイントの情報を蓄える為に空きを用意する
		nextPoint.clear();

		bool goalFlag = false;

		searchMap.push_back({});
		//現在追跡中のルート分探索
		for (int startPointIndex = 0; startPointIndex < startPoint.size(); ++startPointIndex)
		{
			for (int i = 0; i < startPoint[startPointIndex].wayPointHandles.size(); ++i)
			{
				//参照するウェイポイントの指定
				Vec2<int>handle(startPoint[startPointIndex].wayPointHandles[i]);
				//ヒューリスティック推定値から探索するべきかどうか判断する
				float nowHeuristicValue = startPoint[startPointIndex].pos.Distance(END_POINT.pos);						//現在地からのゴールまでのヒューリスティック推定値
				float nextHeuristicValue = wayPoints[handle.y][handle.x].pos.Distance(END_POINT.pos);	//参照しているウェイポイントからゴールまでのヒューリスティック推定値

				//ヒューリスティックが0ならゴールにたどり着いた事になるので探索しない
				if (nowHeuristicValue <= 0.0f)
				{
					continue;
				}

				int layerArrayNum = searchMap.size() - 1;
				int nowHandleArrayNum = searchMap[layerArrayNum].size();
				//探索していなかったらデバック情報を追加する
				if (!CheckQueue(handle))
				{
					searchMap[layerArrayNum].push_back(SearchMapData(handle, Color(0, 255, 0, 255)));
				}
				//現在地よりヒューリスティック推定値が小さい&&キューにスタックしてないならスタックする
				if (nextHeuristicValue <= nowHeuristicValue && !CheckQueue(handle))
				{
					searchMap[layerArrayNum][nowHandleArrayNum].color = Color(223, 144, 53, 255);

					//キューにはハンドルとヒューリスティック推定値+パス数の合計値をスタックする
					queue.push_back(QueueData(handle, nextHeuristicValue));
					//次に探索する地点を記録する
					nextPoint.push_back(wayPoints[handle.y][handle.x]);
				}
			}
		}

		//次に探索できる場所が無ければ探索終了
		if (nextPoint.size() == 0)
		{
			break;
		}
	}
}

inline bool NavigationAI::CheckQueue(const Vec2<int> &HANDLE)
{
	for (int i = 0; i < queue.size(); ++i)
	{
		if (queue[i].handle == HANDLE)
		{
			return true;
		}
	}
	return false;
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
					wayPoints[y][x].RegistHandle(DATA->handle);
					DATA->RegistHandle({ x,y });
				}
			}
		}
	}
}
