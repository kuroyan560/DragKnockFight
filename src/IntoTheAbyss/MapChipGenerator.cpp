#include "MapChipGenerator.h"
#include"StageMgr.h"
#include"MapChipCollider.h"
#include"KuroMath.h"
#include"KuroFunc.h"
#include"DebugParameter.h"
#include"SelectStage.h"
#include<algorithm>

void MapChipGenerator::Generate(const Vec2<float> &GeneratePos)
{
	//�������W����ɂ��̏ꏊ�̃`�b�v�ԍ��擾
	Vec2<int>centerIdx =
	{
		static_cast<int>((GeneratePos.x + MAP_CHIP_HALF_SIZE) / MAP_CHIP_SIZE),
		static_cast<int>((GeneratePos.y + MAP_CHIP_HALF_SIZE) / MAP_CHIP_SIZE)
	};

	//��������ꏊ�z��
	const std::vector<Vec2<int>>generateIndices
	{
		centerIdx,	//����
		centerIdx + Vec2<int>(0,-1),	//��
		centerIdx + Vec2<int>(0,1),	//��
		centerIdx + Vec2<int>(-1,0),	//��
		centerIdx + Vec2<int>(1,0),	//�E
	};

	for (auto &idx : generateIndices)
	{
		StageMgr::Instance()->WriteMapChipData(idx, 1);
	}
}

float MapChipGenerator_SplineOrbit::GetDistRate()
{
	static const float DIST_BASE = 500.0f;	//�P�Ƃ��鋗��
	float dist = targetPos[1].Distance(targetPos[2]);
	return DIST_BASE / dist;
}

Vec2<float> MapChipGenerator_SplineOrbit::GetRandPos()
{
	//�}�b�v���̃����_���ȍ��W���擾
	Vec2<float>range = StageMgr::Instance()->GetMapGrobalSize();
	range -= RADIUS * 2.0f;
	return Vec2<float>(RADIUS + KuroFunc::GetRand(range.x), RADIUS + KuroFunc::GetRand(range.y));
}

void MapChipGenerator_SplineOrbit::Init()
{
	Vec2<float>mapSize = StageMgr::Instance()->GetMapGrobalSize();

	//���S�X�^�[�g
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
	for (auto &tp : targetPos)
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

	//��ʊO�̂Ƃ��͖��
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

int MapChipGenerator_RandPattern::GetSpan()
{
	static const int MAX = 60 * 5;
	static const int MIN = 60 * 2;
	return KuroFunc::GetRand(MIN, MAX);
}

void MapChipGenerator_RandPattern::DesideNextIndices()
{
	predictionIdxArray.clear();

	auto mapData = StageMgr::Instance()->GetLocalMap();
	Vec2<int>chipIdxMax;
	chipIdxMax.x = (*mapData)[0].size();
	chipIdxMax.y = (*mapData).size();

	static const int GENERATE_NUM_MAX = 20;
	static const int GENERATE_NUM_MIN = 3;

	static const enum PATTERN_TYPE { CROSS, CUBE, CIRCLE, NUM };
	static bool INIT = false;
	static OffsetPattern PATTERN[NUM];
	if (!INIT)
	{
		PATTERN[CROSS] =
		{
			{0,0},	//����
			{0,-1},	//��
			{0,1},
			{-1,0},	//��
			{1,0},	//�E
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
				for (auto &idx : PATTERN[CIRCLE])
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

	const int generateNum = KuroFunc::GetRand(GENERATE_NUM_MIN, GENERATE_NUM_MAX);
	for (int i = 0; i < generateNum; ++i)
	{
		Vec2<int>centerIdx = { KuroFunc::GetRand(chipIdxMax.x - 1),KuroFunc::GetRand(chipIdxMax.y - 1) };

		const auto patternType = PATTERN[KuroFunc::GetRand(PATTERN_TYPE::NUM - 1)];
		for (auto &offsetIdx : patternType)
		{
			Vec2<int> idx = offsetIdx + centerIdx;
			if (idx.x < 0)continue;
			if (chipIdxMax.x <= idx.x)continue;
			if (idx.y < 0)continue;
			if (chipIdxMax.y <= idx.y)continue;

			predictionIdxArray.emplace_back(idx);
		}
	}
}

void MapChipGenerator_RandPattern::Init()
{
	timer = 0;
	span = GetSpan();
	DesideNextIndices();
}

void MapChipGenerator_RandPattern::Update()
{
	timer++;

	if (span <= timer)
	{
		for (auto &idx : predictionIdxArray)
		{
			StageMgr::Instance()->WriteMapChipData(idx, 1);
		}
		DesideNextIndices();
		timer = 0;
		span = GetSpan();
	}
}

#include"DrawMap.h"
void MapChipGenerator_RandPattern::Draw()
{
	static DrawMap DRAW_MAP;

	// �`�悷��`�b�v�̃T�C�Y
	const float DRAW_MAP_CHIP_SIZE = MAP_CHIP_SIZE * ScrollMgr::Instance()->zoom;

	//�����܂ł̗\��
	float predictionRate = (float)timer / span;
	predictionRate = KuroMath::Ease(In, Circ, predictionRate, 0.0f, 1.0f);


	for (const auto &idx : predictionIdxArray)
	{
		ChipData chipData;
		// �X�N���[���ʂ���`�悷��ʒu�����߂�B
		const Vec2<float> drawPos = ScrollMgr::Instance()->Affect({ idx.x * MAP_CHIP_SIZE,idx.y * MAP_CHIP_SIZE });

		// ��ʊO��������`�悵�Ȃ��B
		if (drawPos.x < -DRAW_MAP_CHIP_SIZE || drawPos.x > WinApp::Instance()->GetWinSize().x + DRAW_MAP_CHIP_SIZE) continue;
		if (drawPos.y < -DRAW_MAP_CHIP_SIZE || drawPos.y > WinApp::Instance()->GetWinSize().y + DRAW_MAP_CHIP_SIZE) continue;

		chipData.pos = drawPos;
		chipData.alpha = predictionRate;
		DRAW_MAP.AddChip(chipData);
	}
	DRAW_MAP.Draw(TexHandleMgr::GetTexBuffer(StageMgr::Instance()->GetWallGraph()));
}

void MapChipGenerator_ChangeMap::Init()
{
	changeMapTimer = 0;
	setMapNum = 0;
}

void MapChipGenerator_ChangeMap::Update()
{
	if (120 <= changeMapTimer)
	{
		SelectStage::Instance()->SelectRoomNum(setMapNum);
		StageMgr::Instance()->SetLocalMapChipData(SelectStage::Instance()->GetStageNum(), setMapNum);
		StageMgr::Instance()->SetLocalMapChipDrawBlock(SelectStage::Instance()->GetStageNum(), setMapNum);

		++setMapNum;
		if (setMapNum == 2)
		{
			setMapNum = 0;
		}
		changeMapTimer = 0;
	}

	++changeMapTimer;
}

void MapChipGenerator_ChangeMap::Draw()
{
}
