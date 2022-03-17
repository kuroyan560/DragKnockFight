#pragma once
#include "Vec.h"

#include"WinApp.h"

// 弾消滅時のパーティクル
class BulletParticle {

public:

	/*===== メンバ変数 =====*/

	Vec2<float> pos;			// 座標
	Vec2<float> forwardVec;	// 移動方向
	Vec2<float> movedVel;		// 移動した量
	float alpha;		// アルファ値
	float speed;		// 移動速度
	bool isAlive;		// 生存フラグ


public:

	/*===== 定数 =====*/

	const float DEF_SPEED = 3.0f;
	const float SCALE = 6.0f;
	const float SUB_SPEED = 0.3f;


public:

	/*===== メンバ関数 =====*/

	// コンストラクタ
	BulletParticle();

	// 初期化処理
	void Init();

	// 生成処理
	void Generate(const Vec2<float>& generatePos, const Vec2<float>& forwardVec);

	// 更新処理
	void Update();

	// 描画処理
	void Draw();

private:

	// 切り上げ
	int RoundUp(int size, int align) {
		return UINT(size + align - 1) & ~(align - 1);
	}


};

/*

処理メモ
・パーティクルは自分のサイズ分の格子状にしか動けない。
・movedVelが自分のサイズ分に達したらposに移動方向にサイズ分の値を足す。
・speedはだんだん遅くなる。
・生成場所は結構ランダム。
・アルファブレンドをかける。

*/