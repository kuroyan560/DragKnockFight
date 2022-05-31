#pragma once
#include"../Common/Singleton.h"
#include<vector>
#include"../Engine/AudioApp.h"

enum Sound
{
	SOUND_FAIL,
	SOUND_GOOD,
	SOUND_GREAT,
	SOUND_EXCELLENT,
	SOUND_PERFECT
};

struct StageEvaluationData
{
	float failRate;
	float goodRate;
	float greatRate;
	float excellentRate;
	float perfectRate;

	StageEvaluationData() :failRate(0.0f), goodRate(0.5f), greatRate(0.8f), excellentRate(1.0f), perfectRate(1.0f)
	{
	}
};

class EvaluationMgr :public Singleton<EvaluationMgr>
{
	std::vector<std::vector<StageEvaluationData>> data;
	std::vector<StageEvaluationData> resultData;


public:
	std::vector<int>soundData;

	EvaluationMgr()
	{
		soundData.resize(5);
		soundData[SOUND_FAIL] = AudioApp::Instance()->LoadAudio("resource/ChainCombat/sound/voice/Voice_failed.wav", 0.13f);
		soundData[SOUND_GOOD] = AudioApp::Instance()->LoadAudio("resource/ChainCombat/sound/voice/Voice_good.wav", 0.13f);
		soundData[SOUND_GREAT] = AudioApp::Instance()->LoadAudio("resource/ChainCombat/sound/voice/Voice_great.wav", 0.13f);
		soundData[SOUND_EXCELLENT] = AudioApp::Instance()->LoadAudio("resource/ChainCombat/sound/voice/Voice_excellent.wav", 0.13f);
		soundData[SOUND_PERFECT] = AudioApp::Instance()->LoadAudio("resource/ChainCombat/sound/voice/Voice_perfect.wav", 0.13f);
	}

	void Init(std::vector<std::vector<StageEvaluationData>> DATA, std::vector<StageEvaluationData> R_DATA)
	{
		data = DATA;
		resultData = R_DATA;
	};

	StageEvaluationData GetData(int STAGE_NUM, int ROOM_NUM)
	{
		return data[STAGE_NUM][ROOM_NUM];
	};

	StageEvaluationData GetResultData(int STAGE_NUM)
	{
		return resultData[STAGE_NUM];
	};


	const int PERFECT_ANIMATION_INTERVAL = 3;
};