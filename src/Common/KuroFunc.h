#pragma once
#include<string>
#include"Vec.h"
#include"Angle.h"
#include"KuroMath.h"
#include<memory>

#define SAFE_DELETE(p) { if(p!=nullptr){ delete p; p = nullptr;}}

namespace KuroFunc
{
	std::wstring GetWideStrFromStr(const std::string& Str);
	std::wstring GetWideStrFromStr(const char* Str);

	std::string GetExtension(const std::string& Path);

	void GetDivideStr(const std::string& Str, std::string* Dir, std::string* FileName);

	Vec2<float> GetCenterVec2(Vec2<float> From, Vec2<float> To);
	Vec3<float> GetCenterVec3(Vec3<float> From, Vec3<float> To);

	//�Q�_��ʂ钼���̌X��
	float GetLineSlope(Vec2<float> From, Vec2<float> To);
	//1�̃x�N�g������p�x���擾
	Angle GetAngle(Vec2<float>Vec);
	//�Q�̃x�N�g������p�x���擾
	Angle GetAngle(Vec2<float>From, Vec2<float>To);

	Vec2<float> ConvertWorldToScreen(Vec3<float> WorldPos,
		const Matrix& ViewMat, const Matrix& ProjMat, const Vec2<int>&WinSize);
	Vec3<float> ConvertScreenToWorld(Vec2<float> ScreenPos, float Z,
		const Matrix& ViewMat, const Matrix& ProjMat, const Vec2<int>&WinSize);

	//���S���W����ʓ��ɉf���Ă��邩
	bool InScreen(Vec3<float> WorldPos, const Matrix& ViewMat, const Matrix& ProjMat, const Vec2<int>& WinSize);

	bool ExistFile(const std::string FilePass);
	bool ExistDir(const std::string FileDir);

	//�x�W�G�p
	float GetYFromXOnBezier(float x, const Vec2<float>& a, const Vec2<float>& b, uint8_t n);
	//�x�W�G(����_�w��)
	float GetBezierFromControlPoint(float t, int ControlPointNum, float* ControlPointArray);

	//����
	int GetRand(int Min, int Max);
	int GetRand(int Max);
	float GetRand(float Min, float Max);
	float GetRand(float Max);
	//���������_��
	int GetRandPlusMinus();
	//��̒l����O�㗐���Z�o
	int GetRandFromCenter(int CenterNum, int Range);


	//�w�肵�����̐��������o��
	int GetSpecifiedDigitNum(int From, int Digit);
	//�������Z�o
	int GetDigit(int Num);

	//�����擾
	int GetNumSign(int Num);
	int GetNumSign(float Num);

	//���K��
	float Clamp(float Min, float Max, float Value);

	//�f�[�^�ǂݍ���
	void LoadData(FILE* Fp, std::string DataName, void* Data, size_t Size, int ElementNum);
	//�f�[�^�ۑ�
	void SaveData(FILE* Fp, std::string DataName, const void* Data, size_t Size, int ElementNum);

	//Vec3�ɍs��𔽉f
	Vec3<float>TransformVec3(const Vec3<float>& Value, const Matrix& Mat);
	//��]�����w�肵�ĉ�]
	Vec3<float>TransformVec3(const Vec3<float>& Value, const Vec3<float>& Axis, const Angle& Angle);
}