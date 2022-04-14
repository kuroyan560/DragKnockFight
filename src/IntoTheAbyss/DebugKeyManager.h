#pragma once
#include"../Common/Singleton.h"
#include"../Engine/UsersInput.h"
#include<vector>
#include<string>
#include<tuple>
#include<memory>
#include<array>
#include"../Common/KuroFunc.h"

//�}�N���֐��Ƃ����������֐����g�����Ƃ��������������B�}�N�����𕶎���ɕϊ�������@��������Ȃ������̂Ŏg�p���Ă��܂��B
//���Ń}�N���֐������悤�Ȃ��Ƃ͌����Ă��܂���
#define TO_STRING(VariableName) # VariableName

class DebugKeyManager:public Singleton<DebugKeyManager>
{
public:
	void CountReset();
	bool DebugKeyTrigger(int KEY, const std::string &KEY_NAME, const std::string &KEY_NUM);
	bool DebugKeyState(int KEY, const std::string &KEY_NAME, const std::string &KEY_NUM);

	void DrawImGui();

private:
	struct DebugData
	{
		std::string keyNumber;
		std::string keyName;

		DebugData(const std::string &KEY_NUM, const std::string &KEY_NAME) :keyNumber(KEY_NUM), keyName(KEY_NAME)
		{
		}
	};

	std::vector<std::shared_ptr<DebugData>> debugStringData;
	std::vector<int> debugKeyStorage;
	std::vector<int> coundKyeUsedNum;

	std::array<std::string, 10> errorName;
	void CheckKey(int KEY,const std::string &KEY_NAME, const std::string &KEY_NUM);

	void CheckErrorNumber(const std::string &KEY_NUM)
	{
		for (int i = 0; i < errorName.size(); ++i)
		{
			if (errorName[i] == KEY_NUM)
			{
				std::string name = KEY_NUM + "�͐����L�[�ł��B�f�o�b�N�p�ɐ����L�[���g��Ȃ��ł�������";
				MessageBox(NULL, KuroFunc::GetWideStrFromStr(name).c_str(), TEXT("�����L�[�֎~"), MB_OK);
				assert(0);
			}
		}
	}

};

