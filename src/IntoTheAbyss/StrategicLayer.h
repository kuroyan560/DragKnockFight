#pragma once
#include"../KuroEngine.h"
#include"../IntoTheAbyss/CharacterAIData.h"
#include"../IntoTheAbyss/TacticalLayer.h"
#include"../IntoTheAbyss/BehavioralLayer.h"
#include"../IntoTheAbyss/NavigationAI.h"
/// <summary>
/// ��p�w
/// </summary>


/// <summary>
/// �X�^�~�i�̉�
/// </summary>
class RestoreStamina
{
public:
	RestoreStamina();

	void Init();

	/// <summary>
	/// ���s
	/// </summary>
	void Update();

	/// <summary>
	/// ���ݎ��s���Ă��鏈���̐i��
	/// </summary>
	/// <returns>FAIL...���s,INPROCESS...���s��,SUCCESS...����</returns>
	AiResult CurrentProgress();


	float EvaluationFunction();


	Vec2<float>startPos, endPos;
	Vec2<int>startHandle,endHandle;
	WayPointData startPoint, endPoint;
	bool startFlag;
	std::vector<WayPointData> route;
private:
	FollowPath followPath;
	SearchWayPoint searchStartPoint,searchGoalPoint;
	MoveToOwnGround moveToOnwGround;

	std::vector<Vec2<float>>itemList;
	bool initRouteFlag;



	//�A�C�e���T��--------------------------
	SphereCollision searchArea;
	int searchItemIndex;
	bool seachItemFlag;
	bool initFlag;
	Vec2<int>prevStartHandle;
	static const float SEARCH_RADIUS;

	bool getFlag;
	int prevItemIndex;
	struct SearchData
	{
		float distance;
		int itemIndex;
	};
	RestoreStamina::SearchData SearchItem(const SphereCollision &DATA);
	//�A�C�e���T��--------------------------

	//����--------------------------
	int staminaGauge;
	int timer;
	int timeOver;
	static const int SUCCEED_GAIN_STAMINA_VALUE;//�ǂꂮ�炢�̒l

};

/// <summary>
/// ���w�Ɍ�����
/// </summary>
class GoToTheField
{
public:
	GoToTheField();

	void Init();

	/// <summary>
	/// ���s
	/// </summary>
	void Update();

	/// <summary>
	/// ���ݎ��s���Ă��鏈���̐i��
	/// </summary>
	/// <returns>FAIL...���s,INPROCESS...���s��,SUCCESS...����</returns>
	AiResult CurrentProgress();

	WayPointData startPoint, endPoint;
	bool startFlag;
	std::vector<WayPointData>route;
	MoveToOwnGround moveToOnwGround;
private:
	int timer;
	int timeOver;

	bool goToTheFieldFlag;

};



/// <summary>
/// �D���Q�[�W���l������
/// </summary>
class AcquireASuperiorityGauge
{
public:
	AcquireASuperiorityGauge();

	void Init();

	/// <summary>
	/// ���s
	/// </summary>
	void Update();

	/// <summary>
	/// ���ݎ��s���Ă��鏈���̐i��
	/// </summary>
	/// <returns>FAIL...���s,INPROCESS...���s��,SUCCESS...����</returns>
	AiResult CurrentProgress();

private:
	static const float SUCCEED_GAUGE_VALUE;//�ǂ��܂ŉ񕜂����琬���Ƃ��邩
	float nowGauge;//�헪�J�n���̗D���Q�[�W
	int timer;
	int timeOver;

	//�N���b�V��������--------------------------
	bool crashEnemyFlag;



	//�N���b�V������Ȃ��悤�ɂ���--------------------------
	bool dontCrashFlag;
};