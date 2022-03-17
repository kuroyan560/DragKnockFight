#pragma once
#include "Vec.h"
#include "Singleton.h"
#include "AuraBlock.h"
#include <vector>
#include <memory>

using namespace std;

// 前フレームの頂点座標と今フレームの頂点座標を保存しておくための構造体
struct VertexData {

	Vec2<float> prevFramePos;	// 前フレームの座標
	Vec2<float> nowFramePos;	// 今フレームの座標

};

// ビューポートとオーラの判定の戻り値に使用する用の構造体
struct ViewPortAuraReturnData {

	float pushBackAmount;	// 押し戻し量
	bool isHit;				// 当たったかどうか
	bool isHold;			// 引っかかっているかどうか

};

// ビューポートのどの線分の当たり判定を行うかを識別する用の列挙体
enum CHECK_HIT_LINE {

	CHECK_HIT_TOP,		// 上方向の判定を行う。
	CHECK_HIT_RIGHT,	// 右方向の判定を行う。
	CHECK_HIT_BOTTOM,	// 下方向の判定を行う。
	CHECK_HIT_LEFT,		// 左方向の判定を行う。

};


// マップチップ以外の当たり判定クラス
class ViewPortCollider : public Singleton<ViewPortCollider> {

public:

	/*===== メンバ変数 =====*/

	// 矩形のどの辺に当たったかを返す用。
	enum RECTANGLE_HIT_AREA {

		HIT_AREA_TOP,		// 上の辺に当たった。
		HIT_AREA_RIGHT,		// 右の辺に当たった。
		HIT_AREA_BOTTOM,	// 下の辺に当たった。
		HIT_AREA_LEFT,		// 左の辺に当たった。
		HIT_AREA_NONE,		// どの辺にも当たっていない。

	};


public:

	/*===== メンバ関数 =====*/

	/// <summary>
	/// 矩形と円の判定 ビューポートと弾の判定に使用します。
	/// </summary>
	/// <param name="centerPos"> ビューポートの中心座標 </param>
	/// <param name="topSize"> ビューポートの上方向のサイズの絶対値 </param>
	/// <param name="rightSize"> ビューポートの右方向のサイズの絶対値 </param>
	/// <param name="bottomSize"> ビューポートの下方向のサイズの絶対値 </param>
	/// <param name="leftSize"> ビューポートの左方向のサイズの絶対値 </param>
	/// <param name="bulletPos"> 弾の座標 </param>
	/// <param name="bulletSize"> 弾のサイズ </param>
	/// <returns> どの辺に当たったか、どのへんにも当たっていないのか。 </returns>
	RECTANGLE_HIT_AREA CheckHitRectanglePoint(const Vec2<float>& centerPos, const float& topSize, const float& rightSize, const float& bottomSize, const float& leftSize, const Vec2<float>& bulletPos, const float& bulletSize);

	/// <summary>
	/// ビューポートの指定の線分とオーラの当たり判定
	/// </summary>
	/// <param name="vertex1"> ビューポートの指定の線分の頂点1 左上方面のやつはこっち </param>
	/// <param name="vertex2"> ビューポートの指定の線分の頂点2 右下方面のやつはこっち </param>
	/// <param name="checkHitLine"> ビューポートの当たり判定を行いたい線分の識別ID </param>
	/// <param name="auraData">	オーラ保存データの配列 </param>
	/// <returns> 戻り値構造体 </returns>
	vector<ViewPortAuraReturnData> CheckHitRectangleAura(VertexData vertex1, VertexData vertex2, const CHECK_HIT_LINE& checkHitLine, vector<unique_ptr<AuraBlock>>& auraData);


private:

	// 線分と線分の当たり判定
	bool IsIntersected(const Vec2<float>& posA1, const Vec2<float>& posA2, const Vec2<float>& posB1, const Vec2<float>& posB2);

	// 線分と線分の交点を求める処理
	Vec2<float> CalIntersectPoint(Vec2<float> posA1, Vec2<float> posA2, Vec2<float> posB1, Vec2<float> posB2);

	// 当たった情報からViewPortAuraReturnDataを返す処理。
	ViewPortAuraReturnData GetViewPortAuraReturnData(const CHECK_HIT_LINE& checkHitLine, VertexData vertex1, VertexData vertex2, const Vec2<float>& leftUpPos, const Vec2<float>& rightUpPos, const Vec2<float>& rightDownPos, const Vec2<float>& leftDownPos, const CHECK_HIT_LINE& auraLine, bool isPrevViewPort = false);

};