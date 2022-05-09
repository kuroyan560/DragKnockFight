#include "SwingLineSegmentMgr.h"
#include "DrawFunc.h"
#include "../Engine/DrawFunc_Color.h"
#include "ScrollMgr.h"
#include "TexHandleMgr.h"
#include "MapChipCollider.h"
#include"StageMgr.h"

void SwingLineSegment::Init()
{

	/*===== 初期化処理 =====*/

	isActive = false;

}

void SwingLineSegment::Update(const Vec2<float>& Start, const Vec2<float>& End, const Vec2<float>& StartDir, const Vec2<float>& EndDir, const int& Alpha, const SEGMENT_ID& ID, const int& Handle)
{

	/*===== 更新処理 =====*/

	start = Start;
	end = End;
	startDir = StartDir;
	endDir = EndDir;
	alpha = Alpha;
	id = ID;
	isActive = true;
	graphHandle = Handle;

}

void SwingLineSegment::Draw(const WHICH_TEAM& Team)
{
	static const int TEAM_COLOR_X[TEAM_NUM] = { 47,239 };
	static const int TEAM_COLOR_Y[TEAM_NUM] = { 255,1 };
	static const int TEAM_COLOR_Z[TEAM_NUM] = { 139,144 };

	/*===== 描画処理 =====*/

	Vec2<float> drawPos = {};
	Vec2<float> size = {};

	switch (id)
	{
	case SwingLineSegment::SEGMENT_ID::SEGMENT_ID_LINE:

		// 仮でアルファで画像を変える。
		DrawFunc_Color::DrawLine2DGraph(ScrollMgr::Instance()->Affect(start), ScrollMgr::Instance()->Affect(end), TexHandleMgr::GetTexBuffer(graphHandle),
			Color(TEAM_COLOR_X[Team], TEAM_COLOR_Y[Team], TEAM_COLOR_Z[Team], alpha), 32);

		break;

	case SwingLineSegment::SEGMENT_ID::SEGMENT_ID_UI:

		drawPos = Vec2<float>(start - end) / 2.0f;
		size = { 64.0f,64.0f };
		DrawFunc_Color::DrawLine2DGraph(ScrollMgr::Instance()->Affect(start), ScrollMgr::Instance()->Affect(end), TexHandleMgr::GetTexBuffer(graphHandle),
			Color(TEAM_COLOR_X[Team], TEAM_COLOR_Y[Team], TEAM_COLOR_Z[Team], alpha), 32);

		break;

	case SwingLineSegment::SEGMENT_ID::SEGMENT_ID_ARROW:

		// 仮でアルファで画像を変える。
		DrawFunc_Color::DrawLine2DGraph(ScrollMgr::Instance()->Affect(start), ScrollMgr::Instance()->Affect(end), TexHandleMgr::GetTexBuffer(graphHandle),
			Color(TEAM_COLOR_X[Team], TEAM_COLOR_Y[Team], TEAM_COLOR_Z[Team], alpha), 32);

		break;

	default:
		break;
	}

	//DrawFunc::DrawLine2D(ScrollMgr::Instance()->Affect(start), ScrollMgr::Instance()->Affect(end), Color(255, 255, 255, alpha), AlphaBlendMode_Trans);

}

void SwingLineSegment::ResetDistance(const Vec2<float>& Pos, const float& Distance)
{

	/*===== 距離修正 =====*/

	start = Pos + startDir * Distance;
	end = Pos + endDir * Distance;

}

void SwingLineSegmentMgr::Setting(const bool& IsClockWise, const int& HandleUI, const int& HandleArrow, const int& HandleLine, const int& ReticleHandle)
{

	/*===== 前準備 =====*/

	isClockWise = IsClockWise;
	UIHandle = HandleUI;
	arrowHandle = HandleArrow;
	lineHandle = HandleLine;
	reticleHandle = ReticleHandle;

}

void SwingLineSegmentMgr::Init()
{

	/*===== 初期化処理 =====*/

	for (int index = 0; index < LINE_COUNT; ++index) {

		lineSegments[index].Init();

	}

	reticlePos = { -1000,-1000 };

}

void SwingLineSegmentMgr::Update(const Vec2<float>& Pos, const Vec2<float>& TargetVec, const float& Distance, const vector<vector<int>>& MapData)
{

	/*===== 更新処理 =====*/

	// 現在の角度
	float nowAngle = atan2f(TargetVec.y, TargetVec.x);

	// 現在の角度が-だった場合は正しい角度に直す。
	if (nowAngle < 0) {

		float angleBuff = 3.14f - fabs(nowAngle);
		nowAngle = 3.14f + angleBuff;

	}

	// 全ての線分を生成する。
	for (int index = 0; index < LINE_COUNT; ++index) {

		// 開始時角度と終了時角度を求める。
		float startAngle = nowAngle;
		float endAngle = nowAngle + (isClockWise ? ANGLE_DIFFERENCE : -ANGLE_DIFFERENCE);

		// 終了時角度が - or 6.28f を超えていたら矯正
		if (endAngle < 0 || 6.28f < endAngle) {

			// 時計回りだったら。
			if (isClockWise) {

				endAngle -= 6.28f;

			}
			// 反時計回りだったら。
			else {

				endAngle += 6.28f;

			}

		}

		// 使用する画像を決める。
		int handle = 0;
		SwingLineSegment::SEGMENT_ID id;
		// 最後の線分は矢印を描画する。
		if (index == LINE_COUNT - 1) {
			handle = arrowHandle;
			id = SwingLineSegment::SEGMENT_ID::SEGMENT_ID_ARROW;
		}
		// それ以外は普通の線分を描画する。
		else {
			handle = lineHandle;
			id = SwingLineSegment::SEGMENT_ID::SEGMENT_ID_LINE;
		}


		Vec2<float> startPos = Pos + Vec2<float>(cosf(startAngle), sinf(startAngle)) * Distance;
		Vec2<float> endPos = Pos + Vec2<float>(cosf(endAngle), sinf(endAngle)) * Distance;

		// 線分を生成。
		lineSegments[index].Update(startPos, endPos, Vec2<float>(cosf(startAngle), sinf(startAngle)), Vec2<float>(cosf(endAngle), sinf(endAngle)), 250, id, handle);

		// 角度を保存。
		nowAngle = endAngle;

	}

	/*===== 各線分とマップチップの当たり判定を行う =====*/

	// 照準画像を吹っ飛ばす。(毎フレーム衝突点を計算するため)。
	reticlePos = { -1000,-1000 };

	// 一度マップチップに当たったらそれ以降は描画しないand当たり判定を行わないようにするために使用。
	int isHitMapChip = false;
	for (int index = 0; index < LINE_COUNT; ++index) {

		// 既にマップチップと当たっていたら処理を飛ばす and 無効化する。
		if (isHitMapChip) {

			lineSegments[index].Deactivate();
			continue;

		}

		// マップチップと当たり判定を行う。
		Vec2<float> resultPos = CheckHitMapChip(lineSegments[index].GetStart(), lineSegments[index].GetEnd());

		// 値が-1だったら当たっていないということなので、処理を飛ばす。
		if (resultPos.x == -1) continue;

		// マップチップと当たった判定にする。
		lineSegments[index].SetEnd(resultPos);

		// スイング中だったら交点のところの線分を矢印にする。
		//if (IsSwing) {
		lineSegments[index].SetID(SwingLineSegment::SEGMENT_ID::SEGMENT_ID_ARROW);
		lineSegments[index].SetHandle(arrowHandle);
		//}

		isHitMapChip = true;

		// 照準の位置を保存。
		reticlePos = resultPos;

	}

	// 照準の位置がふっとばされていたら、線分の終端に置く。
	if (reticlePos.x < -100) {

		reticlePos = lineSegments[LINE_COUNT - 1].GetEnd();

	}

	// マップチップと当たっていたら、全ての線分の色をちょっとだけ濃くする。
	isHitWallFlag = false;
	if (isHitMapChip) {
		for (int index = 0; index < LINE_COUNT; ++index) {

			// 線分は濃くしない。	橋本さんから貰う画像を入れる際はこの処理はいらなくなる。
			if (lineSegments[index].GetID() == SwingLineSegment::SEGMENT_ID::SEGMENT_ID_ARROW)
			{
				isHitWallFlag = true;
				continue;
			}
			lineSegments[index].SetAlpha(200);

		}

	}

}

void SwingLineSegmentMgr::Draw(const WHICH_TEAM& Team)
{

	/*===== 描画処理 =====*/

	for (int index = 0; index < LINE_COUNT; ++index) {

		if (!lineSegments[index].GetIsActive())continue;

		lineSegments[index].Draw(Team);

	}

	// 照準を描画。
	Vec2<float> size = { 32,32 };
	DrawFunc::DrawExtendGraph2D(ScrollMgr::Instance()->Affect(reticlePos - size), ScrollMgr::Instance()->Affect(reticlePos + size), TexHandleMgr::GetTexBuffer(reticleHandle));

}

#include "WinApp.h"
#include <IntoTheAbyss/StageMgr.h>
#include "SelectStage.h"
#include "Collider.h"
bool SwingLineSegmentMgr::IsHitWall()
{
	return isHitWallFlag;
}
float SwingLineSegmentMgr::CalSwingEndDistance(const Vec2<float>& CharaPos, const Vec2<float>& SwingTargetVec, const float& Distance)
{

	// レティクルの位置がふっとばされていたら。
	if (reticlePos.x < -100 || reticlePos.y < -100) {

		// 開始地点から終了地点までの距離を求めて、それを返す。
		Vec2<float> endPos = CharaPos + SwingTargetVec * Distance;
		float buff = (swingStartPos - endPos).Length();
		return buff;

	}
	else {

		float buff = (reticlePos - swingStartPos).Length();
		return buff;

	}

}
const Vec2<float>& SwingLineSegmentMgr::CheckHitMapChip(const Vec2<float>& StartPos, const Vec2<float>& EndPos)
{
	//どうやって使うか
	Vec2<float>handSegmentStart(StartPos), handSegmentEnd(EndPos);//線分
	Vec2<float>handSegmentDir(EndPos - StartPos);					//線分の方向
	Vec2<float>handPos(StartPos);									//線分の始点
	Vec2<float>sightPos;						//求められた交点の中の最短距離
	RoomMapChipArray mapData = StageMgr::Instance()->GetMapChipData(SelectStage::Instance()->GetStageNum(), SelectStage::Instance()->GetRoomNum());					//マップ
	//どうやって使うか


	// 最短距離を保存するようの配列。
	std::vector<std::pair<Vec2<float>, float>> shortestPoints;

	// 照準のレイの方向によって当たり判定を無効化する為のフラグをセットする。
	bool isTop = handSegmentDir.y < 0;
	bool isLeft = handSegmentDir.x < 0;

	SizeData mapChipSizeData = StageMgr::Instance()->GetMapChipSizeData(MAPCHIP_TYPE_STATIC_BLOCK);

	// 次にマップチップとの最短距離を求める。
	const int MAP_Y = mapData.size();
	for (int height = 0; height < MAP_Y; ++height) {

		const int MAP_X = mapData[height].size();
		for (int width = 0; width < MAP_X; ++width) {

			// このマップチップが1~9以外だったら判定を飛ばす。
			if (mapData[height][width] < mapChipSizeData.min ||
				mapChipSizeData.max < mapData[height][width])
			{
				continue;
			}

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

			// 線分の始点とのマップチップの距離が、視点と終点の距離より大きかったら処理を行わない。
			float startEndDistance = (StartPos - EndPos).Length();
			float blockDistance = (StartPos - BLOCK_POS).Length();
			if (startEndDistance < blockDistance) continue;

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
		return Vec2<float>(-1, -1);
	}

	for (int index = 0; index < SHORTEST_COUNT; ++index) {

		// 保存されているデータより大きかったら処理を飛ばす。
		if (shortestPoints[index].second >= shortestLength) continue;

		// データを保存する。
		shortestLength = shortestPoints[index].second;
		sightPos = shortestPoints[index].first;
	}


	//最短距離が一つでも算出されたら当たり判定を出す
	return sightPos;

}
