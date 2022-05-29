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

	auto type = (*mapData)[Idx.y][Idx.x];
	if (type == MAPCHIP_TYPE_STATIC_BOUNCE_BLOCK)return false;
	if (type == MAPCHIP_TYPE_STATIC_RARE_BLOCK)return false;
	if (type == 1)return false;
	return true;
}

int MapChipGenerator::GetRandChipType()
{
	//出現確率
	const int RARE = DebugParameter::Instance()->emitRare;
	const int BOUNCE = DebugParameter::Instance()->emitBounce;

	int appearType = 1;

	float rate = KuroFunc::GetRand(99.0f);
	if (rate < (RARE < BOUNCE ? RARE : BOUNCE))
	{
		appearType = MAPCHIP_TYPE_STATIC_RARE_BLOCK;
	}
	else if (rate < RARE + BOUNCE)
	{
		appearType = MAPCHIP_TYPE_STATIC_BOUNCE_BLOCK;
	}
	return appearType;
}

void MapChipGenerator::Generate(const Vec2<float>& GeneratePos)
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
		if (!CanChange(idx))continue;
		StageMgr::Instance()->WriteMapChipData(idx, GetRandChipType(), CharacterManager::Instance()->Left()->pos, CharacterManager::Instance()->Left()->size.x, CharacterManager::Instance()->Right()->pos, CharacterManager::Instance()->Right()->size.x);
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

void MapChipGenerator_SplineOrbit::Init()
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

void MapChipGenerator_SplineOrbit::Update()
{
	std::vector<Vec2<float>>targetPosVector;
	for (auto& tp : targetPos)
	{
		targetPosVector.emplace_back(tp);
	}

	pos = KuroMath::GetSpline(t, 1, targetPosVector);
	Generate(pos);

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
}

#include"DrawFunc.h"
#include"ScrollMgr.h"
#include"TexHandleMgr.h"
void MapChipGenerator_SplineOrbit::Draw()
{
	static int ARROW_HANDLE = TexHandleMgr::LoadGraph("resource/ChainCombat/arrow_enemy.png");
	static const float ARROW_EXP = 0.4f;
	static auto ARROW_SIZE = TexHandleMgr::GetTexBuffer(ARROW_HANDLE)->GetGraphSize() * ARROW_EXP;

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
		DrawFunc::DrawCircle2D(drawPos, drawRadius, Color(47, 255, 139, 255), false, 3);
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
	}
}

void MapChipGenerator_RandPattern::Init()
{
	timer = 0;
	span = GetRandSpan();

	auto mapData = StageMgr::Instance()->GetLocalMap();
	Vec2<int>chipIdxMax;
	chipIdxMax.x = (*mapData)[0].size();
	chipIdxMax.y = (*mapData).size();

	DesideNextIndices(PATTERN_TYPE(KuroFunc::GetRand(PATTERN_TYPE::NUM - 1)), { KuroFunc::GetRand(chipIdxMax.x - 1),KuroFunc::GetRand(chipIdxMax.y - 1) });
}

void MapChipGenerator_RandPattern::Update()
{
	timer++;

	if (span <= timer)
	{
		for (auto& pre : predictionIdxArray)
		{
			if (!CanChange(pre.idx))continue;
			StageMgr::Instance()->WriteMapChipData(pre.idx, pre.type, CharacterManager::Instance()->Left()->pos, CharacterManager::Instance()->Left()->size.x, CharacterManager::Instance()->Right()->pos, CharacterManager::Instance()->Right()->size.x);
		}

		auto mapData = StageMgr::Instance()->GetLocalMap();
		Vec2<int>chipIdxMax;
		chipIdxMax.x = (*mapData)[0].size();
		chipIdxMax.y = (*mapData).size();
		predictionIdxArray.clear();
		DesideNextIndices(PATTERN_TYPE(KuroFunc::GetRand(PATTERN_TYPE::NUM - 1)), Vec2<int>(KuroFunc::GetRand(1, chipIdxMax.x), KuroFunc::GetRand(1, chipIdxMax.y)));
		timer = 0;
		span = GetRandSpan();
	}
}

#include"DrawMap.h"
void MapChipGenerator_RandPattern::Draw()
{
	static DrawMap DRAW_MAP_WALL;
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
			predictionIdxArray.back().type = StageMgr::Instance()->GetMap(stageNumber, setMapNumber)[y][x];
		}
	}
	setMapNumber++;
}

void MapChipGenerator_ChangeMap::Init()
{
	stageNumber = SelectStage::Instance()->GetStageNum();
	setMapNumber = 1;
	RegisterMap();
}

#include"RoundFinishEffect.h"
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

void MapChipGenerator_Crossing::Init()
{
	timer = 0;
	span = 60;

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
			if (!CanChange(pre.idx))continue;
			StageMgr::Instance()->WriteMapChipData(pre.idx, pre.type, CharacterManager::Instance()->Left()->pos, CharacterManager::Instance()->Left()->size.x, CharacterManager::Instance()->Right()->pos, CharacterManager::Instance()->Right()->size.x);
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
