#include "NavigationAI.h"
#include"../Engine/DrawFunc.h"
#include"ScrollMgr.h"
#include"../Engine/UsersInput.h"
#include"../IntoTheAbyss/DebugKeyManager.h"
#include<queue>
#include "StaminaItemMgr.h"

const float NavigationAI::SERACH_RADIUS = 180.0f;
const float NavigationAI::WAYPOINT_RADIUS = 20.0f;
const float NavigationAI::SERACH_LAY_UPDOWN_DISTANCE = 300.0f;
const float NavigationAI::SERACH_LAY_LEFTRIGHT_DISTANCE = 350.0f;
const float NavigationAI::SERACH_LAY_NANAME_DISTANCE = 400.0f;

void NavigationAI::Init(const RoomMapChipArray& MAP_DATA)
{
	SizeData wallMemorySize = StageMgr::Instance()->GetMapChipSizeData(MAPCHIP_TYPE_STATIC_BLOCK);

	//座標の設定--------------------------
	int xNum = MAP_DATA[0].size();
	int yNum = MAP_DATA.size();

	// マップチップの数分ウェイポイントを生成。
	wayPoints.resize(yNum);
	debugColor.resize(yNum);
	for (int index = 0; index < yNum; ++index) {
		wayPoints[index].resize(xNum);
		debugColor[index].resize(xNum);
	}

	// ウェイポイントの数を保存。
	wayPointXCount = wayPoints[0].size() - 1;
	wayPointYCount = wayPoints.size() - 1;

	for (int y = 0; y < yNum; ++y)
	{
		for (int x = 0; x < xNum; ++x)
		{
			debugColor[y][x] = Color(255, 255, 255, 255);

			//一度ワールド座標に変換してからマップチップ座標に変換する
			const Vec2<float> worldPos(MAP_CHIP_SIZE * x, MAP_CHIP_SIZE * y);
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

			wayPoints[y][x] = std::make_shared<WayPointData>();
			//等間隔で開ける
			wayPoints[y][x]->pos = worldPos;
			wayPoints[y][x]->radius = WAYPOINT_RADIUS;
			wayPoints[y][x]->handle = { x,y };
			wayPoints[y][x]->isWall = wallFlag;
			wayPoints[y][x]->numberOfItemHeld = 0;
		}
	}
	//座標の設定--------------------------


	//近くにあるウェイポイントへの繋ぎ--------------------------
	for (int y = 0; y < wayPointYCount; ++y)
	{
		for (int x = 0; x < wayPointXCount; ++x)
		{
			if (wayPoints[y][x].get() == nullptr) continue;
			if (!DontUse(wayPoints[y][x]->handle)) continue;
			if (wayPoints[y][x]->isWall) continue;		// 壁の場合は繋げない。
			RegistHandle(wayPoints[y][x]);
		}
	}
	//近くにあるウェイポイントへの繋ぎ--------------------------

	wayPointFlag = true;
	serachFlag = false;
	lineFlag = true;

	checkingHandle = { -1,-1 };
	checkTimer = 0;
	startFlag = false;
}

void NavigationAI::Update(const Vec2<float>& POS)
{

	// ウェイポイントのアイテム保持数を計算する。
	CheckNumberOfItemHeldCount();

#ifdef _DEBUG

	resetSearchFlag = false;
	Vec2<float>pos = UsersInput::Instance()->GetMousePos();

	for (int y = 0; y < wayPointYCount; ++y)
	{
		for (int x = 0; x < wayPointXCount; ++x)
		{
			if (wayPoints[y][x].get() == nullptr) continue;
			if (wayPoints[y][x]->isWall) continue;
			if (DontUse(wayPoints[y][x]->handle))
			{
				SphereCollision data1, data2;

				//スクリーン座標と判定を取る為にスクリーン座標に変換する
				data1.center = &ScrollMgr::Instance()->Affect(wayPoints[y][x]->pos);
				data1.radius = wayPoints[y][x]->radius;
				data2.center = &pos;
				data2.radius = WAYPOINT_RADIUS;

				bool hitFlag = BulletCollision::Instance()->CheckSphereAndSphere(data1, data2);

				//マウスカーソルと合ったらウェイポイントの情報を参照する
				if (hitFlag)
				{
					checkingHandle = wayPoints[y][x]->handle;
					checkTimer = 0;
				}
				//右クリックしたらスタート地点に設定する
				if (hitFlag && DebugKeyManager::Instance()->DebugKeyTrigger(DIK_H, "SetStart", "DIK_H"))
				{
					startPoint = *wayPoints[y][x];
				}
				//左クリックしたらゴール地点に設定する
				if (hitFlag && DebugKeyManager::Instance()->DebugKeyTrigger(DIK_N, "SetGoal", "DIK_N"))
				{
					endPoint = *wayPoints[y][x];
				}
			}
		}
	}

	//五秒以上どのウェイポイントも参照しなかったらウェイポイント情報を描画しない--------------------------
	++checkTimer;
	if (60 * 5 <= checkTimer)
	{
		checkingHandle = { -1,-1 };
	}
	//五秒以上どのウェイポイントも参照しなかったらウェイポイント情報を描画しない--------------------------


	//スタート地点かゴール地点を変える際は白統一する
	if (startPoint.handle != prevStartPoint.handle || endPoint.handle != prevEndPoint.handle)
	{
		resetSearchFlag = true;
	}
	prevStartPoint = startPoint;
	prevEndPoint = endPoint;

	//毎フレーム初期化の必要のある物を初期化する
	for (int y = 0; y < wayPointYCount; ++y)
	{
		for (int x = 0; x < wayPointXCount; ++x)
		{
			if (wayPoints[y][x].get() == nullptr) continue;
			if (wayPoints[y][x]->isWall) continue;
			debugColor[y][x] = Color(255, 255, 255, 255);
			wayPoints[y][x]->branchReferenceCount = 0;
			wayPoints[y][x]->branchHandle = -1;
		}
	}

	//段階に分けて探索する色を出力する
	for (int layer = 0; layer < layerNum; ++layer)
	{
		for (int num = 0; num < searchMap[layer].size(); ++num)
		{
			Vec2<int>handle = searchMap[layer][num].handle;
			debugColor[handle.y][handle.x] = searchMap[layer][num].color;
		}
	}

#endif // DEBUG

	if (startPoint.handle != Vec2<int>(-1, -1)&&
		endPoint.handle != Vec2<int>(-1, -1))
	{
		startFlag = true;
	}


	//スタートとゴールを設定したらAスターの探索を開始する
	if (startFlag)
	{
		AStart(startPoint, endPoint);
	}
}

void NavigationAI::Draw()
{
	//デバックの時のみ表示
#ifdef _DEBUG

	//ウェイポイントの描画
	for (int y = 0; y < wayPointYCount; ++y)
	{
		for (int x = 0; x < wayPointXCount; ++x)
		{
			if (wayPoints[y][x].get() == nullptr) continue;
			if (wayPoints[y][x]->isWall) continue;
			if (DontUse(wayPoints[y][x]->handle))
			{
				if (wayPointFlag)
				{
					bool isCheckingFlag = false;
					Color color = debugColor[y][x];

					//その場所がマウスカーソルと合ったら確認中の場所だと認識する
					isCheckingFlag = checkingHandle == wayPoints[y][x]->handle;
					if (isCheckingFlag)
					{
						color = Color(255, 0, 0, 255);
					}

					//最短ルートの描画
					for (int i = 0; i < shortestRoute.size(); ++i)
					{
						if (shortestRoute[i]->handle == wayPoints[y][x]->handle)
						{
							color = Color(255, 0, 255, 255);
							break;
						}
					}

					//その場所がスタート地点なら色を変える
					isCheckingFlag = startPoint.handle == wayPoints[y][x]->handle;
					if (isCheckingFlag)
					{
						color = Color(255, 255, 0, 255);
					}

					//その場所がゴール地点なら色を変える
					isCheckingFlag = endPoint.handle == wayPoints[y][x]->handle;
					if (isCheckingFlag)
					{
						color = Color(0, 255, 255, 255);
					}

					//ウェイポイントの描画
					DrawFunc::DrawCircle2D(ScrollMgr::Instance()->Affect(wayPoints[y][x]->pos), wayPoints[y][x]->radius, color);
				}
				//探索範囲の描画
				if (serachFlag)
				{
					for (int i = 0; i < 8; ++i)
					{
						Vec2<float>endPos = CaluLine(wayPoints[y][x]->pos, i * (360.0f / 8.0f));

						DrawFunc::DrawLine2D(ScrollMgr::Instance()->Affect(wayPoints[y][x]->pos), ScrollMgr::Instance()->Affect(endPos), Color(255, 255, 255, 255));
					}
				}
			}
		}
	}

	////繋がりの描画
	//if (lineFlag)
	//{
	//	for (int y = 0; y < wayPointYCount; ++y)
	//	{
	//		for (int x = 0; x < wayPointXCount; ++x)
	//		{
	//			if (wayPoints[y][x].get() == nullptr) continue;
	//			if (DontUse(wayPoints[y][x]->handle))
	//			{
	//				//登録したハンドルから線を繋げる
	//				for (int i = 0; i < wayPoints[y][x]->wayPointHandles.size(); ++i)
	//				{
	//					Vec2<int> endHandle = wayPoints[y][x]->wayPointHandles[i];
	//					DrawFunc::DrawLine2D
	//					(
	//						ScrollMgr::Instance()->Affect(wayPoints[y][x]->pos),
	//						ScrollMgr::Instance()->Affect(wayPoints[endHandle.y][endHandle.x]->pos),
	//						Color(0, 155, 0, 255)
	//					);
	//				}
	//			}
	//		}
	//	}
	//}

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


	ImGui::Begin("Queue");
	for (int i = 0; i < queue.size(); ++i)
	{
		Vec2<int>handle = queue[i]->handle;
		std::string name = "Handle:" + std::to_string(i) + ",X:" +
			std::to_string(handle.x) + ",Y:" +
			std::to_string(handle.y) + ",Distance+Pass:" + std::to_string(queue[i]->sum);
		ImGui::Text(name.c_str());
	}
	ImGui::End();


	ImGui::Begin("ShortestRoute");
	ImGui::Text("ShortestRoute");
	for (int branch = 0; branch < shortestRoute.size(); ++branch)
	{
		ImGui::Text("Handle:%d,X:%d,Y:%d", branch, shortestRoute[branch]->handle.x, shortestRoute[branch]->handle.y);
	}
	ImGui::Text("");
	for (int branchNum = 0; branchNum < branchQueue.size(); ++branchNum)
	{
		ImGui::Text("Route:%d", branchNum);
		if (reachToGoalFlag[branchNum])
		{
			ImGui::SameLine();
			ImGui::Text("Reach!!!");
		}
		for (int route = 0; route < branchQueue[branchNum].size(); ++route)
		{
			ImGui::Text("Handle:%d,X:%d,Y:%d",
				route,
				branchQueue[branchNum][route]->handle.x,
				branchQueue[branchNum][route]->handle.y
			);
		}
		ImGui::Text("");
	}
	ImGui::End();




	if (checkingHandle.x != -1 && checkingHandle.y != -1)
	{
		Vec2<int>handle = checkingHandle;
		ImGui::Begin("WayPointData");
		ImGui::Text("Handle:X%d,Y:%d", wayPoints[handle.y][handle.x]->handle.x, wayPoints[handle.y][handle.x]->handle.y);
		ImGui::Text("Pos:X%f,Y:%f", wayPoints[handle.y][handle.x]->pos.x, wayPoints[handle.y][handle.x]->pos.y);
		ImGui::Text("Radius:%f", wayPoints[handle.y][handle.x]->radius);
		ImGui::Text("RelateHandles");
		for (int i = 0; i < wayPoints[handle.y][handle.x]->wayPointHandles.size(); ++i)
		{
			std::string name = "Handle:" + std::to_string(i) + ",X:" +
				std::to_string(wayPoints[handle.y][handle.x]->wayPointHandles[i].x) + ",Y:" +
				std::to_string(wayPoints[handle.y][handle.x]->wayPointHandles[i].y);
			ImGui::Text(name.c_str());
		}
		ImGui::Text("PassNum:%d", wayPoints[handle.y][handle.x]->passNum);
		ImGui::End();
	}
}

std::vector<WayPointData> NavigationAI::GetShortestRoute()
{
	//キューからウェイポイントの配列に変換する
	std::vector<WayPointData>result;
	for (int i = 0; i < shortestRoute.size(); ++i)
	{
		int x = shortestRoute[i]->handle.x;
		int y = shortestRoute[i]->handle.y;
		result.push_back(*wayPoints[y][x]);
	}
	return result;

}

inline const Vec2<int>& NavigationAI::GetMapChipNum(const Vec2<float>& WORLD_POS)
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

inline bool NavigationAI::DontUse(const Vec2<int>& HANDLE)
{
	return HANDLE != Vec2<int>(-1, -1);
}

void NavigationAI::AStart(const WayPointData& START_POINT, const WayPointData& END_POINT)
{
	std::vector<std::shared_ptr<WayPointData>>startPoint;//ウェイポイントの探索開始位置
	std::vector<std::shared_ptr<WayPointData>>nextPoint; //次のウェイポイントの探索開始位置
	std::vector<std::shared_ptr<WayPointData>>failPoint; //探索候補外のウェイポイント

	Vec2<int>sHandle(START_POINT.handle);

	nextPoint.push_back(std::make_shared<WayPointData>());
	nextPoint[0] = wayPoints[sHandle.y][sHandle.x];
	searchMap.clear();
	queue.clear();
	branchQueue.clear();
	bool failFlag = false;

	//最初のブランチを作る
	branchQueue.push_back({});
	branchQueue[0].push_back(std::make_shared<WayPointData>());
	branchQueue[0][0] = wayPoints[sHandle.y][sHandle.x];
	branchQueue[0][0]->branchHandle = 0;

	//次に向かうべき場所を探索する
	while (1)
	{
		//次に探索するウェイポイントのデータをスタート地点とする
		startPoint = nextPoint;
		//次のウェイポイントの情報を蓄える為に空きを用意する
		for (int i = 0; i < nextPoint.size(); ++i)
		{
			nextPoint.pop_back();
		}


		searchMap.push_back({});
		//現在追跡中のルート分探索
		for (int startPointIndex = 0; startPointIndex < startPoint.size(); ++startPointIndex)
		{
			for (int i = 0; i < startPoint[startPointIndex]->wayPointHandles.size(); ++i)
			{
				//参照するウェイポイントの指定
				Vec2<int>handle(startPoint[startPointIndex]->wayPointHandles[i]);
				//ヒューリスティック推定値から探索するべきかどうか判断する
				float nowHeuristicValue = startPoint[startPointIndex]->pos.Distance(END_POINT.pos);						//現在地からのゴールまでのヒューリスティック推定値
				float nextHeuristicValue = wayPoints[handle.y][handle.x]->pos.Distance(END_POINT.pos);	//参照しているウェイポイントからゴールまでのヒューリスティック推定値

				//ヒューリスティックが0ならゴールにたどり着いた事になるので探索しない
				//スタート地点を二回キューに入れない
				if (wayPoints[handle.y][handle.x]->handle == START_POINT.handle)
				{
					continue;
				}

				int layerArrayNum = searchMap.size() - 1;
				int nowHandleArrayNum = searchMap[layerArrayNum].size();
				//探索していなかったらデバック情報を追加する
				if (!CheckQueue(handle))
				{
					searchMap[layerArrayNum].push_back(SearchMapData(handle, Color(0, 255, 0, 255)));

					//現在地よりヒューリスティック推定値が小さいしてないならスタックする
					if (nextHeuristicValue < nowHeuristicValue)
					{
						//探索失敗直後から始めている際は、参照したウェイポイントが成功した際、前のウェイポイントも候補に加える
						if (failFlag)
						{
							//キューにはハンドルと現在地からゴールまでの距離(ヒューリスティック推定値)をスタックする
							queue.push_back(std::make_shared<QueueData>(startPoint[startPointIndex]->handle, nowHeuristicValue));
						}

						searchMap[layerArrayNum][nowHandleArrayNum].color = Color(223, 144, 53, 255);
						//パス数の記録
						wayPoints[handle.y][handle.x]->passNum = startPoint[startPointIndex]->passNum + 1;
						//キューにはハンドルと現在地からゴールまでの距離(ヒューリスティック推定値)をスタックする
						queue.push_back(std::make_shared<QueueData>(handle, nextHeuristicValue + wayPoints[handle.y][handle.x]->passNum));

						//次に探索する地点を記録する,ゴールの場合は次の探索の候補にしない
						if (endPoint.handle != handle)
						{
							nextPoint.push_back(std::make_shared<WayPointData>());
							nextPoint[nextPoint.size() - 1] = wayPoints[handle.y][handle.x];
						}

						//現在地のウェイポイントのブランチのハンドルを見る
						Vec2<int>startHandle(startPoint[startPointIndex]->handle);
						//探索中のルートを保存する
						if (wayPoints[handle.y][handle.x]->branchHandle == -1)
						{
							//そのブランチの探索された回数が一回なら追加する
							if (wayPoints[startHandle.y][startHandle.x]->branchReferenceCount < 1)
							{
								//現在地のウェイポイントのブランチハンドルから参照先のウェイポイントにブランチハンドルを渡した後、ルート追加
								int branchHandle = wayPoints[startHandle.y][startHandle.x]->branchHandle;
								wayPoints[handle.y][handle.x]->branchHandle = branchHandle;
								++wayPoints[startHandle.y][startHandle.x]->branchReferenceCount;

								branchQueue[branchHandle].push_back(std::make_shared<WayPointData>());
								branchQueue[branchHandle][branchQueue[branchHandle].size() - 1] = wayPoints[handle.y][handle.x];
							}
							//二回以上探索されたら別のブランチを作成し、今までのルートを渡し追加する
							else
							{
								//ブランチを追加し、ハンドルを渡す
								branchQueue.push_back({});
								int newbranchHandle = branchQueue.size() - 1;
								wayPoints[handle.y][handle.x]->branchHandle = newbranchHandle;

								int branchHandle = wayPoints[startHandle.y][startHandle.x]->branchHandle;
								//既に別のウェイポイントが追加されているので最後の場所に上書きする
								branchQueue[newbranchHandle] = branchQueue[branchHandle];
								branchQueue[newbranchHandle][branchQueue[branchHandle].size() - 1] = wayPoints[handle.y][handle.x];
							}
						}
						//現在地がゴールでそのゴールが他のブランチにも追加済みの場合、他のブランチにも追加できるようにする
						else if (wayPoints[handle.y][handle.x]->branchHandle != -1 && endPoint.handle == handle)
						{
							int branchHandle = wayPoints[startHandle.y][startHandle.x]->branchHandle;
							branchQueue[branchHandle].push_back(std::make_shared<WayPointData>());
							branchQueue[branchHandle][branchQueue[branchHandle].size() - 1] = wayPoints[handle.y][handle.x];
						}
					}
					//探索に失敗したウェイポイントを記録する
					else
					{
						failPoint.push_back(std::make_shared<WayPointData>());
						failPoint[failPoint.size() - 1] = wayPoints[handle.y][handle.x];
						//failPoint[failPoint.size() - 1]->branchHandle = startPoint[startPointIndex]->branchHandle;
						Vec2<int>startHandle(startPoint[startPointIndex]->handle);

						//そのブランチの探索された回数が一回なら追加する
						if (wayPoints[startHandle.y][startHandle.x]->branchReferenceCount < 1)
						{
							//現在地のウェイポイントのブランチハンドルから参照先のウェイポイントにブランチハンドルを渡した後、ルート追加
							int branchHandle = wayPoints[startHandle.y][startHandle.x]->branchHandle;
							wayPoints[handle.y][handle.x]->branchHandle = branchHandle;
							++wayPoints[startHandle.y][startHandle.x]->branchReferenceCount;

							branchQueue[branchHandle].push_back(std::make_shared<WayPointData>());
							branchQueue[branchHandle][branchQueue[branchHandle].size() - 1] = wayPoints[handle.y][handle.x];
						}
						//二回以上探索されたら別のブランチを作成し、今までのルートを渡し追加する
						else
						{
							//ブランチを追加し、ハンドルを渡す
							branchQueue.push_back({});
							int newbranchHandle = branchQueue.size() - 1;
							wayPoints[handle.y][handle.x]->branchHandle = newbranchHandle;

							int branchHandle = wayPoints[startHandle.y][startHandle.x]->branchHandle;
							//既に別のウェイポイントが追加されているので最後の場所に上書きする
							branchQueue[newbranchHandle] = branchQueue[branchHandle];
							branchQueue[newbranchHandle][branchQueue[branchHandle].size() - 1] = wayPoints[handle.y][handle.x];
						}
					}
				}

			}
		}
		//次の場所を探索した後failFlagが立っていたら下ろす
		failFlag = false;

		//次に探索できる場所が無ければゴールまでたどり着いているか確認する
		if (nextPoint.size() == 0)
		{
			bool goalFlag = false;
			for (int i = 0; i < queue.size(); ++i)
			{
				if (queue[i]->handle == END_POINT.handle)
				{
					goalFlag = true;
					break;
				}
			}

			//目的地までたどり着いていたら探索を終える
			if (goalFlag)
			{
				break;
			}
			//そうでなければ探索に失敗したウェイポイント達をnextPointに入れて再探索する
			else
			{
				nextPoint = failPoint;
				failFlag = true;
				//ソートする際に順番がずれないよう今までのキューに調整を入れる
				//現在のウェイポイントの間隔から算出している
				for (int i = 0; i < queue.size(); ++i)
				{
					queue[i]->sum += 325.0f / 2.0f;
				}
			}
		}
	}

	//今までの候補から最短ルートを作る
	queue = ConvertToShortestRoute(queue);
	shortestRoute = ConvertToShortestRoute2(branchQueue);
}

void NavigationAI::RegistBranch(const WayPointData& DATA)
{
}

inline bool NavigationAI::CheckQueue(const Vec2<int>& HANDLE)
{
	for (int i = 0; i < queue.size(); ++i)
	{
		if (queue[i]->handle == HANDLE && endPoint.handle != HANDLE)
		{
			return true;
		}
	}
	return false;
}

std::vector<std::shared_ptr<NavigationAI::QueueData>> NavigationAI::ConvertToShortestRoute(const std::vector<std::shared_ptr<QueueData>>& QUEUE)
{
	std::vector<float>sumArray;
	for (int i = 0; i < QUEUE.size(); ++i)
	{
		sumArray.push_back(QUEUE[i]->sum);
	}
	std::sort(sumArray.begin(), sumArray.end());

	//ソートした順に並び替え
	std::vector<std::shared_ptr<QueueData>>sortData;
	for (int sumIndex = 0; sumIndex < sumArray.size(); ++sumIndex)
	{
		for (int queueIndex = 0; queueIndex < QUEUE.size(); ++queueIndex)
		{
			bool sameSumFlag = QUEUE[queueIndex]->sum == sumArray[sumIndex];
			if (sameSumFlag)
			{
				sortData.push_back(QUEUE[queueIndex]);
			}
		}
	}


	int resultHandle = 0;
	std::vector<std::shared_ptr<QueueData>> result;
	std::vector<std::shared_ptr<QueueData>> fail;

	while (1)
	{
		for (int sortIndex = 0; sortIndex < sortData.size(); ++sortIndex)
		{
			Vec2<int>handle(sortData[sortIndex]->handle);

			//前のウェイポイントと繋がっているかどうか
			if (result.size() != 0)
			{
				for (int linkHandle = 0; linkHandle < wayPoints[handle.y][handle.x]->wayPointHandles.size(); ++linkHandle)
				{
					//ウェイポイントが繋がっているかどうか
					bool linkFlag = wayPoints[handle.y][handle.x]->wayPointHandles[linkHandle] == result[resultHandle]->handle;

					//成功
					if (linkFlag)
					{
						result.push_back(sortData[sortIndex]);
						++resultHandle;
					}
					//失敗
					else if (linkFlag)
					{
						fail.push_back(sortData[sortIndex]);
					}
				}
			}
			else
			{
				result.push_back(sortData[sortIndex]);
			}
		}




		////ソート順に並べる&&前のウェイポイントと繋がっている箇所を繋げていく
		//for (int sumIndex = 0; sumIndex < sumArray.size(); ++sumIndex)
		//{
		//	for (int queueIndex = 0; queueIndex < QUEUE.size(); ++queueIndex)
		//	{
		//		//ソート順に並べる合図
		//		bool sameSumFlag = QUEUE[queueIndex].sum == sumArray[sumIndex];
		//		if (sameSumFlag)
		//		{
		//			//前のウェイポイントと繋がっているかどうか
		//			if (result.size() != 0)
		//			{
		//				Vec2<int>handle(QUEUE[queueIndex].handle);
		//				for (int linkHandle = 0; linkHandle < wayPoints[handle.y][handle.x].wayPointHandles.size(); ++linkHandle)
		//				{
		//					//ウェイポイントが繋がっているかどうか
		//					bool linkFlag = wayPoints[handle.y][handle.x].wayPointHandles[linkHandle] == result[resultHandle].handle;

		//					//ヒューリスティックは現在地より小さいか
		//					bool distanceFlag = true;

		//					//成功
		//					if (linkFlag && distanceFlag)
		//					{
		//						result.push_back(QUEUE[queueIndex]);
		//						++resultHandle;
		//					}
		//					//失敗
		//					else if (linkFlag)
		//					{
		//						fail.push_back(QUEUE[queueIndex]);
		//					}
		//				}
		//			}
		//			else
		//			{
		//				result.push_back(QUEUE[queueIndex]);
		//			}
		//		}
		//	}
		//}

		bool startFlag = false;
		bool goalFlag = false;
		//スタートとゴールが繋がっていなければ探索し直し
		for (int i = 0; i < result.size(); ++i)
		{
			if (result[i]->handle == Vec2<int>())
			{

			}
			if (result[i]->handle == Vec2<int>())
			{

			}
		}

		//スタートからゴールまで繋がったら成功、最短ルートして扱う
		if (startFlag && goalFlag)
		{

		}
		//失敗したらfailから候補を探す
		else
		{

		}

		break;
	}

	std::shared_ptr<QueueData> startPoint(std::make_shared<QueueData>(startPoint.handle, 0.0f));
	result.push_back(startPoint);

	return result;
}

Vec2<float> NavigationAI::CaluLine(const Vec2<float>& CENTRAL_POS, int angle)
{
	float distance = 0.0f;

	int adj = 15;

	float upDown = SERACH_LAY_UPDOWN_DISTANCE;
	float leftRight = SERACH_LAY_LEFTRIGHT_DISTANCE;
	float naname = SERACH_LAY_NANAME_DISTANCE;

	if (angle == 0)
	{
		distance = leftRight;
	}
	else if (angle == 45)
	{
		angle -= adj;
		distance = naname;
	}
	else if (angle == 90)
	{
		distance = upDown;
	}
	else if (angle == 135)
	{
		angle += adj;
		distance = naname;
	}
	else if (angle == 180)
	{
		distance = leftRight;
	}
	else if (angle == 225)
	{
		angle -= adj;
		distance = naname;
	}
	else if (angle == 270)
	{
		distance = upDown;
	}
	else if (angle == 315)
	{
		angle += adj;
		distance = naname;
	}
	else
	{
		distance = leftRight;
	}
	float dir = Angle::ConvertToRadian(angle);
	Vec2<float>lineEndPos(cosf(dir), sinf(dir));

	return CENTRAL_POS + lineEndPos * distance;
}

bool NavigationAI::ConnectWayPoint(std::shared_ptr<WayPointData> DATA, const Vec2<int>& SEARCH_OFFSET, const SizeData& CHIP_DATA)
{

	Vec2<float> searchHandle = { (float)DATA->handle.x + SEARCH_OFFSET.x, (float)DATA->handle.y + SEARCH_OFFSET.y };
	int chipData = StageMgr::Instance()->GetMapChipBlock(0, 0, searchHandle);

	// 検索する座標が壁じゃなかったら接続する。
	if (!(CHIP_DATA.min <= chipData && chipData <= CHIP_DATA.max)) {

		// 接続する。
		wayPoints[(int)searchHandle.y][(int)searchHandle.x]->RegistHandle(DATA->handle);
		DATA->RegistHandle({ (int)searchHandle.x,(int)searchHandle.y });

		return true;

	}

	return false;

}

float NavigationAI::SearchWall(std::shared_ptr<WayPointData> DATA, const Vec2<float>& SEARCH_DIR, const SizeData& CHIP_DATA)
{
	int searchCounter = 0;
	Vec2<float> searchIndex = { (float)DATA->handle.x, (float)DATA->handle.y };

	// マップの四方か壁で囲まれているため、配列外へオーバーすることがないと思うので配列外への対処の処理は書かないでおきます！例外スローしたら書きます…。
	while (1) {

		// 検索対象のマップチップ番号のブロック番号を検索する。
		int chipData = StageMgr::Instance()->GetMapChipBlock(0, 0, searchIndex);

		// ブロックじゃなかったらIdnexを動かして次へ。
		if (CHIP_DATA.min <= chipData && chipData <= CHIP_DATA.max) {

			// ブロックだったら距離を計算する。
			return MAP_CHIP_SIZE * searchCounter;

		}
		else {
			searchIndex += SEARCH_DIR;
		}

		++searchCounter;

		// サーチ回数が100000回を超えたら(上下左右100000ブロックの間に壁がないということ)明らかにそれはバグなのでアサート。
		if (100000 < searchCounter) assert(0);

	}
}

// ウェイポイントのアイテム保持数を計算する。
void NavigationAI::CheckNumberOfItemHeldCount()
{

	// 一度アイテム保持数を0にする。
	for (int y = 0; y < wayPointYCount; ++y)
	{
		for (int x = 0; x < wayPointXCount; ++x)
		{
			if (wayPoints[y][x].get() == nullptr) continue;
			if (wayPoints[y][x]->isWall) continue;
			if (DontUse(wayPoints[y][x]->handle))
			{
				wayPoints[y][x]->numberOfItemHeld = 0;
			}
		}
	}

	// スタミナアイテムのいちに応じてWayPointのアイテム保持数を変更する。
	std::array<StaminaItem, 100> staminaItem = StaminaItemMgr::Instance()->GetItemArray();
	const int ITEM_COUNT = staminaItem.size();
	for (int index = 0; index < ITEM_COUNT; ++index) {

		// 生成されていない or 取得されている だったら処理を飛ばす。
		if (!staminaItem[index].GetIsActive() || staminaItem[index].GetIsAcquired()) continue;

		// アイテムのマップチップインデックス番号を求める。
		Vec2<int> mapChipIndex = { (int)(staminaItem[index].GetPos().x / MAP_CHIP_SIZE), (int)(staminaItem[index].GetPos().y / MAP_CHIP_SIZE) };

		// 各インデックスが既定値を超えてないかをチェック。
		if (mapChipIndex.x < 0 || wayPointXCount <= mapChipIndex.x) continue;
		if (mapChipIndex.y < 0 || wayPointYCount <= mapChipIndex.y) continue;

		// 相当するウェイポイントにアイテム数を追加。
		++wayPoints[mapChipIndex.y][mapChipIndex.x]->numberOfItemHeld;

	}

}

std::vector<std::shared_ptr<WayPointData>> NavigationAI::ConvertToShortestRoute2(const std::vector<std::vector<std::shared_ptr<WayPointData>>>& QUEUE)
{
	std::vector<std::vector<std::shared_ptr<WayPointData>>> route;

	reachToGoalFlag.clear();
	reachToGoalFlag.reserve(QUEUE.size());
	reachToGoalFlag.resize(QUEUE.size());
	//スタート地点からゴール地点まで繋がっているルートを探す
	for (int branch = 0; branch < QUEUE.size(); ++branch)
	{
		if (QUEUE[branch][0]->handle == startPoint.handle &&
			QUEUE[branch][QUEUE[branch].size() - 1]->handle == endPoint.handle)
		{
			route.push_back(QUEUE[branch]);
			reachToGoalFlag[branch] = true;
		}
		else
		{
			reachToGoalFlag[branch] = false;
		}
	}


	int minSize = 100000;
	int shortestRoute = 0;
	//ゴールまでのパス数を比較し、一番少ないパス数のルートを最短距離とする
	for (int i = 0; i < route.size(); ++i)
	{
		if (route[i].size() < minSize)
		{
			shortestRoute = i;
			minSize = route[i].size();
		}
	}

	return route[shortestRoute];
}

inline void NavigationAI::RegistHandle(std::shared_ptr<WayPointData> DATA)
{

	// ウェイポイントを繋げる処理。

	SizeData mapChipSizeData = StageMgr::Instance()->GetMapChipSizeData(MAPCHIP_TYPE_STATIC_BLOCK);


	// まずは上下左右から繋げる。
	bool isLeft = false;
	isLeft = ConnectWayPoint(DATA, { -1,0 }, mapChipSizeData);
	bool isRight = false;
	isRight = ConnectWayPoint(DATA, { 1,0 }, mapChipSizeData);
	bool isTop = false;
	isTop = ConnectWayPoint(DATA, { 0,-1 }, mapChipSizeData);
	bool isBottom = false;
	isBottom = ConnectWayPoint(DATA, { 0,1 }, mapChipSizeData);

	// 次に斜めに繋げる。
	if (isLeft && isTop) {

		// 左上に繋げる。
		ConnectWayPoint(DATA, { -1,-1 }, mapChipSizeData);

	}
	if (isRight && isTop) {

		// 右上に繋げる。
		ConnectWayPoint(DATA, { 1,-1 }, mapChipSizeData);

	}
	if (isLeft && isBottom) {

		// 左下に繋げる。
		ConnectWayPoint(DATA, { -1,1 }, mapChipSizeData);

	}
	if (isRight && isBottom) {

		// 右下に繋げる。
		ConnectWayPoint(DATA, { 1,1 }, mapChipSizeData);

	}


	// 次に各方向の壁までの距離を求める。

	// 上方向に検索する。
	DATA->wallDistanceTop = SearchWall(DATA, Vec2<float>(0, -1), mapChipSizeData);
	// 下方向に検索する。
	DATA->wallDistanceBottom = SearchWall(DATA, Vec2<float>(0, 1), mapChipSizeData);
	// 右方向に検索する。
	DATA->wallDistanceRight = SearchWall(DATA, Vec2<float>(1, 0), mapChipSizeData);
	// 左方向に検索する。
	DATA->wallDistanceLeft = SearchWall(DATA, Vec2<float>(-1, 0), mapChipSizeData);


}
