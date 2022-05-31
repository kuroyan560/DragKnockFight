#include "MapChipGenerator.h"
#include"StageMgr.h"
#include"MapChipCollider.h"
#include"KuroMath.h"
#include"KuroFunc.h"
#include"DebugParameter.h"
#include"SelectStage.h"
#include<algorithm>
#include "CharacterManager.h"

bool MapChipGenerator::CanChange(const Vec2<int>& Idx)
{
	auto mapData = StageMgr::Instance()->GetLocalMap();
	Vec2<int> idxSize = { (int)(*mapData)[0].size(),(int)(*mapData).size() };

	if (Idx.x <= 0)return false;
	else if (idxSize.x - 1 <= Idx.x)return false;
	if (Idx.y <= 0)return false;
	else if (idxSize.y - 1 <= Idx.y)return false;

	auto type = (*mapData)[Idx.y][Idx.x].chipType;
	if (type != 0)return false;
	return true;
}

int MapChipGenerator::GetRandChipType()
{
	//出現確率
	const float RARE = 1.0f;
	const float BOUNCE = 0.5f;

	int appearType = 1;

	float rate = KuroFunc::GetRand(99.0f);
	if (rate < (BOUNCE < RARE ? BOUNCE : RARE))
	{
		appearType = MAPCHIP_TYPE_STATIC_BOUNCE_BLOCK;
	}
	else if (rate < RARE + BOUNCE)
	{
		appearType = MAPCHIP_TYPE_STATIC_RARE_BLOCK;
	}
	return appearType;
}

void MapChipGenerator::Generate(const Vec2<int>& GenerateIdx, const int& ChipType, const int& WallGraph)
{
	if (!CanChange(GenerateIdx))return;
	StageMgr::Instance()->WriteMapChipData(GenerateIdx, ChipType, CharacterManager::Instance()->Left()->pos, CharacterManager::Instance()->Left()->size.x, CharacterManager::Instance()->Right()->pos, CharacterManager::Instance()->Right()->size.x, true, INTERSECTED_NONE, true, WallGraph);
}


void MapChipGenerator::Generate(const Vec2<float>& GeneratePos, const int& ChipType, const int& WallGraph)
{
	//生成座標を基にその場所のチップ番号取得
	Vec2<int>centerIdx =
	{
		static_cast<int>((GeneratePos.x + MAP_CHIP_HALF_SIZE) / MAP_CHIP_SIZE),
		static_cast<int>((GeneratePos.y + MAP_CHIP_HALF_SIZE) / MAP_CHIP_SIZE)
	};

	//生成する場所配列
	const std::vector<Vec2<int>>generateIndices
	{
		centerIdx,	//中央
		centerIdx + Vec2<int>(0,-1),	//上
		centerIdx + Vec2<int>(0,1),	//下
		centerIdx + Vec2<int>(-1,0),	//左
		centerIdx + Vec2<int>(1,0),	//右
	};

	for (auto& idx : generateIndices)
	{
		Generate(idx, ChipType, WallGraph);
	}
}

float MapChipGenerator_SplineOrbit::GetDistRate()
{
	static const float DIST_BASE = 500.0f;	//１とする距離
	float dist = targetPos[1].Distance(targetPos[2]);
	return DIST_BASE / dist;
}

Vec2<float> MapChipGenerator_SplineOrbit::GetRandPos()
{
	//マップ内のランダムな座標を取得
	Vec2<float>range = StageMgr::Instance()->GetLocalMapGrobalSize();
	range -= RADIUS * 2.0f;
	return Vec2<float>(RADIUS + KuroFunc::GetRand(range.x), RADIUS + KuroFunc::GetRand(range.y));
}

MapChipGenerator_SplineOrbit::MapChipGenerator_SplineOrbit()
{
	Vec2<float>mapSize = StageMgr::Instance()->GetLocalMapGrobalSize();

	//中心スタート
	pos = mapSize / 2.0f;

	targetPos[0] = GetRandPos();
	targetPos[1] = pos;
	targetPos[2] = GetRandPos();
	targetPos[3] = GetRandPos();
	t = 0.0f;
}

#include"ParticleMgr.h"
void MapChipGenerator_SplineOrbit::Update()
{
	std::vector<Vec2<float>>targetPosVector;
	for (auto& tp : targetPos)
	{
		targetPosVector.emplace_back(tp);
	}

	pos = KuroMath::GetSpline(t, 1, targetPosVector);
	Generate(pos, GetRandChipType());

	if (1.0f <= t)
	{
		targetPos[0] = targetPos[1];
		targetPos[1] = targetPos[2];
		targetPos[2] = targetPos[3];
		targetPos[3] = GetRandPos();
		t = 0.0f;
	}
	else
	{
		t += 0.02f * GetDistRate();
		if (1.0f < t)t = 1.0f;
	}

	static int EMIT_TIMER = 0;
	const int EMIT_SPAN = 3;
	if (EMIT_SPAN < EMIT_TIMER)
	{
		ParticleMgr::Instance()->Generate(pos, { 0,0 }, CHIP_SPLINE_GENERATOR);
		EMIT_TIMER = 0;
	}
	EMIT_TIMER++;
}

#include"DrawFunc.h"
#include"ScrollMgr.h"
#include"TexHandleMgr.h"
void MapChipGenerator_SplineOrbit::Draw()
{
	static int ARROW_HANDLE = TexHandleMgr::LoadGraph("resource/ChainCombat/select_scene/arrow.png");
	static const float ARROW_EXP = 0.4f;
	static auto ARROW_SIZE = TexHandleMgr::GetTexBuffer(ARROW_HANDLE)->GetGraphSize() * ARROW_EXP;
	static const int AIM_GRAPH = TexHandleMgr::LoadGraph("resource/ChainCombat/generateAim.png");

	auto drawPos = ScrollMgr::Instance()->Affect(pos);
	const auto drawRadius = ScrollMgr::Instance()->zoom * RADIUS;
	const auto winSize = WinApp::Instance()->GetExpandWinSize();

	//画面外のときは矢印
	if (drawPos.x + drawRadius < 0.0f || winSize.x < drawPos.x - drawRadius || drawPos.y + drawRadius < 0.0f || winSize.y < drawPos.y - drawRadius)
	{
		if (drawPos.x + ARROW_SIZE.x < 0.0f)drawPos.x = ARROW_SIZE.x;
		else if (winSize.x < drawPos.x - ARROW_SIZE.x)drawPos.x = winSize.x - ARROW_SIZE.x;
		if (drawPos.y + ARROW_SIZE.y < 0.0f)drawPos.y = ARROW_SIZE.y;
		else if (winSize.y < drawPos.y - ARROW_SIZE.y)drawPos.y = winSize.y - ARROW_SIZE.y;

		float rad = KuroFunc::GetAngle(WinApp::Instance()->GetExpandWinCenter(), drawPos);
		DrawFunc::DrawRotaGraph2D(drawPos, { ARROW_EXP,ARROW_EXP }, rad, TexHandleMgr::GetTexBuffer(ARROW_HANDLE));
	}
	else
	{
		//DrawFunc::DrawCircle2D(drawPos, drawRadius, Color(47, 255, 139, 255), false, 3);
		//DrawFunc::DrawRotaGraph2D(drawPos, { ScrollMgr::Instance()->zoom ,ScrollMgr::Instance()->zoom }, { 0.0f }, TexHandleMgr::GetTexBuffer(AIM_GRAPH));
	}

	static const int ORBIT_GRAPH = TexHandleMgr::LoadGraph("resource/ChainCombat/generateArrow_point.png");
	const float DRAW_POINT_SPAN = 64.0f;
	Vec2<float>exp = { ScrollMgr::Instance()->zoom,ScrollMgr::Instance()->zoom };
	for (int i = 1; i <= 2; ++i)
	{
		Vec2<float>vec = targetPos[i + 1] - targetPos[i];
		Vec2<float> vecUint = vec / DRAW_POINT_SPAN;
		int pointNum = vec.Length() / DRAW_POINT_SPAN;
		for (int j = 0; j < pointNum; ++j)
		{
			DrawFunc::DrawRotaGraph2D(ScrollMgr::Instance()->Affect(targetPos[i] + vecUint * j), exp, KuroFunc::GetAngle(vec), TexHandleMgr::GetTexBuffer(ORBIT_GRAPH));
		}
	}

	//照準
	for (int i = 1; i < 4; ++i)
	{
		DrawFunc::DrawRotaGraph2D(ScrollMgr::Instance()->Affect(targetPos[i]), { ScrollMgr::Instance()->zoom ,ScrollMgr::Instance()->zoom }, { 0.0f }, TexHandleMgr::GetTexBuffer(AIM_GRAPH));
	}

}

int MapChipGenerator_RandPattern::GetRandSpan()
{
	return 60;
	static const int MAX = 60 * 9;
	static const int MIN = 60 * 5;
	return KuroFunc::GetRand(MIN, MAX);
	//return KuroFunc::GetRand(DebugParameter::Instance()->generatorSpanMin, DebugParameter::Instance()->generatorSpanMax);
}

void MapChipGenerator_RandPattern::DesideNextIndices(const PATTERN_TYPE& PatternType, const Vec2<int>& GenerateIdx)
{
	auto mapData = StageMgr::Instance()->GetLocalMap();
	Vec2<int>chipIdxMax;
	chipIdxMax.x = (*mapData)[0].size();
	chipIdxMax.y = (*mapData).size();

	static bool INIT = false;
	static OffsetPattern PATTERN[NUM];
	if (!INIT)
	{
		PATTERN[CROSS] =
		{
			{0,0},	//中央
			{0,-1},	//上
			{0,1},
			{-1,0},	//左
			{1,0},	//右
		};

		PATTERN[CUBE] =
		{
			{-1,-1},
			{0,-1},
			{1,-1},
			{-1,0},
			{0,0},
			{1,0},
			{-1,1},
			{0,1},
			{1,1},
		};

		static const float RADIUS = 500.0f;
		static const Angle RAD_UINT = Angle(5);
		static const int THICKNESS = 2;
		for (auto rad = Angle(0); rad.radian < Angle::PI() * 2.0f; rad += RAD_UINT)
		{
			for (int i = 0; i < THICKNESS * MAP_CHIP_SIZE; i += MAP_CHIP_SIZE / 2)
			{
				Vec2<float>offsetPos = { cos(rad) * (i + RADIUS),sin(rad) * (i + RADIUS) };
				Vec2<int>offsetIdx;
				offsetIdx.x = offsetPos.x / MAP_CHIP_SIZE;
				offsetIdx.y = offsetPos.y / MAP_CHIP_SIZE;

				bool same = false;
				for (auto& idx : PATTERN[CIRCLE])
				{
					if (idx != offsetIdx)continue;
					same = true;
					break;
				}
				if (!same)PATTERN[CIRCLE].emplace_back(offsetIdx);
			}
		}

		INIT = true;
	};

	// 水平なやつ
	PATTERN[HORIZON].clear();
	for (int index = 0; index < chipIdxMax.x; ++index)
	{
		PATTERN[HORIZON].emplace_back(Vec2<int>(index, 0));

	}
	// 垂直なやつ
	PATTERN[VERT].clear();
	for (int index = 0; index < chipIdxMax.y; ++index)
	{
		PATTERN[VERT].emplace_back(Vec2<int>(0, index));
	}

	const auto patternType = PATTERN[PatternType];
	for (auto& offsetIdx : patternType)
	{
		Vec2<int> idx = offsetIdx + GenerateIdx;
		if (idx.x < 0)continue;
		if (chipIdxMax.x <= idx.x)continue;
		if (idx.y < 0)continue;
		if (chipIdxMax.y <= idx.y)continue;

		if (!CanChange(idx))continue;

		predictionIdxArray.emplace_back();
		predictionIdxArray.back().idx = idx;
		predictionIdxArray.back().type = GetRandChipType();
		if (predictionIdxArray.back().type == 1)predictionIdxArray.back().wallGraph = StageMgr::Instance()->GetRandNormalWallGraphHandle();
	}
}

MapChipGenerator_RandPattern::MapChipGenerator_RandPattern(const int& Span, const bool& RandPattern)
{
	span = Span;

	timer = 0;

	if (RandPattern)
	{
		auto mapData = StageMgr::Instance()->GetLocalMap();
		Vec2<int>chipIdxMax;
		chipIdxMax.x = (*mapData)[0].size();
		chipIdxMax.y = (*mapData).size();

		DesideNextIndices(PATTERN_TYPE(KuroFunc::GetRand(PATTERN_TYPE::NUM - 1)), { KuroFunc::GetRand(chipIdxMax.x - 1),KuroFunc::GetRand(chipIdxMax.y - 1) });
	}
}

void MapChipGenerator_RandPattern::Update()
{
	timer++;

	if (span <= timer)
	{
		for (auto& pre : predictionIdxArray)
		{
			Generate(pre.idx, pre.type, pre.wallGraph);
		}

		auto mapData = StageMgr::Instance()->GetLocalMap();
		Vec2<int>chipIdxMax;
		chipIdxMax.x = (*mapData)[0].size();
		chipIdxMax.y = (*mapData).size();
		predictionIdxArray.clear();
		DesideNextIndices(PATTERN_TYPE(KuroFunc::GetRand(PATTERN_TYPE::NUM - 1)), Vec2<int>(KuroFunc::GetRand(1, chipIdxMax.x), KuroFunc::GetRand(1, chipIdxMax.y)));
		timer = 0;
	}
}

#include"DrawMap.h"
#include<map>
void MapChipGenerator_RandPattern::Draw()
{
	static std::map<int, DrawMap>DRAW_MAP_WALL;
	static DrawMap DRAW_MAP_RARE;
	static DrawMap DRAW_MAP_BOUNCE;
	static DrawMap DRAW_MAP_UNBROKEN;
	static DrawMap DRAW_MAP_NON_SCORE;
	static DrawMap DRAW_MAP_EMPTY;
	static const auto EMPTY_TEX = D3D12App::Instance()->GenerateTextureBuffer(Color(56, 22, 74, 255), DXGI_FORMAT_R32G32B32A32_FLOAT, 32);

	// 描画するチップのサイズ
	const float DRAW_MAP_CHIP_SIZE = MAP_CHIP_SIZE * ScrollMgr::Instance()->zoom;

	//生成までの予測
	float predictionRate = (float)timer / span;
	predictionRate = KuroMath::Ease(In, Circ, predictionRate, 0.0f, 1.0f);


	for (const auto& pred : predictionIdxArray)
	{
		ChipData chipData;
		// スクロール量から描画する位置を求める。
		const Vec2<float> drawPos = ScrollMgr::Instance()->Affect({ pred.idx.x * MAP_CHIP_SIZE,pred.idx.y * MAP_CHIP_SIZE });

		// 画面外だったら描画しない。
		if (drawPos.x < -DRAW_MAP_CHIP_SIZE || drawPos.x > WinApp::Instance()->GetWinSize().x + DRAW_MAP_CHIP_SIZE) continue;
		if (drawPos.y < -DRAW_MAP_CHIP_SIZE || drawPos.y > WinApp::Instance()->GetWinSize().y + DRAW_MAP_CHIP_SIZE) continue;
		if (!CanChange(pred.idx))continue;

		chipData.pos = drawPos;
		chipData.alpha = predictionRate;

		if (pred.type == MAPCHIP_TYPE_STATIC_RARE_BLOCK)
		{
			DRAW_MAP_RARE.AddChip(chipData);
		}
		else if (pred.type == MAPCHIP_TYPE_STATIC_BOUNCE_BLOCK)
		{
			DRAW_MAP_BOUNCE.AddChip(chipData);
		}
		else if (pred.type == MAPCHIP_TYPE_STATIC_UNBROKEN_BLOCK)
		{
			DRAW_MAP_UNBROKEN.AddChip(chipData);
		}
		else if (pred.type == MAPCHIP_TYPE_STATIC_NON_SCORE_BLOCK)
		{
			DRAW_MAP_NON_SCORE.AddChip(chipData);
		}
		else if (pred.type == 1)
		{
			DRAW_MAP_WALL[pred.wallGraph].AddChip(chipData);
		}
		else
		{
			DRAW_MAP_EMPTY.AddChip(chipData);
		}
	}
	for (auto& drawMap : DRAW_MAP_WALL)
	{
		drawMap.second.Draw(TexHandleMgr::GetTexBuffer(drawMap.first));
	}
	DRAW_MAP_RARE.Draw(TexHandleMgr::GetTexBuffer(StageMgr::Instance()->GetChipGraoh(MAPCHIP_TYPE_STATIC_RARE_BLOCK)));
	DRAW_MAP_BOUNCE.Draw(TexHandleMgr::GetTexBuffer(StageMgr::Instance()->GetChipGraoh(MAPCHIP_TYPE_STATIC_BOUNCE_BLOCK)));
	DRAW_MAP_NON_SCORE.Draw(TexHandleMgr::GetTexBuffer(StageMgr::Instance()->GetChipGraoh(MAPCHIP_TYPE_STATIC_NON_SCORE_BLOCK)));
	DRAW_MAP_UNBROKEN.Draw(TexHandleMgr::GetTexBuffer(StageMgr::Instance()->GetChipGraoh(MAPCHIP_TYPE_STATIC_UNBROKEN_BLOCK)));
	DRAW_MAP_EMPTY.Draw(EMPTY_TEX);
}

void MapChipGenerator_ChangeMap::RegisterMap()
{
	predictionIdxArray.clear();

	if (!StageMgr::Instance()->HaveMap(stageNumber, setMapNumber))setMapNumber = 0;

	auto mapIdxSize = StageMgr::Instance()->GetMapIdxSize(stageNumber, setMapNumber);
	for (int y = 0; y < mapIdxSize.y; ++y)
	{
		for (int x = 0; x < mapIdxSize.x; ++x)
		{
			predictionIdxArray.emplace_back();
			predictionIdxArray.back().idx = { x,y };
			predictionIdxArray.back().type = StageMgr::Instance()->GetMap(stageNumber, setMapNumber)[y][x].chipType;
		}
	}
	setMapNumber++;
}

#include"RoundFinishEffect.h"
MapChipGenerator_ChangeMap::MapChipGenerator_ChangeMap()
{
	stageNumber = SelectStage::Instance()->GetStageNum();
	setMapNumber = 1;
	RegisterMap();
}

void MapChipGenerator_ChangeMap::Update()
{
}

void MapChipGenerator_ChangeMap::Draw()
{
	static DrawMap DRAW_MAP_WALL;
	static DrawMap DRAW_MAP_RARE;
	static DrawMap DRAW_MAP_BOUNCE;
	static DrawMap DRAW_MAP_UNBROKEN;
	static DrawMap DRAW_MAP_NON_SCORE;
	static DrawMap DRAW_MAP_EMPTY;
	static const auto EMPTY_TEX = D3D12App::Instance()->GenerateTextureBuffer(Color(56, 22, 74, 255), DXGI_FORMAT_R32G32B32A32_FLOAT, 32);

	if (!RoundFinishEffect::Instance()->changeMap)return;

	// 描画するチップのサイズ
	const float DRAW_MAP_CHIP_SIZE = MAP_CHIP_SIZE * ScrollMgr::Instance()->zoom;

	//生成までの予測
	float predictionRate = (float)RoundFinishEffect::Instance()->timer / RoundFinishEffect::Instance()->NUM4_RETURN_DEFPOS_TIMER;
	predictionRate = KuroMath::Ease(In, Circ, predictionRate, 0.0f, 1.0f);


	for (const auto& pred : predictionIdxArray)
	{
		ChipData chipData;
		// スクロール量から描画する位置を求める。
		const Vec2<float> drawPos = ScrollMgr::Instance()->Affect({ pred.idx.x * MAP_CHIP_SIZE,pred.idx.y * MAP_CHIP_SIZE });

		// 画面外だったら描画しない。
		if (drawPos.x < -DRAW_MAP_CHIP_SIZE || drawPos.x > WinApp::Instance()->GetWinSize().x + DRAW_MAP_CHIP_SIZE) continue;
		if (drawPos.y < -DRAW_MAP_CHIP_SIZE || drawPos.y > WinApp::Instance()->GetWinSize().y + DRAW_MAP_CHIP_SIZE) continue;

		chipData.pos = drawPos;
		chipData.alpha = predictionRate;

		if (pred.type == MAPCHIP_TYPE_STATIC_RARE_BLOCK)
		{
			DRAW_MAP_RARE.AddChip(chipData);
		}
		else if (pred.type == MAPCHIP_TYPE_STATIC_BOUNCE_BLOCK)
		{
			DRAW_MAP_BOUNCE.AddChip(chipData);
		}
		else if (pred.type == MAPCHIP_TYPE_STATIC_UNBROKEN_BLOCK)
		{
			DRAW_MAP_UNBROKEN.AddChip(chipData);
		}
		else if (pred.type == MAPCHIP_TYPE_STATIC_NON_SCORE_BLOCK)
		{
			DRAW_MAP_NON_SCORE.AddChip(chipData);
		}
		else if (pred.type == 1)
		{
			DRAW_MAP_WALL.AddChip(chipData);
		}
		else
		{
			DRAW_MAP_EMPTY.AddChip(chipData);
		}
	}
	DRAW_MAP_WALL.Draw(TexHandleMgr::GetTexBuffer(StageMgr::Instance()->GetChipGraoh(1)));
	DRAW_MAP_RARE.Draw(TexHandleMgr::GetTexBuffer(StageMgr::Instance()->GetChipGraoh(MAPCHIP_TYPE_STATIC_RARE_BLOCK)));
	DRAW_MAP_BOUNCE.Draw(TexHandleMgr::GetTexBuffer(StageMgr::Instance()->GetChipGraoh(MAPCHIP_TYPE_STATIC_BOUNCE_BLOCK)));
	DRAW_MAP_NON_SCORE.Draw(TexHandleMgr::GetTexBuffer(StageMgr::Instance()->GetChipGraoh(MAPCHIP_TYPE_STATIC_NON_SCORE_BLOCK)));
	DRAW_MAP_UNBROKEN.Draw(TexHandleMgr::GetTexBuffer(StageMgr::Instance()->GetChipGraoh(MAPCHIP_TYPE_STATIC_UNBROKEN_BLOCK)));
	DRAW_MAP_EMPTY.Draw(EMPTY_TEX);
}

MapChipGenerator_Crossing::MapChipGenerator_Crossing(const int& Span) : MapChipGenerator_RandPattern(Span, false)
{
	timer = 0;

	generateLine = { 1,1 };

	DesideNextIndices(HORIZON, { 0,generateLine.y });
	DesideNextIndices(VERT, { generateLine.x,0 });
}

void MapChipGenerator_Crossing::Update()
{
	timer++;

	if (span <= timer)
	{
		for (auto& pre : predictionIdxArray)
		{
			Generate(pre.idx, pre.type, pre.wallGraph);
		}

		auto mapData = StageMgr::Instance()->GetLocalMap();
		Vec2<int>chipIdxMax;
		chipIdxMax.x = (*mapData)[0].size();
		chipIdxMax.y = (*mapData).size();

		predictionIdxArray.clear();
		DesideNextIndices(HORIZON, { 0,++generateLine.y });
		DesideNextIndices(VERT, { ++generateLine.x,0 });

		if (chipIdxMax.x - 2 <= generateLine.x)generateLine.x = 1;
		if (chipIdxMax.y - 2 <= generateLine.y)generateLine.y = 1;
		timer = 0;
	}
}

void MapChipGenerator_Crossing::Draw()
{
	MapChipGenerator_RandPattern::Draw();
}


void MapChipGenerator_RiseUp::RiseUp()
{
	auto mapData = StageMgr::Instance()->GetLocalMap();
	auto mapIdxSize = StageMgr::Instance()->GetMapIdxSize(SelectStage::Instance()->GetStageNum(), SelectStage::Instance()->GetRoomNum());

	static const int SPACE_LINE = 0;

	if (dir == LEFT_TO_RIGHT)
	{
		//右列から処理
		for (int x = mapIdxSize.x - 1 - 1 - SPACE_LINE; 1 <= x; --x)
		{
			for (int y = 0; y < mapIdxSize.y; ++y)
			{
				//自身の右が空でないならスルー
				if ((*mapData)[y][x + 1].chipType != 0)continue;

				//自身のタイプを右チップに移す
				StageMgr::Instance()->WriteMapChipData(Vec2<int>(x + 1, y), (*mapData)[y][x].chipType, CharacterManager::Instance()->Left()->pos, CharacterManager::Instance()->Left()->size.x, CharacterManager::Instance()->Right()->pos, CharacterManager::Instance()->Right()->size.x, false, INTERSECTED_RIGHT, false);
				//自身は空にする
				StageMgr::Instance()->WriteMapChipData(Vec2<int>(x, y), 0, CharacterManager::Instance()->Left()->pos, CharacterManager::Instance()->Left()->size.x, CharacterManager::Instance()->Right()->pos, CharacterManager::Instance()->Right()->size.x, false, INTERSECTED_RIGHT, false);
			}
		}

		//一番左の列はランダム生成
		int x = 1;
		for (int y = 0; y < mapIdxSize.y; ++y)
		{
			//自身が空でないならスルー
			if ((*mapData)[y][x].chipType != 0)continue;

			//ランダム生成
			StageMgr::Instance()->WriteMapChipData(Vec2<int>(x, y), GetRandChipType(), CharacterManager::Instance()->Left()->pos, CharacterManager::Instance()->Left()->size.x, CharacterManager::Instance()->Right()->pos, CharacterManager::Instance()->Right()->size.x, false, INTERSECTED_RIGHT, false);
		}
	}
	else if (dir == UP_TO_BOTTOM)
	{
		//下列から処理
		for (int y = mapIdxSize.y - 1 - 1 - SPACE_LINE; 1 <= y; --y)
		{
			for (int x = 0; x < mapIdxSize.x; ++x)
			{
				//自身の下が空でないならスルー
				if ((*mapData)[y + 1][x].chipType != 0)continue;

				//自身のタイプを下チップに移す
				StageMgr::Instance()->WriteMapChipData(Vec2<int>(x, y + 1), (*mapData)[y][x].chipType, CharacterManager::Instance()->Left()->pos, CharacterManager::Instance()->Left()->size.x, CharacterManager::Instance()->Right()->pos, CharacterManager::Instance()->Right()->size.x, false, INTERSECTED_BOTTOM, false);
				//自身は空にする
				StageMgr::Instance()->WriteMapChipData(Vec2<int>(x, y), 0, CharacterManager::Instance()->Left()->pos, CharacterManager::Instance()->Left()->size.x, CharacterManager::Instance()->Right()->pos, CharacterManager::Instance()->Right()->size.x, false, INTERSECTED_BOTTOM, false);
			}
		}

		//一番上の列はランダム生成
		int y = 1;
		for (int x = 0; x < mapIdxSize.x; ++x)
		{
			//自身が空でないならスルー
			if ((*mapData)[y][x].chipType != 0)continue;

			//ランダム生成
			StageMgr::Instance()->WriteMapChipData(Vec2<int>(x, y), GetRandChipType(), CharacterManager::Instance()->Left()->pos, CharacterManager::Instance()->Left()->size.x, CharacterManager::Instance()->Right()->pos, CharacterManager::Instance()->Right()->size.x, false, INTERSECTED_BOTTOM, false);
		}
	}
	else if (dir == RIGHT_TO_LEFT)
	{
		//左列から処理
		for (int x = 1 + SPACE_LINE; x <= mapIdxSize.x - 1 - 1; x++)
		{
			for (int y = 0; y < mapIdxSize.y; ++y)
			{
				//自身の左が空でないならスルー
				if ((*mapData)[y][x - 1].chipType != 0)continue;

				//自身のタイプを左チップに移す
				StageMgr::Instance()->WriteMapChipData(Vec2<int>(x - 1, y), (*mapData)[y][x].chipType, CharacterManager::Instance()->Left()->pos, CharacterManager::Instance()->Left()->size.x, CharacterManager::Instance()->Right()->pos, CharacterManager::Instance()->Right()->size.x, false, INTERSECTED_LEFT, false);
				//自身は空にする
				StageMgr::Instance()->WriteMapChipData(Vec2<int>(x, y), 0, CharacterManager::Instance()->Left()->pos, CharacterManager::Instance()->Left()->size.x, CharacterManager::Instance()->Right()->pos, CharacterManager::Instance()->Right()->size.x, false, INTERSECTED_LEFT, false);
			}
		}

		//一番右の列はランダム生成
		int x = mapIdxSize.x - 1 - 1;
		for (int y = 0; y < mapIdxSize.y; ++y)
		{
			//自身が空でないならスルー
			if ((*mapData)[y][x].chipType != 0)continue;

			//ランダム生成
			StageMgr::Instance()->WriteMapChipData(Vec2<int>(x, y), GetRandChipType(), CharacterManager::Instance()->Left()->pos, CharacterManager::Instance()->Left()->size.x, CharacterManager::Instance()->Right()->pos, CharacterManager::Instance()->Right()->size.x, false, INTERSECTED_LEFT, false);
		}
	}
	else if (dir == BOTTOM_TO_UP)
	{
		//下列から処理
		for (int y = 1 + SPACE_LINE; y <= mapIdxSize.y - 1 - 1; y++)
		{
			for (int x = 0; x < mapIdxSize.x; ++x)
			{
				//自身の上が空でないならスルー
				if ((*mapData)[y - 1][x].chipType != 0)continue;

				//自身のタイプを上チップに移す
				StageMgr::Instance()->WriteMapChipData(Vec2<int>(x, y - 1), (*mapData)[y][x].chipType, CharacterManager::Instance()->Left()->pos, CharacterManager::Instance()->Left()->size.x, CharacterManager::Instance()->Right()->pos, CharacterManager::Instance()->Right()->size.x, false, INTERSECTED_TOP, false);
				//自身は空にする
				StageMgr::Instance()->WriteMapChipData(Vec2<int>(x, y), 0, CharacterManager::Instance()->Left()->pos, CharacterManager::Instance()->Left()->size.x, CharacterManager::Instance()->Right()->pos, CharacterManager::Instance()->Right()->size.x, false, INTERSECTED_TOP, false);
			}
		}

		//一番下の列はランダム生成
		int y = mapIdxSize.y - 1 - 1;
		for (int x = 0; x < mapIdxSize.x; ++x)
		{
			//自身が空でないならスルー
			if ((*mapData)[y][x].chipType != 0)continue;

			//ランダム生成
			StageMgr::Instance()->WriteMapChipData(Vec2<int>(x, y), GetRandChipType(), CharacterManager::Instance()->Left()->pos, CharacterManager::Instance()->Left()->size.x, CharacterManager::Instance()->Right()->pos, CharacterManager::Instance()->Right()->size.x, false, INTERSECTED_TOP, false);
		}
	}
}

MapChipGenerator_RiseUp::MapChipGenerator_RiseUp(const int& Span, const RISE_UP_GENERATOR_DIRECTION& Direction) : span(Span), dir(Direction)
{
	timer = 0;
}

void MapChipGenerator_RiseUp::Update()
{
	timer++;
	if (span <= timer)
	{
		RiseUp();
		timer = 0;
	}
}

void MapChipGenerator_RiseUp::Draw()
{
	//Color col = Color(8, 217, 255, 255);
	//const Vec2<float>leftUpPos = { 30,600 };
	//const Vec2<float>size = { 500,64 };

	////ゲージ
	//DrawFunc::DrawBox2D(leftUpPos, leftUpPos + size * Vec2<float>((float)timer / span, 1.0f), col, true);
	////フレーム
	//DrawFunc::DrawBox2D(leftUpPos, leftUpPos + size, col);
}
