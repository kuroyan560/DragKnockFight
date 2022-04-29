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

	WayPointData() :handle(Vec2<int>(-1, -1)), passNum(0)
	{
	}

	void RegistHandle(const Vec2<int> &HANDLE)
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
	/// <summary>
	/// ポイントの生成
	/// </summary>
	/// <param name="MAP_DATA">ステージのCSV</param>
	void Init(const RoomMapChipArray &MAP_DATA);

	void Update(const Vec2<float> &POS);

	void Draw();

	void ImGuiDraw();
private:

	static const int WAYPOINT_MAX_X = 10;	 //X軸のウェイポイントの数
	static const int WAYPOINT_MAX_Y = 10;	 //Y軸のウェイポイントの数
	static const float SERACH_RADIUS;//ウェイポイント同士のリンク付けする範囲

	static const float WAYPOINT_RADIUS;//ウェイポイントのデバック描画の大きさ

	static const int MAP_CHIP_SIZE = 50;
	static const int MAP_CHIP_HALF_SIZE = MAP_CHIP_SIZE / 2;



	std::array<std::array<WayPointData, WAYPOINT_MAX_Y>, WAYPOINT_MAX_X> wayPoints;//ウェイポイントの配列

	inline const Vec2<int> &GetMapChipNum(const Vec2<float> &WORLD_POS);

	inline void RegistHandle(const SphereCollision &HANDLE, WayPointData *DATA);

	inline bool DontUse(const WayPointData &DATA);

	bool CheckMapChipWallAndRay(const Vec2<float> &START_POS, const Vec2<float> &END_POS)
	{
		//どうやって使うか
		Vec2<float>handSegmentStart(START_POS), handSegmentEnd(END_POS);//線分
		Vec2<float>handSegmentDir(END_POS - START_POS);					//線分の方向
		Vec2<float>handPos(START_POS);									//線分の始点
		Vec2<float>sightPos;						//求められた交点の中の最短距離
		RoomMapChipArray mapData = StageMgr::Instance()->GetMapChipData(SelectStage::Instance()->GetStageNum(), SelectStage::Instance()->GetRoomNum());					//マップ
		//どうやって使うか


		// 最短距離を保存するようの配列。
		std::vector<std::pair<Vec2<float>, float>> shortestPoints;

		// 照準のレイの方向によって当たり判定を無効化する為のフラグをセットする。
		bool isTop = handSegmentDir.y < 0;
		bool isLeft = handSegmentDir.x < 0;

		// 次にマップチップとの最短距離を求める。
		const int MAP_Y = mapData.size();
		for (int height = 0; height < MAP_Y; ++height) {

			const int MAP_X = mapData[height].size();
			for (int width = 0; width < MAP_X; ++width) {

				// このマップチップが1~9以外だったら判定を飛ばす。
				if (mapData[height][width] < 1 || mapData[height][width] > 9) continue;

				// このインデックスのブロックの座標を取得。
				const Vec2<float> BLOCK_POS = Vec2<float>(width * MAP_CHIP_SIZE, height * MAP_CHIP_SIZE);

				Vec2<int> windowSize = WinApp::Instance()->GetWinCenter();

				// 一定範囲以外だったら処理を飛ばす。
				bool checkInsideTop = BLOCK_POS.y < handPos.y - windowSize.y;
				bool checkInsideBottom = handPos.y + windowSize.y > BLOCK_POS.y;
				bool checkInsideLeft = BLOCK_POS.x < handPos.x + windowSize.x;
				bool checkInsideRight = handPos.x + windowSize.x > BLOCK_POS.x;
				if (checkInsideTop && checkInsideBottom && checkInsideLeft && checkInsideRight) {
					//player.onGround = false;
					continue;
				}

				// そのブロックが内包されているブロックだったら処理を飛ばす。
				Vec2<int> mapChipIndex = { width, height };
				if (StageMgr::Instance()->IsItWallIn(SelectStage::Instance()->GetStageNum(), SelectStage::Instance()->GetRoomNum(), mapChipIndex)) {

					continue;

				}

				// レイの方向とブロックの位置関係で処理を飛ばす。ウィンドウを4分割するやつ
				float offsetHandPos = MAP_CHIP_SIZE;
				if (isLeft && handPos.x + offsetHandPos < BLOCK_POS.x) continue;
				if (!isLeft && BLOCK_POS.x < handPos.x - offsetHandPos) continue;
				if (isTop && handPos.y + offsetHandPos < BLOCK_POS.y) continue;
				if (!isTop && BLOCK_POS.y < handPos.y - offsetHandPos) continue;


				// 四辺分交点を求める。

				// 交点保存用
				vector<Vec2<float>> intersectedPos;

				// 上方向
				if (Collider::Instance()->IsIntersected(handSegmentStart, handSegmentEnd, Vec2<float>(BLOCK_POS.x - MAP_CHIP_HALF_SIZE, BLOCK_POS.y - MAP_CHIP_HALF_SIZE), Vec2<float>(BLOCK_POS.x + MAP_CHIP_HALF_SIZE, BLOCK_POS.y - MAP_CHIP_HALF_SIZE))) {

					// 交点を求めて保存する。
					intersectedPos.push_back(Collider::Instance()->CalIntersectPoint(handSegmentStart, handSegmentEnd, Vec2<float>(BLOCK_POS.x - MAP_CHIP_HALF_SIZE, BLOCK_POS.y - MAP_CHIP_HALF_SIZE), Vec2<float>(BLOCK_POS.x + MAP_CHIP_HALF_SIZE, BLOCK_POS.y - MAP_CHIP_HALF_SIZE)));

				}
				// 右方向
				if (Collider::Instance()->IsIntersected(handSegmentStart, handSegmentEnd, Vec2<float>(BLOCK_POS.x + MAP_CHIP_HALF_SIZE, BLOCK_POS.y - MAP_CHIP_HALF_SIZE), Vec2<float>(BLOCK_POS.x + MAP_CHIP_HALF_SIZE, BLOCK_POS.y + MAP_CHIP_HALF_SIZE))) {

					// 交点を求めて保存する。
					intersectedPos.push_back(Collider::Instance()->CalIntersectPoint(handSegmentStart, handSegmentEnd, Vec2<float>(BLOCK_POS.x + MAP_CHIP_HALF_SIZE, BLOCK_POS.y - MAP_CHIP_HALF_SIZE), Vec2<float>(BLOCK_POS.x + MAP_CHIP_HALF_SIZE, BLOCK_POS.y + MAP_CHIP_HALF_SIZE)));

				}
				// 下方向
				if (Collider::Instance()->IsIntersected(handSegmentStart, handSegmentEnd, Vec2<float>(BLOCK_POS.x - MAP_CHIP_HALF_SIZE, BLOCK_POS.y + MAP_CHIP_HALF_SIZE), Vec2<float>(BLOCK_POS.x + MAP_CHIP_HALF_SIZE, BLOCK_POS.y + MAP_CHIP_HALF_SIZE))) {

					// 交点を求めて保存する。
					intersectedPos.push_back(Collider::Instance()->CalIntersectPoint(handSegmentStart, handSegmentEnd, Vec2<float>(BLOCK_POS.x - MAP_CHIP_HALF_SIZE, BLOCK_POS.y + MAP_CHIP_HALF_SIZE), Vec2<float>(BLOCK_POS.x + MAP_CHIP_HALF_SIZE, BLOCK_POS.y + MAP_CHIP_HALF_SIZE)));

				}
				// 左方向
				if (Collider::Instance()->IsIntersected(handSegmentStart, handSegmentEnd, Vec2<float>(BLOCK_POS.x - MAP_CHIP_HALF_SIZE, BLOCK_POS.y - MAP_CHIP_HALF_SIZE), Vec2<float>(BLOCK_POS.x - MAP_CHIP_HALF_SIZE, BLOCK_POS.y + MAP_CHIP_HALF_SIZE))) {

					// 交点を求めて保存する。
					intersectedPos.push_back(Collider::Instance()->CalIntersectPoint(handSegmentStart, handSegmentEnd, Vec2<float>(BLOCK_POS.x - MAP_CHIP_HALF_SIZE, BLOCK_POS.y - MAP_CHIP_HALF_SIZE), Vec2<float>(BLOCK_POS.x - MAP_CHIP_HALF_SIZE, BLOCK_POS.y + MAP_CHIP_HALF_SIZE)));

				}

				// 最短距離を求める。
				Vec2<float> shortestPos = {};
				float shoterstLength = 1000000.0f;

				// サイズが0だったら処理を飛ばす。
				const int INTERSECTED_COUNT = intersectedPos.size();
				if (INTERSECTED_COUNT <= 0) continue;

				// 最短距離を求める。
				for (int index = 0; index < INTERSECTED_COUNT; ++index) {

					// 保存されているデータより大きかったら処理を飛ばす。
					float lengthBuff = Vec2<float>(intersectedPos[index] - handPos).Length();
					if (lengthBuff >= shoterstLength) continue;

					// データを保存する。
					shoterstLength = lengthBuff;
					shortestPos = intersectedPos[index];

				}

				// 最短の距離を保存する。
				pair<Vec2<float>, float> buff = { shortestPos, shoterstLength };
				shortestPoints.push_back(buff);
			}
		}


		/*-- ここまでの過程で様々な最短を求めることができた。 --*/

		// 最短の座標を保存する用変数。
		float shortestLength = 100000.0f;

		// 全ての最短の中から最も短いものを求める。
		const int SHORTEST_COUNT = shortestPoints.size();

		// サイズが0だったら照準をどっかに飛ばしてリターン。
		if (SHORTEST_COUNT <= 0) {

			sightPos = { -100,-100 };
			return false;
		}

		for (int index = 0; index < SHORTEST_COUNT; ++index) {

			// 保存されているデータより大きかったら処理を飛ばす。
			if (shortestPoints[index].second >= shortestLength) continue;

			// データを保存する。
			shortestLength = shortestPoints[index].second;
			sightPos = shortestPoints[index].first;
		}


		//最短距離が一つでも算出されたら当たり判定を出す
		return 0 < shortestPoints.size();
	}


	//A*-------------------------------
	/// <summary>
	/// 探索する際にキューに詰め込むべき情報
	/// </summary>
	struct QueueData
	{
		Vec2<int>handle;
		float sum;
		QueueData(const Vec2<int> &HANDLE, float SUM) :handle(HANDLE), sum(SUM)
		{
		};
	};
	std::vector<QueueData>queue;	//探索用のキュー

	/// <summary>
	/// Aスターによる探索を行う関数
	/// </summary>
	/// <param name="START_POINT">スタート地点</param>
	/// <param name="END_POINT">ゴール地点</param>
	void AStart(const WayPointData &START_POINT, const WayPointData &END_POINT);

	/// <summary>
	/// 同じハンドルがスタックされているかどうか
	/// </summary>
	inline bool CheckQueue(const Vec2<int> &HANDLE);


	WayPointData startPoint, endPoint;
	WayPointData oldStartPoint, oldEndPoint;


	//デバック--------------------------

	Vec2<int> checkingHandle;	//マウスカーソルで参照しているウェイポイント
	int checkTimer;				//ウェイポイントを参照している時間

	bool serachFlag;
	bool lineFlag;
	bool wayPointFlag;

	std::array<std::array<Color, WAYPOINT_MAX_Y>, WAYPOINT_MAX_X> debugColor;

	struct SearchMapData
	{
		Vec2<int>handle;
		Color color;
		SearchMapData(const Vec2<int> &HANDLE, const Color &COLOR) :handle(HANDLE), color(COLOR)
		{
		}
	};
	std::vector<std::vector<SearchMapData>>searchMap;
	int layerNum;
};

