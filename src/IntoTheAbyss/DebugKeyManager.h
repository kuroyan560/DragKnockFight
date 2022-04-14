#pragma once
#include"../Common/Singleton.h"
#include"../Engine/UsersInput.h"
#include<vector>
#include<string>


class DebugKeyManager:public Singleton<DebugKeyManager>
{
public:
	void CountReset();
	bool DebugKeyTrigger(int KEY, const std::string &KEY_NAME);
	bool DebugKeyState(int KEY, const std::string &KEY_NAME);

	void DrawImGui();

private:
	std::vector<std::string> debugStringData;
	std::vector<int> debugKeyStorage;
	std::vector<int> coundKyeUsedNum;

	void CheckKey(int KEY,const std::string &KEY_NAME);
};

