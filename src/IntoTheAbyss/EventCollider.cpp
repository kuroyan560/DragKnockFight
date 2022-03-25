#include "EventCollider.h"
#include <vector>

using namespace std;

INTERSECTED_LINE EventCpllider::CheckHitVel(Vec2<float> pushBackPosA, const Vec2<float> prevFramePosA, const Vec2<float>& velA, const Vec2<float>& sizeA, const Vec2<float>& posB, const Vec2<float>& sizeB)
{
	/*===== 矩形と矩形の移動量を使った押し戻し付き当たり判定 =====*/


	// 押し戻し対象の移動量を取得
	Vec2<float> prevFramePosBuff = prevFramePosA;
	Vec2<float> velBuff = velA;
	Vec2<float> playerVel = (pushBackPosA + velBuff) - prevFramePosBuff;

	// 押し戻し対象の移動量の長さが押し戻し対象のサイズよりも小さかったら、押し戻し対象のサイズまで伸ばす。
	if (fabs(playerVel.Length()) < sizeA.x) {

		playerVel.Normalize();
		playerVel = playerVel * sizeA;

	}

	// 押し戻し対象との距離が一定以上離れていたら処理を行わない。
	if (Vec2<float>(posB.x - pushBackPosA.x, posB.y - pushBackPosA.y).Length() >= 100.0f) {
		return INTERSECTED_NONE;
	}

	// 判定対象を押し戻し対象分拡張して4頂点を求める。
	const Vec2<float> rightTop = { posB.x + sizeB.x , posB.y - sizeB.y };
	const Vec2<float> rightBottom = { posB.x + sizeB.x , posB.y + sizeB.y };
	const Vec2<float> leftTop = { posB.x - sizeB.x , posB.y - sizeB.y };
	const Vec2<float> leftBottom = { posB.x - sizeB.x , posB.y + sizeB.y };

	// 交点保存用
	vector<pair<Vec2<float>, INTERSECTED_LINE>> intersectPos;

	// 全ての線分との当たり判定を行う。

	// 上辺
	if (IsIntersected(rightTop, leftTop, prevFramePosBuff - playerVel, prevFramePosBuff + playerVel)) {

		// 当たっていたら交点を計算して保存
		pair<Vec2<float>, INTERSECTED_LINE> buff;
		buff.first = CalIntersectPoint(rightTop, leftTop, prevFramePosBuff, prevFramePosBuff + playerVel);

		// 上に当たったというデータを保存。
		buff.second = INTERSECTED_TOP;

		// 交点データを保存。
		intersectPos.push_back(buff);

	}


	// 右辺
	if (IsIntersected(rightTop, rightBottom, prevFramePosBuff - playerVel, prevFramePosBuff + playerVel)) {

		// 当たっていたら交点を計算して保存
		pair<Vec2<float>, INTERSECTED_LINE> buff;
		buff.first = CalIntersectPoint(rightTop, rightBottom, prevFramePosBuff, prevFramePosBuff + playerVel);

		// 右に当たったというデータを保存。
		buff.second = INTERSECTED_RIGHT;

		// 交点データを保存。
		intersectPos.push_back(buff);

	}

	// 下辺
	if (IsIntersected(leftBottom, rightBottom, prevFramePosBuff - playerVel, prevFramePosBuff + playerVel)) {

		// 当たっていたら交点を計算して保存
		pair<Vec2<float>, INTERSECTED_LINE> buff;
		buff.first = CalIntersectPoint(leftBottom, rightBottom, prevFramePosBuff, prevFramePosBuff + playerVel);

		// 下に当たったというデータを保存。
		buff.second = INTERSECTED_BOTTOM;

		// 交点データを保存。
		intersectPos.push_back(buff);

	}

	// 左辺
	if (IsIntersected(leftBottom, leftTop, prevFramePosBuff - playerVel, prevFramePosBuff + playerVel)) {

		// 当たっていたら交点を計算して保存
		pair<Vec2<float>, INTERSECTED_LINE> buff;
		buff.first = CalIntersectPoint(leftBottom, leftTop, prevFramePosBuff, prevFramePosBuff + playerVel);

		// 左側に当たったという判定を保存。
		buff.second = INTERSECTED_LEFT;

		// 交点データを保存。
		intersectPos.push_back(buff);

	}

	// 交点が0個だったら終わる。
	if (intersectPos.size() <= 0) return INTERSECTED_NONE;

	// 全ての交点の中から押し戻し対象との距離が最小のものを求める。
	pair<Vec2<float>, INTERSECTED_LINE> miniIntersectedPoint;
	miniIntersectedPoint.first = { 100000,100000 };
	for (int index = 0; index < intersectPos.size(); ++index) {

		// 二点間の距離が保存されているものよりも小さかったら、その座標を保存する。
		if (Vec2<float>(intersectPos[index].first - pushBackPosA).Length() < Vec2<float>(miniIntersectedPoint.first - pushBackPosA).Length()) {

			miniIntersectedPoint = intersectPos[index];

		}

	}

	// 押し戻してから当たった辺を返す。

	// 最小の交点の種類によって処理を分ける。
	if (miniIntersectedPoint.second == INTERSECTED_TOP) {

		// 押し戻す。
		pushBackPosA.y = miniIntersectedPoint.first.y - sizeA.y;

	}
	else if (miniIntersectedPoint.second == INTERSECTED_RIGHT) {

		// 押し戻す。
		pushBackPosA.x = miniIntersectedPoint.first.x + sizeA.x;

	}
	else if (miniIntersectedPoint.second == INTERSECTED_BOTTOM) {

		// 押し戻す。
		float offset = 1.0f;
		pushBackPosA.y = miniIntersectedPoint.first.y + sizeA.y + offset;

	}
	else if (miniIntersectedPoint.second == INTERSECTED_LEFT) {

		// 押し戻す。
		pushBackPosA.x = miniIntersectedPoint.first.x - sizeA.x;

	}

	return miniIntersectedPoint.second;

}

INTERSECTED_LINE EventCpllider::CheckHitSize(Vec2<float> posA, const Vec2<float>& sizeA, const Vec2<float>& posB, const Vec2<float>& sizeB, const INTERSECTED_LINE& dir)
{
	/*===== マップチップとプレイヤーの当たり判定 =====*/


	// 当たり判定に使用する線分を生成。
	Vec2<float> checkHitDirection = {};

	switch (dir) {

	case INTERSECTED_TOP:

		// 上方向の線分を生成。
		checkHitDirection = { posA.x, posA.y - sizeA.y };

		break;

	case INTERSECTED_BOTTOM:

		// 下方向の線分を生成。
		checkHitDirection = { posA.x, posA.y + sizeA.y };

		break;

	case INTERSECTED_LEFT:

		// 左方向の線分を生成。
		checkHitDirection = { posA.x - sizeA.x, posA.y };

		break;

	case INTERSECTED_RIGHT:

		// 右方向の線分を生成。
		checkHitDirection = { posA.x + sizeA.x, posA.y };

		break;

	default:

		break;

	}

	// プレイヤーとの距離が一定以上離れていたら処理を行わない。
	if (Vec2<float>(posB.x - posA.x, posB.y - posA.y).Length() >= 1000.0f) {
		return INTERSECTED_NONE;
	}

	// マップチップをプレイヤー分拡張して4頂点を求める。
	const Vec2<float> rightTop = { posB.x + sizeB.x , posB.y - sizeB.y };
	const Vec2<float> rightBottom = { posB.x + sizeB.x , posB.y + sizeB.y };
	const Vec2<float> leftTop = { posB.x - sizeB.x , posB.y - sizeB.y };
	const Vec2<float> leftBottom = { posB.x - sizeB.x , posB.y + sizeB.y };

	// 交点保存用
	vector<pair<Vec2<float>, INTERSECTED_LINE>> intersectPos;


	// 全ての線分との当たり判定を行う。

	// 上辺
	if (dir == INTERSECTED_BOTTOM) {
		if (IsIntersected(rightTop, leftTop, posA, checkHitDirection)) {

			// 当たっていたら交点を計算して保存
			pair<Vec2<float>, INTERSECTED_LINE> buff;
			buff.first = CalIntersectPoint(rightTop, leftTop, posA, checkHitDirection);

			// 上方向に当たった情報を保存。
			buff.second = INTERSECTED_TOP;

			// 交点データを保存。
			intersectPos.push_back(buff);

		}
	}

	// 右辺
	if (dir == INTERSECTED_LEFT) {
		if (IsIntersected(rightTop, rightBottom, posA, checkHitDirection)) {

			// 当たっていたら交点を計算して保存
			pair<Vec2<float>, INTERSECTED_LINE> buff;
			buff.first = CalIntersectPoint(rightTop, rightBottom, posA, checkHitDirection);

			// 右側に当たった情報を保存。
			buff.second = INTERSECTED_RIGHT;

			// 交点データを保存。
			intersectPos.push_back(buff);

		}
	}

	// 下辺
	if (dir == INTERSECTED_TOP) {
		if (IsIntersected(leftBottom, rightBottom, posA, checkHitDirection)) {

			// 当たっていたら交点を計算して保存
			pair<Vec2<float>, INTERSECTED_LINE> buff;
			buff.first = CalIntersectPoint(leftBottom, rightBottom, posA, checkHitDirection);

			// 下方向に当たった情報を保存。
			buff.second = INTERSECTED_BOTTOM;

			// 交点データを保存。
			intersectPos.push_back(buff);

		}
	}

	// 左辺
	if (dir == INTERSECTED_RIGHT) {
		if (IsIntersected(leftBottom, leftTop, posA, checkHitDirection)) {

			// 当たっていたら交点を計算して保存
			pair<Vec2<float>, INTERSECTED_LINE> buff;
			buff.first = CalIntersectPoint(leftBottom, leftTop, posA, checkHitDirection);

			// 左側に当たった情報を保存。
			buff.second = INTERSECTED_LEFT;

			// 交点データを保存。
			intersectPos.push_back(buff);

		}
	}

	// 交点が0個だったら次へ
	if (intersectPos.size() <= 0) return INTERSECTED_NONE;

	// 全ての交点の中からプレイヤーとの距離が最小のものを求める。
	pair<Vec2<float>, INTERSECTED_LINE> miniIntersectedPoint;
	miniIntersectedPoint.first = { 100000,100000 };
	for (int index = 0; index < intersectPos.size(); ++index) {

		// 二点間の距離が保存されているものよりも小さかったら、その座標を保存する。
		if (Vec2<float>(intersectPos[index].first - posA).Length() < Vec2<float>(miniIntersectedPoint.first - posA).Length()) {

			miniIntersectedPoint = intersectPos[index];

		}

	}

	// ピッタリ押し戻したら不都合なことが起こるので、多少オフセット値を設ける。
	float pushBackOffset = -1.0f;

	// 最小の交点の種類によって処理を分ける。
	if (miniIntersectedPoint.second == INTERSECTED_TOP) {

		// 押し戻す。
		posA.y = miniIntersectedPoint.first.y - sizeA.y - pushBackOffset;

	}
	else if (miniIntersectedPoint.second == INTERSECTED_RIGHT) {

		// 押し戻す。
		posA.x = miniIntersectedPoint.first.x + sizeA.x + pushBackOffset;

	}
	else if (miniIntersectedPoint.second == INTERSECTED_BOTTOM) {

		// 押し戻す。
		posA.y = miniIntersectedPoint.first.y + sizeA.y + pushBackOffset;

	}
	else if (miniIntersectedPoint.second == INTERSECTED_LEFT) {

		// 押し戻す。
		posA.x = miniIntersectedPoint.first.x - sizeA.x - pushBackOffset;

	}

	return miniIntersectedPoint.second;

}

bool EventCpllider::IsIntersected(const Vec2<float>& posA1, const Vec2<float>& posA2, const Vec2<float>& posB1, const Vec2<float>& posB2)
{
	/*--線分の外積を計算して交差判定を行う--*/
	//第一回 線分Aから見たBの交差判定
	Vec2<float> buffA = Vec2<float>(posA2.x - posA1.x, posA2.y - posA1.y);
	buffA.Normalize();
	Vec2<float> buffB = Vec2<float>(posB1.x - posA1.x, posB1.y - posA1.y);
	buffB.Normalize();
	Vec2<float> buffC = Vec2<float>(posA2.x - posA1.x, posA2.y - posA1.y);
	buffC.Normalize();
	Vec2<float> buffD = Vec2<float>(posB2.x - posA1.x, posB2.y - posA1.y);
	buffD.Normalize();
	float buffE = buffA.Cross(buffB);
	float buffF = buffC.Cross(buffD);
	float result1 = buffE * buffF;
	bool zero1 = false;
	if (buffE * buffF == 0) zero1 = true;
	//第二回 線分Bから見たAの交差判定
	buffA = Vec2<float>(posB2.x - posB1.x, posB2.y - posB1.y);
	buffA.Normalize();
	buffB = Vec2<float>(posA1.x - posB1.x, posA1.y - posB1.y);
	buffB.Normalize();
	buffC = Vec2<float>(posB2.x - posB1.x, posB2.y - posB1.y);
	buffC.Normalize();
	buffD = Vec2<float>(posA2.x - posB1.x, posA2.y - posB1.y);
	buffD.Normalize();
	buffE = buffA.Cross(buffB);
	buffF = buffC.Cross(buffD);
	float result2 = buffE * buffF;
	bool zero2 = false;
	if (buffE * buffF == 0) zero2 = true;

	//線分が交差している時は、線分から見て交差判定したい線分の端点2つが両側にある時。
	//外積で左右判定をすると、交差している時は値の結果が+と-になる。
	//つまり両方の外積を掛けて結果が-になった場合のみ交差している。
	//線分AからみてBを判定、線分BからみてAを判定と二通り判定を行う。
	//この2つの判定結果を掛けた時に-、若しくは0の時2つの線分は交差している。
	if ((result1 <= 0 && result2 <= 0) ||
		(result1 <= 0 && zero2) ||
		(zero1 && result2 <= 0)) {
		return true;
	}
	return false;
}

Vec2<float> EventCpllider::CalIntersectPoint(Vec2<float> posA1, Vec2<float> posA2, Vec2<float> posB1, Vec2<float> posB2)
{
	//交点を求める この式は資料そのまま
	Vec2<float> buff = Vec2<float>(posB2.x - posB1.x, posB2.y - posB1.y);
	double d1 = fabs(buff.Cross(Vec2<float>(posA1.x - posB1.x, posA1.y - posB1.y)));
	double d2 = fabs(buff.Cross(Vec2<float>(posA2.x - posB1.x, posA2.y - posB1.y)));
	double t = d1 / (d1 + d2);

	return Vec2<float>(posA1.x + (posA2.x - posA1.x) * t, posA1.y + (posA2.y - posA1.y) * t);
}
