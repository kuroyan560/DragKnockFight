#include "MapChipCollider.h"
#include "DrawFunc.h"

bool MapChipCollider::IsIntersected(const Vec2<float>& posA1, const Vec2<float>& posA2, const Vec2<float>& posB1, const Vec2<float>& posB2)
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

Vec2<float> MapChipCollider::CalIntersectPoint(Vec2<float> posA1, Vec2<float> posA2, Vec2<float> posB1, Vec2<float> posB2)
{
	//交点を求める この式は資料そのまま
	Vec2<float> buff = Vec2<float>(posB2.x - posB1.x, posB2.y - posB1.y);
	double d1 = abs(buff.Cross(Vec2<float>(posA1.x - posB1.x, posA1.y - posB1.y)));
	double d2 = abs(buff.Cross(Vec2<float>(posA2.x - posB1.x, posA2.y - posB1.y)));
	double t = d1 / (d1 + d2);

	return Vec2<float>(posA1.x + (posA2.x - posA1.x) * t, posA1.y + (posA2.y - posA1.y) * t);
}

INTERSECTED_LINE MapChipCollider::CheckHitMapChipBasedOnTheVel(Vec2<float>& pos, const Vec2<float>& prevFramePos, const Vec2<float>& vel, const Vec2<float>& size, bool& onGround, const vector<vector<int>>& mapChipData)
{
	/*===== マップチップとプレイヤーの当たり判定 =====*/


	// プレイヤーの移動量を取得
	Vec2<float> prevFramePosBuff = prevFramePos;
	Vec2<float> velBuff = vel;
	Vec2<float> playerVel = (pos + velBuff) - prevFramePosBuff;

	// プレイヤーの移動量の長さがプレイヤーのサイズよりも小さかったら、プレイヤーのサイズまで伸ばす。
	if (fabs(playerVel.Length()) < size.x) {

		playerVel.Normalize();
		playerVel = playerVel * size;

	}

	const int HEIGHT = mapChipData.size();
	for (int height = 0; height < HEIGHT; ++height) {

		// マップの横
		const int WIDTH = mapChipData[height].size();
		for (int width = 0; width < WIDTH; ++width) {

			// マップIDが0だったら処理を飛ばす。
			if (!(mapChipData[height][width] >= 1 && mapChipData[height][width] <= 9)) continue;

			// このマップの中心座標を求める。
			const float centerX = width * MAP_CHIP_SIZE;
			const float centerY = height * MAP_CHIP_SIZE;

			// 配列外だったら処理を飛ばす。
			//if (centerX < 0) return INTERSECTED_NONE;
			//if (centerX >= WIDTH) return INTERSECTED_NONE;
			//if (centerY < 0) return INTERSECTED_NONE;
			//if (centerY >= HEIGHT) return INTERSECTED_NONE;

			// プレイヤーとの距離が一定以上離れていたら処理を行わない。
			if (Vec2<float>(centerX - pos.x, centerY - pos.y).Length() >= MAP_CHIP_SIZE * 2.0f) {
				onGround = false;
				continue;
			}

			// マップチップをプレイヤー分拡張して4頂点を求める。
			const Vec2<float> rightTop = { centerX + MAP_CHIP_HALF_SIZE , centerY - MAP_CHIP_HALF_SIZE };
			const Vec2<float> rightBottom = { centerX + MAP_CHIP_HALF_SIZE , centerY + MAP_CHIP_HALF_SIZE };
			const Vec2<float> leftTop = { centerX - MAP_CHIP_HALF_SIZE , centerY - MAP_CHIP_HALF_SIZE };
			const Vec2<float> leftBottom = { centerX - MAP_CHIP_HALF_SIZE , centerY + MAP_CHIP_HALF_SIZE };

			// 交点保存用
			vector<pair<Vec2<float>, INTERSECTED_LINE>> intersectPos;

			// 全ての線分との当たり判定を行う。

			// 上辺
			if (IsIntersected(rightTop, leftTop, prevFramePosBuff - playerVel, prevFramePosBuff + playerVel)) {

				// 当たっていたら交点を計算して保存
				pair<Vec2<float>, INTERSECTED_LINE> buff;
				buff.first = CalIntersectPoint(rightTop, leftTop, prevFramePosBuff, prevFramePosBuff + playerVel);

				// そもそも一つ上のインデックスがあるのかどうかをチェックする。
				if (height - 1 >= 0) {

					// 交点の上のマップチップが壁だったら無効化する。
					if (!(mapChipData[height - 1][width] >= 1 && mapChipData[height - 1][width] <= 9)) {

						buff.second = INTERSECTED_TOP;
						intersectPos.push_back(buff);

					}

				}

			}

			// 右辺
			if (IsIntersected(rightTop, rightBottom, prevFramePosBuff - playerVel, prevFramePosBuff + playerVel)) {

				// 当たっていたら交点を計算して保存
				pair<Vec2<float>, INTERSECTED_LINE> buff;
				buff.first = CalIntersectPoint(rightTop, rightBottom, prevFramePosBuff, prevFramePosBuff + playerVel);

				// そもそも一つ右のインデックスがあるのかどうかをチェックする。
				if (width + 1 < mapChipData[height].size()) {

					// 交点の右のマップチップが壁だったら無効化する。
					if (!(mapChipData[height][width + 1] >= 1 && mapChipData[height][width + 1] <= 9)) {

						buff.second = INTERSECTED_RIGHT;
						intersectPos.push_back(buff);

					}

				}

			}

			// 下辺
			if (IsIntersected(leftBottom, rightBottom, prevFramePosBuff - playerVel, prevFramePosBuff + playerVel)) {

				// 当たっていたら交点を計算して保存
				pair<Vec2<float>, INTERSECTED_LINE> buff;
				buff.first = CalIntersectPoint(leftBottom, rightBottom, prevFramePosBuff, prevFramePosBuff + playerVel);

				// 交点との距離がプレイヤーの半径+マップ半径より小さかったら保存する。
				//if (fabs(Vec2<float>(buff.first - player.centerPos).x) < player.PLAYER_SIZE.y + MAP_CHIP_HALF_SIZE &&
				//	fabs(Vec2<float>(buff.first - player.centerPos).y - MAP_CHIP_HALF_SIZE) < player.PLAYER_SIZE.y + MAP_CHIP_HALF_SIZE) {

				// そもそも一つ右のインデックスがあるのかどうかをチェックする。
				if (height + 1 < mapChipData.size()) {

					// 交点の上のマップチップが壁だったら無効化する。
					if (!(mapChipData[height + 1][width] >= 1 && mapChipData[height + 1][width] <= 9)) {

						buff.second = INTERSECTED_BOTTOM;
						intersectPos.push_back(buff);

					}

				}

			}

			// 左辺
			if (IsIntersected(leftBottom, leftTop, prevFramePosBuff - playerVel, prevFramePosBuff + playerVel)) {

				// 当たっていたら交点を計算して保存
				pair<Vec2<float>, INTERSECTED_LINE> buff;
				buff.first = CalIntersectPoint(leftBottom, leftTop, prevFramePosBuff, prevFramePosBuff + playerVel);

				// そもそも一つ上のインデックスがあるのかどうかをチェックする。
				if (width - 1 >= 0) {

					// 交点の左のマップチップが壁だったら無効化する。
					if (!(mapChipData[height][width - 1] >= 1 && mapChipData[height][width - 1] <= 9)) {

						buff.second = INTERSECTED_LEFT;
						intersectPos.push_back(buff);

					}

				}

			}

			// 交点が0個だったら次へ
			if (intersectPos.size() <= 0) continue;

			// 全ての交点の中からプレイヤーとの距離が最小のものを求める。
			pair<Vec2<float>, INTERSECTED_LINE> miniIntersectedPoint;
			miniIntersectedPoint.first = { 100000,100000 };
			for (int index = 0; index < intersectPos.size(); ++index) {

				// 二点間の距離が保存されているものよりも小さかったら、その座標を保存する。
				if (Vec2<float>(intersectPos[index].first - pos).Length() < Vec2<float>(miniIntersectedPoint.first - pos).Length()) {

					miniIntersectedPoint = intersectPos[index];

				}

			}

			// 押し戻してから当たった辺を返す。

			// 最小の交点の種類によって処理を分ける・
			if (miniIntersectedPoint.second == INTERSECTED_TOP) {

				// 押し戻す。
				pos.y = miniIntersectedPoint.first.y - size.y;

			}
			else if (miniIntersectedPoint.second == INTERSECTED_RIGHT) {

				// ひとつ上のマップチップが空白だったら押し戻さないで上に飛ばすという処理を挟む。
				int mapX = (miniIntersectedPoint.first.x - MAP_CHIP_HALF_SIZE) / MAP_CHIP_SIZE;
				int mapY = (pos.y) / MAP_CHIP_SIZE;
				if (mapX > 0 && mapX < mapChipData[0].size() && mapY > 0 && mapY < mapChipData.size() && mapChipData[mapY][mapX] == 0) {

					// 上に移動させる。

					// 上に移動させるべき量を調べる。
					float risingAmount = (pos.y) - (MAP_CHIP_SIZE * mapY);

					float offset = 1.0f;

					// 移動させる。
					pos.y -= risingAmount + offset;

					// 壁にあたった判定をさせないために戻り値用のIDを無効化する。
					miniIntersectedPoint.second = INTERSECTED_RISE;

					if (0 < vel.x) {

						pos.x += offset;

					}
					else if (vel.x < 0) {

						pos.x -= offset;

					}

				}
				else {

					// 押し戻す。
					pos.x = miniIntersectedPoint.first.x + size.x;

				}

			}
			else if (miniIntersectedPoint.second == INTERSECTED_BOTTOM) {

				// 押し戻す。
				float offset = 1.0f;
				pos.y = miniIntersectedPoint.first.y + size.y + offset;

			}
			else if (miniIntersectedPoint.second == INTERSECTED_LEFT) {

				// ひとつ上のマップチップが空白だったら押し戻さないで上に飛ばすという処理を挟む。
				int mapX = (miniIntersectedPoint.first.x + MAP_CHIP_HALF_SIZE) / MAP_CHIP_SIZE;
				int mapY = (pos.y) / MAP_CHIP_SIZE;
				if (mapX > 0 && mapX < mapChipData[0].size() && mapY > 0 && mapY < mapChipData.size() && mapChipData[mapY][mapX] == 0) {

					// 上に移動させる。

					// 上に移動させるべき量を調べる。
					float risingAmount = (pos.y) - (MAP_CHIP_SIZE * mapY);

					float offset = 1.0f;

					// 移動させる。
					pos.y -= risingAmount + offset;

					// 壁にあたった判定をさせないために戻り値用のIDを無効化する。
					miniIntersectedPoint.second = INTERSECTED_RISE;

					if (0 < vel.x) {

						pos.x += offset;

					}
					else if (vel.x < 0) {

						pos.x -= offset;

					}

				}
				else {

					// 押し戻す。
					pos.x = miniIntersectedPoint.first.x - size.x;

				}


			}

			return miniIntersectedPoint.second;

		}

	}

	return INTERSECTED_NONE;
}

INTERSECTED_LINE MapChipCollider::CheckHitMapChipBasedOnTheScale(Vec2<float>& pos, const Vec2<float>& size, const vector<vector<int>>& mapChipData, const INTERSECTED_LINE& direction, const bool& onGimmick)
{
	/*===== マップチップとプレイヤーの当たり判定 =====*/


	// 当たり判定に使用する線分を生成。
	Vec2<float> checkHitDirection = {};
	Vec2<float> checkHitPos = {};

	switch (direction) {

	case INTERSECTED_TOP:

		// 上方向の線分を生成。
		checkHitDirection = { pos.x, pos.y - size.y };
		checkHitPos = { pos.x, pos.y + size.y };

		break;

	case INTERSECTED_BOTTOM:

		// 下方向の線分を生成。
		checkHitDirection = { pos.x, pos.y + size.y };
		checkHitPos = { pos.x, pos.y - size.y };

		break;

	case INTERSECTED_LEFT:

		// 左方向の線分を生成。
		checkHitDirection = { pos.x - size.x, pos.y };
		checkHitPos = { pos.x + size.x, pos.y };

		break;

	case INTERSECTED_RIGHT:

		// 右方向の線分を生成。
		checkHitDirection = { pos.x + size.x, pos.y };
		checkHitPos = { pos.x - size.x, pos.y };

		break;

	default:

		break;

	}


	const int HEIGHT = mapChipData.size();
	for (int height = 0; height < HEIGHT; ++height) {

		// マップの横
		const int WIDTH = mapChipData[height].size();
		for (int width = 0; width < WIDTH; ++width) {

			// マップIDが0だったら処理を飛ばす。
			if (!(mapChipData[height][width] >= 1 && mapChipData[height][width] <= 9)) continue;

			// このマップの中心座標を求める。
			const float centerX = width * MAP_CHIP_SIZE;
			const float centerY = height * MAP_CHIP_SIZE;

			// 配列外だったら処理を飛ばす。
			//if (centerX < 0) return INTERSECTED_NONE;
			//if (centerX >= WIDTH) return INTERSECTED_NONE;
			//if (centerY < 0) return INTERSECTED_NONE;
			//if (centerY >= HEIGHT) return INTERSECTED_NONE;

			// プレイヤーとの距離が一定以上離れていたら処理を行わない。
			if (size.x * 2.0f <= fabs(centerX - pos.x)) {
				//player.onGround = false;
				continue;
			}
			if (size.y * 2.0f <= fabs(centerY - pos.y)) {
				//player.onGround = false;
				continue;
			}

			// マップチップをプレイヤー分拡張して4頂点を求める。
			const Vec2<float> rightTop = { centerX + MAP_CHIP_HALF_SIZE , centerY - MAP_CHIP_HALF_SIZE };
			const Vec2<float> rightBottom = { centerX + MAP_CHIP_HALF_SIZE , centerY + MAP_CHIP_HALF_SIZE };
			const Vec2<float> leftTop = { centerX - MAP_CHIP_HALF_SIZE , centerY - MAP_CHIP_HALF_SIZE };
			const Vec2<float> leftBottom = { centerX - MAP_CHIP_HALF_SIZE , centerY + MAP_CHIP_HALF_SIZE };

			// 交点保存用
			vector<pair<Vec2<float>, INTERSECTED_LINE>> intersectPos;


			// 全ての線分との当たり判定を行う。

			// 上辺
			//if (direction == INTERSECTED_BOTTOM) {
			if (IsIntersected(rightTop, leftTop, checkHitPos, checkHitDirection)) {

				// 当たっていたら交点を計算して保存
				pair<Vec2<float>, INTERSECTED_LINE> buff;
				buff.first = CalIntersectPoint(rightTop, leftTop, checkHitPos, checkHitDirection);

				// そもそも一つ上のインデックスがあるのかどうかをチェックする。
				if (height - 1 >= 0) {

					// 交点の上のマップチップが壁だったら無効化する。
					if (!(mapChipData[height - 1][width] >= 1 && mapChipData[height - 1][width] <= 9)) {

						buff.second = INTERSECTED_TOP;
						intersectPos.push_back(buff);

					}

				}

			}
			//		}

			// 右辺
			//if (direction == INTERSECTED_LEFT) {
			if (IsIntersected(rightTop, rightBottom, checkHitPos, checkHitDirection)) {

				// 当たっていたら交点を計算して保存
				pair<Vec2<float>, INTERSECTED_LINE> buff;
				buff.first = CalIntersectPoint(rightTop, rightBottom, checkHitPos, checkHitDirection);

				// そもそも一つ右のインデックスがあるのかどうかをチェックする。
				if (width + 1 < mapChipData[height].size()) {

					// 交点の右のマップチップが壁だったら無効化する。
					if (!(mapChipData[height][width + 1] >= 1 && mapChipData[height][width + 1] <= 9)) {

						buff.second = INTERSECTED_RIGHT;
						intersectPos.push_back(buff);

					}

				}

			}
			//		}

			// 下辺
			//if (direction == INTERSECTED_TOP) {
			if (IsIntersected(leftBottom, rightBottom, checkHitPos, checkHitDirection)) {

				// 当たっていたら交点を計算して保存
				pair<Vec2<float>, INTERSECTED_LINE> buff;
				buff.first = CalIntersectPoint(leftBottom, rightBottom, checkHitPos, checkHitDirection);

				// 交点との距離がプレイヤーの半径+マップ半径より小さかったら保存する。
				//if (fabs(Vec2<float>(buff.first - player.centerPos).x) < player.PLAYER_SIZE.y + MAP_CHIP_HALF_SIZE &&
				//	fabs(Vec2<float>(buff.first - player.centerPos).y - MAP_CHIP_HALF_SIZE) < player.PLAYER_SIZE.y + MAP_CHIP_HALF_SIZE) {

				// そもそも一つ右のインデックスがあるのかどうかをチェックする。
				if (height + 1 < mapChipData.size()) {

					// 交点の上のマップチップが壁だったら無効化する。
					if (!(mapChipData[height + 1][width] >= 1 && mapChipData[height + 1][width] <= 9)) {

						buff.second = INTERSECTED_BOTTOM;
						intersectPos.push_back(buff);

					}

				}

			}
			//	}

			// 左辺
			//if (direction == INTERSECTED_RIGHT) {
			if (IsIntersected(leftBottom, leftTop, checkHitPos, checkHitDirection)) {

				// 当たっていたら交点を計算して保存
				pair<Vec2<float>, INTERSECTED_LINE> buff;
				buff.first = CalIntersectPoint(leftBottom, leftTop, checkHitPos, checkHitDirection);

				// そもそも一つ上のインデックスがあるのかどうかをチェックする。
				if (width - 1 >= 0) {

					// 交点の左のマップチップが壁だったら無効化する。
					if (!(mapChipData[height][width - 1] >= 1 && mapChipData[height][width - 1] <= 9)) {

						buff.second = INTERSECTED_LEFT;
						intersectPos.push_back(buff);

					}

				}

			}
			//	}

			// 交点が0個だったら次へ
			if (intersectPos.size() <= 0) continue;

			// 全ての交点の中からプレイヤーとの距離が最小のものを求める。
			pair<Vec2<float>, INTERSECTED_LINE> miniIntersectedPoint;
			miniIntersectedPoint.first = { 100000,100000 };
			for (int index = 0; index < intersectPos.size(); ++index) {

				// 二点間の距離が保存されているものよりも小さかったら、その座標を保存する。
				if (Vec2<float>(intersectPos[index].first - pos).Length() < Vec2<float>(miniIntersectedPoint.first - pos).Length()) {

					miniIntersectedPoint = intersectPos[index];

				}

			}

			// ピッタリ押し戻したら不都合なことが起こるので、多少オフセット値を設ける。
			float pushBackOffset = -1.0f;

			// 最小の交点の種類によって処理を分ける。
			if (miniIntersectedPoint.second == INTERSECTED_TOP) {

				// 押し戻す。
				pos.y = miniIntersectedPoint.first.y - size.y - pushBackOffset;

			}
			else if (miniIntersectedPoint.second == INTERSECTED_RIGHT) {

				// ひとつ上のマップチップが空白だったら押し戻さないで上に飛ばすという処理を挟む。
				int mapX = (miniIntersectedPoint.first.x - MAP_CHIP_HALF_SIZE) / MAP_CHIP_SIZE;
				int mapY = (pos.y) / MAP_CHIP_SIZE;
				if (!onGimmick && mapX > 0 && mapX < mapChipData[0].size() && mapY > 0 && mapY < mapChipData.size() && mapChipData[mapY][mapX] == 0) {

					// 上に移動させる。

					// 上に移動させるべき量を調べる。
					float risingAmount = (pos.y) - (MAP_CHIP_SIZE * mapY);

					float offset = 1.0f;

					// 移動させる。
					pos.y -= risingAmount + offset;

					// 壁にあたった判定をさせないために戻り値用のIDを無効化する。
					miniIntersectedPoint.second = INTERSECTED_RISE;

					pos.x -= offset;

				}
				else {

					// 押し戻す。
					pos.x = miniIntersectedPoint.first.x + size.x + pushBackOffset;

				}

			}
			else if (miniIntersectedPoint.second == INTERSECTED_BOTTOM) {

				// 押し戻す。
				pos.y = miniIntersectedPoint.first.y + size.y + pushBackOffset;

			}
			else if (miniIntersectedPoint.second == INTERSECTED_LEFT) {

				// ひとつ上のマップチップが空白だったら押し戻さないで上に飛ばすという処理を挟む。
				int mapX = (miniIntersectedPoint.first.x + MAP_CHIP_HALF_SIZE) / MAP_CHIP_SIZE;
				int mapY = (pos.y) / MAP_CHIP_SIZE;
				if (!onGimmick && mapX > 0 && mapX < mapChipData[0].size() && mapY > 0 && mapY < mapChipData.size() && mapChipData[mapY][mapX] == 0) {

					// 上に移動させる。

					// 上に移動させるべき量を調べる。
					float risingAmount = (pos.y) - (MAP_CHIP_SIZE * mapY);

					float offset = 1.0f;

					// 移動させる。
					pos.y -= risingAmount + offset;

					// 壁にあたった判定をさせないために戻り値用のIDを無効化する。
					miniIntersectedPoint.second = INTERSECTED_RISE;

					pos.x += offset;

				}
				else {

					// 押し戻す。
					pos.x = miniIntersectedPoint.first.x - size.x - pushBackOffset;

				}

			}

			return miniIntersectedPoint.second;

		}

	}

	return INTERSECTED_NONE;
}
