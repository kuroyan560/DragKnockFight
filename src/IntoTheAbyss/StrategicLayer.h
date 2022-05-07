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
/// �헪�w�̒��ۃN���X
/// </summary>
class IStrategicLayer
{
public:
	~IStrategicLayer() {};

	virtual void Init() = 0;

	/// <summary>
	/// ���s
	/// </summary>
	virtual void Update() = 0;

	/// <summary>
	/// ���ݎ��s���Ă��鏈���̐i��
	/// </summary>
	/// <returns>FAIL...���s,INPROCESS...���s��,SUCCESS...����</returns>
	virtual AiResult CurrentProgress() = 0;

	/// <summary>
	/// �]���l�̎Z�o
	/// </summary>
	/// <returns>�]���l</returns>
	virtual float EvaluationFunction() = 0;

	WayPointData startPoint, endPoint;
	std::vector<WayPointData> route;
	bool startFlag;

protected:
	int timer;
	int timeOver;

	//�A�C�e���T��
	struct SearchData
	{
		float distance;
		int itemIndex;
	};
	SearchData SearchItem(const SphereCollision &DATA);
	static const float SEARCH_RADIUS;
};


/// <summary>
/// �X�^�~�i�̉�
/// </summary>
class RestoreStamina :public IStrategicLayer
{
public:
	RestoreStamina();

	void Init()override;

	/// <summary>
	/// ���s
	/// </summary>
	void Update()override;

	/// <summary>
	/// ���ݎ��s���Ă��鏈���̐i��
	/// </summary>
	/// <returns>FAIL...���s,INPROCESS...���s��,SUCCESS...����</returns>
	AiResult CurrentProgress()override;


	float EvaluationFunction()override;


private:
	FollowPath followPath;
	SearchWayPoint searchStartPoint, searchGoalPoint;
	MoveToOwnGround moveToOnwGround;

	std::vector<Vec2<float>>itemList;
	bool initRouteFlag;



	//�A�C�e���T��--------------------------
	SphereCollision searchArea;
	int searchItemIndex;
	bool seachItemFlag;
	bool initFlag;
	Vec2<int>prevStartHandle;

	bool getFlag;
	int prevItemIndex;
	//�A�C�e���T��--------------------------

	//����--------------------------
	float staminaGauge;
	static const float SUCCEED_GAIN_STAMINA_VALUE;//�ǂꂮ�炢�̒l

};

/// <summary>
/// ���w�Ɍ�����
/// </summary>
class GoToTheField :public IStrategicLayer
{
public:
	GoToTheField();

	void Init()override;

	/// <summary>
	/// ���s
	/// </summary>
	void Update()override;

	/// <summary>
	/// ���ݎ��s���Ă��鏈���̐i��
	/// </summary>
	/// <returns>FAIL...���s,INPROCESS...���s��,SUCCESS...����</returns>
	AiResult CurrentProgress()override;

	float EvaluationFunction();

	MoveToOwnGround moveToOnwGround;
private:
	bool goToTheFieldFlag;

	int swingCoolTime;
	const int SWING_MAX_COOL_TIME = 30;//�U��񂵂̃N�[���^�C��
};



/// <summary>
/// �D���Q�[�W���l������
/// </summary>
class AcquireASuperiorityGauge :public IStrategicLayer
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

	float EvaluationFunction();

private:
	static const float SUCCEED_GAUGE_VALUE;//�ǂ��܂ŉ񕜂����琬���Ƃ��邩
	float nowGauge;//�헪�J�n���̗D���Q�[�W
	//�N���b�V��������--------------------------
	bool crashEnemyFlag;



	//�N���b�V������Ȃ��悤�ɂ���--------------------------
	bool dontCrashFlag;
};