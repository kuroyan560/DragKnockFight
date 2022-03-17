#pragma once
#include"Vec.h"
#include<array>
#include"ViewPortCollider.h"
#include"Singleton.h"

/// <summary>
/// ビューポートに関する処理
/// </summary>
class ViewPort : public Singleton<ViewPort>
{
public:

	void Init(Vec2<float> PLAYER_POS, Vec2<float> WINDOW_SIZE);
	void Update(const Vec2<float>& playerPos);
	void Draw();

	void MoveLine(int LINE, float ZOOM_RATE);

	void SetPlayerPosX(float PLAYER_POS);
	void SetPlayerPosY(float PLAYER_POS);

	// 押し戻し処理
	void PushBackAura(const Vec2<float>& pushBackAmount);
	void PushBackAuraHoldUp(const float& pushBackAmount);
	void PushBackAuraHoldDown(const float& pushBackAmount);
	void PushBackAuraHoldRight(const float& pushBackAmount);
	void PushBackAuraHoldLeft(const float& pushBackAmount);
	void PushBackLine(int LINE, const float& pushBackAmount);

	// ウィンドウサイズを可変にする。
	void AddAddLineValue(const float& value, const int& ID);
	void SavePrevFlamePos();

	float zoomRate;
	Vec2<float> windowSize;


	enum { LEFT_UP, LEFT_DOWN, RIGHT_UP, RIGHT_DOWN };
	std::array<Vec2<float>, 4> pointPos;//矩形を構成する四つの点の情報
	std::array<VertexData, 4> pointData;	// 当たり判定に使用する頂点データ 前フレームの座標などを保存してある。
	Vec2<float> centralPos;
	Vec2<float> playerPos, prevPlayerPos;
	array<bool, 4> enableAuraFlags;//何処の部分と当たっているか

public:

	enum Dir { UP, DOWN, LEFT, RIGHT };
	std::array<Vec2<float>, 4> initPointPos;//矩形を構成する四つの点の情報の初期値
	Vec2<float> honraiCentralPos;


	array<bool, 4>hitLineFlag;//何処の辺と当たったか
	array<float, 4>addLineValue;//何処の辺と当たったか
	array<float, 4>honraiAddLineValue;//何処の辺と当たったか
	array<float, 4> increLineVel;
	array<int, 4> countHitLine;//弾が辺と当たった回数を記録する
	array<float, 4>addVel;		//今プレイヤーが弾を撃った腕がどの方向に向いているか
	float decreVel;
	array<int, 4> intervalTimer;
	array<bool, 4> holdFlags;//何処の部分と当たっているか
	array<bool, 4> noHitFlags;//どの辺の当たり判定を無効にするか


	bool isHitRight;		// X軸でオーラに当たっているかどうかのフラグ
	bool isHitLeft;			// X軸でオーラに当たっているかどうかのフラグ
	bool isHitBottom;		// Y軸でオーラに当たっているかどうかのフラグ
	bool isHitTop;			// Y軸でオーラに当たっているかどうかのフラグ


	bool zoomFlag;
	const float zoomValue = 0.1f;		//ズーム量
	const float decreVelValue = 0.001f;	//ズームの減らす移動量
	float zoomVel;

	bool RayHit(Vec2<float> a, Vec2<float> b, Vec2<float> c, Vec2<float> d)
	{
		double s, t;
		s = (a.x - b.x) * (c.y - a.y) - (a.y - b.y) * (c.x - a.x);
		t = (a.x - b.x) * (d.y - a.y) - (a.y - b.y) * (d.x - a.x);
		if (s * t > 0)
			return false;

		s = (c.x - d.x) * (a.y - c.y) - (c.y - d.y) * (a.x - c.x);
		t = (c.x - d.x) * (b.y - c.y) - (c.y - d.y) * (b.x - c.x);
		if (s * t > 0)
			return false;
		return true;
	}
};
