#include "DistanceCounter.h"
#include "CharacterManager.h"

DistanceCounter::DistanceCounter()
{

	/*===== コンストラクタ =====*/

	distance = 0;
	lineCenterPos = { -1000,-10000 };

}

void DistanceCounter::Init()
{

	/*===== 初期化 =====*/

	distance = 0;
	lineCenterPos = { -1000,-10000 };

}

void DistanceCounter::Update()
{

	/*===== 更新処理 =====*/

	// 二キャラの座標を足した値。
	Vec2<float> addCharaPos = CharacterManager::Instance()->Right()->pos + CharacterManager::Instance()->Left()->pos;

	// 線の中心を求める。
	lineCenterPos = addCharaPos / 2.0f;

	// 2点間の距離を求める。
	distance = addCharaPos.Length() - DEAD_LINE;

}

void DistanceCounter::Draw()
{

	/*===== 描画処理 =====*/

}
