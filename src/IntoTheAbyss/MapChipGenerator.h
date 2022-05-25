#pragma once
#include"Vec.h"
#include<array>
#include<vector>
static const enum MAP_CHIP_GENERATOR 
{
	SPLINE_ORBIT,
	RAND_PATTERN,
	CHANGE_MAP,
	MAP_CHIP_GENERATOR_NUM
};

//�}�b�v�`�b�v��\���I�ɐ�������@�\
class MapChipGenerator
{
protected:
	void Generate(const Vec2<float>& GeneratePos);
public:
	virtual void Init() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;
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
	void Init()override;
	void Update()override;
	void Draw()override;
};

//�����_���Ő������ꂩ��o��ʒu�ɗ\��
class MapChipGenerator_RandPattern : public MapChipGenerator
{
	typedef std::vector<Vec2<int>>OffsetPattern;
	std::vector<Vec2<int>>predictionIdxArray;
	int span;
	int timer;
	int GetSpan();
	void DesideNextIndices();	//���̐����ʒu������

public:
	void Init()override;
	void Update()override;
	void Draw()override;
};

//�}�b�v�`�b�v����莞�Ԃ��Ƃɐ؂�ւ���
class MapChipGenerator_ChangeMap :public MapChipGenerator
{
public:
	void Init()override;
	void Update()override;
	void Draw()override;

private:
	int changeMapTimer;
	int changeMapMaxTimer;

	int setMapNumber;
	int stageNumber;
	int maxRoomNumber;
};
