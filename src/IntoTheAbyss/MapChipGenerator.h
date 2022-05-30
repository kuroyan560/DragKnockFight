#pragma once
#include"Vec.h"
#include<array>
#include<vector>

static const enum MAP_CHIP_GENERATOR 
{
	NON_GENERATE,
	SPLINE_ORBIT,
	RAND_PATTERN,
	CLOSSING,
	RISE_UP_L_TO_R,
	RISE_UP_TOP_TO_BOTTOM,
	RISE_UP_R_TO_L,
	RISE_UP_BOTTOM_TO_TOP,
	MAP_CHIP_GENERATOR_NUM
};

static bool HaveGenerateTimeParameter(const MAP_CHIP_GENERATOR& Type)
{
	if (Type == RAND_PATTERN)return true;
	if (Type == CLOSSING)return true;
	if (Type == RISE_UP_L_TO_R)return true;
	if (Type == RISE_UP_TOP_TO_BOTTOM)return true;
	if (Type == RISE_UP_R_TO_L)return true;
	if (Type == RISE_UP_BOTTOM_TO_TOP)return true;
	return false;
}

//�}�b�v�`�b�v��\���I�ɐ�������@�\
class MapChipGenerator
{
protected:
	bool CanChange(const Vec2<int>& Idx);
	int GetRandChipType();
	void Generate(const Vec2<int>& GenerateIdx, const int& ChipType,const int& WallGraph = -1);
	void Generate(const Vec2<float>& GeneratePos, const int& ChipType,const int& WallGraph = -1);
public:
	virtual void Update() = 0;
	virtual void Draw() = 0;
};

class MapChipGenerator_Non : public MapChipGenerator
{
public:
	void Update()override {};
	void Draw()override {};
};

//�Ƃ肠�����X�v���C���œ����悤��
class MapChipGenerator_SplineOrbit : public MapChipGenerator
{
private:
	const float RADIUS = 64.0f;

	Vec2<float>pos;
	float t = 0.0f;
	static const int ARRAY_SIZE = 4;
	std::array<Vec2<float>, ARRAY_SIZE>targetPos;	//��������z��
	float GetDistRate();
	Vec2<float> GetRandPos();

public:
	MapChipGenerator_SplineOrbit();
	void Update()override;
	void Draw()override;
};

//�����_���Ő������ꂩ��o��ʒu�ɗ\��
class MapChipGenerator_RandPattern : public MapChipGenerator
{
protected:
	static const enum PATTERN_TYPE { CROSS, CUBE, CIRCLE, HORIZON, VERT, NUM };

	typedef std::vector<Vec2<int>>OffsetPattern;
	struct Prediction
	{
		Vec2<int>idx;
		int type;
		int wallGraph;
	};
	std::vector<Prediction>predictionIdxArray;
	int span;
	int timer;
	int GetRandSpan();
	void DesideNextIndices(const PATTERN_TYPE& PatternType, const Vec2<int>& GenerateIdx);	//���̐����ʒu������

public:
	MapChipGenerator_RandPattern(const int& Span, const bool& RandPattern = true);
	void Update()override;
	void Draw()override;
};

//�}�b�v�`�b�v�����E���h���Ƃɐ؂�ւ���
class MapChipGenerator_ChangeMap :public MapChipGenerator
{
	struct Prediction
	{
		Vec2<int>idx;
		int type;
	};
	std::vector<Prediction>predictionIdxArray;
public:
	MapChipGenerator_ChangeMap();
	void Update()override;
	void Draw()override;
	void RegisterMap();

private:
	int setMapNumber;
	int stageNumber;
};

//�㉺���E����}�b�v�`�b�v���o���\���`�ŉ��؂�
class MapChipGenerator_Crossing : public MapChipGenerator_RandPattern
{
	Vec2<int> generateLine;	//�񐶐����鎲
public:
	MapChipGenerator_Crossing(const int& Span);
	void Update()override;
	void Draw()override;
};

//�P�ӂ��点��オ���Ă���
static const enum RISE_UP_GENERATOR_DIRECTION
{
	LEFT_TO_RIGHT,
	UP_TO_BOTTOM,
	RIGHT_TO_LEFT,
	BOTTOM_TO_UP,
	RISE_UP_GENERATOR_DIRECTION_NUM
};

class MapChipGenerator_RiseUp : public MapChipGenerator
{
	const RISE_UP_GENERATOR_DIRECTION dir;
	int span;
	int timer;
	void RiseUp();
public:
	MapChipGenerator_RiseUp(const int& Span, const RISE_UP_GENERATOR_DIRECTION& Direction);
	void Update()override;
	void Draw()override;
};