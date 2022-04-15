#pragma once
#include"../Engine/ImguiApp.h"
#include"../Common/Singleton.h"
#include<string>
#include<vector>
#include<array>

class DebugImGuiManager :public Singleton<DebugImGuiManager>
{
	friend class Singleton<DebugImGuiManager>;
	DebugImGuiManager();
public:
	int Add(const std::string &NAME);

	const bool &DrawFlag(const int &HANDLE);
	void DrawImGui();

private:
	std::array<std::string, 20> item;
	std::array<bool, 20> drawFlags;
	std::array<bool, 20> initFlag;
	int handle;

};

