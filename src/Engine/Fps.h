#pragma once
#include <Windows.h>
#include <tchar.h>
#include <sstream>

#include <mmsystem.h>
#pragma comment(lib,"winmm.lib")
class Fps
{
	const int frameRate;
	const float minFrameTime;
	float frameTime = 0;
	LARGE_INTEGER timeStart;
	LARGE_INTEGER timeEnd;
	LARGE_INTEGER timeFreq;
	// fps���擾����Ȃ�0�ŏ��������Ȃ��ƃS�~���������ă}�C�i�X����n�܂����肷��(�����m��Ȃ�)
	float fps = 0;

public:
	Fps(const int& FrameRate);
	void Update();

	const float& GetNowFps()const { return fps; }
};