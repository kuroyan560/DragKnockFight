#pragma once
#include"Vec.h"
#include<vector>
#include "StageMgr.h"
using namespace std;
#include "Player.h"
#include "Enemy.h"
#include"AuraBlock.h"
#include "DossunBlock.h"
#include "Bubble.h"
#include"GimmickLoader.h"
#include"EventBlock.h"
#include "Boss.h"

#include<memory>
class TextureBuffer;

#include"DrawMap.h"
#include"LightManager.h"
class RenderTarget;
#include"MassChip.h"
#include"DoorBlock.h"

#include"ThornBlock.h"
#include"HomeBase.h"

struct MassChipData
{
	Vec2<float>leftUpPos;
	Vec2<float>rightDownPos;
	bool sideOrUpDownFlag;

	MassChipData(const Vec2<float>& LEFT_UP_POS, const Vec2<float>& RIGHT_DOWN_POS, const bool& SIDE_OR_UPDOWN_FLAG) :
		leftUpPos(LEFT_UP_POS), rightDownPos(RIGHT_DOWN_POS), sideOrUpDownFlag(SIDE_OR_UPDOWN_FLAG)
	{
	}
};


//���\�����[�V������main�������܂Ƃ߂�����
class Game
{
	std::vector<std::unique_ptr<MassChipData>> AddData(RoomMapChipArray MAPCHIP_DATA, const int& CHIP_NUM);
	void DrawMapChip(const vector<vector<int>>& mapChipData, vector<vector<MapChipDrawData>>& mapChipDrawData, const int& mapBlockGraph, const int& stageNum, const int& roomNum);
	const int& GetChipNum(const vector<vector<int>>& MAPCHIP_DATA, const int& MAPCHIP_NUM, int* COUNT_CHIP_NUM, Vec2<float>* POS);


	int mapBlockGraph;
	// ���I�u���b�N�̉摜�B
	int movingBlockGraph;



	// �G
	Enemy bossEnemy;
	static const int SMALL_ENEMY = 100;
	array<Enemy, SMALL_ENEMY> smallEnemy;
	static const int NOMOVEMENT_ENEMY = 100;
	array<Enemy, NOMOVEMENT_ENEMY> noMovementEnemy;
	int enemyGenerateTimer;
	const int SMALL_GENERATE_TIMER = 60;
	int nomoveMentTimer;
	const int NOMOVEMENT_GENERATE_TIMER = 30;

	// �{�X
	Boss boss;

	// �{�Xor�v���C���[�������������Ă��邩�̃t���O
	bool isCatchMapChipBoss;
	bool isCatchMapChipPlayer;

	// �{�X�v���C���[�Ԃ̐�
	float lineLengthPlayer;				// �v���C���[�̕R�̒���
	float lineLengthBoss;				// �{�X�̕R�̒���
	float addLineLengthPlayer;			// �v���C���[�������������ĐL�т���
	float addLineLengthBoss;			// �{�X�������������ĐL�т���
	const float LINE_LENGTH = 150.0f;	// �R�̃f�t�H���g�̒����̔���
	Vec2<float> lineCenterPos;			// �R�̒��S�_
	Vec2<float> prevLineCenterPos;		// �O�t���[���̕R�̒��S�_

	// �}�b�v�`�b�v�̃f�[�^
	vector<vector<int>> mapData;

	vector<DossunBlock> dossunBlock;
	vector<Bubble> bubbleBlock;


	int oldStageNum = -1;
	int oldRoomNum = -1;

	vector<std::unique_ptr<AuraBlock>> auraBlock;
	vector<vector<MapChipDrawData>> mapChipDrawData;
	std::vector<std::unique_ptr<DoorBlock>> doorBlocks;
	std::vector<std::unique_ptr<ThornBlock>> thornBlocks;

	int countStopNum = 0;
	int countHitNum = 0;

	Vec2<float> prevPlayerChipPos;

	bool sceneBlackFlag;//�t���O�����ƈÓ]����
	bool sceneLightFlag;//�t���O�����Ɩ��]����
	bool sceneUpDownFlag;
	bool sceneChangingFlag;
	int alphaValue;
	int timer;
	Vec2<float> responePos;
	Vec2<float> restartPos;
	int doorNumber;
	int giveDoorNumber;
	int doorArrayNumber;
	int sceneChangeHandle;//�V�[���J�ڗp�̉摜
	E_DOOR_DIR door;		//�ǂ̕����ŏo�邩
	bool initJumpFlag;
	float gravity;
	E_DOOR_DIR doorDir;
	bool sceneChangeDeadFlag;//�v���C��������ł�����t���O�𗧂ăV�[���J�ڒ��ɓ���ȏ���������
	bool initDeadFlag;

	int restartTimer;//�V�[���J�ڌォ�牽�b�o�߂��Ă��邩

	std::vector<std::unique_ptr<MassChip>> massChipData;
	std::vector<std::unique_ptr<EventBlock>>eventBlocks;


	std::vector<DrawMap>drawMap;

	//���C�g���
	LightManager ligMgr;
	const float PT_LIG_RANGE = 128.0;
	const float PT_LIG_BRIGHT = 0.3f;
	const float PT_LIG_Z = -2.0f;
	Light::Point ptLig;
	const float SPOT_LIG_RANGE = 128.0;
	const float SPOT_LIG_TARGET_OFFSET_Y = -10.0f;
	const float SPOT_LIG_Z = -20.0f;
	Light::Spot spotLig;
	Light::HemiSphere hemiLig;


	//�����֘A
	int countRound;						//���E���h��
	int countPlayerWin, countEnemyWin;	//������

	//�����J��
	bool roundFinishFlag;		//���E���h�I�����̉��o�J�n�p�̃t���O
	bool readyToStartRoundFlag; //���E���h�J�n���̉��o�J�n�p�̃t���O
	bool gameStartFlag;			//�Q�[���J�n���̃t���O

	void InitGame(const int& STAGE_NUM, const int& ROOM_NUM);

public:
	std::unique_ptr<HomeBase> playerHomeBase, enemyHomeBase;

	// �v���C���[
	Player player;
	array<int, 2> debugStageData = { 0,0 };//�f�o�b�N�p�̃X�e�[�W�ƕ����ԍ�
	int nowSelectNum = 0;

	Game();
	void Init();
	void Update();
	void Draw(std::weak_ptr<RenderTarget>EmissiveMap);
	void Scramble();

	LightManager& GetLigManager() { return ligMgr; }


};

