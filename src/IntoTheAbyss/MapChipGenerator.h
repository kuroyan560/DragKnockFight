#pragma once
#include"Vec.h"
#include<array>
//�}�b�v�`�b�v��\���I�ɐ�������@�\
class MapChipGenerator
{
public:
	void Update(const Vec2<float>& GeneratePos);
};

//�Ƃ肠�����X�v���C���œ����悤��
struct MapChipGenerator_Test
{
private:
	const float RADIUS = 64.0f;

	Vec2<float>pos;
	float t = 0.0f;
	static const int ARRAY_SIZE = 4;
	std::array<Vec2<float>, ARRAY_SIZE>targetPos;	//��������z��
	MapChipGenerator generator;
	float GetDistRate();
	Vec2<float> GetRandPos();

public:
	void Init();
	void Update();
	void Draw();
};