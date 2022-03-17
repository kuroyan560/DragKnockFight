#pragma once
#include"Vec.h"
class Primitive
{
public:
	static const enum TYPE { SPHERE, PLANE, TRIANGLE, RAY };
	virtual TYPE GetPrimitiveType() = 0;
};

// ��
class Sphere : public Primitive
{
public:
	// ���S���W
	Vec3<float> center = {};
	// ���a
	float radius = 1.0f;

	TYPE GetPrimitiveType()override { return SPHERE; }
};

//����
class Plane : public Primitive
{
public:
	// �@���x�N�g��
	Vec3<float> normal = { 0,1,0 };
	// ���_(0,0,0)����̋���
	float distance = 0.0f;

	TYPE GetPrimitiveType()override { return PLANE; }
};

// �@���t���O�p�`�i���v��肪�\�ʁj
class Triangle : public Primitive
{
public:
	// ���_���W3��
	Vec3<float>	p0;
	Vec3<float>	p1;
	Vec3<float>	p2;
	// �@���x�N�g��
	Vec3<float>	normal;

	// �@���̌v�Z
	void ComputeNormal()
	{
		Vec3<float> p0_p1 = p1 - p0;
		Vec3<float> p0_p2 = p2 - p0;

		// �O�ςɂ��A2�ӂɐ����ȃx�N�g�����Z�o����
		normal = p0_p1.Cross(p0_p2).GetNormal();
	}

	TYPE GetPrimitiveType()override { return TRIANGLE; }
};

class Ray : public Primitive
{
public:
	// �n�_���W
	Vec3<float>	start = { 0,0,0 };
	// ����
	Vec3<float>	dir = { 1,0,0 };

	TYPE GetPrimitiveType()override { return RAY; }
};