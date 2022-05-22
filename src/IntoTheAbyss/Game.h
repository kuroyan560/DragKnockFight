#pragma once
#include"Vec.h"
#include<vector>
#include "StageMgr.h"
using namespace std;

#include"GimmickLoader.h"

#include<memory>
class TextureBuffer;

#include"DrawMap.h"
class RenderTarget;
#include"MassChip.h"

#include"ThornBlock.h"
#include"HomeBase.h"
#include"MapChipCollider.h"

#include"MiniMap.h"

#include"RoundChangeEffect.h"
#include"ScreenEdgeEffect.h"

#include"NavigationAI.h"
#include"CharacterAI.h"

#include"BossHandMgr.h"

#include "Barrages.h"

class CharacterInterFace;

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
	void DrawMapChip(const vector<vector<int>>& mapChipData, vector<vector<MapChipDrawData>>& mapChipDrawData, const int& stageNum, const int& roomNum);
	const int& GetChipNum(const vector<vector<int>>& MAPCHIP_DATA, const int& MAPCHIP_NUM, int* COUNT_CHIP_NUM, Vec2<float>* POS);

	// �{�Xor�v���C���[�������������Ă��邩�̃t���O
	bool isCatchMapChipBoss;
	bool isCatchMapChipPlayer;

	// �{�X�v���C���[�Ԃ̐�
	Vec2<float> lineCenterPos;			// �R�̒��S�_
	Vec2<float> prevLineCenterPos;		// �O�t���[���̕R�̒��S�_

	// �}�b�v�`�b�v�̃f�[�^
	vector<vector<int>>* mapData;

	int oldStageNum = -1;
	int oldRoomNum = -1;

	vector<vector<MapChipDrawData>>* mapChipDrawData;
	int prevDrawChipStageNum;
	int prevDrawChipRoomNum;

	int countStopNum = 0;
	int countHitNum = 0;

	Vec2<float> prevPlayerChipPos;

	bool sceneUpDownFlag;
	bool sceneChangingFlag;
	int alphaValue;
	int timer;
	Vec2<float> responePos;
	Vec2<float> restartPos;
	int sceneChangeHandle;//�V�[���J�ڗp�̉摜
	bool sceneChangeDeadFlag;//�v���C��������ł�����t���O�𗧂ăV�[���J�ڒ��ɓ���ȏ���������
	bool initDeadFlag;

	int restartTimer;//�V�[���J�ڌォ�牽�b�o�߂��Ă��邩

	std::vector<std::unique_ptr<MassChip>> massChipData;


	std::vector<DrawMap>drawMap;

	//�����J��
	bool roundFinishFlag;		//���E���h�I�����̉��o�J�n�p�̃t���O
	bool readyToStartRoundFlag; //���E���h�J�n���̉��o�J�n�p�̃t���O
	bool gameStartFlag;			//�Q�[���J�n���̃t���O

	float lineExtendScale;
	const float lineExtendMaxScale = 10.0f;


	int roundTimer;
	int bgm;

	bool firstLoadFlag;
	void InitGame(const int& STAGE_NUM, const int& ROOM_NUM);

	MiniMap miniMap;
	Vec2<float>cameraBasePos;

	RoundChangeEffect roundChangeEffect;
	bool playerOrEnemeyWinFlag;

	bool turnResultScene = false;


	Color areaHitColor;
	Color playerHitColor;

	ScreenEdgeEffect screenEdgeEffect;

	Vec2<float>responeScrollPos;

	//�o�ꉉ�o���s�����̃t���O
	bool practiceMode = true;

	std::unique_ptr<BossHandMgr> bossHandMgr;
	std::unique_ptr<BossHandMgr> playerHandMgr;
public:
	HomeBase playerHomeBase, enemyHomeBase;

	array<int, 2> debugStageData = { 0,0 };//�f�o�b�N�p�̃X�e�[�W�ƕ����ԍ�
	int nowSelectNum = 0;

	Game();
	void Init(const bool& PractiveMode = false);
	void Update(const bool& Loop = false);
	void Draw();
	void Scramble();
	void CalCenterPos();

	Vec2<float>GetStageSize();
	const bool& TurnResultScene() { return turnResultScene; }


	// �X�e�[�W�؂�ւ��֐�
	void SwitchingStage();

	// �w�n�̔���
	void DeterminationOfThePosition();

	// ���E���h�J�n�����o
	void RoundStartEffect(const bool& Loop, const RoomMapChipArray& tmpMapData);

	// ���E���h�I�������o
	void RoundFinishEffect(const bool& Loop);

};

