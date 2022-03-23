#pragma once
#include "Vec.h"
#include "Beacon.h"
#include <vector>
using namespace std;
#include"WinApp.h"

class LightManager;
#include"AfterImg.h"

// プレイヤーの手クラス
class PlayerHand {

public:

	/*-- メンバ変数 --*/

	Vec2<float> handPos;			// 腕の描画座標
	Vec2<float> vel;				// 弾を打った時の移動量
	Vec2<float> sightPos;			// 照準座標
	Vec2<float> drawPos;			// Draw You
	float armDistance;		// プレイヤーの中心からの距離 右手と左手で変えるため
	float inputAngle;		// 入力された角度
	bool isFirstShot;		// 最初の一発は強い反動フラグ
	bool isNoInputTimer;	// Input Sitenai Tokiha Honrai Amount Wo Herasu

	Beacon teleportPike;	// 瞬間移動の短槍
	Beacon timeStopPike;	// 時間停止の短槍
	int pikeCooltime;		// 短槍全般ののクールタイム

	AfterImg afterImg;

	/*-- 定数 --*/

public:

	const float ARM_RANGE_OF_MOTION = 15.0f;	// 手の可動域
	const float SHOT_VEL = 15.0f;				// 弾を撃った時の反動
	const float FIRST_SHOT_VEL = 20.0f;			// 最初の一発目を撃った時の反動
	const float SIGHT_SIZE = 5.0f;				// 照準のサイズ
	const int PIKE_COOL_TIME = 60;				// ビーコンのクールタイム


	/*-- メンバ関数 --*/

public:

	// コンストラクタ
	PlayerHand();

	// 初期化処理
	void Init(const float& armDistance);

	// 更新処理
	void Update(const Vec2<float>& playerCenterPos);

	// 描画処理
	void Draw(LightManager& LigManager, const Vec2<float>& ExtRate, const int& GraphHandle, const float& InitAngle, const Vec2<float>& RotaCenterUV, const bool &DRAW_CURSOR);

	// 弾を打った時の処理
	void Shot(const Vec2<float>& forwardVec, const bool& isFirstShot);

	// 当たり判定関数
	void CheckHit(const vector<vector<int>>& mapData);

	// 照準を描画する最短距離を求める。
	void CheckShortestPoint(const vector<vector<int>>& mapData);

	bool IsIntersected(const Vec2<float>& posA1, const Vec2<float>& posA2, const Vec2<float>& posB1, const Vec2<float>& posB2);

	Vec2<float> CalIntersectPoint(Vec2<float> posA1, Vec2<float> posA2, Vec2<float> posB1, Vec2<float> posB2);

	// Angle wo Tikadukeru
	inline void PutCloseAngle(const float& defAngle) { inputAngle += (defAngle - inputAngle) / 10.0f; }

	// isNoInputTimer no Setter
	inline void SetIsNoInputTimer(const bool& set) { isNoInputTimer = set; }

	// 壁にあたった時に最初に当たったフラグを初期化する処理.
	inline void Hit() { isFirstShot = false; }

	// 最初に撃った時の処理。
	inline void FirstShot() { isFirstShot = true; }

	// 角度のセッタ
	inline void SetAngle(const float& angle) { inputAngle = angle; }
	// 角度のゲッタ
	inline const float& GetAngle() { return inputAngle; }
	// 座標のゲッタ
	inline const Vec2<float>& GetPos() { return handPos; }
	// 最初に当たったフラグのゲッタ
	inline const bool& GetIsFirstShot() { return isFirstShot; }

	//残像を出す
	void EmitAfterImg(const Vec2<float>& TeleAmount, const int& GraphHandle, const Vec2<float>& GraphSize, const Vec2<bool>& Miror);

private:

	// 切り上げ
	int RoundUp(int size, int align) {
		return UINT(size + align - 1) & ~(align - 1);
	}

	//スクロールなどにも考慮した中心座標
	Vec2<float> GetCenterDrawPos();
};