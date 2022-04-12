#pragma once
#include"Vec.h"
#include<vector>

struct Color
{
	//0 ~ 255
	static Vec4<float>ConvertToVec4(int R, int G, int B, int A)
	{
		auto color = Color(R, G, B, A);
		return color.color;
	}

private:
	Vec4<float>color = { 1,1,1,1 };

public:
	//RGBA 0~255����擾
	void SetColor(Vec4<int>RGBA)
	{
		color = { RGBA.x / 255.0f,RGBA.y / 255.0f,RGBA.z / 255.0f,RGBA.w / 255.0f };
	}
	const Vec4<float>& GetColor()const { return color; }

	//�R���X�g���N�^
	Color() {};
	Color(const Color& Color) :color(Color.color) {}
	Color(float R, float G, float B, float A) : color(Vec4<float>(R, G, B, A)) {}
	Color(int R, int G, int B, int A) {
		SetColor(Vec4<int>(R, G, B, A));
	}

	float& Alpha() { return color.w; }

	//��r���Z�q
	bool operator==(const Color& rhs) {
		if (color.x != rhs.color.x)return false;
		if (color.y != rhs.color.y)return false;
		if (color.z != rhs.color.z)return false;
		if (color.w != rhs.color.w)return false;
		return true;
	}
	bool operator!=(const Color& rhs) {
		return !(*this == rhs);
	}

	//�����Z�͐F���̂݁A�A���t�@�l�͕ω����Ȃ�
	Color operator+(const Color& rhs)const {
		return Color(color.x + rhs.color.x, color.y + rhs.color.y, color.z + rhs.color.z, color.w);
	}
	Color operator*(const float& rhs)const {
		return Color(color.x * rhs, color.y * rhs, color.z * rhs, color.w);
	}
	Color operator/(const float& rhs)const {
		return Color(color.x / 2.0f, color.y / 2.0f, color.z / 2.0f, color.w);
	}

	//������Z�q
	void operator=(const Color& rhs)
	{
		color.x = rhs.color.x;
		color.y = rhs.color.y;
		color.z = rhs.color.z;
		color.w = rhs.color.w;
	}
	void operator*=(const float& rhs) {
		color.x *= rhs;
		color.y *= rhs;
		color.z *= rhs;
	}
	void operator/=(const float& rhs) {
		color.x /= 2.0f;
		color.y /= 2.0f;
		color.z /= 2.0f;
	}
};