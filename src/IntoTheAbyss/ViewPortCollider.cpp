#include "ViewPortCollider.h"
#include "MapChipCollider.h"

ViewPortCollider::RECTANGLE_HIT_AREA ViewPortCollider::CheckHitRectanglePoint(const Vec2<float>& centerPos, const float& topSize, const float& rightSize, const float& bottomSize, const float& leftSize, const Vec2<float>& bulletPos, const float& bulletSize)
{

	/*===== 矩形と円の当たり判定 -ビューポートと弾用- =====*/

	// 各頂点を計算する。
	Vec2<float> rectangleTopLeft = Vec2<float>(centerPos.x - leftSize, centerPos.y - topSize);
	Vec2<float> rectangleTopRight = Vec2<float>(centerPos.x + rightSize, centerPos.y - topSize);
	Vec2<float> rectangleBottomRight = Vec2<float>(centerPos.x + rightSize, centerPos.y + bottomSize);
	Vec2<float> rectangleBottomLeft = Vec2<float>(centerPos.x - leftSize, centerPos.y + bottomSize);

	// 矩形の外に出ているのかをチェックする。
	if (!(bulletPos.x >= rectangleTopLeft.x + bulletSize && bulletPos.x <= rectangleTopRight.x - bulletSize &&
		bulletPos.y >= rectangleTopRight.y + bulletSize && bulletPos.y <= rectangleBottomRight.y - bulletSize)) {

		// 矩形の外に出ているということは、どこかしらに当たったということ。


		// 上下左右それぞれの距離を求める。
		array<pair<float, ViewPortCollider::RECTANGLE_HIT_AREA>, ViewPortCollider::HIT_AREA_NONE> distanceBuff;

		// 距離を求める。
		distanceBuff[ViewPortCollider::HIT_AREA_TOP] = { fabs(rectangleTopRight.y) - fabs(bulletPos.y), ViewPortCollider::HIT_AREA_TOP };
		distanceBuff[ViewPortCollider::HIT_AREA_RIGHT] = { fabs(bulletPos.x) - fabs(rectangleTopRight.x), ViewPortCollider::HIT_AREA_RIGHT };
		distanceBuff[ViewPortCollider::HIT_AREA_BOTTOM] = { fabs(bulletPos.y) - fabs(rectangleBottomRight.y), ViewPortCollider::HIT_AREA_BOTTOM };
		distanceBuff[ViewPortCollider::HIT_AREA_LEFT] = { fabs(rectangleTopLeft.x) - fabs(bulletPos.x), ViewPortCollider::HIT_AREA_LEFT };

		// 一番大きい要素を検索する。
		pair<float, ViewPortCollider::RECTANGLE_HIT_AREA> mostBigAmount = { -100000, ViewPortCollider::HIT_AREA_NONE };

		for (int index = 0; index < ViewPortCollider::HIT_AREA_NONE; ++index) {

			// 要素が小さかったら次へ
			if (mostBigAmount.first >= distanceBuff[index].first) continue;

			// 要素を保存。
			mostBigAmount = distanceBuff[index];

		}

		// 一番大きい要素を返す。
		return mostBigAmount.second;

	}

	// 矩形の内側にいるということは、まだどこにも当たっていないということ。

	return ViewPortCollider::HIT_AREA_NONE;
}

vector<ViewPortAuraReturnData> ViewPortCollider::CheckHitRectangleAura(VertexData vertex1, VertexData vertex2, const CHECK_HIT_LINE& checkHitLine, vector<unique_ptr<AuraBlock>>& auraData)
{

	/*===== ビューポートの指定の線分とオーラの当たり判定 =====*/

	// 戻り値用配列。
	vector<ViewPortAuraReturnData> returnData;

	// 検索対象の線分の前フレームの座標などを使って矩形を生成する。
	Vec2<float> viewportLeftUpPos = {};

	// オーラデータのサイズを求める。
	const int MAX_AURA_BLOCK = auraData.size();

	// サイズ分配列を回して、全てのブロックとの当たり判定を行う。
	for (int auraIndex = 0; auraIndex < MAX_AURA_BLOCK; ++auraIndex) {

		float offset = 0.0f;

		// わかりやすくするため、オーラの各頂点を一旦ここに書き出す。
		Vec2<float> leftUpPos = auraData[auraIndex]->leftUpPos + Vec2<float>(offset, offset);
		Vec2<float> leftDownPos = Vec2<float>(auraData[auraIndex]->leftUpPos.x, auraData[auraIndex]->rightDownPos.y) + Vec2<float>(offset, -offset);
		Vec2<float> rightUpPos = Vec2<float>(auraData[auraIndex]->rightDownPos.x, auraData[auraIndex]->leftUpPos.y) + Vec2<float>(-offset, offset);
		Vec2<float> rightDownPos = auraData[auraIndex]->rightDownPos + Vec2<float>(-offset, -offset);

		/*-- 矩形同士で全ての線分と線分の当たり判定を行う。 --*/

		// まずは今フレームとのビューポートの線分との判定を行う。
		{

			// オーラの上の辺から当たり判定を行う。
			if (IsIntersected(vertex1.nowFramePos, vertex2.nowFramePos, leftUpPos, rightUpPos)) {

				// 引数のID、今フレームの座標と前フレームの座標の関係から押し戻す方向と引っかかりフラグの有無を調べる。
				ViewPortAuraReturnData buff = GetViewPortAuraReturnData(checkHitLine, vertex1, vertex2, leftUpPos, rightUpPos, rightDownPos, leftDownPos, CHECK_HIT_TOP);

				// データを保存。
				returnData.push_back(buff);

			}
			// オーラの右の辺の当たり判定を行う。
			if (IsIntersected(vertex1.nowFramePos, vertex2.nowFramePos, rightUpPos, rightDownPos)) {

				// 引数のID、今フレームの座標と前フレームの座標の関係から押し戻す方向と引っかかりフラグの有無を調べる。
				ViewPortAuraReturnData buff = GetViewPortAuraReturnData(checkHitLine, vertex1, vertex2, leftUpPos, rightUpPos, rightDownPos, leftDownPos, CHECK_HIT_RIGHT);

				// データを保存。
				returnData.push_back(buff);

			}
			// オーラの下の辺の当たり判定を行う。
			if (IsIntersected(vertex1.nowFramePos, vertex2.nowFramePos, leftDownPos, rightDownPos)) {

				// 引数のID、今フレームの座標と前フレームの座標の関係から押し戻す方向と引っかかりフラグの有無を調べる。
				ViewPortAuraReturnData buff = GetViewPortAuraReturnData(checkHitLine, vertex1, vertex2, leftUpPos, rightUpPos, rightDownPos, leftDownPos, CHECK_HIT_BOTTOM);

				// データを保存。
				returnData.push_back(buff);

			}
			// オーラの左の辺の当たり判定を行う。
			if (IsIntersected(vertex1.nowFramePos, vertex2.nowFramePos, Vec2<float>(leftUpPos.x + 0.0f, leftUpPos.y), leftDownPos)) {

				// 引数のID、今フレームの座標と前フレームの座標の関係から押し戻す方向と引っかかりフラグの有無を調べる。
				ViewPortAuraReturnData buff = GetViewPortAuraReturnData(checkHitLine, vertex1, vertex2, leftUpPos, rightUpPos, rightDownPos, leftDownPos, CHECK_HIT_LEFT);

				// データを保存。
				returnData.push_back(buff);

			}

		}

		// 次に前フレームとのビューポートの線分との判定を行う。
		{

			//オーラの上の辺から当たり判定を行う。
			if (IsIntersected(vertex1.prevFramePos, vertex2.prevFramePos, leftUpPos, Vec2<float>(rightUpPos.x + 0.0f, rightUpPos.y))) {

				// 引数のID、今フレームの座標と前フレームの座標の関係から押し戻す方向と引っかかりフラグの有無を調べる。
				ViewPortAuraReturnData buff = GetViewPortAuraReturnData(checkHitLine, vertex1, vertex2, leftUpPos, rightUpPos, rightDownPos, leftDownPos, CHECK_HIT_TOP, true);

				// データを保存。
				returnData.push_back(buff);

			}
			// オーラの右の辺の当たり判定を行う。
			if (IsIntersected(vertex1.prevFramePos, vertex2.prevFramePos, rightUpPos, rightDownPos)) {

				// 引数のID、今フレームの座標と前フレームの座標の関係から押し戻す方向と引っかかりフラグの有無を調べる。
				ViewPortAuraReturnData buff = GetViewPortAuraReturnData(checkHitLine, vertex1, vertex2, leftUpPos, rightUpPos, rightDownPos, leftDownPos, CHECK_HIT_RIGHT, true);

				// データを保存。
				returnData.push_back(buff);

			}
			// オーラの下の辺の当たり判定を行う。
			if (IsIntersected(vertex1.prevFramePos, vertex2.prevFramePos, leftDownPos, rightDownPos)) {

				// 引数のID、今フレームの座標と前フレームの座標の関係から押し戻す方向と引っかかりフラグの有無を調べる。
				ViewPortAuraReturnData buff = GetViewPortAuraReturnData(checkHitLine, vertex1, vertex2, leftUpPos, rightUpPos, rightDownPos, leftDownPos, CHECK_HIT_BOTTOM, true);

				// データを保存。
				returnData.push_back(buff);

			}
			// オーラの左の辺の当たり判定を行う。
			if (IsIntersected(vertex1.prevFramePos, vertex2.prevFramePos, Vec2<float>(leftUpPos.x + 0.0f, leftUpPos.y), Vec2<float>(leftDownPos.x - 0.0f, leftDownPos.y))) {

				// 引数のID、今フレームの座標と前フレームの座標の関係から押し戻す方向と引っかかりフラグの有無を調べる。
				ViewPortAuraReturnData buff = GetViewPortAuraReturnData(checkHitLine, vertex1, vertex2, leftUpPos, rightUpPos, rightDownPos, leftDownPos, CHECK_HIT_LEFT, true);

				// データを保存。
				returnData.push_back(buff);

			}

		}

	}

	return returnData;

}

bool ViewPortCollider::IsIntersected(const Vec2<float>& posA1, const Vec2<float>& posA2, const Vec2<float>& posB1, const Vec2<float>& posB2)
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
	// T時の時はtrueを返させない為に実装。
	//if (buffE == 0 && buffF != 0) return false;
	//if (buffE != 0 && buffF == 0) return false;
	if (buffE == 0 && buffF == 0) zero1 = true;
	if (result1 > -0.001f && result1 < 0) zero1 = true;
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
	//if (buffE == 0 && buffF != 0) return false;
	//if (buffE != 0 && buffF == 0) return false;
	if (buffE * buffF == 0) zero2 = true;
	if (fabs(result2) < 0.001f) zero2 = true;

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

Vec2<float> ViewPortCollider::CalIntersectPoint(Vec2<float> posA1, Vec2<float> posA2, Vec2<float> posB1, Vec2<float> posB2)
{
	//交点を求める この式は資料そのまま
	Vec2<float> buff = Vec2<float>(posB2.x - posB1.x, posB2.y - posB1.y);
	double d1 = abs(buff.Cross(Vec2<float>(posA1.x - posB1.x, posA1.y - posB1.y)));
	double d2 = abs(buff.Cross(Vec2<float>(posA2.x - posB1.x, posA2.y - posB1.y)));
	double t = d1 / (d1 + d2);

	return Vec2<float>(posA1.x + (posA2.x - posA1.x) * t, posA1.y + (posA2.y - posA1.y) * t);
}

ViewPortAuraReturnData ViewPortCollider::GetViewPortAuraReturnData(const CHECK_HIT_LINE& checkHitLine, VertexData vertex1, VertexData vertex2, const Vec2<float>& leftUpPos, const Vec2<float>& rightUpPos, const Vec2<float>& rightDownPos, const Vec2<float>& leftDownPos, const CHECK_HIT_LINE& auraLine, bool isPrevViewPort)
{
	// ヒット情報保存用変数。
	ViewPortAuraReturnData hitData{};

	const float offset = 0.0f;

	// 上だったら。
	if (checkHitLine == CHECK_HIT_TOP) {

		// 今フレームのほうが上だったら(下から上に登っている場合)。
		if (vertex1.prevFramePos.y > vertex1.nowFramePos.y) {

			// 前フレームの判定だった場合、下、右、左の場合は処理を行わない。
			if (!(isPrevViewPort && (auraLine == CHECK_HIT_TOP || auraLine == CHECK_HIT_RIGHT || auraLine == CHECK_HIT_LEFT))) {

				// 交点との差分を求めて押し戻す量を求める。
				hitData.pushBackAmount = leftDownPos.y - vertex1.nowFramePos.y + offset;

				// この場合は引っかかっていない。
				hitData.isHold = false;

				// 当たっている。
				hitData.isHit = true;

			}

		}
		// 前フレームのほうが上だったら(上から下に落ちてきている場合)。
		else if (vertex1.prevFramePos.y < vertex1.nowFramePos.y) {

			// 前フレームの判定だった場合、上、右、左の場合は処理を行わない。
			if (!(isPrevViewPort && (auraLine == CHECK_HIT_BOTTOM || auraLine == CHECK_HIT_RIGHT || auraLine == CHECK_HIT_LEFT))) {

				// 交点との差分を求めて押し戻す量を求める。
				hitData.pushBackAmount = leftUpPos.y - vertex1.nowFramePos.y - offset;

				// この場合は引っかかっている。
				hitData.isHold = true;

				// 当たっている。
				hitData.isHit = true;

			}

		}

	}
	// 右側だったら。
	else if (checkHitLine == CHECK_HIT_RIGHT) {

		// 前フレームのほうが右だったら(右から左に来ている場合)。
		if (vertex1.prevFramePos.x > vertex1.nowFramePos.x) {

			// 前フレームの判定だった場合、上、右、下の場合は処理を行わない。
			if (!(isPrevViewPort && (auraLine == CHECK_HIT_TOP || auraLine == CHECK_HIT_LEFT || auraLine == CHECK_HIT_BOTTOM))) {

				// 交点との差分を求めて押し戻す量を求める。
				hitData.pushBackAmount = rightDownPos.x - vertex1.nowFramePos.x + 0.0f;

				// この場合は引っかかっている。
				hitData.isHold = true;

				// 当たっている。
				hitData.isHit = true;

			}

		}
		// 今フレームのほうが右だったら(左から右に来ている場合)。
		else if (vertex1.prevFramePos.x < vertex1.nowFramePos.x) {

			// 前フレームの判定だった場合、上、右、下の場合は処理を行わない。
			if (!(isPrevViewPort && (auraLine == CHECK_HIT_TOP || auraLine == CHECK_HIT_RIGHT || auraLine == CHECK_HIT_BOTTOM))) {

				// 交点との差分を求めて押し戻す量を求める。
				hitData.pushBackAmount = leftDownPos.x - vertex1.nowFramePos.x - 0.0f;

				// この場合は引っかかっていない。
				hitData.isHold = false;

				// 当たっている。
				hitData.isHit = true;

			}

		}

	}
	// 下側だったら。
	else if (checkHitLine == CHECK_HIT_BOTTOM) {

		// 前フレームのほうが下だったら(下から上に来ている場合)。
		if (vertex1.prevFramePos.y > vertex1.nowFramePos.y) {

			// 前フレームの判定だった場合、下、右、左の場合は処理を行わない。
			if (!(isPrevViewPort && (auraLine == CHECK_HIT_TOP || auraLine == CHECK_HIT_RIGHT || auraLine == CHECK_HIT_LEFT))) {

				// 交点との差分を求めて押し戻す量を求める。
				hitData.pushBackAmount = rightDownPos.y - vertex1.nowFramePos.y + offset;

				// この場合は引っかかっている。
				hitData.isHold = true;

				// 当たっている。
				hitData.isHit = true;

			}

		}
		// 今フレームのほうが下だったら(上から下に来ている場合)。
		else if (vertex1.prevFramePos.y < vertex1.nowFramePos.y) {

			// 前フレームの判定だった場合、下、右、左の場合は処理を行わない。
			if (!(isPrevViewPort && (auraLine == CHECK_HIT_BOTTOM || auraLine == CHECK_HIT_RIGHT || auraLine == CHECK_HIT_LEFT))) {

				// 交点との差分を求めて押し戻す量を求める。
				hitData.pushBackAmount = rightUpPos.y - vertex1.nowFramePos.y - offset;

				// この場合は引っかかっていない。
				hitData.isHold = false;

				// 当たっている。
				hitData.isHit = true;

			}

		}

	}
	// 左側だったら。
	else if (checkHitLine == CHECK_HIT_LEFT) {

		// 前フレームのほうが左だったら(右から左に来ている場合)。
		if (vertex1.prevFramePos.x < vertex1.nowFramePos.x) {

			// 前フレームの判定だった場合、上、右、下の場合は処理を行わない。
			if (!(isPrevViewPort && (auraLine == CHECK_HIT_TOP || auraLine == CHECK_HIT_RIGHT || auraLine == CHECK_HIT_BOTTOM))) {

				// 交点との差分を求めて押し戻す量を求める。
				hitData.pushBackAmount = leftDownPos.x - vertex1.nowFramePos.x - 0.0f;

				// この場合は引っかかっている。
				hitData.isHold = true;

				// 当たっている。
				hitData.isHit = true;

			}

		}
		// 前フレームのほうが左だったら(左から右に来ている場合)。
		else if (vertex1.prevFramePos.x > vertex1.nowFramePos.x) {

			// 前フレームの判定だった場合、上、右、下の場合は処理を行わない。
			if (!(isPrevViewPort && (auraLine == CHECK_HIT_TOP || auraLine == CHECK_HIT_LEFT || auraLine == CHECK_HIT_BOTTOM))) {

				// 交点との差分を求めて押し戻す量を求める。
				hitData.pushBackAmount = rightDownPos.x - vertex1.nowFramePos.x + 0.0f;

				// この場合は引っかかっていない。
				hitData.isHold = false;

				// 当たっている。
				hitData.isHit = true;

			}

		}

	}

	return hitData;
}
