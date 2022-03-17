#pragma once
#include "Vec.h"
#include "Singleton.h"
#include <memory>
#include <vector>
using namespace std;
class MovingBlock;

// 動的ブロック管理クラス
class MovingBlockMgr : public Singleton<MovingBlockMgr> {

public:

	/*===== メンバ変数 =====*/

	vector<shared_ptr<MovingBlock>> movingBlock;	// 動的ブロック


public:

	/*===== メンバ関数 =====*/

	// 動的ブロックのロード
	void Load(const vector<vector<int>>& mapData);

	// 初期化処理
	void Init();

	// 更新処理
	void Update(const Vec2<float>& playerPos);

	// 描画処理
	void Draw(const int& graphHandle);

	// マップチップとの当たり判定
	void CheckHit(const vector<vector<int>>& mapData);

};