#pragma once
#include<windows.h>
#include<string>
#include"Vec.h"

class WinApp
{
private:
	static WinApp* INSTANCE;
public:
	static WinApp* Instance() 
	{ 
		if (INSTANCE == nullptr)
		{
			printf("WinApp�̃C���X�^���X���Ăяo�����Ƃ��܂�����nullptr�ł���\n");
			assert(0);
		}
		return INSTANCE; 
	}

private:
	Vec2<int>winSize;
	Vec2<float>winDifferRate = { 1.0f,1.0f };	//�t���X�N���[���ɂ����ۂ̃E�B���h�E�T�C�Y�{��
	WNDCLASSEX wc{};
	HWND hwnd;

	void Initialize(const std::string& WinName, const Vec2<int>WinSize, const bool& FullScreen, const wchar_t* IconPath);

public:
	WinApp(const std::string& WinName, const Vec2<int>WinSize, const bool& FullScreen, const wchar_t* IconPath = nullptr)
	{
		if (INSTANCE != nullptr)
		{
			printf("���ɃC���X�^���X������܂��BWinApp�͂P�̃C���X�^���X�������Ă܂���\n");
			assert(0);
		}
		INSTANCE = this;
		Initialize(WinName, WinSize, FullScreen, IconPath);
	}
	const HWND& GetHwnd() { return hwnd; }
	const WNDCLASSEX& GetWinClass() { return wc; }
	const Vec2<int>& GetWinSize() { return winSize; }
	const Vec2<int>& GetWinCenter() { return winSize / 2; }
	Vec2<float>GetExpandWinSize() { return winSize.Float() * winDifferRate; }	//�E�B���h�E�T�C�Y�{�������������T�C�Y
	Vec2<float>GetExpandWinCenter() { return GetExpandWinSize() / 2.0f; }
	const Vec2<float>& GetWinDifferRate() { return winDifferRate; }
};

