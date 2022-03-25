#pragma once
#include "Vec.h"
#include "Singleton.h"
#include <vector>

using namespace std;

// データクラス
struct SightCollisionData {

	Vec2<float> pos;		// 座標のポインタ
	Vec2<float> scale;		// 大きさ

	SightCollisionData(Vec2<float>& pos, const Vec2<float>& scale) {

		this->pos = pos;
		this->scale = scale;

	}
	SightCollisionData() {

		this->pos = {};
		this->scale = {};

	}

};

// 照準の判定に使用するデータを保存するクラス
class SightCollisionStorage : public Singleton<SightCollisionStorage> {

public:

	/*===== メンバ変数 =====*/

	vector<SightCollisionData*> data;		// 昇順の当たり判定に使用するデータの配列。


public:

	/*===== メンバ関数 =====*/

	// セットする処理。
	inline void SetData(Vec2<float>* pos, const Vec2<float>& scale) {

		// nullptrだったら処理を飛ばす。
		//if (pos == nullptr) return;

		//// データを追加。
		//data.push_back(SightCollisionData(pos, scale));

	}


};