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
	RestoreStamina(
		const std::shared_ptr<FollowPath> &FOLLOW_PATH,
		const std::shared_ptr<MovingBetweenTwoPoints> &MOVING_BETWEEN_TOW_POINTS,
		const const std::vector<std::vector<std::shared_ptr<WayPointData>>> &WAYPOINTS
	);

	void Init(const Vec2<float> &START_POS, const Vec2<float> &END_POS);

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
	Vec2<int>startHandle, endHandle;
	WayPointData startPoint, endPoint;
	bool startFlag;
	std::vector<WayPointData> route;
private:
	std::shared_ptr<FollowPath> followPath;
	std::unique_ptr<SearchWayPoint> searchStartPoint, searchGoalPoint;
	std::unique_ptr<MoveToOwnGround> moveToOnwGround;

	bool readyToFollowPathFlag;
	bool readyToGoToGoalFlag;

	std::vector<Vec2<float>>itemList;
	float serachItemRadius;

	bool initRouteFlag;


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
};

/// <summary>
/// ���w�Ɍ�����
/// </summary>
class GoToTheField
{
public:
	GoToTheField();

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
};



/// <summary>
/// �D���Q�[�W���l������
/// </summary>
class AcquireASuperiorityGauge
{
public:
	AcquireASuperiorityGauge();

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
};