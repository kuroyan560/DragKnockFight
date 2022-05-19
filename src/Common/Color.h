#pragma once
#include"Vec.h"
#include<vector>

struct Color
{
	//0 ~ 255
	static Vec4<float>ConvertToVec4(int R, int G, int B, int A)
	{
		auto color = Color(R, G, B, A);
		return Vec4<float>(color.r, color.g, color.b, color.a);
	}

public:
	float r = 1.0f;
	float g = 1.0f;
	float b = 1.0f;
	float a = 1.0f;

	//�R���X�g���N�^
	Color() {};
	Color(const Color& Color) :r(Color.r), g(Color.g), b(Color.b), a(Color.a) {}
	Color(float R, float G, float B, float A) : r(R), g(G), b(B), a(A) {}
	Color(int R, int G, int B, int A) {
		r = R / 255.0f;
		g = G / 255.0f;
		b = B / 255.0f;
		a = A / 255.0f;
	}

	//��r���Z�q
	bool operator==(const Color& rhs) {
		if (r != rhs.r)return false;
		if (g != rhs.g)return false;
		if (b != rhs.b)return false;
		if (a != rhs.a)return false;
		return true;
	}
	bool operator!=(const Color& rhs) {
		return !(*this == rhs);
	}

	//�����Z�͐F���̂݁A�A���t�@�l�͕ω����Ȃ�
	Color operator+(const Color& rhs)const {
		return Color(r + rhs.r, g + rhs.g, b + rhs.b, a);
	}
	Color operator*(const float& rhs)const {
		return Color(r * rhs, g * rhs, b * rhs, a);
	}
	Color operator/(const float& rhs)const {
		return Color(r / 2.0f, g / 2.0f, b / 2.0f, a);
	}

	//������Z�q
	void operator=(const Color& rhs)
	{
		r = rhs.r;
		g = rhs.g;
		b = rhs.b;
		a = rhs.a;
	}
	void operator*=(const float& rhs) {
		r *= rhs;
		g *= rhs;
		b *= rhs;
	}
	void operator/=(const float& rhs) {
		r /= 2.0f;
		g /= 2.0f;
		b /= 2.0f;
	}
};