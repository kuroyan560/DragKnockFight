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
#include"MapChipCollider.h"
#include"MapChipInfo.h"
#include"MapChipGenerator.h"
#include"CharacterInfo.h"

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
	MAPCHIP_TYPE_STATIC_CHANGE_AREA = 17,
	MAPCHIP_TYPE_STATIC_UNBROKEN_BLOCK,
	MAPCHIP_TYPE_STATIC_RARE_BLOCK,
	MAPCHIP_TYPE_STATIC_BOUNCE_BLOCK,
	MAPCHIP_TYPE_STATIC_NON_SCORE_BLOCK,
	MAPCHIP_TYPE_STATIC_ELEC_ON_ALLWAYS,
	MAPCHIP_TYPE_STATIC_RESPONE_PLAYER = 30,
	MAPCHIP_TYPE_STATIC_RESPONE_BOSS = 31,
	MAPCHIP_TYPE_MAX
};

enum MapChipBlockData
{
	MAPCHIP_BLOCK_START = 30,
	MAPCHIP_BLOCK_GOAL = 31
};

enum MapChipType
{
	MAPCHIP_BLOCK_NONE,
	MAPCHIP_BLOCK_SPACE,
	MAPCHIP_BLOCK_WALL,
	MAPCHIP_BLOCK_CHANGE_AREA,
	MAPCHIP_BLOCK_COLOR_LEFT,
	MAPCHIP_BLOCK_COLOR_RIGHT,
	MAPCHIP_BLOCK_ELEC_ON,
	MAPCHIP_BLOCK_ELEC_OFF,
	MAPCHIP_BLOCK_ELEC_ON_ALLWAYS,
};


struct MapChipAnimationData
{
	vector<int> handle;	//�Đ�����A�j���[�V�����̑���
	int maxInterval;		//�A�j���[�V�����̊Ԋu

	MapChipAnimationData(const vector<int> &HANDLE, int INTERVAL = -1) :handle(HANDLE), maxInterval(INTERVAL)
	{
	};
};

enum GimickHandle
{
	GMMICK_RED,
	GMMICK_GREEN,
	GMMICK_ELEC_OFF
};


typedef std::vector<RelationData> RoomRelationData;
typedef std::vector<RoomRelationData> StageRelationData;

struct StageInfo
{
	MapChipArray mapChips;
	int swingCount = 0;
	int gameMaxTimer = 60;
	MAP_CHIP_GENERATOR generatorType = NON_GENERATE;
	int generatorSpan = 60;
	PLAYABLE_CHARACTER_NAME characterName = PLAYABLE_LUNA;
};

//�S�ẴX�e�[�W�̃}�b�v�`�b�v�f�[�^���Ǘ�����N���X
class StageMgr :public Singleton<StageMgr>
{
public:
	StageMgr();

	/// <summary>
	/// ���̃X�e�[�W�Ŏg���}�b�v�`�b�v��ݒ肵�܂�
	/// </summary>
	/// <param name="STAGE_NUMBER">�X�e�[�W�ԍ�</param>
	/// <param name="ROOM_NUMBER">�������ԍ�</param>
	void SetLocalMapChipData(const int &STAGE_NUMBER, const int &ROOM_NUMBER);

	/// <summary>
	/// ������ǂ��̕����Ɍ���������Ԃ��܂�
	/// </summary>
	/// <param name="STAGE_NUMBER">�X�e�[�W�ԍ�</param>
	/// <param name="ROOM_NUMBER">�������ԍ�</param>
	/// <param name="DOOR_NUMBER">�h�A�ԍ�</param>
	/// <returns>���Ɍ������������ԍ�</returns>
	//const int &GetRelationData(const int &STAGE_NUMBER, const int &ROOM_NUMBER, const int &DOOR_NUMBER);

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
		auto& tmp = stageInfos[STAGE_NUMBER][ROOM_NUMBER].mapChips;

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
		int result = tmp[mapChipPos.y][mapChipPos.x].chipType;
		//�w�肵���ꏊ�̃}�b�v�`�b�v�ԍ���Ԃ�
		return result;
	};


	const bool &CheckStageNum(const int &STAGE_NUMBER);
	const bool &CheckRoomNum(const int &STAGE_NUMBER, const int &ROOM_NUMBER);


	inline const bool &IsItWallIn(const int &STAGE_NUM, const int &ROOM_NUM, const Vec2<int> &MAPCHIP_POS)
	{
		if (stageInfos[STAGE_NUM][ROOM_NUM].mapChips[MAPCHIP_POS.y][MAPCHIP_POS.x].drawData.handle == mapChipGraphHandle[MAPCHIP_DRAW_WALL_IN])
		{
			return true;
		}
		else
		{
			return false;
		}
	};

	/// <summary>
	/// �w�肵���X�e�[�W�ԍ��̏������̐���Ԃ��܂�
	/// </summary>
	/// <param name="STAGE_NUMBER">�X�e�[�W�ԍ�</param>
	/// <returns>�������̐�</returns>
	inline const int &GetMaxRoomNumber(const int &STAGE_NUMBER)
	{
		return stageInfos[STAGE_NUMBER].size();
	}

	/// <summary>
	/// ���ۊǂ���Ă���X�e�[�W�̐���Ԃ��܂�
	/// </summary>
	/// <returns></returns>
	inline const int &GetMaxStageNumber()
	{
		return stageInfos.size();
	}

	std::vector<std::shared_ptr<MapChipAnimationData>> animationData;//�}�b�v�`�b�v�̃A�j���[�V�������̈ꗗ


	void WriteMapChipData(const Vec2<int> MAPCHIP_NUM, const int& CHIPNUM, const Vec2<float>& LeftCharaPos, const float& LeftCharaSize, const Vec2<float>& RightCharaPos, const float& RightCharaSize, const bool& CharaCheck = true, const int& ChipGraph = -1);

	MapChipType GetMapChipType(const int &STAGE_NUM, const int &ROOM_NUM, const Vec2<int> MAPCHIP_NUM);

	MapChipType GetLocalMapChipType(const Vec2<int> MAPCHIP_NUM);

	inline const int &GetLocalMapChipBlock(const Vec2<int> &MAPCHIP_POS)
	{
		auto& tmp = localRoomMapChipArray;

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
		int result = tmp[mapChipPos.y][mapChipPos.x].chipType;
		//�w�肵���ꏊ�̃}�b�v�`�b�v�ԍ���Ԃ�
		return result;
	};


	MapChipArray* GetLocalMap() { return &localRoomMapChipArray; }

	int GetAllLocalWallBlocksNum(int RARE_BLOCK_COUNT = 10);
	int GetAllRoomWallBlocksNum(int STAGE_NUM, int RARE_BLOCK_COUNT = 10);
	int GetAllWallBlocksNum(int STAGE_NUM, int ROOM_NUM);


	int GetMaxMapChipNum();

	int GetEnableToUseRoomNumber(int STAGE_NUMBER);
	int GetEnableToUseStageNumber();


	Vec2<float>GetPlayerResponePos(const int& StageNum, const int& RoomNum);
	Vec2<float>GetBossResponePos(const int& StageNum, const int& RoomNum);

	
	int GetSwingCount(int STAGE_NUM, int ROOM_NUM);
	int GetMaxTime(int STAGE_NUM, int ROOM_NUM);

private:
	CSVLoader loder;	//CSV�f�[�^��ǂݍ��ވׂ̃N���X

	//�l�����z��
	/*
	[0]...�X�e�[�W�ԍ�
	[1]...�������ԍ�
	[2]...Y��
	[3]...X��
	*/
	std::array<SizeData, MAPCHIP_TYPE_MAX> mapChipMemoryData;//�}�b�v�`�b�v�ԍ��̗̈�

	static const int SECRET_DOOR_NUMBER = 5;

	MapChipArray localRoomMapChipArray;

	int thowGraphHandle;
	std::array<int, 12> eventChipHandle;

	array<int, 30> mapChipGraphHandle;
	array<int, 3> gimmcikGraphHandle;
	array<int, 12> sparkGraphHandle;

	std::vector<std::vector<StageInfo>>stageInfos;

	enum MapChipDrawEnum
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


public:
	//�w��̔ԍ����ǂ��ǂ������f����
	inline void AlimentWallNumber(int *CHIP_NUMBER)
	{
		bool outSideFlag = *CHIP_NUMBER == -1;
		bool isWallFlag = false;
		//bool doorFlag = mapChipMemoryData[MAPCHIP_TYPE_DOOR].min <= *CHIP_NUMBER && *CHIP_NUMBER <= mapChipMemoryData[MAPCHIP_TYPE_DOOR].max;
		//bool thownFlag = *CHIP_NUMBER == MAPCHIP_BLOCK_THOWN;

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

	//�}�b�v�̃C���f�b�N�X�̃T�C�Y���擾
	Vec2<int>GetMapIdxSize(const int& StageNum, const int& RoomNum)
	{
		return Vec2<int>(stageInfos[StageNum][RoomNum].mapChips[0].size(), stageInfos[StageNum][RoomNum].mapChips.size());
	}

	Vec2<int>GetLocalMapIdxSize()
	{
		return Vec2<int>(localRoomMapChipArray[0].size(), localRoomMapChipArray.size());
	}
	//�}�b�v�̍��W��̃T�C�Y���擾
	Vec2<float>GetLocalMapGrobalSize()
	{
		return GetLocalMapIdxSize().Float() * MAP_CHIP_SIZE;
	}

	const int GetChipGraoh(const int& ChipType)
	{
		if (ChipType == MAPCHIP_TYPE_STATIC_RARE_BLOCK)return gimmcikGraphHandle[GMMICK_RED];
		else if (ChipType == MAPCHIP_TYPE_STATIC_BOUNCE_BLOCK)return sparkGraphHandle[0];
		else if (ChipType == MAPCHIP_TYPE_STATIC_UNBROKEN_BLOCK)return gimmcikGraphHandle[GMMICK_GREEN];
		else if (ChipType == MAPCHIP_TYPE_STATIC_NON_SCORE_BLOCK)return gimmcikGraphHandle[GMMICK_ELEC_OFF];
		return mapChipGraphHandle[0];
	}

	const bool HaveMap(const int& StageNum, const int& RoomNum)
	{
		if (stageInfos.size() <= StageNum)return false;
		if (stageInfos[StageNum].size() <= RoomNum)return false;
		return true;
	}
	const int GetMaxLap(const int& StageNum)
	{
		return stageInfos[StageNum].size();
	}
	const MapChipArray& GetMap(const int& StageNum, const int& RoomNum)
	{
		return stageInfos[StageNum][RoomNum].mapChips;
	}
	const StageInfo& GetStageInfo(const int& StageNum, const int& RoomNum)
	{
		return stageInfos[StageNum][RoomNum];
	}
	int GetRandNormalWallGraphHandle();

private:
	const int &GetGimmickNumber(const int &NUMBER)
	{
		int num = NUMBER;
		int sum = 0;

		for (; 0 <= num - 2;)
		{
			num -= 2;
			++sum;
		}
		return sum;
	}

	void SetGimmickGraphHandle(const int &STAGE_NUM, const int &ROOM_NUM, const Vec2<int> &MAPCHIP_NUM);

	void SetLocalGimmickGraphHandle(const Vec2<int> &MAPCHIP_NUM, const int &CHIP_NUM)
	{
		localRoomMapChipArray[MAPCHIP_NUM.y][MAPCHIP_NUM.x].drawData.Reset();

		//�`��̏�������
		if (CHIP_NUM == MAPCHIP_TYPE_STATIC_UNBROKEN_BLOCK)
		{
			localRoomMapChipArray[MAPCHIP_NUM.y][MAPCHIP_NUM.x].drawData.handle = gimmcikGraphHandle[GMMICK_GREEN];
		}
		else if (CHIP_NUM == MAPCHIP_TYPE_STATIC_RARE_BLOCK)
		{
			localRoomMapChipArray[MAPCHIP_NUM.y][MAPCHIP_NUM.x].drawData.handle = gimmcikGraphHandle[GMMICK_RED];
		}
		else if (CHIP_NUM == MAPCHIP_TYPE_STATIC_BOUNCE_BLOCK)
		{
			localRoomMapChipArray[MAPCHIP_NUM.y][MAPCHIP_NUM.x].drawData.handle = 0;
			localRoomMapChipArray[MAPCHIP_NUM.y][MAPCHIP_NUM.x].drawData.animationFlag = true;
			localRoomMapChipArray[MAPCHIP_NUM.y][MAPCHIP_NUM.x].drawData.interval = 5;
		}
		else if (CHIP_NUM == MAPCHIP_TYPE_STATIC_NON_SCORE_BLOCK)
		{
			localRoomMapChipArray[MAPCHIP_NUM.y][MAPCHIP_NUM.x].drawData.handle = gimmcikGraphHandle[GMMICK_ELEC_OFF];
		}
		else if (CHIP_NUM == MAPCHIP_TYPE_STATIC_ELEC_ON_ALLWAYS)
		{
			localRoomMapChipArray[MAPCHIP_NUM.y][MAPCHIP_NUM.x].drawData.handle = 0;
			localRoomMapChipArray[MAPCHIP_NUM.y][MAPCHIP_NUM.x].drawData.animationFlag = true;
			localRoomMapChipArray[MAPCHIP_NUM.y][MAPCHIP_NUM.x].drawData.interval = 5;
		}
	}

};

