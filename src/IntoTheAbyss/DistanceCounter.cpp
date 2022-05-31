#include "DistanceCounter.h"
#include "CharacterManager.h"
#include "TexHandleMgr.h"
#include "ScrollMgr.h"
#include "KuroFunc.h"
#include "DrawFunc.h"
#include "DrawFunc_FillTex.h"

DistanceCounter::DistanceCounter()
{

	/*===== コンストラクタ =====*/

	distance = 0;
	lineCenterPos = { -1000,-10000 };

	TexHandleMgr::LoadDivGraph("resource/ChainCombat/UI/num.png", 12, Vec2<int>(12, 1), fontGraph.data());
	TexHandleMgr::LoadDivGraph("resource/ChainCombat/UI/num_red.png", 11, Vec2<int>(11, 1), redFontGraph.data());

	isExpSmall = false;
	exp = 1.0f;

	shakeAmount = {};

}

void DistanceCounter::Init()
{

	/*===== 初期化 =====*/

	distance = 0;
	lineCenterPos = { -1000,-10000 };

	isExpSmall = false;
	exp = 1.0f;

	shakeAmount = {};

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

	if (isExpSmall) {

		exp -= exp / 10.0f;

	}
	else {

		exp += (1.0f - exp) / 10.0f;

	}

	// シェイクの更新処理
	if (distance < MAX_SHAKE_DISTANCE) {

		// 割合を求める。
		float shakeRate = distance / MAX_SHAKE_DISTANCE;
		shakeRate = 1.0f - shakeRate;

		float shakeValue = shakeRate * MAX_SHAKE_AMOUNT;

		shakeAmount = { KuroFunc::GetRand(shakeValue * 2.0f) - shakeValue, KuroFunc::GetRand(shakeValue * 2.0f) - shakeValue };

	}

}

#include "GameSceneCamerMove.h"

void DistanceCounter::Draw()
{

	/*===== 描画処理 =====*/
	static const int BLUE_METER = TexHandleMgr::LoadGraph("resource/ChainCombat/UI/meter_blue.png");
	static const int RED_METER = TexHandleMgr::LoadGraph("resource/ChainCombat/UI/meter_red.png");

	// インデックスごとにずらす量
	const float INDEX_OFFSET = 62.0f;
	// 描画する数字を決める。
	const int distanceDisitCount = KuroFunc::GetDigit(distance);
	// 描画でずらすオフセットの値。
	const float OFFSET_X = static_cast<float>(distanceDisitCount) / 2.0f;
	// 描画座標
	Vec2<float> drawPos = ScrollMgr::Instance()->Affect(lineCenterPos + shakeAmount);
	// 距離によるレート
	float rate = 1.0f - (distance / (MAX_SHAKE_DISTANCE * 2.0f));
	if (rate < 0) rate = 0;
	rate = KuroMath::Ease(InOut, Exp, rate, 0.0f, 1.0f);
	float zoom = ScrollMgr::Instance()->zoom;
	zoom = 1.0f - zoom;
	for (int index = 0; index < distanceDisitCount; ++index) {

		// 描画する数字。
		const int drawDisit = KuroFunc::GetSpecifiedDigitNum(distance, index);

		if (drawDisit < 0 || 9 < drawDisit) continue;

		// 描画する。
		DrawFunc_FillTex::DrawRotaGraph2D(drawPos - Vec2<float>(INDEX_OFFSET * zoom * index + OFFSET_X * zoom, 0) + GameSceneCameraMove::Instance()->move, 
			Vec2<float>(zoom * exp, zoom * exp), 0, TexHandleMgr::GetTexBuffer(fontGraph[drawDisit]), TexHandleMgr::GetTexBuffer(redFontGraph[drawDisit]), rate);
	}
	DrawFunc_FillTex::DrawRotaGraph2D(drawPos - Vec2<float>(INDEX_OFFSET * zoom * -1 + OFFSET_X * zoom, -8) + GameSceneCameraMove::Instance()->move,
		Vec2<float>(zoom * exp, zoom * exp), 0, TexHandleMgr::GetTexBuffer(BLUE_METER), TexHandleMgr::GetTexBuffer(RED_METER), rate);

}
