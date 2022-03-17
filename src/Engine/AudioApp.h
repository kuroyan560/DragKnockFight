#pragma once
#include<xaudio2.h>
#pragma comment(lib,"xaudio2.lib")

#include<wrl.h>

#include<fstream>
#include<assert.h>

#include<list>

#include<string>

class AudioApp
{
private:
	static AudioApp* INSTANCE;
public:
	static AudioApp* Instance()
	{
		if (INSTANCE == nullptr)
		{
			printf("AudioApp�̃C���X�^���X���Ăяo�����Ƃ��܂�����nullptr�ł���\n");
			assert(0);
		}
		return INSTANCE;
	}
private:
	//�`�����N�w�b�_
	struct Chunk
	{
		char id[4];		//�`�����N����ID
		int32_t size;	//�`�����N�̃T�C�Y
	};

	//RTFF�w�b�_�`�����N
	struct RiffHeader
	{
		Chunk chunk;	//"RIFF"
		char type[4];	//"WAVE"
	};

	//FMT�`�����N
	struct FormatChunck
	{
		Chunk chunk;	//"fmt "
		WAVEFORMAT fmt;	//�g�`�t�H�[�}�b�g
	};

	Microsoft::WRL::ComPtr<IXAudio2>xAudio2;
	IXAudio2MasteringVoice* masterVoice;

	class XAudio2VoiceCallback : public IXAudio2VoiceCallback
	{
	public:
		virtual ~XAudio2VoiceCallback() {};
		//�{�C�X�����p�X�̊J�n��
		STDMETHOD_(void, OnVoiceProcessingPassStart)(THIS_ UINT32 BytesRequired) {};
		//�{�C�X�����p�X�̏I����
		STDMETHOD_(void, OnVoiceProcessingPassEnd) (THIS) {};
		//�o�b�t�@�X�g���[���̍Đ����I�������Ƃ�
		STDMETHOD_(void, OnStreamEnd) (THIS) {};
		//�o�b�t�@�̎g�p�J�n��
		STDMETHOD_(void, OnBufferStart) (THIS_ void* pBufferContext) {};
		//�o�b�t�@�̖����ɒB������
		STDMETHOD_(void, OnBufferEnd) (THIS_ void* pBufferContext){	};
		//�Đ������[�v�ʒu�ɒB������
		STDMETHOD_(void, OnLoopEnd) (THIS_ void* pBufferContext) {};
		//�{�C�X�̎��s�G���[��
		STDMETHOD_(void, OnVoiceError) (THIS_ void* pBufferContext, HRESULT Error) {};
	};
	XAudio2VoiceCallback voiceCallback;

	class AudioData
	{
	public:
		std::string filePass;
		RiffHeader riff;
		FormatChunck format;
		Chunk data;
		char* pBuffer = nullptr;
		IXAudio2SourceVoice* pSourceVoice = nullptr;
		float volume = 1.0f;
		bool loop = false;
		bool playTrigger = false;

		AudioData(std::string FilePass)
			:filePass(FilePass) {};
		void Unload();
	};

	std::list<AudioData>audios;

public:
	AudioApp();
	~AudioApp();

	void Update();
	bool NowPlay(const int& Handle);
	void ChangeVolume(const int& Handle, float Volume);
	float GetVolume(const int& Handle);
	int LoadAudio(std::string FileName);
	int PlayWave(const int& Handle, bool LoopFlag = false);
	void StopWave(const int& Handle);
};