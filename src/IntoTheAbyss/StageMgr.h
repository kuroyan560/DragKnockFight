#pragma once
#include<string>
#include"CSVLoader.h"
#include"Singleton.h"
#include<array>
#include"Vec.h"
#include<assert.h>
#include"GimmickLoader.h"

#include"WinApp.h"
#include"Angle.h"
#include"KuroFunc.h"

/// <summary>
/// ���������m�̕����̃����N�t������ۂɕK�v�ȏ��
/// </summary>
struct RelationData
{
	int doorNumber;
	int nextRoomNumber;

	RelationData() :doorNumber(-1), nextRoomNumber(-1)
	{
	}
};

/// <summary>
/// �}�b�v�`�b�v�̈�̍ŏ��l�ƍő�l
/// </summary>
struct SizeData
{
	int min;
	int max;
};

/// <summary>
/// �}�b�v�`�b�v�̎��
/// </summary>
enum MapChipData
{
	MAPCHIP_TYPE_STATIC_BLOCK,
	MAPCHIP_TYPE_MOVING_BLOCK,
	MAPCHIP_TYPE_TOCH,
	MAPCHIP_TYPE_DOOR,
	MAPCHIP_TYPE_THOWNP,
	MAPCHIP_TYPE_BUBBLE,
	MAPCHIP_TYPE_MAX
};
enum MapChipBlockData
{
	MAPCHIP_BLOCK_START = 30,
	MAPCHIP_BLOCK_GOAL = 31,
	MAPCHIP_BLOCK_DEBUG_START = 32,
	MAPCHIP_BLOCK_AURABLOCK = 40
};

struct MapChipDrawData
{
	int handle = -1;	//�摜�n���h��
	float radian;//�摜�̊p�x
	bool animationFlag;//�A�j���[�V������L���ɂ���t���O
	int interval;		//�A�j���[�V�����̊Ԋu
	int animationNum;//���ݎQ�Ƃ��Ă���A�j���[�V�����̉摜
	Vec2<float> offset;	//32*32�̉摜����Ȃ��ꍇ�̉摜�ʒu����
	Vec2<float> offsetInterval;//�㉺���E�̉摜�̊Ԋu���󂯂�

	//MapChipDrawData() : handle(-1), radian(0.0f), animationFlag(false), interval(0), animationNum(0), offset(Vec2<float>(0.0f, 0.0f))
	MapChipDrawData() : radian(0.0f), animationFlag(false), interval(0), animationNum(0), offset(Vec2<float>(0.0f, 0.0f))
	{
	};
};

struct MapChipAnimationData
{
	vector<int> handle;	//�Đ�����A�j���[�V�����̑���
	int maxInterval;		//�A�j���[�V�����̊Ԋu

	MapChipAnimationData(const vector<int> &HANDLE, int INTERVAL = -1) :handle(HANDLE), maxInterval(INTERVAL)
	{
	};
};

typedef std::vector<std::vector<int>> RoomMapChipArray;
typedef std::vector<RoomMapChipArray> StageMapChipData;

typedef std::vector<RelationData> RoomRelationData;
typedef std::vector<RoomRelationData> StageRelationData;

typedef std::vector<std::vector<MapChipDrawData>> RoomMapChipDrawArray;
typedef std::vector<RoomMapChipDrawArray> StageMapChipDrawData;

//�S�ẴX�e�[�W�̃}�b�v�`�b�v�f�[�^���Ǘ�����N���X
class StageMgr :public Singleton<StageMgr>
{
public:
	StageMgr();

	/// <summary>
	/// �������̃}�b�v�`�b�v�f�[�^���󂯎��܂�
	/// </summary>
	/// <param name="STAGE_NUMBER">�X�e�[�W�ԍ�</param>
	/// <param name="ROOM_NUMBER">�������ԍ�</param>
	/// <returns>vector<vector<int>>�̃}�b�v�`�b�v�z��</returns>
	const RoomMapChipArray &GetMapChipData(const int &STAGE_NUMBER, const int &ROOM_NUMBER);

	/// <summary>
	/// ������ǂ��̕����Ɍ���������Ԃ��܂�
	/// </summary>
	/// <param name="STAGE_NUMBER">�X�e�[�W�ԍ�</param>
	/// <param name="ROOM_NUMBER">�������ԍ�</param>
	/// <param name="DOOR_NUMBER">�h�A�ԍ�</param>
	/// <returns>���Ɍ������������ԍ�</returns>
	const int &GetRelationData(const int &STAGE_NUMBER, const int &ROOM_NUMBER, const int &DOOR_NUMBER);

	/// <summary>
	/// �}�b�v�`�b�v�̈�̏���Ԃ��܂�
	/// </summary>
	/// <param name="TYPE">�}�b�v�`�b�v�̎��</param>
	/// <returns>�}�b�v�`�b�v�̈�̏��</returns>
	const SizeData GetMapChipSizeData(MapChipData TYPE);

	/// <summary>
	/// X��Y�̃u���b�N���W�̏�񂩂珬�����̃`�b�v�ԍ���Ԃ��܂�
	/// </summary>
	/// <param name="STAGE_NUMBER">�X�e�[�W�ԍ�</param>
	/// <param name="ROOM_NUMBER">�������ԍ�</param>
	/// <param name="MAPCHIP_POS">�}�b�v�`�b�v���W</param>
	/// <returns>�`�b�v�ԍ�</returns>
	inline const int &GetMapChipBlock(const int &STAGE_NUMBER, const int &ROOM_NUMBER, const Vec2<float> &MAPCHIP_POS)
	{
		RoomMapChipArray tmp = allMapChipData[STAGE_NUMBER][ROOM_NUMBER];

		Vec2<int>mapChipPos;
		mapChipPos.x = MAPCHIP_POS.x;
		mapChipPos.y = MAPCHIP_POS.y;

		//Y���̃}�b�v�`�b�v���͈͊O�Ȃ�-1��Ԃ�
		if (tmp.size() <= mapChipPos.y || mapChipPos.y < 0)
		{
			return -1;
		}
		//X���̃}�b�v�`�b�v���͈͊O�Ȃ�-1��Ԃ�
		if (tmp[mapChipPos.y].size() <= mapChipPos.x || mapChipPos.x < 0)
		{
			return -1;
		}
		int result = tmp[mapChipPos.y][mapChipPos.x];
		//�w�肵���ꏊ�̃}�b�v�`�b�v�ԍ���Ԃ�
		return result;
	};

	RoomMapChipDrawArray GetMapChipDrawBlock(const int &STAGE_NUMBER, const int &ROOM_NUMBER);


	const bool &CheckStageNum(const int &STAGE_NUMBER);
	const bool &CheckRoomNum(const int &STAGE_NUMBER, const int &ROOM_NUMBER);


	/// <summary>
	/// �w�肵���X�e�[�W�ԍ��̏������̐���Ԃ��܂�
	/// </summary>
	/// <param name="STAGE_NUMBER">�X�e�[�W�ԍ�</param>
	/// <returns>�������̐�</returns>
	inline const int &GetMaxRoomNumber(const int &STAGE_NUMBER)
	{
		return allMapChipData[STAGE_NUMBER].size();
	}

	/// <summary>
	/// ���ۊǂ���Ă���X�e�[�W�̐���Ԃ��܂�
	/// </summary>
	/// <returns></returns>
	inline const int &GetMaxStageNumber()
	{
		return allMapChipData.size();
	}

	std::vector<MapChipAnimationData *> animationData;//�}�b�v�`�b�v�̃A�j���[�V�������̈ꗗ

private:
	CSVLoader loder;	//CSV�f�[�^��ǂݍ��ވׂ̃N���X

	//�l�����z��
	/*
	[0]...�X�e�[�W�ԍ�
	[1]...�������ԍ�
	[2]...Y��
	[3]...X��
	*/
	std::vector<StageMapChipData> allMapChipData;	//�Q�[�����ɂ���S�ẴX�e�[�W�̃}�b�v�`�b�v�f�[�^
	std::vector<StageRelationData> relationRoomData;//�Q�[�����ɂ���S�ẴX�e�[�W�̏��������m�̃����N�t��
	std::array<SizeData, MAPCHIP_TYPE_MAX> mapChipMemoryData;//�}�b�v�`�b�v�ԍ��̗̈�

	std::vector<StageMapChipDrawData> allMapChipDrawData;//�}�b�v�`�b�v�̕`����

	array<int, 30> mapChipGraphHandle;
	enum
	{
		MAPCHIP_DRAW_WALL_LEFT_UP,
		MAPCHIP_DRAW_FLOOR,
		MAPCHIP_DRAW_WALL_RIGHT_UP,
		MAPCHIP_DRAW_WALL_TOP,
		MAPCHIP_DRAW_WALL_IN_RIGHT_DOWN,
		MAPCHIP_DRAW_WALL_IN_LEFT_DOWN,
		MAPCHIP_DRAW_WALL_LEFT,
		MAPCHIP_DRAW_WALL_IN,
		MAPCHIP_DRAW_WALL_RIGHT,
		MAPCHIP_DRAW_WALL_CENTRAL,
		MAPCHIP_DRAW_WALL_IN_RIGHT_UP,
		MAPCHIP_DRAW_WALL_IN_LEFT_UP,
		MAPCHIP_DRAW_WALL_LEFT_DOWN,
		MAPCHIP_DRAW_WALL_DOWN,
		MAPCHIP_DRAW_WALL_RIGHT_DOWN,
		MAPCHIP_DRAW_WALL_BUTTOM,
		MAPCHIP_DRAW_WALL_SPACE1,
		MAPCHIP_DRAW_WALL_SCPACE2,
		MAPCHIP_DRAW_WALL_CORNER,
		MAPCHIP_DRAW_WALL_TBLOCK,
		MAPCHIP_DRAW_WALL_TBLOCK2,
		MAPCHIP_DRAW_WALL_LEFTDOWN_1PIXEL,
		MAPCHIP_DRAW_WALL_RIGHTDOWN_1PIXEL,
		MAPCHIP_DRAW_ERROR,
		MAPCHIP_DRAW_DOOR
	};


	float Radian(float ANGLE)
	{
		return ANGLE * Angle::PI() / 180.0f;
	}
	;

	bool CheckDoor(vector<Vec2<float>> *DATA, int STAGE_NUM, int ROOM_NUM, Vec2<float> MAPCHIP, int DOOR_NUM);

	inline void DoorError(int STAGE_NUM, int ROOM_NUM, int DOOR_NUM)
	{
		string text("�X�e�[�W");
		string text2("�����ԍ�");
		string text3("�h�A�ԍ�");
		string text4("�͓��������ɓ�ȏ�g���Ă��܂�");

		string result =
			text + std::to_string(STAGE_NUM) +
			text2 + std::to_string(ROOM_NUM) +
			text3 + std::to_string(DOOR_NUM) +
			text4 + "\nOK���������㋭���I�����܂��̂ŁA������x�ݒ肵�����ĉ�����";

		//MessageBox(NULL, result.c_str(), TEXT("�h�A�ԍ����"), MB_OK);
		MessageBox(NULL, KuroFunc::GetWideStrFromStr(result).c_str(), TEXT("�h�A�ԍ����"), MB_OK);
		assert(0);
	}

	inline void StartError(int STAGE_NUM, int ROOM_NUM, int DOOR_NUM)
	{
		string text("�X�e�[�W");
		string text2("�����ԍ�");
		string text3("�h�A�ԍ�");
		string text4("�͓��������ɓ�ȏ�g���Ă��܂�");

		string result =
			text + std::to_string(STAGE_NUM) +
			text2 + std::to_string(ROOM_NUM) +
			text3 + std::to_string(DOOR_NUM) +
			text4 + "\nOK���������㋭���I�����܂��̂ŁA������x�ݒ肵�����ĉ�����";

		//MessageBox(NULL, result.c_str(), TEXT("�h�A�ԍ����"), MB_OK);
		MessageBox(NULL, KuroFunc::GetWideStrFromStr(result).c_str(), TEXT("�h�A�ԍ����"), MB_OK);
		assert(0);
	}

	//�w��̔ԍ����ǂ��ǂ������f����
	inline void AlimentWallNumber(int *CHIP_NUMBER)
	{
		bool outSideFlag = *CHIP_NUMBER == -1;
		bool isWallFlag = false;

		//�`�b�v�ԍ���1����9�ȉ��Ȃ�ǔ�����o��
		if (1 <= *CHIP_NUMBER && *CHIP_NUMBER <= 9)
		{
			isWallFlag = true;
		}

		if (isWallFlag || outSideFlag)
		{
			*CHIP_NUMBER = 1;
		}
	};

	//�w��̔ԍ����󔒂��ǂ������f����
	inline void AlimentSpaceNumber(int *CHIP_NUMBER)
	{
		bool isSpaceFlag = false;

		//�`�b�v�ԍ���0��������10�ȏ�Ȃ�󔒔�����o��
		if (*CHIP_NUMBER == 0 || 10 <= *CHIP_NUMBER)
		{
			isSpaceFlag = true;
		}

		if (isSpaceFlag)
		{
			*CHIP_NUMBER = 0;
		}
	};

};

