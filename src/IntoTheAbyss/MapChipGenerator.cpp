#include "MapChipGenerator.h"
#include"StageMgr.h"
#include"MapChipCollider.h"
#include"KuroMath.h"
#include"KuroFunc.h"

void MapChipGenerator::Update(const Vec2<float>& GeneratePos)
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

	for (auto& idx : generateIndices)
	{
		StageMgr::Instance()->WriteMapChipData(idx, 1);
	}
}

float MapChipGenerator_Test::GetDistRate()
{
	static const float DIST_BASE = 2000.0f;	//�P�Ƃ��鋗��
	float dist = targetPos[1].Distance(targetPos[2]);
	return DIST_BASE / dist;
}

Vec2<float> MapChipGenerator_Test::GetRandPos()
{
	//�}�b�v���̃����_���ȍ��W���擾
	Vec2<float>range = StageMgr::Instance()->GetMapGrobalSize();
	range -= radius * 2.0f;
	return Vec2<float>(radius + KuroFunc::GetRand(range.x), radius + KuroFunc::GetRand(range.y));
}

void MapChipGenerator_Test::Init()
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

void MapChipGenerator_Test::Update()
{
	std::vector<Vec2<float>>targetPosVector;
	for (auto& tp : targetPos)
	{
		targetPosVector.emplace_back(tp);
	}

	pos = KuroMath::GetSpline(t, 1, targetPosVector);
	generator.Update(pos);

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
void MapChipGenerator_Test::Draw()
{
	DrawFunc::DrawCircle2D(ScrollMgr::Instance()->Affect(pos), ScrollMgr::Instance()->zoom * radius, Color(1.0f, 0.0f, 0.0f, 0.0f), true);
}
