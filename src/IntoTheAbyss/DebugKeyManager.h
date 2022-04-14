#pragma once
#include"../Common/Singleton.h"
#include"../Engine/UsersInput.h"
#include<vector>
#include<string>
#include<tuple>
#include<memory>

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

	void CheckKey(int KEY,const std::string &KEY_NAME, const std::string &KEY_NUM);
};

