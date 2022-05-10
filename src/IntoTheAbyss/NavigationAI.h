#pragma once
#include"../KuroEngine.h"
#include<vector>
#include<array>
#include"StageMgr.h"
#include"BulletCollision.h"
#include"SelectStage.h"
#include"Collider.h"

static const int GOAL_MAX_NUM = 5;

/// <summary>
/// ウェイポイントが持つ情報
/// </summary>
struct WayPointData
{
	Vec2<int> handle;						//ハンドル
	Vec2<float> pos;						//座標
	float radius;							//大きさ
	std::vector<Vec2<int>> wayPointHandles;	//どの方向に行けるかハンドル取ったもの
	int passNum;							//目標地点までのパス数
	int branchHandle;						//探索中の分岐のハンドル
	int branchReferenceCount;				//何回その分岐を参照したか
	bool isWall;							//壁かどうか 今までは壁にはウェイポイントを配置しないという処理になっていたが、接続の探索のしやすさから壁にもウェイポイントを設置するようにしました。

	int numberOfItemHeld;					// 保持アイテム数

	float wallDistanceTop;					//上方向の壁までの距離
	float wallDistanceBottom;				//下方向の壁までの距離
	float wallDistanceLeft;					//左方向の壁までの距離
	float wallDistanceRight;				//右方向の壁までの距離

	WayPointData() :handle(Vec2<int>(-1, -1)), passNum(0), branchHandle(-1), branchReferenceCount(0), radius(0.0f)
	{
	}

	void RegistHandle(const Vec2<int>& HANDLE)
	{
		for (int i = 0; i < wayPointHandles.size(); ++i)
		{
			bool sameFlag = wayPointHandles[i] == HANDLE;
			if (sameFlag)
			{
				return;
			}
		}

		bool mineFlag = handle == HANDLE;
		if (mineFlag)
		{
			return;
		}

		wayPointHandles.push_back(HANDLE);
	}
};

class NavigationAI
{
public:
	NavigationAI();

	/// <summary>
	/// ポイントの生成
	/// </summary>
	/// <param name="MAP_DATA">ステージのCSV</param>
	void Init(const RoomMapChipArray& MAP_DATA);

	void Update(const Vec2<float>& POS);

	void Draw();

	void ImGuiDraw();

	/// <summary>
	/// 最短ルートを受け取る
	/// </summary>
	/// <returns></returns>
	std::vector<WayPointData> GetShortestRoute();

	/// <summary>
	/// 探索する際にキューに詰め込むべき情報
	/// </summary>
	struct QueueData
	{
		Vec2<int>handle;
		float sum;
		QueueData(const Vec2<int>& HANDLE, float SUM) :handle(HANDLE), sum(SUM)
		{
		};
	};

	bool resetSearchFlag;


	std::vector<std::vector<WayPointData>> wayPoints;//ウェイポイントの配列
	int wayPointXCount;
	int wayPointYCount;

	WayPointData startPoint;	//探索する際のスタート地点
	WayPointData endPoint;		//探索する際のゴール地点

	bool startFlag;
private:

	static const float SERACH_RADIUS;//ウェイポイント同士のリンク付けする範囲
	static const float SERACH_LAY_UPDOWN_DISTANCE;//ウェイポイント同士のリンク付けする範囲
	static const float SERACH_LAY_LEFTRIGHT_DISTANCE;//ウェイポイント同士のリンク付けする範囲
	static const float SERACH_LAY_NANAME_DISTANCE;//ウェイポイント同士のリンク付けする範囲

	static const float WAYPOINT_RADIUS;//ウェイポイントのデバック描画の大きさ

	static const int MAP_CHIP_SIZE = 50;
	static const int MAP_CHIP_HALF_SIZE = MAP_CHIP_SIZE / 2;



	/// <summary>
	/// マップチップ座標に変換しfloatからintに切り替える際に切り落とすか切り上げるかを判断する
	/// </summary>
	/// <param name="WORLD_POS">ワールド座標</param>
	/// <returns>マップチップ座標</returns>
	inline const Vec2<int>& GetMapChipNum(const Vec2<float>& WORLD_POS);

	/// <summary>
	/// 引数に渡したウェイポイントを周りのウェイポイントと繋げる
	/// </summary>
	/// <param name="HANDLE">ウェイポイントを繋げる為の判定</param>
	/// <param name="DATA">リンク付けする対象</param>
	inline void RegistHandle(WayPointData DATA);

	/// <summary>
	/// 使用しているウェイポイントかどうか
	/// </summary>
	/// <param name="DATA">ウェイポイントのデータ</param>
	/// <returns>true...使われている,false...使われていない</returns>
	inline bool DontUse(const Vec2<int>& HANDLE);

	/// <summary>
	/// マップチップとレイの判定
	/// </summary>
	/// <param name="START_POS">始点</param>
	/// <param name="END_POS">終点</param>
	/// <returns>true...当たった、false...当たっていない</returns>
	bool CheckMapChipWallAndRay(const Vec2<float> &START_POS, const Vec2<float> &END_POS)
	{

		// マップチップの情報。
		RoomMapChipArray mapData = StageMgr::Instance()->GetMapChipData(SelectStage::Instance()->GetStageNum(), SelectStage::Instance()->GetRoomNum());					//マップ

		// マップチップの識別用データ
		SizeData mapChipSizeData = StageMgr::Instance()->GetMapChipSizeData(MAPCHIP_TYPE_STATIC_BLOCK);

		// 次にマップチップとの最短距離を求める。
		const int MAP_Y = mapData.size();
		for (int height = 0; height < MAP_Y; ++height) {

			const int MAP_X = mapData[height].size();
			for (int width = 0; width < MAP_X; ++width) {

				// このマップチップが1~9以外だったら判定を飛ばす。
				if (!(mapChipSizeData.min <= mapData[height][width] && mapData[height][width] <= mapChipSizeData.max)) continue;

				// このインデックスのブロックの座標を取得。
				const Vec2<float> BLOCK_POS = Vec2<float>(width * MAP_CHIP_SIZE, height * MAP_CHIP_SIZE);

				Vec2<int> windowSize = WinApp::Instance()->GetWinCenter();

				// 一定範囲以外だったら処理を飛ばす。
				bool checkInsideTop = BLOCK_POS.y < START_POS.y - windowSize.y;
				bool checkInsideBottom = START_POS.y + windowSize.y > BLOCK_POS.y;
				bool checkInsideLeft = BLOCK_POS.x < START_POS.x + windowSize.x;
				bool checkInsideRight = START_POS.x + windowSize.x > BLOCK_POS.x;
				if (checkInsideTop && checkInsideBottom && checkInsideLeft && checkInsideRight) {
					continue;
				}

				// そのブロックが内包されているブロックだったら処理を飛ばす。
				Vec2<int> mapChipIndex = { width, height };
				if (StageMgr::Instance()->IsItWallIn(SelectStage::Instance()->GetStageNum(), SelectStage::Instance()->GetRoomNum(), mapChipIndex)) {
					continue;
				}

				// 四辺分交点を求める。

				// 上方向
				if (Collider::Instance()->IsIntersected(START_POS, END_POS, Vec2<float>(BLOCK_POS.x - MAP_CHIP_HALF_SIZE, BLOCK_POS.y - MAP_CHIP_HALF_SIZE), Vec2<float>(BLOCK_POS.x + MAP_CHIP_HALF_SIZE, BLOCK_POS.y - MAP_CHIP_HALF_SIZE))) {

					// 当たった判定
					return true;

				}
				// 右方向
				if (Collider::Instance()->IsIntersected(START_POS, END_POS, Vec2<float>(BLOCK_POS.x + MAP_CHIP_HALF_SIZE, BLOCK_POS.y - MAP_CHIP_HALF_SIZE), Vec2<float>(BLOCK_POS.x + MAP_CHIP_HALF_SIZE, BLOCK_POS.y + MAP_CHIP_HALF_SIZE))) {

					// 当たった判定
					return true;

				}
				// 下方向
				if (Collider::Instance()->IsIntersected(START_POS, END_POS, Vec2<float>(BLOCK_POS.x - MAP_CHIP_HALF_SIZE, BLOCK_POS.y + MAP_CHIP_HALF_SIZE), Vec2<float>(BLOCK_POS.x + MAP_CHIP_HALF_SIZE, BLOCK_POS.y + MAP_CHIP_HALF_SIZE))) {

					// 当たった判定
					return true;

				}
				// 左方向
				if (Collider::Instance()->IsIntersected(START_POS, END_POS, Vec2<float>(BLOCK_POS.x - MAP_CHIP_HALF_SIZE, BLOCK_POS.y - MAP_CHIP_HALF_SIZE), Vec2<float>(BLOCK_POS.x - MAP_CHIP_HALF_SIZE, BLOCK_POS.y + MAP_CHIP_HALF_SIZE))) {

					// 当たった判定
					return true;

				}

			}

		}

		return false;

	};


	//A*-------------------------------
	std::vector<QueueData>queue;	//最短ルートの候補を纏めた配列

	float routeRate;	//ランダムでルートを取った際の数字
	bool initRouteFlag;	//ゴール地点が変わった際にルート候補からランダムで選択する

	/// <summary>
	/// Aスターによる探索を行う関数
	/// </summary>
	/// <param name="START_POINT">スタート地点</param>
	/// <param name="END_POINT">ゴール地点</param>
	void AStart(const WayPointData& START_POINT, const WayPointData& END_POINT);

	/// <summary>
	/// キューに同じハンドルがスタックされているかどうか
	/// ゴール地点は複数スタックしても良い
	/// </summary>
	/// <param name="HANDLE">ウェイポイントのハンドル</param>
	inline bool CheckQueue(const Vec2<int>& HANDLE);

	/// <summary>
	/// 探索した際の最短ルート候補から最短ルートに絞る
	/// </summary>
	/// <param name="QUEUE">最短ルート候補</param>
	/// <returns>スタートからゴールまでの最短ルート</returns>
	std::vector<WayPointData> ConvertToShortestRoute2(const std::vector<std::vector<WayPointData>>& QUEUE);


	void RegistBranch(const WayPointData& DATA);


	WayPointData prevStartPoint;//探索する際の前フレームのスタート地点
	WayPointData prevEndPoint;	//探索する際の前フレームのゴール地点

	std::vector<std::vector<WayPointData>> branchQueue;//探索中のルートを保存する
	std::vector<WayPointData> shortestRoute;
	std::vector<bool> reachToGoalFlag;

	//デバック--------------------------
	Vec2<int> checkingHandle;	//マウスカーソルで参照しているウェイポイント
	int checkTimer;				//ウェイポイントを参照している時間

	bool serachFlag;			//ウェイポイント同士のリンク付けする為の判定の描画
	bool lineFlag;				//ウェイポイント同士のリンクしているかの確認
	bool wayPointFlag;			//ウェイポイントの描画

	std::vector<std::vector<Color>> debugColor;	//ウェイポイントの色

	struct SearchMapData
	{
		Vec2<int>handle;
		Color color;
		SearchMapData(const Vec2<int>& HANDLE, const Color& COLOR) :handle(HANDLE), color(COLOR)
		{
		}
	};
	std::vector<std::vector<SearchMapData>>searchMap;
	int layerNum;


	bool CheckHitLineCircle(const Vec2<float>& LineStartPos, const Vec2<float>& LineEndPos, const Vec2<float>& CircleCenterPos, const float& CircleRadius) {

		// 始点から終点までのベクトルを正規化する。
		Vec2<float>& startEndPos = LineEndPos - LineStartPos;
		startEndPos.Normalize();

		// [始点から終点までのベクトル]と、[始点から円の中心までのベクトル]の結果の外積が円の半径よりも小さかったら判定の第一段階クリア！
		if (fabs(startEndPos.Cross(CircleCenterPos - LineStartPos)) < CircleRadius) {

			// [始点から終点までのベクトルと始点から円の中心までのベクトルの内積] と [始点から終点までのベクトルと終点から円の中心までのベクトルの内積] のかけた値が-だったら当たっている！
			if ((LineEndPos - LineStartPos).Dot(CircleCenterPos - LineStartPos) * (LineEndPos - LineStartPos).Dot(CircleCenterPos - LineEndPos) < 0) {

				return true;

			}
			else {

				// 上の条件式で当たっていなかった場合でも、始点終点からの距離が円の半径よりも小さかったら当たっている。
				float startCenterDistance = (LineStartPos - CircleCenterPos).Length();
				float endCenterDistance = (LineEndPos - CircleCenterPos).Length();
				if (fabs(startCenterDistance) < CircleRadius || fabs(endCenterDistance) < CircleRadius) {

					return true;

				}

			}

		}
		return false;
	}


	Vec2<float>CaluLine(const Vec2<float>& CENTRAL_POS, int angle);

	/// <summary>
	/// ウェイポイントを繋げる処理
	/// </summary>
	/// <param name="DATA"> 検索する基準のウェイポイント </param>
	/// <param name="SEARCH_OFFSET"> 検索するマップチップの位置 (DATAを基準としてどこを検索するかの値) </param>
	/// <param name="CHIP_DATA"> マップチップのブロックのIDを取得して壁判定するためのデータ </param>
	/// <returns> 繋げられたかが返される。 繋げる処理はこの関数の内部にある。 </returns>
	bool ConnectWayPoint(WayPointData DATA, const Vec2<float>& SEARCH_DIR, const SizeData& CHIP_DATA);

	/// <summary>
	/// ウェイポイントから壁までの距離を計算する処理
	/// </summary>
	/// <param name="DATA"> 検索する基準のウェイポイント </param>
	/// <param name="SEARCH_DIR"> 検索する方向 </param>
	/// <param name="CHIP_DATA"> マップチップのブロックのIDを取得して壁判定するためのデータ </param>
	/// <returns> 検索した方向での壁までの距離を返す。 </returns>
	float SearchWall(WayPointData DATA, const Vec2<float>& SEARCH_DIR, const SizeData& CHIP_DATA);

	void CheckNumberOfItemHeldCount();
};

