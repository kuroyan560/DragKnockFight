#pragma once
#include"Vec.h"
#include<vector>
#include <assert.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include<string>
#include<array>
#include<memory>
#include<windows.h>

enum E_GIMMICK
{
	GIMMICK_NONE = -1,
	GIMMICK_DOSSN_OFF,
	GIMMICK_DOSSN_ON_LOW,
	GIMMICK_DOSSN_ON_HIGH,
};

struct ThownpeData
{
	Vec2<float> startPos;		//�J�n�ʒu
	Vec2<float> endPos;		//�I���ʒu
	Vec2<float> size;			//�摜�̑傫��
	E_GIMMICK type;		//�M�~�b�N�̎��

	ThownpeData(const Vec2<float> &SIZE = Vec2<float>(-1.0f, -1.0f), const E_GIMMICK &TYPE = GIMMICK_NONE) :size(SIZE), type(TYPE), startPos(Vec2<float>(0.0f, 0.0f)), endPos(Vec2<float>(0.0f, 0.0f))
	{
	};
};

struct BubbleData
{
	Vec2<float> pos;	//�������W
	BubbleData(const Vec2<float> &POS = Vec2<float>(-1.0f, -1.0f)) :pos(POS)
	{
	};
};

class GimmickLoader
{
public:
	GimmickLoader(const int &STAGE_NUM);
	void LoadData(const int &STAGE_NUM, const int &ROOM_NUM, const std::string &FILE_PASS);
	const std::vector< std::shared_ptr<ThownpeData>> &GetThowpeData(const int &STAGE_NUM, const int &ROOM_NUM);
	const std::vector< std::shared_ptr<BubbleData>> &GetBubbleData(const int &STAGE_NUM, const int &ROOM_NUM);


	void SetThwompStartPos(const int &STAGE_NUM, const int &ROOM_NUM, const int &GIMMICK_NUMBER, const Vec2<float> &POS);
	void SetThwompEndPos(const int &STAGE_NUM, const int &ROOM_NUM, const int &GIMMICK_NUMBER, const Vec2<float> &POS);
	void PushBubbleData(const int &STAGE_NUM, const int &ROOM_NUM, const int &GIMMICK_NUMBER, const Vec2<float> &POS);

private:
	std::vector<std::vector<std::vector<std::shared_ptr<ThownpeData>>>> allGimmickData;//�h�b�X���Ɋւ�����
	std::vector<std::vector<std::vector<std::shared_ptr<BubbleData>>>> allBubbleData;//�h�b�X���Ɋւ�����

	enum GimmickName
	{
		GIMMCK_NAME_NONE = -1,
		GIMMCK_NAME_THOWMPE,
		GIMMCK_NAME_MAX,
	};

	enum GimmickCommon
	{
		GIMMCK_COMMON_DATA_NAME,
		GIMMCK_COMMON_DATA_MAX
	};

	enum GimmickThompeData
	{
		GIMMCK_THOWMPE_DATA_TYPE,
		GIMMCK_THOWMPE_DATA_SIZE,
		GIMMCK_THOWMPE_DATA_MAX
	};

	enum GimmickThowmpeType
	{
		GIMMCK_THOWMPE_TYPE_OFF,
		GIMMCK_THOWMPE_TYPE_ON_LOW,
		GIMMCK_THOWMPE_TYPE_ON_HIGH,
		GIMMCK_THOWMPE_TYPE_MAX,
	};

	std::array<std::string, GIMMCK_NAME_MAX>gimmickName;					//�M�~�b�N����ǂݍ��ލۂɖ����ȕ����������ĂȂ����m�F����ׂ̔�r�Ώ�
	std::array<std::string, GIMMCK_COMMON_DATA_MAX>gimmickCommonName;		//Key�l��ǂݍ��ލۂɖ����ȕ����������ĂȂ����m�F����ׂ̔�r�ΏہB�S�ẴM�~�b�N���ʂɎg��Key�l���i�[�����
	std::array<std::string, GIMMCK_THOWMPE_DATA_MAX>gimmickThowmpeDataName;	//Key�l��ǂݍ��ލۂɖ����ȕ����������ĂȂ����m�F����ׂ̔�r�ΏہB����̃M�~�b�N�Ɏg��Key�l���i�[�����
	std::array<std::string, GIMMCK_THOWMPE_TYPE_MAX>gimmickThowmpeType;		//�M�~�b�N�̍��ڂ�ǂݍ��ލۂɖ����ȕ����������ĂȂ����m�F����ׂ̔�r�Ώ�

	void LoadThowmpeData(const std::string &KEY, std::istringstream *LINE_STREAM, std::shared_ptr<ThownpeData> THOWNPE_DATA);
};

