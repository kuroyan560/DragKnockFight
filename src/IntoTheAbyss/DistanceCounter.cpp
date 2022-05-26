#include "DistanceCounter.h"
#include "CharacterManager.h"
#include "TexHandleMgr.h"
#include "ScrollMgr.h"
#include "KuroFunc.h"
#include "DrawFunc.h"

DistanceCounter::DistanceCounter()
{

	/*===== コンストラクタ =====*/

	distance = 0;
	lineCenterPos = { -1000,-10000 };

	TexHandleMgr::LoadDivGraph("resource/ChainCombat/UI/num.png", 12, Vec2<int>(12, 1), fontGraph.data());

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
	distance = (CharacterManager::Instance()->Right()->pos - CharacterManager::Instance()->Left()->pos).Length() - DEAD_LINE;

}

void DistanceCounter::Draw()
{

	/*===== 描画処理 =====*/

	// インデックスごとにずらす量
	const float INDEX_OFFSET = 256.0f;
	// 描画する数字を決める。
	const int distanceDisitCount = KuroFunc::GetDigit(distance);
	// 描画でずらすオフセットの値。
	const float OFFSET_X = static_cast<float>(distanceDisitCount) / 2.0f;
	for (int index = 0; index < distanceDisitCount; ++index) {

		// 描画する数字。
		const int drawDisit = KuroFunc::GetSpecifiedDigitNum(distance, index);

		if (drawDisit < 0 || 9 < drawDisit) continue;

		float zoom = ScrollMgr::Instance()->zoom;
		zoom = 1.0f - zoom;

		// 描画する。
		DrawFunc::DrawRotaGraph2D(ScrollMgr::Instance()->Affect(lineCenterPos - Vec2<float>((INDEX_OFFSET * zoom) * index, 0)), Vec2<float>(1.0f * zoom, 1.0f * zoom), 0, TexHandleMgr::GetTexBuffer(fontGraph[drawDisit]));

	}

}
